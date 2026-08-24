#!/usr/bin/env python3
"""Generate the Russian BotW location table from paired Switch message archives."""

from __future__ import annotations

import argparse
import ast
import re
import struct
import sys
import unicodedata
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Mapping, Sequence, Tuple


LOCATION_RE = re.compile(
    r'Data::Location\(\s*(?P<hash>\d+)\s*,\s*"(?P<name>(?:\\.|[^"\\])*)"',
)
TABLE_RE = re.compile(r'\{(?P<hash>\d+)u,\s*"(?P<name>(?:\\.|[^"\\])*)"\}')
WHITESPACE_RE = re.compile(r'\s+')


@dataclass(frozen=True)
class Location:
    hash: int
    english: str


@dataclass(frozen=True)
class Match:
    location: Location
    status: str
    internal_id: str | None = None
    russian: str | None = None
    detail: str = ""


def yaz0_decompress(data: bytes) -> bytes:
    if data[:4] != b"Yaz0":
        return data
    size = struct.unpack_from(">I", data, 4)[0]
    output = bytearray()
    source = 16
    code = bits_left = 0
    while len(output) < size:
        if bits_left == 0:
            code = data[source]
            source += 1
            bits_left = 8
        if code & 0x80:
            output.append(data[source])
            source += 1
        else:
            first, second = data[source], data[source + 1]
            source += 2
            distance = ((first & 0x0F) << 8) | second
            count = first >> 4
            if count == 0:
                count = data[source] + 0x12
                source += 1
            else:
                count += 2
            start = len(output) - distance - 1
            for index in range(count):
                output.append(output[start + index])
        code <<= 1
        bits_left -= 1
    return bytes(output)


def sarc_entries(data: bytes):
    data = yaz0_decompress(data)
    if data[:4] != b"SARC":
        raise ValueError("input is not a SARC archive")
    endian = "<" if data[6:8] == b"\xff\xfe" else ">"
    header_size = struct.unpack_from(endian + "H", data, 4)[0]
    data_offset = struct.unpack_from(endian + "I", data, 0x0C)[0]
    if data[header_size:header_size + 4] != b"SFAT":
        raise ValueError("missing SFAT header")
    count = struct.unpack_from(endian + "H", data, header_size + 6)[0]
    nodes = header_size + 0x0C
    sfnt = nodes + count * 0x10
    if data[sfnt:sfnt + 4] != b"SFNT":
        raise ValueError("missing SFNT header")
    strings = sfnt + 8
    for index in range(count):
        node = nodes + index * 0x10
        name_data, start, end = struct.unpack_from(endian + "III", data, node + 4)
        if not name_data >> 24:
            continue
        name_start = strings + ((name_data & 0x00FFFFFF) * 4)
        name_end = data.index(b"\0", name_start)
        name = data[name_start:name_end].decode("utf-8")
        yield name, data[data_offset + start:data_offset + end]


def parse_msbt(data: bytes) -> Dict[str, str]:
    if data[:8] != b"MsgStdBn":
        raise ValueError("input is not an MSBT file")
    endian = "<" if data[8:10] == b"\xff\xfe" else ">"
    sections = struct.unpack_from(endian + "H", data, 0x0E)[0]
    # MSBT has a fixed 0x20-byte header. Every section has an 0x10-byte header.
    offset = 0x20
    labels: Dict[int, str] = {}
    texts: Dict[int, str] = {}
    for _ in range(sections):
        magic = data[offset:offset + 4]
        size = struct.unpack_from(endian + "I", data, offset + 4)[0]
        payload = offset + 0x10
        if magic == b"LBL1":
            groups = struct.unpack_from(endian + "I", data, payload)[0]
            for group in range(groups):
                count, relative = struct.unpack_from(endian + "II", data, payload + 4 + group * 8)
                cursor = payload + relative
                for _ in range(count):
                    length = data[cursor]
                    cursor += 1
                    label = data[cursor:cursor + length].decode("utf-8")
                    cursor += length
                    index = struct.unpack_from(endian + "I", data, cursor)[0]
                    cursor += 4
                    if index in labels:
                        raise ValueError("duplicate MSBT text index {}".format(index))
                    labels[index] = label
        elif magic == b"TXT2":
            count = struct.unpack_from(endian + "I", data, payload)[0]
            for index in range(count):
                relative = struct.unpack_from(endian + "I", data, payload + 4 + index * 4)[0]
                cursor = payload + relative
                end = cursor
                while data[end:end + 2] != b"\0\0":
                    end += 2
                text = data[cursor:end].decode("utf-16" + ("le" if endian == "<" else "be"))
                texts[index] = text
        offset = payload + size
        offset = (offset + 15) & ~15
    result = {}
    for index, label in labels.items():
        if index not in texts:
            raise ValueError("MSBT label {!r} has no text".format(label))
        result[label] = texts[index]
    return result


