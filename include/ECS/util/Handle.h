#ifndef HANDLE_H
#define HANDLE_H

#include <vector>
#include "ECS/API.h"

#ifndef _WIN32
#include <climits>
#endif

#pragma warning(push)

// warning C4293: '<<': shift count negative or too big, undefined behavior
// note we are using a static_assert to ensure this won't be the case.
#pragma warning(disable: 4293)

namespace ECS::util {
    namespace Internal {
        template<
                typename handle_value_type,
                size_t version_bits,
                size_t index_bits
        >
        union Handle {
            static_assert(sizeof(handle_value_type) * CHAR_BIT >= (version_bits + index_bits), "Invalid handle layout. More bits used than base value type can hold!");

            using value_type = handle_value_type;

            static constexpr size_t NUM_VERSION_BITS{version_bits};
            static constexpr size_t NUM_INDEX_BITS{index_bits};

            static constexpr value_type MIN_VERSION{0};
            static constexpr value_type MAX_VERSION{(value_type(1) << NUM_VERSION_BITS) - value_type(2)};
            static constexpr value_type MAX_INDICES{(value_type(1) << NUM_INDEX_BITS) - value_type(2)};

            static constexpr value_type INVALID_HANDLE{std::numeric_limits<value_type>::max()};

        private:
            value_type value;

        public:
            struct {
                value_type index: NUM_INDEX_BITS;
                value_type version: NUM_VERSION_BITS;
            };

            Handle() = default;

            Handle(value_type value) :
                    value(value) {}

            Handle(value_type index, value_type version) :
                    index(index), version(version) {}

            inline operator value_type() const { return value; }
        };
    }

    /// Summary:	Defines a 32 bit handle
    /// Max. possible handles							: 1048576
    /// Max. possible versions until loop per handle	: 4096
    using Handle32 = Internal::Handle<u32, 12, 20>;

    /// Summary:	Defines a 64 bit handle
    /// Max. possible handles							: 1099511627776
    /// Max. possible versions until loop per handle	: 16777216
#ifdef _64BIT
    using Handle64 = Internal::Handle<u64, 24, 40>;
#else
    using Handle64 = Handle32;
#endif

    template<class T, class handle_type, size_t grow = 1024>
    class HandleTable {
        using Handle = handle_type;
        using TableEntry = std::pair<typename Handle::value_type, T *>;
        std::vector<TableEntry> table;

        void GrowTable()
        {
            size_t oldSize = this->table.size();

            assert(oldSize < Handle::MAX_INDICES && "Max table capacity reached!");

            size_t newSize = std::min(oldSize + grow, (size_t)Handle::MAX_INDICES);
            this->table.resize(newSize);

            for (typename Handle::value_type i = oldSize; i < newSize; ++i)
                this->table[i] = TableEntry(Handle::MIN_VERSION, nullptr);
        }

    public:
        HandleTable()
        {
            this->GrowTable();
        }

        ~HandleTable() = default;

        Handle AcquireHandle(T *rawObject)
        {
            typename Handle::value_type i = 0;
            for (; i < this->table.size(); ++i)
            {
                if (this->table[i].second == nullptr)
                {
                    this->table[i].second = rawObject;
                    this->table[i].first = ((this->table[i].first + 1) > Handle::MAX_VERSION) ? Handle::MIN_VERSION : this->table[i].first + 1;
                    return Handle(i, this->table[i].first);
                }
            }

            this->GrowTable();

            this->table[i].first = 1;
            this->table[i].second = rawObject;

            return Handle(i, this->table[i].first);
        }

        void ReleaseHandle(Handle handle)
        {
            assert((handle.index < this->table.size() && handle.version == this->table[handle.index].first) && "Invalid handle!");
            this->table[handle.index].second = nullptr;
        }

        inline bool IsExpired(Handle handle) const
        {
            return this->table[handle.index].first != handle.version;
        }

        inline Handle operator[](typename Handle::value_type index) const
        {
            assert(index < this->table.size() && "Invalid handle!");
            return Handle(index, this->table[index].first);
        }

        inline T *operator[](Handle handle)
        {
            return (handle.index < this->table.size() && this->table[handle.index].first == handle.version ? this->table[handle.index].second : nullptr);
        }
    };
}

#pragma warning(pop)

#endif 
