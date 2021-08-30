#include "Editor/CacheSystem.h"

#include "Log.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

CacheSystem::CacheSystem()
{
    if (!fs::exists(fs::current_path().string() + "/Cache"))
        fs::create_directory(fs::current_path().string() + "/Cache");

    std::ifstream cacheDBf("Cache/Caches.db");

    if (cacheDBf.is_open())
    {
        registry = json_utils::TryParse(Utils::FileToString(std::move(cacheDBf)));
        currentFreeId = registry["oldFreeId"];
    }

    if (!registry.contains("removed"))
        registry["removed"] = {};

    if (!registry.contains("Entries"))
        registry["Entries"] = {};
}

CacheSystem::~CacheSystem()
{
    try
    {
        registry["oldFreeId"] = currentFreeId;
        std::ofstream cacheDBf("Cache/Caches.db", std::ios::trunc);
        cacheDBf << registry.dump(4);
        cacheDBf.close();
    }
    catch (const std::exception& ex)
    {
        logger->Error("Cache resources not save! Error in CacheSystem, resource leak. Message: %s", ex.what());
    }
}

CacheEntry CacheSystem::CreateCache(u64 lifetime)
{
    u64 currentId;
    if (auto It = registry["removed"].begin(); It != registry["removed"].end())
    {
        currentId = strtoull((*It).get<std::string>().c_str(), nullptr, 10);
        registry["removed"].erase(It);
    }
    else
        currentId = ++currentFreeId;

    std::string pathCache = std::string("/Cache/") + "tmp" + std::to_string(currentId) + ".bin";
    std::ofstream(fs::current_path().string() + pathCache, std::ios_base::trunc).close();

    auto fTime = fs::last_write_time(fs::current_path().string() + pathCache);
    u64 timeSec = std::chrono::duration_cast<std::chrono::hours>(fTime.time_since_epoch()).count();

    registry["Entries"][std::to_string(currentId)] = {timeSec, timeSec + lifetime, pathCache};

    CacheEntry entry(timeSec, timeSec + lifetime, currentId, pathCache);

    return std::move(entry);
}

CacheEntry CacheSystem::CreateCache(std::chrono::hours lifetime)
{
    return CreateCache(lifetime.count());
}

CacheEntry CacheSystem::GetCache(u64 id)
{
    if (registry["Entries"].contains(std::to_string(id)))
        return std::move(CacheEntry(registry["Entries"][std::to_string(id)][0], registry["Entries"][std::to_string(id)][1], id, registry["Entries"][std::to_string(id)][2]));

    return std::move(CacheEntry());
}

void CacheSystem::UpdateCache(const CacheEntry &entry) noexcept
{
    if (registry["Entries"].contains(std::to_string(entry.GetID())))
        registry["Entries"][std::to_string(entry.GetID())][1] = entry.GetLifetime().count() != 0 ? entry.GetTimeCreate().count() + entry.GetLifetime().count() : 0;
}

void CacheSystem::RemoveCache(const CacheEntry& entry)
{
    if (!registry["removed"].is_null())
        registry["removed"].insert(registry["removed"].end(), std::to_string(entry.GetID()));
    else
        registry["removed"] = { std::to_string(entry.GetID()) };
}

void CacheSystem::SafeClean()
{
    auto secTime = std::chrono::duration_cast<std::chrono::hours>(std::chrono::system_clock::now().time_since_epoch()).count();
    for (auto It = registry["Entries"].begin(); It != registry["Entries"].end(); ++It)
    {
        if((*It)[1] < secTime && (*It)[1] != 0)
            registry["removed"].insert(registry["removed"].end(), It.key());
    }

    for (auto It = registry["removed"].begin(); It != registry["removed"].end(); ++It)
    {
        if (!(*It).is_null())
        {
            json j = registry["Entries"][(*It).get<std::string>()];
            registry["Entries"].erase((*It).get<std::string>());
            fs::remove(fs::current_path().string() + j[2].get<std::string>());
        }
    }

    registry["removed"] = {};
    if (registry["Entries"].begin() == registry["Entries"].end())
        currentFreeId = 0;
}

void CacheSystem::UnsafeClean()
{
    auto It = registry["Entries"].begin();
    while (It != registry["Entries"].end())
    {
        if (!(*It).is_null())
            fs::remove(fs::current_path().string() +(*It)[2].get<std::string>());

        registry["Entries"].erase(It);
        It = registry["Entries"].begin();
    }

    registry["removed"] = {};
    registry["Entries"] = {};
    currentFreeId = 0;
}

bool CacheEntry::IsAlive() const noexcept
{
    return (timeToDelete == 0 || timeCreate < timeToDelete);
}

void CacheEntry::AddLifetime(u64 time) noexcept
{
    if (timeToDelete != 0)
        timeToDelete += time;
    else
        timeToDelete = timeCreate + time;
}

void CacheEntry::AddLifetime(std::chrono::hours time) noexcept
{
    if (timeToDelete != 0)
        timeToDelete += time.count();
    else
        timeToDelete = timeCreate + time.count();
}

std::chrono::hours CacheEntry::GetTimeCreate() const noexcept
{
    return std::chrono::hours(timeCreate);
}

std::chrono::hours CacheEntry::GetLifetime() const noexcept
{
    return timeToDelete == 0 ? std::chrono::hours(0) : std::chrono::hours(timeToDelete - timeCreate);
}