def parse_switch_location_marker(path: Path) -> Dict[str, str]:
    for name, contents in sarc_entries(path.read_bytes()):
        if name == "StaticMsg/LocationMarker.msbt":
            return parse_msbt(contents)
    raise ValueError("StaticMsg/LocationMarker.msbt not found in {}".format(path))


def parse_locations(path: Path) -> List[Location]:
    locations = [Location(int(match["hash"]), ast.literal_eval('"{}"'.format(match["name"])))
                 for match in LOCATION_RE.finditer(path.read_text(encoding="utf-8"))]
    if not locations:
        raise ValueError("no Data::Location entries found in {}".format(path))
    hashes = [location.hash for location in locations]
    if len(hashes) != len(set(hashes)):
        raise ValueError("duplicate location hash in {}".format(path))
    return locations


def parse_existing_table(path: Path) -> Dict[int, str]:
    if not path.exists():
        return {}
    return {int(match["hash"]): ast.literal_eval('"{}"'.format(match["name"]))
            for match in TABLE_RE.finditer(path.read_text(encoding="utf-8"))}


def normalize(text: str) -> str:
    """Conservative fallback for punctuation and whitespace differences only."""
    text = unicodedata.normalize("NFKC", text)
    text = text.replace("’", "'").replace("‘", "'").replace("`", "'")
    return WHITESPACE_RE.sub(" ", text).strip()


def index_english(entries: Mapping[str, str]) -> Tuple[Dict[str, List[str]], Dict[str, List[str]]]:
    exact: Dict[str, List[str]] = {}
    normalized: Dict[str, List[str]] = {}
    for internal_id, english in entries.items():
        if not english:
            continue
        exact.setdefault(english, []).append(internal_id)
        normalized.setdefault(normalize(english), []).append(internal_id)
    return exact, normalized


def choose_id(ids: Sequence[str], russian: Mapping[str, str]) -> Tuple[str | None, str]:
    """Accept duplicate IDs only if they have exactly the same official Russian text."""
    if len(ids) == 1:
        return ids[0], ""
    translated = {russian.get(internal_id) for internal_id in ids}
    if None not in translated and len(translated) == 1:
        return sorted(ids)[0], "duplicate English text has identical Russian translation"
    return None, "candidate IDs: {}".format(", ".join(sorted(ids)))


def build_matches(locations: Sequence[Location], english: Mapping[str, str], russian: Mapping[str, str]) -> List[Match]:
    exact, normalized = index_english(english)
    results: List[Match] = []
    for location in locations:
        ids = exact.get(location.english)
        status = "EXACT"
        if ids is None:
            ids = normalized.get(normalize(location.english))
            status = "NORMALIZED"
        if not ids:
            results.append(Match(location, "MISSING_EN"))
            continue
        internal_id, detail = choose_id(ids, russian)
        if internal_id is None:
            results.append(Match(location, "AMBIGUOUS", detail=detail))
            continue
        russian_name = russian.get(internal_id)
        if not russian_name:
            results.append(Match(location, "MISSING_RU", internal_id, detail=detail))
            continue
        results.append(Match(location, status, internal_id, russian_name, detail))
    return results


def cpp_quote(text: str) -> str:
    return text.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n")


