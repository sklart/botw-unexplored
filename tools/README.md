# Switch location-name generator

`generate_location_localization.py` creates `source/LocationLocalization.cpp`
from the official Switch `Msg_USen.product.ssarc` and `Msg_EUru.product.ssarc`
archives. It matches the English `displayName` in `source/Data.cpp` to the USen
text, obtains the internal message ID, and reads the Russian text using that
same ID from EUru. This is neither a manual translation nor an
English-to-Russian text match.

Extract the language archives from your own Switch game dump into the ignored
`localization_sources/` directory. The expected paths in this workspace are:

```text
localization_sources/base/Bootup_USen/Message/Msg_USen.product.ssarc
localization_sources/base/Bootup_EUru/Message/Msg_EUru.product.ssarc
```

Then run from the repository root:

```bash
python tools/generate_location_localization.py \
  --en localization_sources/base/Bootup_USen/Message/Msg_USen.product.ssarc \
  --ru localization_sources/base/Bootup_EUru/Message/Msg_EUru.product.ssarc
```

To verify the generated file without changing it:

```bash
python tools/generate_location_localization.py \
  --en localization_sources/base/Bootup_USen/Message/Msg_USen.product.ssarc \
  --ru localization_sources/base/Bootup_EUru/Message/Msg_EUru.product.ssarc \
  --check
```

Pass `--report path/to/report.txt` to produce the complete comparison report.
The generator fails on missing or ambiguous matches. Normalization is limited to
Unicode punctuation and whitespace; fuzzy matching is never used. Do not edit
`LocationLocalization.cpp` by hand.

Shrines and DLC shrines are intentionally out of scope for this generator. It
covers only the 187 entries in `Data::Locations`.
