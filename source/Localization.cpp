#include "Localization.h"

#include <cstdio>
#include <sys/stat.h>

namespace
{
    Localization::Language currentLanguage = Localization::Language::Russian;
    const char* LanguageFilePath = "sdmc:/switch/botw-unexplored/language.txt";

    char ToStoredValue(Localization::Language language)
    {
        if (language == Localization::Language::English) return 'e';
        if (language == Localization::Language::Spanish) return 's';
        return 'r';
    }

    bool FromStoredValue(int value, Localization::Language& language)
    {
        if (value == 'e') { language = Localization::Language::English; return true; }
        if (value == 's') { language = Localization::Language::Spanish; return true; }
        if (value == 'r') { language = Localization::Language::Russian; return true; }
        return false;
    }

    const std::string english[] = {
        "Exit", "Choose another profile", "No save data found for that user", "Make sure you chose the correct profile.",
        "BotW is running - can't load save", "Please run this app at least once without BotW running", "After that you can use it while playing.",
        "No", "Yes", "Master mode save file detected", "Would you like to load it?", "Legend", "X - close", "Koroks",
        "Shrines", "Hinoxes", "Taluses", "Moldugas", "Locations", "Show Completed", "B - mark as found",
        "Press X: legend", "BotW is running.", "Loaded older save", "Press Y: master mode",
        "Press Y: normal mode", "L/R: zoom  (-): user  (+): exit", "EN"
    };

    const std::string spanish[] = {
        "Salir", "Elegir otro perfil", "No se encontraron datos guardados para este usuario", "Asegúrate de haber elegido el perfil correcto.",
        "BotW está en ejecución - no se puede cargar la partida", "Ejecuta esta aplicación al menos una vez sin BotW en ejecución.", "Después podrás usarla mientras juegas.",
        "No", "Sí", "Se detectó una partida en modo maestro", "¿Quieres cargarla?", "Leyenda", "X - cerrar", "Kologs",
        "Santuarios", "Hinóx", "Taludes", "Moldugas", "Ubicaciones", "Mostrar completados", "B - marcar como encontrado",
        "X: leyenda", "BotW está en ejecución.", "Se cargó una partida anterior", "Y: modo maestro",
        "Y: modo normal", "L/R: zoom  (-): usuario  (+): salir", "ES"
    };
    const std::string russian[] = {
        "Выход", "Выбрать другой профиль", "Для этого пользователя не найдены сохранения", "Убедитесь, что выбран правильный профиль.",
        "BotW запущена - сохранение недоступно", "Запустите приложение хотя бы раз, когда BotW не запущена.", "После этого им можно будет пользоваться во время игры.",
        "Нет", "Да", "Найдено сохранение режима мастера", "Загрузить его?", "Легенда", "X - закрыть", "Короки",
        "Святилища", "Хиноксы", "Каменные талусы", "Молдоры", "Локации", "Показывать найденное", "B - отметить найденным",
        "X: открыть легенду", "BotW запущена.", "Загружено старое сохранение", "Y: режим мастера",
        "Y: обычный режим", "L/R: масштаб  (-): профиль  (+): выход", "RU"
    };
}

const std::string& Localization::Get(Text text)
{
    const unsigned int index = static_cast<unsigned int>(text);
    if (currentLanguage == Language::Russian) return russian[index];
    if (currentLanguage == Language::Spanish) return spanish[index];
    return english[index];
}

Localization::Language Localization::GetLanguage()
{
    return currentLanguage;
}

void Localization::ToggleLanguage()
{
    if (currentLanguage == Language::Russian) currentLanguage = Language::English;
    else if (currentLanguage == Language::English) currentLanguage = Language::Spanish;
    else currentLanguage = Language::Russian;
    Save();
}

void Localization::Load()
{
    std::FILE* file = std::fopen(LanguageFilePath, "r");
    if (file == nullptr)
        return;

    Language language;
    if (FromStoredValue(std::fgetc(file), language))
        currentLanguage = language;
    std::fclose(file);
}

void Localization::Save()
{
    // The directory is also used by the save-backup feature. mkdir is harmless if it already exists.
    mkdir("sdmc:/switch/botw-unexplored", 0777);

    std::FILE* file = std::fopen(LanguageFilePath, "w");
    if (file == nullptr)
        return;

    std::fputc(ToStoredValue(currentLanguage), file);
    std::fclose(file);
}