def write_utf8(path: Path, contents: str) -> None:
    """Write deterministic LF-terminated UTF-8 output on supported Python 3 versions."""
    with path.open("w", encoding="utf-8", newline="\n") as output:
        output.write(contents)


def render_table(matches: Sequence[Match]) -> str:
    generated = [match for match in matches if match.russian is not None]
    rows = "\n".join("        {{{}u, \"{}\"}},".format(match.location.hash, cpp_quote(match.russian))
                     for match in generated)
    return """// AUTO-GENERATED FILE.
// Generated by tools/generate_location_localization.py.
// Source: Switch Msg_USen/Msg_EUru product.ssarc (USen -> internal message ID -> EUru).
// Do not edit manually.

#include \"Localization.h\"

#include <map>

namespace
{{
    const std::map<uint32_t, std::string> russianLocations = {{
{rows}
    }};
}}

const std::string& Localization::GetLocationName(uint32_t hash, const std::string& fallback)
{{
    if (GetLanguage() != Language::Russian)
        return fallback;

    const auto it = russianLocations.find(hash);
    return it == russianLocations.end() ? fallback : it->second;
}}
""".format(rows=rows)


def render_report(matches: Sequence[Match], old: Mapping[int, str]) -> str:
    lines = ["hash\tEnglish\told Russian\tofficial Russian\tstatus\tdetail"]
    for match in matches:
        if match.status in ("EXACT", "NORMALIZED"):
            status = "SAME" if old.get(match.location.hash) == match.russian else "CHANGED"
        else:
            status = match.status
        lines.append("{}\t{}\t{}\t{}\t{}\t{}".format(
            match.location.hash, match.location.english, old.get(match.location.hash, ""),
            match.russian or "", status, match.detail or "-"))
    return "\n".join(lines) + "\n"


def summary(matches: Sequence[Match]) -> str:
    counts = {status: sum(match.status == status for match in matches)
              for status in ("EXACT", "NORMALIZED", "MISSING_EN", "MISSING_RU", "AMBIGUOUS")}
    generated = counts["EXACT"] + counts["NORMALIZED"]
    return ("Locations in Data.cpp:      {}\nExact matches:              {}\n"
            "Normalized matches:         {}\nMissing EN:                  {}\n"
            "Missing RU:                  {}\nAmbiguous:                   {}\n"
            "Russian names generated:    {}".format(
                len(matches), counts["EXACT"], counts["NORMALIZED"], counts["MISSING_EN"],
                counts["MISSING_RU"], counts["AMBIGUOUS"], generated))


def main(argv: Sequence[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--data", type=Path, default=Path("source/Data.cpp"))
    parser.add_argument("--en", type=Path, required=True, help="Switch Msg_USen.product.ssarc")
    parser.add_argument("--ru", type=Path, required=True, help="Switch Msg_EUru.product.ssarc")
    parser.add_argument("--output", type=Path, default=Path("source/LocationLocalization.cpp"))
    parser.add_argument("--report", type=Path, help="write comparison report to this path")
    parser.add_argument("--check", action="store_true", help="verify generated output without writing it")
    args = parser.parse_args(argv)

    locations = parse_locations(args.data)
    old = parse_existing_table(args.output)
    matches = build_matches(locations, parse_switch_location_marker(args.en), parse_switch_location_marker(args.ru))
    print(summary(matches))
    report = render_report(matches, old)
    if args.report:
        write_utf8(args.report, report)
    else:
        for line in report.splitlines()[1:]:
            if "\tCHANGED\t" in line or "\tMISSING_" in line or "\tAMBIGUOUS\t" in line:
                print(line)

    failures = [match for match in matches if match.status not in ("EXACT", "NORMALIZED")]
    if failures:
        print("generation stopped: unresolved location names", file=sys.stderr)
        return 1

    rendered = render_table(matches)
    if args.check:
        if not args.output.exists() or args.output.read_text(encoding="utf-8") != rendered:
            print("generated file is out of date: {}".format(args.output), file=sys.stderr)
            return 1
        print("generated file is up to date")
        return 0
    write_utf8(args.output, rendered)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
