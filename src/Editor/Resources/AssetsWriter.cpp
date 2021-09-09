#include "Editor/Resources/AssetsWriter.h"

AssetsWriter::~AssetsWriter()
{
    for (auto &res : resources)
    {
        Code result = res.get();
        if (result == Code::Failed)
            logger->Error("Resource not save! Error in save function!");
    }
}

void AssetsWriter::AddAsset(const AssetsHandle &assetsHandle, std::function<void(const AssetsHandle &)> deleter)
{
    auto saveFunction = [&, assetsHandle, deleter](){
        std::string path = fs::current_path().string() + "/data/" + assetsHandle->GetName();
        std::string pathBin;
        std::ofstream file = std::ofstream(path, std::ios::trunc);
        if (file.is_open())
        {
            file << assetsHandle->SerializeObj().dump(4);
            file.close();

            if (assetsHandle->IsBinaryNeeded())
            {
                file.open(path + ".bin", std::ios::trunc | std::ios::binary);
                if (file.is_open())
                {
                    assetsHandle->SerializeBin(file);
                    file.close();
                    pathBin = path + ".bin";
                }
                else
                    return Code::Failed;
            }
        }
        else
            return Code::Failed;

        referenceDbLock.lock();
        json staticReferenceDb = json_utils::TryParse(Utils::FileToString(std::ifstream("data/staticReference.db")));
        staticReferenceDb[assetsHandle->GetName()] = {path, pathBin};
        file = std::ofstream("data/staticReference.db", std::ios::trunc);
        if (file.is_open())
        {
            file << staticReferenceDb.dump(4);
            file.close();
        }
        else
            return Code::Failed;
        referenceDbLock.unlock();

        deleter(std::cref(assetsHandle));

        return Code::Success;
    };

    resources.push_back(std::async(std::launch::async, saveFunction));
}
