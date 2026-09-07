#!/usr/bin/env python3
"""Lightweight checks that keep data counts and completion hashes consistent."""

import pathlib
import re


ROOT = pathlib.Path(__file__).resolve().parents[1]
HEADER = (ROOT / "source" / "Data.h").read_text(encoding="utf-8")
DATA = (ROOT / "source" / "Data.cpp").read_text(encoding="utf-8")


def count_entries(type_name: str, array_name: str) -> list[str]:
    match = re.search(
        rf"Data::{type_name} {array_name}\[[^]]+\] = \{{(.*?\)\s*\}};)",
        DATA,
        re.DOTALL,
    )
    if not match:
        raise AssertionError(f"{type_name} data array not found")
    return re.findall(rf"Data::{type_name}\((\d+),", match.group(1))


def declared_count(name: str) -> int:
    match = re.search(rf"const int {name}Count = (\d+);", HEADER)
    if not match:
        raise AssertionError(f"{name} count not declared")
    return int(match.group(1))


for plural, type_name, array_name in (
    ("Koroks", "Korok", "Koroks"),
    ("Shrine", "Shrine", "Shrines"),
    ("DLCShrine", "DLCShrine", "DLCShrines"),
    ("Locations", "Location", "Locations"),
    ("Hinoxes", "Hinox", "Hinoxes"),
    ("Taluses", "Talus", "Taluses"),
    ("Moldugas", "Molduga", "Moldugas"),
):
    hashes = count_entries(type_name, array_name)
    assert len(hashes) == declared_count(plural), (type_name, len(hashes), declared_count(plural))
    assert len(hashes) == len(set(hashes)), f"duplicate {type_name} completion hash"
