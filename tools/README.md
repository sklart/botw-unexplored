# Russian location-name generator

`generate_location_localization.py` creates `source/LocationLocalization.cpp`
from the paired published `LocationMarker.po` files in
[`kenshiisod/botw-gettext`](https://github.com/kenshiisod/botw-gettext). It
matches the English `displayName` in `source/Data.cpp` to USen, obtains the
internal message ID, and then reads the Russian text for that same ID from EUru.
This is therefore neither a manual translation nor an English-to-Russian text
match.

Download only these two files externally; do not add them to this repository:

```text
botw-wiiu/USen/StaticMsg/LocationMarker.po
botw-wiiu/EUru/StaticMsg/LocationMarker.po
```

Then run from the repository root:

```bash
python tools/generate_location_localization.py \
  --en /path/to/USen/LocationMarker.po \
  --ru /path/to/EUru/LocationMarker.po
```

To verify the generated file without changing it:

```bash
python tools/generate_location_localization.py \
  --en /path/to/USen/LocationMarker.po \
  --ru /path/to/EUru/LocationMarker.po \
  --check
```

Pass `--report path/to/report.txt` to produce the complete comparison report.
The generator fails on missing or ambiguous matches. Normalization is limited to
Unicode punctuation and whitespace; fuzzy matching is never used. Do not edit
`LocationLocalization.cpp` by hand.

Shrines and DLC shrines are intentionally out of scope for this generator. It
covers only the 187 entries in `Data::Locations`.
