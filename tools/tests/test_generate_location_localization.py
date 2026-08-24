import importlib.util
import struct
import sys
import unittest
from pathlib import Path


SCRIPT = Path(__file__).parents[1] / "generate_location_localization.py"
SPEC = importlib.util.spec_from_file_location("generator", SCRIPT)
generator = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = generator
SPEC.loader.exec_module(generator)


def make_msbt(label, text):
    label_data = label.encode("utf-8")
    lbl_payload = struct.pack("<I", 1) + struct.pack("<II", 1, 12)
    lbl_payload += bytes([len(label_data)]) + label_data + struct.pack("<I", 0)
    txt_data = text.encode("utf-16le") + b"\0\0"
    txt_payload = struct.pack("<II", 1, 8) + txt_data

    def section(name, payload):
        result = name + struct.pack("<I", len(payload)) + b"\0" * 8 + payload
        return result + b"\0" * ((-len(result)) % 16)

    header = bytearray(0x20)
    header[:8] = b"MsgStdBn"
    header[8:10] = b"\xff\xfe"
    header[0x0C] = 1
    header[0x0D] = 3
    header[0x0E:0x10] = struct.pack("<H", 2)
    return bytes(header) + section(b"LBL1", lbl_payload) + section(b"TXT2", txt_payload)


class GeneratorTests(unittest.TestCase):
    def test_msbt_parser_supports_cyrillic_and_quotes(self):
        entries = generator.parse_msbt(make_msbt("Marker", '«Ворота»'))
        self.assertEqual(entries, {"Marker": '«Ворота»'})

    def test_normalization_handles_apostrophe_and_whitespace(self):
        locations = [generator.Location(1, "King's Gate")]
        matches = generator.build_matches(locations, {"marker": "King’s  Gate"}, {"marker": "Ворота"})
        self.assertEqual(matches[0].status, "NORMALIZED")

    def test_duplicate_english_with_different_russian_is_ambiguous(self):
        matches = generator.build_matches([generator.Location(1, "Same")],
                                          {"one": "Same", "two": "Same"},
                                          {"one": "Один", "two": "Два"})
        self.assertEqual(matches[0].status, "AMBIGUOUS")

    def test_missing_russian_id(self):
        matches = generator.build_matches([generator.Location(1, "Name")], {"one": "Name"}, {})
        self.assertEqual(matches[0].status, "MISSING_RU")

    def test_output_is_deterministic_and_escapes_quotes(self):
        matches = generator.build_matches([generator.Location(2, 'Quoted "name"')],
                                          {"marker": 'Quoted "name"'},
                                          {"marker": '"Название"'})
        first = generator.render_table(matches)
        self.assertEqual(first, generator.render_table(matches))
        self.assertIn('\\"Название\\"', first)


if __name__ == "__main__":
    unittest.main()
