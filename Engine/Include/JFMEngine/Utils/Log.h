#pragma once

#include <string>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <atomic>

namespace JFM {

    // 日志级别
    enum class LogLevel {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        CRITICAL = 5
    };

    // 日志消息结构
    struct LogMessage {
        LogLevel level;
        std::string logger_name;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        std::thread::id thread_id;

        LogMessage(LogLevel lvl, std::string name, std::string msg)
            : level(lvl), logger_name(std::move(name)), message(std::move(msg)),
              timestamp(std::chrono::system_clock::now()),
              thread_id(std::this_thread::get_id()) {}
    };

    // 日志输出接口
    class LogSink {
    public:
        virtual ~LogSink() = default;
        virtual void log(const LogMessage& msg) = 0;
        virtual void flush() {}

        void set_level(LogLevel level) { level_ = level; }
        [[nodiscard]] LogLevel get_level() const { return level_; }

    protected:
        [[nodiscard]] bool should_log(LogLevel level) const { return level >= level_; }

    private:
        LogLevel level_ = LogLevel::TRACE;
    };

    // 控制台输出
    class ConsoleSink : public LogSink {
    public:
        explicit ConsoleSink(bool use_colors = true) : use_colors_(use_colors) {}
        void log(const LogMessage& msg) override;

    private:
        [[nodiscard]] static std::string get_color_code(LogLevel level);
        [[nodiscard]] static std::string get_level_string(LogLevel level);
        bool use_colors_;
        std::mutex mutex_;
    };

    // 文件输出
    class FileSink : public LogSink {
    public:
        explicit FileSink(const std::string& filename);
        ~FileSink() override;
        void log(const LogMessage& msg) override;
        void flush() override;

    private:
        [[nodiscard]] static std::string get_level_string(LogLevel level);
        std::ofstream file_;
        std::mutex mutex_;
    };

    // 轮转文件输出
    class RotatingFileSink : public LogSink {
    public:
        RotatingFileSink(const std::string& base_filename, size_t max_size, size_t max_files);
        ~RotatingFileSink() override;
        void log(const LogMessage& msg) override;
        void flush() override;

    private:
        void rotate_file();
        [[nodiscard]] static std::string get_level_string(LogLevel level);

        std::string base_filename_;
        size_t max_size_;
        size_t max_files_;
        size_t current_size_;
        std::ofstream file_;
        std::mutex mutex_;
    };

    // 格式化辅助函数声明
    template<typename T, typename... Args>
    void format_recursive(std::ostringstream& oss, const std::string& fmt, size_t& pos, size_t& arg_index, T&& value, Args&&... args);

    void format_recursive(std::ostringstream& oss, const std::string& fmt, size_t& pos, size_t& arg_index);

    // 格式化字符串函数
    template<typename... Args>
    std::string format_string(const std::string& fmt, Args&&... args) {
        std::ostringstream oss;
        size_t pos = 0;
        size_t arg_index = 0;
        format_recursive(oss, fmt, pos, arg_index, std::forward<Args>(args)...);
        return oss.str();
    }

    // 异步日志器
    class AsyncLogger {
    public:
        explicit AsyncLogger(const std::string& name);
        ~AsyncLogger();

        void add_sink(const std::shared_ptr<LogSink>& sink);
        void set_level(LogLevel level) { level_ = level; }
        [[nodiscard]] LogLevel get_level() const { return level_; }

        void log(LogLevel level, const std::string& message);
        void flush();

        // 便捷方法
        void trace(const std::string& message);
        void debug(const std::string& message);
        void info(const std::string& message);
        void warn(const std::string& message);
        void error(const std::string& message);
        void critical(const std::string& message);

        // 模板方法用于格式化参数
        template<typename... Args>
        void trace(const std::string& fmt, Args&&... args);

        template<typename... Args>
        void debug(const std::string& fmt, Args&&... args);

        template<typename... Args>
        void info(const std::string& fmt, Args&&... args);

        template<typename... Args>
        void warn(const std::string& fmt, Args&&... args);

        template<typename... Args>
        void error(const std::string& fmt, Args&&... args);

        template<typename... Args>
        void critical(const std::string& fmt, Args&&... args);

