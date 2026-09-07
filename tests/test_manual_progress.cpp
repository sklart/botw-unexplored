#include <cassert>
#include <sstream>

#include "ManualProgress.h"

int main()
{
    ManualProgress::Clear();
    ManualProgress::Context normal = {1, 2, false};
    ManualProgress::Context master = {1, 2, true};
    ManualProgress::Context otherProfile = {3, 4, false};
    ManualProgress::MarkFound(normal, Data::ObjectType::Korok, 100);
    ManualProgress::MarkFound(master, Data::ObjectType::Korok, 100);
    ManualProgress::MarkFound(otherProfile, Data::ObjectType::Shrine, 200);
    assert(ManualProgress::Entries().size() == 3);
    assert(ManualProgress::IsMarkedFound(normal, Data::ObjectType::Korok, 100));
    assert(!ManualProgress::IsMarkedFound(normal, Data::ObjectType::Shrine, 200));
    ManualProgress::ConfirmFromSave(normal, Data::ObjectType::Korok, 100);
    assert(!ManualProgress::IsMarkedFound(normal, Data::ObjectType::Korok, 100));
    assert(ManualProgress::IsMarkedFound(master, Data::ObjectType::Korok, 100));

    std::stringstream file;
    assert(ManualProgress::Save(file));
    ManualProgress::Clear();
    assert(ManualProgress::Load(file) == ManualProgress::LoadResult::Current);
    assert(ManualProgress::Entries().size() == 2);
    std::stringstream invalid("BOTW_UNEXPLORED_MANUAL_PROGRESS\n99\n");
    assert(ManualProgress::Load(invalid) == ManualProgress::LoadResult::FutureVersion);
    assert(!ManualProgress::CanPersist());
    assert(!ManualProgress::MarkFound(normal, Data::ObjectType::Korok, 300));
    std::stringstream malformed("not a manual-progress file\n");
    assert(ManualProgress::Load(malformed) == ManualProgress::LoadResult::Invalid);
    assert(ManualProgress::CanPersist());
}
