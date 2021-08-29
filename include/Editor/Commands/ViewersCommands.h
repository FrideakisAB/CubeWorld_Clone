#ifndef VIEWERSCOMMANDS_H
#define VIEWERSCOMMANDS_H

#include <string>
#include <filesystem>
#include "Engine.h"
#include "GameObject.h"
#include "Editor/Editor.h"
#include "Editor/CacheSystem.h"
#include "Editor/Commands/ICommand.h"

namespace fs = std::filesystem;

template<typename T>
class DeleteComponent final : public ICommand {
private:
    GameObject *gameObject;
    u64 id = 0;
    CacheEntry cache;

public:
    explicit DeleteComponent(GameObject *go)
        : gameObject(go)
    {}

    ~DeleteComponent() final
    {
        cache = GameEditor->CacheSystem.GetCache(cache.GetID());
        if(!cache.GetPath().empty())
            GameEditor->CacheSystem.RemoveCache(cache);
    }

    void Execute() final
    {
        if (id == 0)
            id = validator.Map(gameObject);

        cache = GameEditor->CacheSystem.CreateCache(8);
        std::string jsonStr = validator.Get(id)->template GetComponent<T>()->SerializeObj().dump(4);
        std::ofstream file = std::ofstream(fs::current_path().string() + cache.GetPath());
        file.write(jsonStr.c_str(), jsonStr.size());
        file.close();

        validator.Get(id)->template RemoveComponent<T>();
    }

    void Undo() final
    {
        validator.Get(id)->template AddComponent<T>();
        json j = json_utils::TryParse(Utils::FileToString(std::ifstream(fs::current_path().string() + cache.GetPath())));
        validator.Get(id)->template GetComponent<T>()->UnSerializeObj(j);
    }
};

template<typename T>
class ChangeState final : public ICommand {
private:
    GameObject *gameObject;
    u64 id = 0;
    CacheEntry cacheRedo;
    CacheEntry cacheUndo;

public:
    explicit ChangeState(GameObject *go)
        : gameObject(go)
    {
        cacheUndo = GameEditor->CacheSystem.CreateCache(8);
        std::string jsonStr = go->GetComponent<T>()->SerializeObj().dump(4);
        std::ofstream file = std::ofstream(fs::current_path().string() + cacheUndo.GetPath());
        file.write(jsonStr.c_str(), jsonStr.size());
        file.close();
    }

    ~ChangeState()
    {
        cacheRedo = GameEditor->CacheSystem.GetCache(cacheRedo.GetID());
        if(!cacheRedo.GetPath().empty())
            GameEditor->CacheSystem.RemoveCache(cacheRedo);
        cacheUndo = GameEditor->CacheSystem.GetCache(cacheUndo.GetID());
        if(!cacheUndo.GetPath().empty())
            GameEditor->CacheSystem.RemoveCache(cacheUndo);
    }

    void Execute() final
    {
        if(id == 0)
        {
            id = validator.Map(gameObject);

            cacheRedo = GameEditor->CacheSystem.CreateCache(8);
            std::string jsonStr = gameObject->GetComponent<T>()->SerializeObj().dump(4);
            std::ofstream file = std::ofstream(fs::current_path().string() + cacheRedo.GetPath());
            file.write(jsonStr.c_str(), jsonStr.size());
            file.close();
        }
        else
        {
            json j = json_utils::TryParse(Utils::FileToString(std::ifstream(fs::current_path().string() + cacheRedo.GetPath())));
            validator.Get(id)->template GetComponent<T>()->UnSerializeObj(j);
        }
    }

    void Undo() final
    {
        json j = json_utils::TryParse(Utils::FileToString(std::ifstream(fs::current_path().string() + cacheUndo.GetPath())));
        validator.Get(id)->template GetComponent<T>()->UnSerializeObj(j);
    }
};

class SetCamera final : public ICommand {
private:
    GameObject* goOld;
    GameObject* goNew;
    u64 goOldId = 0, goNewId = 0;

public:
    explicit SetCamera(GameObject *newCamera);

    void Execute() final;
    void Undo() final;
};

#endif