    private:
        [[nodiscard]] bool should_log(LogLevel level) const { return level >= level_; }
        void worker_function();

        std::string name_;
        LogLevel level_;
        std::vector<std::shared_ptr<LogSink>> sinks_;

        std::queue<LogMessage> message_queue_;
        std::mutex queue_mutex_;
        std::condition_variable queue_cv_;
        std::thread worker_thread_;
        std::atomic<bool> should_stop_;
    };

    // 主日志类
    class Log {
    public:
        static void Initialize();
        static void Shutdown();

        static std::shared_ptr<AsyncLogger>& GetCoreLogger() { return s_CoreLogger; }
        static std::shared_ptr<AsyncLogger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<AsyncLogger> s_CoreLogger;
        static std::shared_ptr<AsyncLogger> s_ClientLogger;
        static bool s_Initialized;
    };

    // 格式化辅助函数实现
    template<typename T, typename... Args>
    void format_recursive(std::ostringstream& oss, const std::string& fmt, size_t& pos, size_t& arg_index, T&& value, Args&&... args) {
        // 查找下一个占位符 {}
        size_t placeholder_pos = fmt.find("{}", pos);
        if (placeholder_pos != std::string::npos) {
            // 添加占位符之前的文本
            oss << fmt.substr(pos, placeholder_pos - pos);
            // 添加参数值
            oss << value;
            // 更新位置
            pos = placeholder_pos + 2;
            arg_index++;
            // 递归处理剩余参数
            format_recursive(oss, fmt, pos, arg_index, std::forward<Args>(args)...);
        } else {
            // 没有更多占位符，添加剩余文本
            oss << fmt.substr(pos);
        }
    }

    // 模板实现
    template<typename... Args>
    void AsyncLogger::trace(const std::string& fmt, Args&&... args) {
        if (should_log(LogLevel::TRACE)) {
            log(LogLevel::TRACE, format_string(fmt, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void AsyncLogger::debug(const std::string& fmt, Args&&... args) {
        if (should_log(LogLevel::DEBUG)) {
            log(LogLevel::DEBUG, format_string(fmt, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void AsyncLogger::info(const std::string& fmt, Args&&... args) {
        if (should_log(LogLevel::INFO)) {
            log(LogLevel::INFO, format_string(fmt, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void AsyncLogger::warn(const std::string& fmt, Args&&... args) {
        if (should_log(LogLevel::WARN)) {
            log(LogLevel::WARN, format_string(fmt, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void AsyncLogger::error(const std::string& fmt, Args&&... args) {
        if (should_log(LogLevel::ERROR)) {
            log(LogLevel::ERROR, format_string(fmt, std::forward<Args>(args)...));
        }
    }

    template<typename... Args>
    void AsyncLogger::critical(const std::string& fmt, Args&&... args) {
        if (should_log(LogLevel::CRITICAL)) {
            log(LogLevel::CRITICAL, format_string(fmt, std::forward<Args>(args)...));
        }
    }

} // namespace JFM

// 便捷宏定义
#define JFM_CORE_TRACE(...)    ::JFM::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define JFM_CORE_DEBUG(...)    ::JFM::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define JFM_CORE_INFO(...)     ::JFM::Log::GetCoreLogger()->info(__VA_ARGS__)
#define JFM_CORE_WARN(...)     ::JFM::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define JFM_CORE_ERROR(...)    ::JFM::Log::GetCoreLogger()->error(__VA_ARGS__)
#define JFM_CORE_CRITICAL(...) ::JFM::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define JFM_TRACE(...)         ::JFM::Log::GetClientLogger()->trace(__VA_ARGS__)
#ifndef JFM_DEBUG
#define JFM_DEBUG(...)         ::JFM::Log::GetClientLogger()->debug(__VA_ARGS__)
#endif
#define JFM_INFO(...)          ::JFM::Log::GetClientLogger()->info(__VA_ARGS__)
#define JFM_WARN(...)          ::JFM::Log::GetClientLogger()->warn(__VA_ARGS__)
#define JFM_ERROR(...)         ::JFM::Log::GetClientLogger()->error(__VA_ARGS__)
#define JFM_CRITICAL(...)      ::JFM::Log::GetClientLogger()->critical(__VA_ARGS__)
