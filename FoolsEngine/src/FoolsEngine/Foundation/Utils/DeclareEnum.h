#pragma once

#include "ForEach.h"

#include "FoolsEngine/Foundation/Debug/Asserts.h"

#define _FROM_STRING(x) if (string == #x) { Value = x; return; }
#define _TO_STRING(x) case x: return #x;

#define FE_DECLARE_ENUM(name, ...)                                                                    \
struct name                                                                                           \
{                                                                                                     \
    enum ValueType                                                                                    \
    {                                                                                                 \
        __VA_ARGS__                                                                                   \
    } Value;                                                                                          \
                                                                                                      \
    constexpr operator const ValueType()& { return Value; }                                           \
    constexpr bool operator==(const name& other) const { return this->Value == other.Value; }         \
    constexpr bool operator!=(const name& other) const { return this->Value != other.Value; }         \
                                                                                                      \
    constexpr bool operator==(const ValueType& other) const { return this->Value == other; }          \
    constexpr bool operator!=(const ValueType& other) const { return this->Value != other; }          \
                                                                                                      \
    constexpr name& operator=(const name& other) { this->Value = other.Value; return *this; }         \
    constexpr name& operator=(const ValueType& other) { this->Value = other; return *this; }          \
                                                                                                      \
    constexpr name& operator=(name&& other) noexcept { this->Value = other.Value; return *this; }     \
    constexpr name& operator=(ValueType&& other) { this->Value = other; return *this; }               \
                                                                                                      \
    name() = default;                                                                                 \
    constexpr name(const name& other) : Value(other.Value) { }                                        \
    constexpr name(name&& other) noexcept : Value(other.Value) { }                                    \
    constexpr name(const ValueType& other) : Value(other) { }                                         \
    constexpr name(ValueType&& other) : Value(other) { }                                              \
                                                                                                      \
    void FromString(const std::string& string)                                                        \
    {                                                                                                 \
        FE_FOR_EACH(_FROM_STRING, __VA_ARGS__)                                                        \
        FE_CORE_ASSERT(false, "Unrecognised string representation of enum");                          \
    }                                                                                                 \
                                                                                                      \
    constexpr const char* ToConstCharPtr() const                                                      \
    {                                                                                                 \
        switch (Value)                                                                                \
        {                                                                                             \
        FE_FOR_EACH(_TO_STRING, __VA_ARGS__)                                                          \
        default:                                                                                      \
            FE_CORE_ASSERT(false, "Unrecognised value of enum");                                      \
            return "";                                                                                \
        }                                                                                             \
    }                                                                                                 \
                                                                                                      \
    constexpr void FromInt(int x)                                                                     \
    {                                                                                                 \
        Value = (ValueType)x;                                                                         \
    }                                                                                                 \
                                                                                                      \
    constexpr int ToInt() const                                                                       \
    {                                                                                                 \
        return (int)Value;                                                                            \
    }                                                                                                 \
};