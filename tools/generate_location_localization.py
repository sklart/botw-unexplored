#!/usr/bin/env python3
"""Generate the Russian BotW location table from paired LocationMarker PO files."""

from __future__ import annotations

import argparse
import ast
import difflib
import re
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


def unquote_po(value: str) -> str:
    """Decode a single PO quoted string using Python's compatible escapes."""
    value = value.strip()
    if not value.startswith('"'):
        raise ValueError("expected a quoted PO string: {!r}".format(value))
    return ast.literal_eval(value)


def parse_po_text(text: str, source: str = "<string>") -> Dict[str, str]:
    """Read msgid -> msgstr entries from gettext PO text without dependencies."""
    entries: Dict[str, str] = {}
    current_id: str | None = None
    current_value: List[str] = []
    reading_value = False

    def finish() -> None:
        nonlocal current_id, current_value, reading_value
        if current_id:
            if current_id in entries:
                raise ValueError("duplicate msgid {!r} in {}".format(current_id, source))
            entries[current_id] = "".join(current_value)
        current_id = None
        current_value = []
        reading_value = False

    for line_number, raw_line in enumerate(text.splitlines(), 1):
        line = raw_line.strip()
        if not line or line.startswith('#'):
            if not line:
                finish()
            continue
        if line.startswith("msgid "):
            finish()
            current_id = unquote_po(line[6:])
            continue
        if line.startswith("msgstr "):
            if current_id is None:
                raise ValueError("msgstr without msgid at {}:{}".format(source, line_number))
            current_value = [unquote_po(line[7:])]
            reading_value = True
            continue
        if line.startswith('"') and reading_value:
            current_value.append(unquote_po(line))
            continue
        raise ValueError("unsupported PO syntax at {}:{}: {}".format(source, line_number, raw_line))
    finish()
    return entries


def parse_po(path: Path) -> Dict[str, str]:
    return parse_po_text(path.read_text(encoding="utf-8"), str(path))


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
// Source: botw-gettext LocationMarker.po (USen -> internal message ID -> EUru).
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
    parser.add_argument("--en", type=Path, required=True, help="USen LocationMarker.po")
    parser.add_argument("--ru", type=Path, required=True, help="EUru LocationMarker.po")
    parser.add_argument("--output", type=Path, default=Path("source/LocationLocalization.cpp"))
    parser.add_argument("--report", type=Path, help="write comparison report to this path")
    parser.add_argument("--check", action="store_true", help="verify generated output without writing it")
    args = parser.parse_args(argv)

    locations = parse_locations(args.data)
    old = parse_existing_table(args.output)
    matches = build_matches(locations, parse_po(args.en), parse_po(args.ru))
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
