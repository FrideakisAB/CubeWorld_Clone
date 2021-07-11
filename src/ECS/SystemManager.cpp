#include "ECS/SystemManager.h"

#include <map>
#include <functional>
#include "ECS/util/Timer.h"

namespace ECS {
    SystemManager::SystemManager()
    {
        this->systemAllocator = new SystemAllocator(ECS_SYSTEM_MEMORY_BUFFER_SIZE, Allocate(ECS_SYSTEM_MEMORY_BUFFER_SIZE, "SystemManager"));
    }

    SystemManager::~SystemManager()
    {
        for (auto it = this->systemWorkOrder.rbegin(); it != this->systemWorkOrder.rend(); ++it)
        {
            (*it)->~ISystem();
            *it = nullptr;
        }

        systemWorkOrder.clear();
        systems.clear();

        Free((void *)this->systemAllocator->GetMemoryAddress0());
        delete this->systemAllocator;
        this->systemAllocator = nullptr;
    }

    void SystemManager::Update()
    {
        f32 dt_ms = ECS_Engine->GetTimer()->GetDeltaTime();
        for (ISystem *system : this->systemWorkOrder)
        {
            system->timeSinceLastUpdate += dt_ms;

            system->needsUpdate = (system->updateInterval < 0.0f) || ((system->updateInterval > 0.0f) && (system->timeSinceLastUpdate > system->updateInterval));

            if (system->enabled == true && system->needsUpdate == true)
                system->PreUpdate();
        }

        for (ISystem *system : this->systemWorkOrder)
        {
            if (system->enabled == true && system->needsUpdate == true)
            {
                system->Update();
                system->timeSinceLastUpdate = 0.0f;
            }
        }

        for (ISystem *system : this->systemWorkOrder)
        {
            if (system->enabled == true && system->needsUpdate == true)
                system->PostUpdate();
        }
    }

    void SystemManager::UpdateSystemWorkOrder()
    {
        const std::function<void(SystemTypeId, std::vector<int> &, const std::vector<std::vector<bool>> &, std::vector<SystemTypeId> &)> DFS = [&](SystemTypeId vertex,
            std::vector<int> &VERTEX_STATE,
            const std::vector<std::vector<bool>> &EDGES,
            std::vector<SystemTypeId> &output) {
            VERTEX_STATE[vertex] = 1; // visited

            for (int i = 0; i < VERTEX_STATE.size(); ++i)
            {
                if (EDGES[i][vertex] && VERTEX_STATE[i] == 0)
                    DFS(i, VERTEX_STATE, EDGES, output);
            }

            VERTEX_STATE[vertex] = 2;
            output.push_back(vertex);
        };

        const size_t NUM_SYSTEMS = this->systemDependencyMatrix.size();

        std::vector<int> INDICES(NUM_SYSTEMS);
        for (int i = 0; i < NUM_SYSTEMS; ++i)
            INDICES[i] = i;

        std::vector<std::vector<SystemTypeId>> VERTEX_GROUPS;
        std::vector<SystemPriority> GROUP_PRIORITY;

        while (!INDICES.empty())
        {
            SystemTypeId index = INDICES.back();
            INDICES.pop_back();

            if (index == -1)
                continue;

            std::vector<SystemTypeId> group;
            std::vector<SystemTypeId> member;

            SystemPriority groupPriority = LOWEST_SYSTEM_PRIORITY;
            member.push_back(index);

            while (!member.empty())
            {
                index = member.back();
                member.pop_back();

                for (int i = 0; i < INDICES.size(); ++i)
                {
                    if (INDICES[i] != -1 && (this->systemDependencyMatrix[i][index] || this->systemDependencyMatrix[index][i]))
                    {
                        member.push_back(i);
                        INDICES[i] = -1;
                    }
                }

                group.push_back(index);

                ISystem *sys = this->systems[index];
                groupPriority = std::max((sys != nullptr ? sys->priority : NORMAL_SYSTEM_PRIORITY), groupPriority);
            }

            VERTEX_GROUPS.push_back(group);
            GROUP_PRIORITY.push_back(groupPriority);
        }

        const size_t NUM_VERTEX_GROUPS = VERTEX_GROUPS.size();

        std::vector<int> vertex_states(NUM_SYSTEMS, 0);

        std::multimap<SystemPriority, std::vector<SystemTypeId>> VERTEX_GROUPS_SORTED;

        for (int i = 0; i < NUM_VERTEX_GROUPS; ++i)
        {
            auto g = VERTEX_GROUPS[i];

            std::vector<SystemTypeId> order;

            for (u64 j : g)
                if (vertex_states[j] == 0)
                    DFS(j, vertex_states, this->systemDependencyMatrix, order);

            std::reverse(order.begin(), order.end());

            // note: MAX - PRIORITY will force the correct sorting behaviour in multimap (by default a multimap sorts int values from low to high)
            VERTEX_GROUPS_SORTED.insert(std::pair<SystemPriority, std::vector<SystemTypeId>>(std::numeric_limits<SystemPriority>::max() - GROUP_PRIORITY[i], order));
        }

        this->systemWorkOrder.clear();
        for (const auto &group : VERTEX_GROUPS_SORTED)
        {
            for (auto m : group.second)
            {
                ISystem *sys = this->systems[m];
                if (sys != nullptr)
                    this->systemWorkOrder.push_back(sys);
            }
        }
    }

    SystemWorkStateMask SystemManager::GetSystemWorkState() const
    {
        SystemWorkStateMask mask(this->systemWorkOrder.size());

        for (int i = 0; i < this->systemWorkOrder.size(); ++i)
            mask[i] = this->systemWorkOrder[i]->enabled;

        return mask;
    }

    void SystemManager::SetSystemWorkState(SystemWorkStateMask mask)
    {
        assert(mask.size() == this->systemWorkOrder.size() && "Provided mask does not match size of current system array.");

        for (int i = 0; i < this->systemWorkOrder.size(); ++i)
            this->systemWorkOrder[i]->enabled = mask[i];
    }
}