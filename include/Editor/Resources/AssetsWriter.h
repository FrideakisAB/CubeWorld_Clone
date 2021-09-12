#ifndef ASSETSWRITER_H
#define ASSETSWRITER_H

#include <mutex>
#include <vector>
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
    std::vector<std::future<Code>> resources;

public:
    ~AssetsWriter();

    void AddAsset(const AssetsHandle &assetsHandle, std::function<void(const AssetsHandle &)> deleter=[](const AssetsHandle &handle){});
};

#endif
