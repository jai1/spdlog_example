#ifndef SPDLOG_EXAMPLE_LOGREGISTRY_H
#define SPDLOG_EXAMPLE_LOGREGISTRY_H

#include <map>
#include <spdlog/spdlog.h>
#include <libgen.h>

class LoggerRegistry {
public:
    using Map = std::map<std::string, std::atomic<spdlog::level::level_enum> >;
private:
    const size_t MAX_FILE_SIZE_BYTES = 1 * 1024 * 1024 * 1024; // 1 GB
    const size_t MAX_NUM_FILES = 10;
    const size_t QUEUE_SIZE = std::pow(2, 20);
    const std::string APP_FILE_NAME = "./application.log";
    const std::string APP_PATTERN_STRING = "%x %T [%l] [%t] %v";

    const std::string ACCESS_FILE_NAME = "./access.log";
    const std::string ACCESS_PATTERN_STRING = "%x %T [%t] - %v";

    std::shared_ptr<spdlog::logger> appLogger_;
    std::shared_ptr<spdlog::logger> accessLogger_;

    Map loggers_;
    std::mutex mtx_;

    explicit LoggerRegistry() {
        static_assert(spdlog::level::trace < spdlog::level::debug);
        static_assert(spdlog::level::debug < spdlog::level::info);
        static_assert(spdlog::level::info < spdlog::level::warn);
        static_assert(spdlog::level::warn < spdlog::level::err);
        static_assert(spdlog::level::err < spdlog::level::critical);
        static_assert(spdlog::level::critical < spdlog::level::off);

        spdlog::set_async_mode(QUEUE_SIZE);

        appLogger_ = spdlog::rotating_logger_mt("logger", APP_FILE_NAME, MAX_FILE_SIZE_BYTES , MAX_NUM_FILES);
        appLogger_->set_level(spdlog::level::info);
        appLogger_->set_pattern(APP_PATTERN_STRING);

        accessLogger_ = spdlog::rotating_logger_mt("accessLogger", ACCESS_FILE_NAME, MAX_FILE_SIZE_BYTES , MAX_NUM_FILES);
        accessLogger_->set_level(spdlog::level::info);
        accessLogger_->set_pattern(ACCESS_PATTERN_STRING);
    }


public:
    static LoggerRegistry& getInstance() {
        static LoggerRegistry instance;
        return instance;
    }

    void setLogLevel(spdlog::level::level_enum logLevel) {
        appLogger_->set_level(logLevel);
    }

    spdlog::level::level_enum getLogLevel() {
        return appLogger_->level();
    }

    void setLogLevel(std::string& loggerName, spdlog::level::level_enum level) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto iterator = loggers_.find(loggerName);

        if (iterator == loggers_.end()) {
            throw std::runtime_error(loggerName + "LoggerName not registered.");
        }
        iterator->second.store(level, std::memory_order_relaxed);
    }

    spdlog::level::level_enum getLogLevel(std::string& loggerName) {
        std::lock_guard<std::mutex> lock(mtx_);
        auto iterator = loggers_.find(loggerName);

        if (iterator == loggers_.end()) {
            throw std::runtime_error(loggerName + "LoggerName not registered.");
        }
        return iterator->second.load(std::memory_order_relaxed);
    }

    Map::iterator registerLogger(char* loggerName) {
        std::lock_guard<std::mutex> lock(mtx_);
        return loggers_.emplace(loggerName, appLogger_->level()).first;
    }

    spdlog::logger& getAppLogger() {
        return *appLogger_;
    }

    spdlog::logger& getAccessLogger() {
        return *accessLogger_;
    }
};

#define INITIALIZE_LOGGER()  static LoggerRegistry::Map::iterator logLevel = LoggerRegistry::getInstance().registerLogger(basename((char *)__FILE__));

#define ACCESS_LOG(format, ...) {\
    LoggerRegistry::getInstance().getAccessLogger().info(format, ##__VA_ARGS__); \
}

#define TRACE_LOG(format, ...) { \
    LoggerRegistry& registry = LoggerRegistry::getInstance(); \
    if (logLevel->second.load(std::memory_order_relaxed) <= spdlog::level::trace) { \
        registry.getAppLogger().trace("[{}::{}:{}] - " format, logLevel->first , __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
}


#define DEBUG_LOG(format, ...) { \
    LoggerRegistry& registry = LoggerRegistry::getInstance(); \
    if (logLevel->second.load(std::memory_order_relaxed) <= spdlog::level::debug) { \
        registry.getAppLogger().debug("[{}::{}:{}] - " format, logLevel->first , __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
}

#define INFO_LOG(format, ...) { \
    LoggerRegistry& registry = LoggerRegistry::getInstance(); \
    if (logLevel->second.load(std::memory_order_relaxed) <= spdlog::level::info) { \
        registry.getAppLogger().info("[{}::{}:{}] - " format, logLevel->first , __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
}

#define WARN_LOG(format, ...) { \
    LoggerRegistry& registry = LoggerRegistry::getInstance(); \
    if (logLevel->second.load(std::memory_order_relaxed) <= spdlog::level::warn) { \
        registry.getAppLogger().warn("[{}::{}:{}] - " format, logLevel->first , __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
}

#define ERR_LOG(format, ...) { \
    LoggerRegistry& registry = LoggerRegistry::getInstance(); \
    if (logLevel->second.load(std::memory_order_relaxed) <= spdlog::level::err) { \
        registry.getAppLogger().error("[{}::{}:{}] - " format, logLevel->first , __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
}
#define CRITICAL_LOG(format, ...) { \
    LoggerRegistry& registry = LoggerRegistry::getInstance(); \
    if (logLevel->second.load(std::memory_order_relaxed) <= spdlog::level::critical) { \
        registry.getAppLogger().critical("[{}::{}:{}] - " format, logLevel->first , __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
}

// Use this for benchmarking since adding additional data (fileName, functionName and line number) degrades performance
#define BENChMARK_LOG(format, ...) { \
    LoggerRegistry& registry = LoggerRegistry::getInstance(); \
    if (logLevel->second.load(std::memory_order_relaxed) <= spdlog::level::info) { \
        registry.getAppLogger().info(format , ##__VA_ARGS__); \
    } \
}

#endif //SPDLOG_EXAMPLE_LOGREGISTRY_H
