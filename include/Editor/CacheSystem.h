#ifndef CACHESYSTEM_H
#define CACHESYSTEM_H

#include <string>
#include <chrono>
#include <utility>
#include "Platform.h"
#include "Utils/json.h"

class CacheEntry {
private:
    std::string resPath;
    u64 timeCreate;
    u64 timeToDelete;
    u64 id;

public:
    CacheEntry() noexcept : timeCreate(0), timeToDelete(0), id(0) {}
    CacheEntry(u64 timeCreate, u64 timeToDelete, u64 id, std::string path) noexcept : timeCreate(timeCreate), timeToDelete(timeToDelete), id(id), resPath(std::move(path)) {}

    [[nodiscard]] const std::string &GetPath() const noexcept { return resPath; }
    [[nodiscard]] bool IsAlive() const noexcept;
    void AddLifetime(u64 time) noexcept;
    void AddLifetime(std::chrono::hours time) noexcept;

    [[nodiscard]] std::chrono::hours GetTimeCreate() const noexcept;
    [[nodiscard]] std::chrono::hours GetLifetime() const noexcept;
    [[nodiscard]] u64 GetID() const noexcept { return id; }
};

class CacheSystem {
private:
    u64 currentFreeId = 1;
    json registry;

public:
    CacheSystem();
    ~CacheSystem();

    [[nodiscard]] CacheEntry CreateCache(u64 lifetime);
    [[nodiscard]] CacheEntry CreateCache(std::chrono::hours lifetime);
    [[nodiscard]] CacheEntry GetCache(u64 id);
    void UpdateCache(const CacheEntry &entry) noexcept;
    void RemoveCache(const CacheEntry &entry);

    void SafeClean();
    void UnsafeClean();
};

#endif
