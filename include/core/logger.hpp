#ifndef VAPI_CORE_LOGGER_HPP
#define VAPI_CORE_LOGGER_HPP

/** @file logger.hpp
 *  @brief Logging API: ConsoleSink, FileSink, Logger, log levels and macros.
 *  Define VAPI_LOG_DETAIL to include device name (hostname + MAC), function entry point (file:line function),
 *  and call-site execution address in every log line. */

#include "core/types.hpp"
#include "core/interfaces/i_log.hpp"
#include <cstdint>
#include <cstdio>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <source_location>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <deque>

namespace vapi {

class ConsoleSink : public ILogSink {
public:
    void write(const LogEntry& entry) override;
    void flush() override;
    void setColorEnabled(bool enabled) { m_colorEnabled = enabled; }
private:
    bool m_colorEnabled{true};
    std::mutex m_mutex;
};

class FileSink : public ILogSink {
public:
    explicit FileSink(std::string_view filepath);
    ~FileSink() override;
    void write(const LogEntry& entry) override;
    void flush() override;
private:
    std::string m_filepath;
    std::FILE* m_file{nullptr};
    std::mutex m_mutex;
};

/** Stores the last N log entries in a ring buffer for debugging/inspection. */
class RingBufferSink : public ILogSink {
public:
    explicit RingBufferSink(std::size_t capacity = 256);
    void write(const LogEntry& entry) override;
    void flush() override;
    /** Returns a copy of the last N entries (oldest first). */
    std::vector<LogEntry> getEntries() const;
    std::size_t capacity() const { return m_capacity; }
    std::size_t size() const;
private:
    std::size_t m_capacity;
    std::deque<LogEntry> m_ring;
    mutable std::mutex m_mutex;
};

class Logger {
public:
    static Logger& instance();

    void setMinLevel(LogLevel level) { m_minLevel = level; }
    void setMinLevel(std::string_view category, LogLevel level);
    void addSink(std::unique_ptr<ILogSink> sink);
    void clearSinks();

    void log(LogLevel level, std::string_view category, std::string_view message,
             std::source_location loc = std::source_location::current(),
             void* callSiteAddress = nullptr);

    [[nodiscard]] bool shouldLog(LogLevel level, std::string_view category) const;

private:
    Logger();
    LogLevel m_minLevel{LogLevel::Debug};
    std::unordered_map<std::string, LogLevel> m_categoryLevels;
    std::vector<std::unique_ptr<ILogSink>> m_sinks;
    mutable std::mutex m_mutex;
};

inline void logDebug(std::string_view cat, std::string_view msg,
                     std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Debug, cat, msg, loc);
}
inline void logInfo(std::string_view cat, std::string_view msg,
                    std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Info, cat, msg, loc);
}
inline void logWarn(std::string_view cat, std::string_view msg,
                   std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Warning, cat, msg, loc);
}
inline void logError(std::string_view cat, std::string_view msg,
                    std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Error, cat, msg, loc);
}
inline void logFatal(std::string_view cat, std::string_view msg,
                    std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Fatal, cat, msg, loc);
}

namespace detail {
    inline std::string formatOnePlaceholder(std::string_view fmt, std::string argStr) {
        const std::string placeholder("{}");
        auto pos = fmt.find(placeholder);
        if (pos == std::string_view::npos) return std::string(fmt);
        return std::string(fmt.substr(0, pos)) + argStr + std::string(fmt.substr(pos + placeholder.size()));
    }
    template<typename T>
    std::string formatOne(std::string_view fmt, const T& arg) {
        std::string argStr;
        if constexpr (std::is_arithmetic_v<T>) {
            argStr = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            argStr = std::string(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            argStr = arg;
        } else if constexpr (std::is_convertible_v<T, std::string_view>) {
            argStr = std::string(std::string_view(arg));
        } else {
            std::ostringstream os;
            os << arg;
            argStr = os.str();
        }
        return formatOnePlaceholder(fmt, std::move(argStr));
    }
}

template<typename T>
inline void logDebug(std::string_view cat, std::string_view fmt, const T& arg,
                    std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Debug, cat, detail::formatOne(fmt, arg), loc);
}
template<typename T>
inline void logInfo(std::string_view cat, std::string_view fmt, const T& arg,
                   std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Info, cat, detail::formatOne(fmt, arg), loc);
}
template<typename T>
inline void logWarn(std::string_view cat, std::string_view fmt, const T& arg,
                   std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Warning, cat, detail::formatOne(fmt, arg), loc);
}
template<typename T>
inline void logError(std::string_view cat, std::string_view fmt, const T& arg,
                    std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Error, cat, detail::formatOne(fmt, arg), loc);
}
template<typename T>
inline void logFatal(std::string_view cat, std::string_view fmt, const T& arg,
                    std::source_location loc = std::source_location::current()) {
    Logger::instance().log(LogLevel::Fatal, cat, detail::formatOne(fmt, arg), loc);
}

#ifdef VAPI_LOG_DETAIL
/** With VAPI_LOG_DETAIL, these macros pass the call-site address so the logger can print execution address. */
# define logDebug(cat, msg)   vapi::Logger::instance().log(vapi::LogLevel::Debug,   (cat), (msg), std::source_location::current(), __builtin_return_address(0))
# define logInfo(cat, msg)    vapi::Logger::instance().log(vapi::LogLevel::Info,    (cat), (msg), std::source_location::current(), __builtin_return_address(0))
# define logWarn(cat, msg)    vapi::Logger::instance().log(vapi::LogLevel::Warning, (cat), (msg), std::source_location::current(), __builtin_return_address(0))
# define logError(cat, msg)   vapi::Logger::instance().log(vapi::LogLevel::Error,   (cat), (msg), std::source_location::current(), __builtin_return_address(0))
# define logFatal(cat, msg)   vapi::Logger::instance().log(vapi::LogLevel::Fatal,   (cat), (msg), std::source_location::current(), __builtin_return_address(0))
# if defined(_MSC_VER) && !defined(__clang__)
#   undef logDebug
#   undef logInfo
#   undef logWarn
#   undef logError
#   undef logFatal
#   define logDebug(cat, msg)   vapi::Logger::instance().log(vapi::LogLevel::Debug,   (cat), (msg), std::source_location::current(), _ReturnAddress())
#   define logInfo(cat, msg)    vapi::Logger::instance().log(vapi::LogLevel::Info,    (cat), (msg), std::source_location::current(), _ReturnAddress())
#   define logWarn(cat, msg)    vapi::Logger::instance().log(vapi::LogLevel::Warning, (cat), (msg), std::source_location::current(), _ReturnAddress())
#   define logError(cat, msg)   vapi::Logger::instance().log(vapi::LogLevel::Error,   (cat), (msg), std::source_location::current(), _ReturnAddress())
#   define logFatal(cat, msg)   vapi::Logger::instance().log(vapi::LogLevel::Fatal,   (cat), (msg), std::source_location::current(), _ReturnAddress())
# endif
#endif

} // namespace vapi

#endif
