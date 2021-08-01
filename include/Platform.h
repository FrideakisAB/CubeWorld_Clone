#ifndef PLATFORM_H
#define PLATFORM_H

#if (defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_X64))
#define _64BIT 1

#elif (defined(_WIN32) && !defined(_WIN64)) || defined(_M_IX86)
#define _32BIT 1
#endif

#include <cmath>
#include <limits>
#include <cstdlib>
#include <cstdarg>
#include <cstdint>
#include <cassert>

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;

#ifdef _64BIT
using i64 = int64_t;
#else
using i64 = int32_t;
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
#ifdef _64BIT
using u64 = uint64_t;
#else
using u64 = uint32_t;
#endif

using f32 = float_t;
using f64 = double_t;

using iptr = intptr_t;
using uptr = uintptr_t;

using ObjectID = size_t;
using TypeID = size_t;

#undef max
static constexpr const ObjectID INVALID_OBJECT_ID = std::numeric_limits<ObjectID>::max();
static constexpr const TypeID INVALID_TYPE_ID = std::numeric_limits<TypeID>::max();

union TimeStamp {
    f32 asFloat;
    u32 asUInt;

    explicit TimeStamp() noexcept: asUInt(0U) {}

    explicit TimeStamp(f32 floatValue) noexcept: asFloat(floatValue) {}

    explicit operator u32() const noexcept { return this->asUInt; }

    inline bool operator==(const TimeStamp &other) const noexcept { return this->asUInt == other.asUInt; }

    inline bool operator!=(const TimeStamp &other) const noexcept { return this->asUInt != other.asUInt; }

    inline bool operator<(const TimeStamp &other) const noexcept { return this->asFloat < other.asFloat; }

    inline bool operator>(const TimeStamp &other) const noexcept { return this->asFloat > other.asFloat; }
};

enum class ByteOrder {
    WrongEndian = 0,
    LittleEndian,
    PDPEndian,
    BigEndian,
    NetworkEndian = BigEndian,
};

namespace Endian {
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    static constexpr ByteOrder const MachineOrder = ByteOrder::LittleEndian;
#elif defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    static constexpr ByteOrder const MachineOrder = ByteOrder::BigEndian;
#elif defined(__BYTE_ORDER__) && defined(__ORDER_PDP_ENDIAN__) && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
    static constexpr ByteOrder const MachineOrder = ByteOrder::PDPEndian;
#elif defined(__LITTLE_ENDIAN__)
    static constexpr ByteOrder const MachineOrder = ByteOrder::LittleEndian;
#elif defined(__BIG_ENDIAN__)
    static constexpr ByteOrder const MachineOrder = ByteOrder::BigEndian;
#elif defined(__PDP_ENDIAN__)
    static constexpr ByteOrder const MachineOrder = ByteOrder::PDPEndian;
#elif defined(_MSC_VER) || defined(__i386__) || defined(__x86_64__)
    static constexpr ByteOrder const MachineOrder = ByteOrder::LittleEndian;
#else
    static constexpr ByteOrder const MachineOrder = ByteOrder::WrongEndian;
#endif
}

#endif