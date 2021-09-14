#ifndef ASSETSWRITER_H
#define ASSETSWRITER_H

#include <map>
#include <mutex>
#include <future>
#include "Platform.h"
#include "Assets/AssetsManager.h"

class AssetsWriter {
private:
    enum class Code : u8 {
        Success = 0,
        Failed
    };

    std::mutex referenceDbLock;
    std::map<std::string, std::future<Code>> resources;

public:
    ~AssetsWriter();

    void AddAsset(const AssetsHandle &assetsHandle, std::function<void(const AssetsHandle &)> deleter=[](const AssetsHandle &handle){});
    std::future<Code> RemoveAsset(const std::string &name);
};

#endif
