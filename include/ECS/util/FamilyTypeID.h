#ifndef FAMILYTYPEID_H
#define FAMILYTYPEID_H

#include "ECS/API.h"

namespace ECS::util::Internal {
    template<class T>
    class FamilyTypeID {
    private:
        inline static TypeID s_count{};

    public:
        template<class U>
        [[nodiscard]] static TypeID Get() noexcept
        {
            static const TypeID STATIC_TYPE_ID{s_count++};
            return STATIC_TYPE_ID;
        }

        [[nodiscard]] static TypeID GetCount() noexcept
        {
            return s_count;
        }
    };
}

#endif 
