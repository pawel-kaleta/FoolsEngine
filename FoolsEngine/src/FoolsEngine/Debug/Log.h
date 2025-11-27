#pragma once

//#include "FoolsEngine/Core/Core.h"

// https://github.com/gabime/spdlog/wiki
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/ostr.h>

#include <FoolsEngine\Utils\DeclareEnum.h>

namespace fe {

	class Log
	{
	public:
		static void Init();

        //FE_DECLARE_ENUM expanded in place, due to circular dependency log-enum
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
            } void FromString(const std::string& string) {
                if (string == "Trace") {
                    Value = Trace; return;
                }if (string == "Debug") {
                    Value = Debug; return;
                }if (string == "Info") {
                    Value = Info; return;
                }if (string == "Warn") {
                    Value = Warn; return;
                }if (string == "Error") {
                    Value = Error; return;
                }if (string == "Fatal") {
                    Value = Fatal; return;
                }if (string == "None") {
                    Value = None; return;
                } if (!(false)) {
                    fe::Log::GetCoreLogger()->error("CORE ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path("C:\\FE_DEV\\FoolsEngine\\FoolsEngine\\src\\FoolsEngine\\Debug\\Log.h").filename().string(), __builtin_FUNCSIG(), 21); fe::Log::GetCoreLogger()->error("Assertion Message: " "Unrecognised string representation of enum"); if (fe::Asserts::EnableCoreAssertionBreak) __debugbreak();;
                };
            } constexpr const char* ToConstCharPtr() const {
                switch (Value) {
                case Trace: return "Trace"; case Debug: return "Debug"; case Info: return "Info"; case Warn: return "Warn"; case Error: return "Error"; case Fatal: return "Fatal"; case None: return "None"; default: if (!(false)) {
                    fe::Log::GetCoreLogger()->error("CORE ASSERTION FAILED: in file {0} in function {1} at line {2}.", std::filesystem::path("C:\\FE_DEV\\FoolsEngine\\FoolsEngine\\src\\FoolsEngine\\Debug\\Log.h").filename().string(), __builtin_FUNCSIG(), 21); fe::Log::GetCoreLogger()->error("Assertion Message: " "Unrecognised value of enum"); if (fe::Asserts::EnableCoreAssertionBreak) __debugbreak();;
                }; return "";
                }
            } constexpr void FromInt(int x) {
                Value = (ValueType)x;
            } constexpr int ToInt() const {
                return (int)Value;
            }
        };

		static std::shared_ptr<spdlog::logger> GetCoreLogger()   { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger> GetClientLogger() { return s_ClientLogger; }

		static void SetCoreLoggingLevel(LoggingLevel lvl);
		static void SetClientLoggingLevel(LoggingLevel lvl);

		static LoggingLevel GetCoreLoggingLevel();
		static LoggingLevel GetClientLoggingLevel();
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;

		Log();
	};
}

// Macros for logging core engine issues
#define FE_LOG_CORE_FATAL(...) fe::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define FE_LOG_CORE_ERROR(...) fe::Log::GetCoreLogger()->error   (__VA_ARGS__)
#define FE_LOG_CORE_WARN(...)  fe::Log::GetCoreLogger()->warn    (__VA_ARGS__)
#define FE_LOG_CORE_INFO(...)  fe::Log::GetCoreLogger()->info    (__VA_ARGS__)
#define FE_LOG_CORE_DEBUG(...) fe::Log::GetCoreLogger()->debug   (__VA_ARGS__)
#define FE_LOG_CORE_TRACE(...) fe::Log::GetCoreLogger()->trace   (__VA_ARGS__)

// Macros for logging client app issues
#define FE_LOG_FATAL(...) fe::Log::GetClientLogger()->critical(__VA_ARGS__)
#define FE_LOG_ERROR(...) fe::Log::GetClientLogger()->error   (__VA_ARGS__)
#define FE_LOG_WARN(...)  fe::Log::GetClientLogger()->warn    (__VA_ARGS__)
#define FE_LOG_INFO(...)  fe::Log::GetClientLogger()->info    (__VA_ARGS__)
#define FE_LOG_DEBUG(...) fe::Log::GetClientLogger()->debug   (__VA_ARGS__)
#define FE_LOG_TRACE(...) fe::Log::GetClientLogger()->trace   (__VA_ARGS__)
