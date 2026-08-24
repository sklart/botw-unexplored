import importlib.util
import sys
import unittest
from pathlib import Path


SCRIPT = Path(__file__).parents[1] / "generate_location_localization.py"
FIXTURES = Path(__file__).parent / "fixtures"
SPEC = importlib.util.spec_from_file_location("generator", SCRIPT)
generator = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = generator
SPEC.loader.exec_module(generator)


class GeneratorTests(unittest.TestCase):
    def test_po_and_normalization(self):
        en = generator.parse_po(FIXTURES / "location_marker_en.po")
        ru = generator.parse_po(FIXTURES / "location_marker_ru.po")
        locations = [generator.Location(1, "King\'s Gate")]
        matches = generator.build_matches(locations, en, ru)
        self.assertEqual(matches[0].status, "NORMALIZED")
        self.assertEqual(matches[0].russian, "Ворота короля")

    def test_duplicate_english_with_different_russian_is_ambiguous(self):
        locations = [generator.Location(1, "Same")]
        matches = generator.build_matches(locations, {"one": "Same", "two": "Same"},
                                          {"one": "Один", "two": "Два"})
        self.assertEqual(matches[0].status, "AMBIGUOUS")

    def test_duplicate_english_with_identical_russian_is_safe(self):
        locations = [generator.Location(1, "Same")]
        matches = generator.build_matches(locations, {"one": "Same", "two": "Same"},
                                          {"one": "Одинаково", "two": "Одинаково"})
        self.assertEqual(matches[0].status, "EXACT")
        self.assertEqual(matches[0].russian, "Одинаково")

    def test_missing_russian_id(self):
        matches = generator.build_matches([generator.Location(1, "Name")], {"one": "Name"}, {})
        self.assertEqual(matches[0].status, "MISSING_RU")

    def test_duplicate_message_id_is_rejected(self):
        with self.assertRaisesRegex(ValueError, "duplicate msgid"):
            generator.parse_po_text('msgid "Marker"\nmsgstr "One"\n\nmsgid "Marker"\nmsgstr "Two"\n')

    def test_output_is_deterministic_and_escapes_quotes(self):
        matches = generator.build_matches(
            [generator.Location(2, 'Quoted "name"'), generator.Location(1, "King's Gate")],
            generator.parse_po(FIXTURES / "location_marker_en.po"),
            generator.parse_po(FIXTURES / "location_marker_ru.po"),
        )
        first = generator.render_table(matches)
        self.assertEqual(first, generator.render_table(matches))
        self.assertIn('\\"Название\\" в кавычках', first)


if __name__ == "__main__":
    unittest.main()
