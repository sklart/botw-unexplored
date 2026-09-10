#include <cassert>

#include "Localization.h"

int main()
{
    for (Localization::Language language : {Localization::Language::English, Localization::Language::Russian, Localization::Language::Spanish})
    {
        Localization::SetLanguage(language);
        for (int value = 0; value < static_cast<int>(Data::ObjectType::Count); ++value)
            assert(!Localization::GetObjectTypeName(static_cast<Data::ObjectType>(value)).empty());
        assert(Localization::GetObjectTypeName(Data::ObjectType::DLCShrine) !=
               Localization::GetObjectTypeName(Data::ObjectType::Shrine));
        assert(Localization::GetObjectTypeName(Data::ObjectType::Count) ==
               Localization::Get(Localization::Text::ObjectInfo));
        assert(!Localization::Get(Localization::Text::ImageViewerBack).empty());
    }
}
