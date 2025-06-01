#pragma once

#include "FoolsEngine\Debug\Asserts.h"

namespace fe
{
#define _EXPAND(x) x

#define _FE_0(WHAT)
#define _FE_1(WHAT, X) WHAT(X) 

#define _FE_2(WHAT, X, ...) WHAT(X)_FE_1(WHAT, __VA_ARGS__)
#define _FE_2_(...) _EXPAND( _FE_2(__VA_ARGS__) )

#define _FE_3(WHAT, X, ...) WHAT(X)_FE_2_(WHAT, __VA_ARGS__)
#define _FE_3_(...) _EXPAND( _FE_3(__VA_ARGS__) )

#define _FE_4(WHAT, X, ...) WHAT(X)_FE_3_(WHAT, __VA_ARGS__)
#define _FE_4_(...) _EXPAND( _FE_4(__VA_ARGS__) )

#define _FE_5(WHAT, X, ...) WHAT(X)_FE_4_(WHAT, __VA_ARGS__)
#define _FE_5_(...) _EXPAND( _FE_5(__VA_ARGS__) )

#define _FE_6(WHAT, X, ...) WHAT(X)_FE_5_(WHAT, __VA_ARGS__)
#define _FE_6_(...) _EXPAND( _FE_6(__VA_ARGS__) )

#define _FE_7(WHAT, X, ...) WHAT(X)_FE_6_(WHAT, __VA_ARGS__)
#define _FE_7_(...) _EXPAND( _FE_7(__VA_ARGS__) )

#define _FE_8(WHAT, X, ...) WHAT(X)_FE_7_(WHAT, __VA_ARGS__)
#define _FE_8_(...) _EXPAND( _FE_8(__VA_ARGS__) )

#define _FE_9(WHAT, X, ...) WHAT(X)_FE_8_(WHAT, __VA_ARGS__)
#define _FE_9_(...) _EXPAND( _FE_9(__VA_ARGS__) )

#define _FE_10(WHAT, X, ...) WHAT(X)_FE_9_(WHAT, __VA_ARGS__)
#define _FE_10_(...) _EXPAND( _FE_10(__VA_ARGS__) )

#define _FE_11(WHAT, X, ...) WHAT(X)_FE_10_(WHAT, __VA_ARGS__)
#define _FE_11_(...) _EXPAND( _FE_11(__VA_ARGS__) )

#define _FE_12(WHAT, X, ...) WHAT(X)_FE_11_(WHAT, __VA_ARGS__)
#define _FE_12_(...) _EXPAND( _FE_12(__VA_ARGS__) )

#define _FE_13(WHAT, X, ...) WHAT(X)_FE_12_(WHAT, __VA_ARGS__)
#define _FE_13_(...) _EXPAND( _FE_13(__VA_ARGS__) )

#define _FE_14(WHAT, X, ...) WHAT(X)_FE_13_(WHAT, __VA_ARGS__)
#define _FE_14_(...) _EXPAND( _FE_14(__VA_ARGS__) )

#define _FE_15(WHAT, X, ...) WHAT(X)_FE_14_(WHAT, __VA_ARGS__)
#define _FE_15_(...) _EXPAND( _FE_15(__VA_ARGS__) )

#define _FE_16(WHAT, X, ...) WHAT(X)_FE_15_(WHAT, __VA_ARGS__)
#define _FE_16_(...) _EXPAND( _FE_16(__VA_ARGS__) )

#define _FE_17(WHAT, X, ...) WHAT(X)_FE_16_(WHAT, __VA_ARGS__)
#define _FE_17_(...) _EXPAND( _FE_17(__VA_ARGS__) )

#define _FE_18(WHAT, X, ...) WHAT(X)_FE_17_(WHAT, __VA_ARGS__)
#define _FE_18_(...) _EXPAND( _FE_18(__VA_ARGS__) )

#define _FE_19(WHAT, X, ...) WHAT(X)_FE_18_(WHAT, __VA_ARGS__)
#define _FE_19_(...) _EXPAND( _FE_19(__VA_ARGS__) )

#define _FE_20(WHAT, X, ...) WHAT(X)_FE_19_(WHAT, __VA_ARGS__)
#define _FE_20_(...) _EXPAND( _FE_20(__VA_ARGS__) )

#define _FE_21(WHAT, X, ...) WHAT(X)_FE_20_(WHAT, __VA_ARGS__)
#define _FE_21_(...) _EXPAND( _FE_21(__VA_ARGS__) )

#define _FE_22(WHAT, X, ...) WHAT(X)_FE_21_(WHAT, __VA_ARGS__)
#define _FE_22_(...) _EXPAND( _FE_22(__VA_ARGS__) )

#define _FE_23(WHAT, X, ...) WHAT(X)_FE_22_(WHAT, __VA_ARGS__)
#define _FE_23_(...) _EXPAND( _FE_23(__VA_ARGS__) )

#define _FE_24(WHAT, X, ...) WHAT(X)_FE_23_(WHAT, __VA_ARGS__)
#define _FE_24_(...) _EXPAND( _FE_24(__VA_ARGS__) )

#define _FE_25(WHAT, X, ...) WHAT(X)_FE_24_(WHAT, __VA_ARGS__)
#define _FE_25_(...) _EXPAND( _FE_25(__VA_ARGS__) )

#define _FE_26(WHAT, X, ...) WHAT(X)_FE_25_(WHAT, __VA_ARGS__)
#define _FE_26_(...) _EXPAND( _FE_26(__VA_ARGS__) )

#define _FE_27(WHAT, X, ...) WHAT(X)_FE_26_(WHAT, __VA_ARGS__)
#define _FE_27_(...) _EXPAND( _FE_27(__VA_ARGS__) )

#define _FE_28(WHAT, X, ...) WHAT(X)_FE_27_(WHAT, __VA_ARGS__)
#define _FE_28_(...) _EXPAND( _FE_28(__VA_ARGS__) )

#define _FE_29(WHAT, X, ...) WHAT(X)_FE_28_(WHAT, __VA_ARGS__)
#define _FE_29_(...) _EXPAND( _FE_29(__VA_ARGS__) )

#define _FE_30(WHAT, X, ...) WHAT(X)_FE_29_(WHAT, __VA_ARGS__)
#define _FE_30_(...) _EXPAND( _FE_30(__VA_ARGS__) )

#define _FE_31(WHAT, X, ...) WHAT(X)_FE_30_(WHAT, __VA_ARGS__)
#define _FE_31_(...) _EXPAND( _FE_31(__VA_ARGS__) )

#define _FE_32(WHAT, X, ...) WHAT(X)_FE_31_(WHAT, __VA_ARGS__)
#define _FE_32_(...) _EXPAND( _FE_32(__VA_ARGS__) )


#define _GET_MACRO(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,NAME,...) NAME 
#define _FOR_EACH(action,...) \
  _EXPAND(_GET_MACRO(_0,__VA_ARGS__,_FE_32_,_FE_31_,_FE_30_,_FE_29_,_FE_28_,_FE_27_,_FE_26_,_FE_25_,_FE_24_,_FE_23_,_FE_22_,_FE_21_,_FE_20_,_FE_19,_FE_18_,_FE_17_,_FE_16_,_FE_15_,_FE_14_,_FE_13_,_FE_12_,_FE_11_,_FE_10_,_FE_9_,_FE_8_,_FE_7_,_FE_6_,_FE_5_,_FE_4_,_FE_3_,_FE_2_,_FE_1,_FE_0))(action,__VA_ARGS__)



#define _FROM_STRING(x) if (string == #x) Value = x;
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
    constexpr name& operator=(name&& other) { this->Value = other.Value; return *this; }              \
    constexpr name& operator=(ValueType&& other) { this->Value = other; return *this; }               \
                                                                                                      \
    name() = default;                                                                                 \
    constexpr name(const name& other) : Value(other.Value) { }                                        \
    constexpr name(name&& other) : Value(other.Value) { }                                             \
    constexpr name(const ValueType& other) : Value(other) { }                                         \
    constexpr name(ValueType&& other) : Value(other) { }                                              \
                                                                                                      \
    void FromString(const std::string& string)                                                        \
    {                                                                                                 \
        _FOR_EACH(_FROM_STRING, __VA_ARGS__)                                                          \
    }                                                                                                 \
                                                                                                      \
    std::string ToString() const                                                                      \
    {                                                                                                 \
        switch (Value)                                                                                \
        {                                                                                             \
        _FOR_EACH(_TO_STRING, __VA_ARGS__)                                                            \
        default: return "";                                                                           \
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
};                                                                                                    \

} // namespace fe