#!/usr/bin/env bash
set -euo pipefail
compiler="${CXX:-g++}"
sanitizers=()
if [[ "${SANITIZE:-0}" == "1" ]]; then
    sanitizers=(-fsanitize=address,undefined -fno-omit-frame-pointer)
fi
"$compiler" -std=c++14 -Wall -Wextra -pedantic "${sanitizers[@]}" -Isource tests/test_settings.cpp source/Settings.cpp source/Utf8.cpp -o /tmp/test_settings
/tmp/test_settings
"$compiler" -std=c++14 -Wall -Wextra -pedantic "${sanitizers[@]}" -Isource tests/test_manual_progress.cpp source/ManualProgress.cpp -o /tmp/test_manual_progress
/tmp/test_manual_progress
"$compiler" -std=c++14 -Wall -Wextra -pedantic "${sanitizers[@]}" -Isource tests/test_legacy_korok_migration.cpp source/LegacyKorokMigration.cpp -o /tmp/test_legacy_korok_migration
/tmp/test_legacy_korok_migration
"$compiler" -std=c++14 -Wall -Wextra -pedantic "${sanitizers[@]}" -Isource tests/test_object_model.cpp source/ObjectModel.cpp -o /tmp/test_object_model
/tmp/test_object_model
"$compiler" -std=c++14 -Wall -Wextra -pedantic "${sanitizers[@]}" -Isource tests/test_save_load_decision.cpp source/SaveLoadDecision.cpp -o /tmp/test_save_load_decision
/tmp/test_save_load_decision
"$compiler" -std=c++14 -Wall -Wextra -pedantic "${sanitizers[@]}" -Isource tests/test_navigation.cpp source/Navigation.cpp -o /tmp/test_navigation
/tmp/test_navigation
"$compiler" -std=c++14 -Wall -Wextra -pedantic "${sanitizers[@]}" -Isource tests/test_localization.cpp source/Localization.cpp -o /tmp/test_localization
/tmp/test_localization
python3 tests/test_data_mapping.py
