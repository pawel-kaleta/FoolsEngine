#pragma once

#include "FoolsEngine/Foundation/DataTypes.h"

namespace fe
{
	//FE_DECLARE_ENUM(LoggingLevel, Trace, Debug, Info, Warn, Error, Fatal, None)
    struct LoggingLevel {
        enum ValueType {
            Trace, Debug, Info, Warn, Error, Fatal, None
        } Value; constexpr operator const ValueType()& {
            return Value;
        } constexpr bool operator==(const LoggingLevel& other) const {
            return this->Value == other.Value;
        } constexpr bool operator!=(const LoggingLevel& other) const {
            return this->Value != other.Value;
        } constexpr bool operator==(const ValueType& other) const {
            return this->Value == other;
        } constexpr bool operator!=(const ValueType& other) const {
            return this->Value != other;
        } constexpr LoggingLevel& operator=(const LoggingLevel& other) {
            this->Value = other.Value; return *this;
        } constexpr LoggingLevel& operator=(const ValueType& other) {
            this->Value = other; return *this;
        } constexpr LoggingLevel& operator=(LoggingLevel&& other) noexcept {
            this->Value = other.Value; return *this;
        } constexpr LoggingLevel& operator=(ValueType&& other) {
            this->Value = other; return *this;
        } LoggingLevel() = default; constexpr LoggingLevel(const LoggingLevel& other) : Value(other.Value) {
        } constexpr LoggingLevel(LoggingLevel&& other) noexcept : Value(other.Value) {
        } constexpr LoggingLevel(const ValueType& other) : Value(other) {
        } constexpr LoggingLevel(ValueType&& other) : Value(other) {
        }
        /*void FromString(String string) {
            if (CompareStringsEqual(string, String("None", sizeof("None")))) {
                Value = None; return;
            }if (CompareStringsEqual(string, String("Trace", sizeof("Trace")))) {
                Value = Trace; return;
            }if (CompareStringsEqual(string, String("Debug", sizeof("Debug")))) {
                Value = Debug; return;
            }if (CompareStringsEqual(string, String("Info", sizeof("Info")))) {
                Value = Info; return;
            }if (CompareStringsEqual(string, String("Warn", sizeof("Warn")))) {
                Value = Warn; return;
            }if (CompareStringsEqual(string, String("Error", sizeof("Error")))) {
                Value = Error; return;
            }if (CompareStringsEqual(string, String("Fatal", sizeof("Fatal")))) {
                Value = Fatal; return;
            } if (!(false)) {
                fe::Log::GetCoreLogger()->error("CORE ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path("D:\\dev\\FoolsEngine\\FoolsEngine\\FoolsEngine\\src\\FoolsEngine\\Foundation\\Debug\\Logger.h").filename().string(), __builtin_FUNCSIG(), 8); fe::Log::GetCoreLogger()->error("Assertion Message: " "Unrecognised string representation of enum"); if (fe::EnableCoreAssertionBreak) __debugbreak();;
            };
        } CString ToCString() const {
            switch (Value) {
            case None: return CString("None", sizeof("None")); case Trace: return CString("Trace", sizeof("Trace")); case Debug: return CString("Debug", sizeof("Debug")); case Info: return CString("Info", sizeof("Info")); case Warn: return CString("Warn", sizeof("Warn")); case Error: return CString("Error", sizeof("Error")); case Fatal: return CString("Fatal", sizeof("Fatal")); default: if (!(false)) {
                fe::Log::GetCoreLogger()->error("CORE ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path("D:\\dev\\FoolsEngine\\FoolsEngine\\FoolsEngine\\src\\FoolsEngine\\Foundation\\Debug\\Logger.h").filename().string(), __builtin_FUNCSIG(), 8); fe::Log::GetCoreLogger()->error("Assertion Message: " "Unrecognised value of enum"); if (fe::EnableCoreAssertionBreak) __debugbreak();;
            }; return CString();
            }
        }*/
        constexpr void FromInt(fe::UInt x) {
            Value = (ValueType)x;
        } constexpr fe::UInt ToInt() const {
            return (fe::UInt)Value;
        }
    };
}