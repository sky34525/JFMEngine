#include "JFMEngine/Utils/Log.h"
#include <filesystem>
#include <iostream>
#include <iomanip>

namespace JFM {

// 静态成员定义
std::shared_ptr<AsyncLogger> Log::s_CoreLogger;
std::shared_ptr<AsyncLogger> Log::s_ClientLogger;
bool Log::s_Initialized = false;

// ========== 格式化辅助函数实现 ==========

void format_recursive(std::ostringstream& oss, const std::string& fmt, size_t& pos, size_t& arg_index) {
    // 基础情况：没有更多参数，添加剩余文本
    oss << fmt.substr(pos);
}

// ========== ConsoleSink 实现 ==========

void ConsoleSink::log(const LogMessage& msg) {
    if (!should_log(msg.level)) return;

    std::lock_guard<std::mutex> lock(mutex_);

    // 获取时间戳
    auto time_t = std::chrono::system_clock::to_time_t(msg.timestamp);
    auto tm = *std::localtime(&time_t);

    std::ostringstream oss;

    if (use_colors_) {
        oss << get_color_code(msg.level);
    }

    // 格式: [时间] [级别] [日志器]: 消息
    oss << "[" << std::put_time(&tm, "%H:%M:%S") << "] "
        << "[" << get_level_string(msg.level) << "] "
        << "[" << msg.logger_name << "]: "
        << msg.message;

    if (use_colors_) {
        oss << "\033[0m"; // 重置颜色
    }

    oss << std::endl;

    if (msg.level >= LogLevel::ERROR) {
        std::cerr << oss.str();
    } else {
        std::cout << oss.str();
    }
}

std::string ConsoleSink::get_color_code(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:    return "\033[37m";  // 白色
        case LogLevel::DEBUG:    return "\033[36m";  // 青色
        case LogLevel::INFO:     return "\033[32m";  // 绿色
        case LogLevel::WARN:     return "\033[33m";  // 黄色
        case LogLevel::ERROR:    return "\033[31m";  // 红色
        case LogLevel::CRITICAL: return "\033[35m";  // 紫色
        default:                 return "\033[0m";   // 默认
    }
}

std::string ConsoleSink::get_level_string(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:    return "TRACE";
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO ";
        case LogLevel::WARN:     return "WARN ";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT ";
        default:                 return "UNKN ";
    }
}

// ========== FileSink 实现 ==========

FileSink::FileSink(const std::string& filename) {
    // 确保目录存在
    std::filesystem::path filepath(filename);
    if (filepath.has_parent_path()) {
        std::filesystem::create_directories(filepath.parent_path());
    }

    file_.open(filename, std::ios::app);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
}

FileSink::~FileSink() {
    if (file_.is_open()) {
        file_.close();
    }
}

void FileSink::log(const LogMessage& msg) {
    if (!should_log(msg.level)) return;

    std::lock_guard<std::mutex> lock(mutex_);

    if (!file_.is_open()) return;

    // 获取时间戳
    auto time_t = std::chrono::system_clock::to_time_t(msg.timestamp);
    auto tm = *std::localtime(&time_t);

    // 格式: [时间] [级别] [线程ID] [日志器]: 消息
    file_ << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
          << "[" << get_level_string(msg.level) << "] "
          << "[" << msg.thread_id << "] "
          << "[" << msg.logger_name << "]: "
          << msg.message << std::endl;
}

void FileSink::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open()) {
        file_.flush();
    }
}

std::string FileSink::get_level_string(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:    return "TRACE";
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO ";
        case LogLevel::WARN:     return "WARN ";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT ";
        default:                 return "UNKN ";
    }
}

// ========== RotatingFileSink 实现 ==========

RotatingFileSink::RotatingFileSink(const std::string& base_filename, size_t max_size, size_t max_files)
    : base_filename_(base_filename), max_size_(max_size), max_files_(max_files), current_size_(0) {

    // 确保目录存在
    std::filesystem::path filepath(base_filename);
    if (filepath.has_parent_path()) {
        std::filesystem::create_directories(filepath.parent_path());
    }

    file_.open(base_filename_, std::ios::app);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open rotating log file: " + base_filename_);
    }

    // 获取当前文件大小
    file_.seekp(0, std::ios::end);
    current_size_ = file_.tellp();
}

RotatingFileSink::~RotatingFileSink() {
    if (file_.is_open()) {
        file_.close();
    }
}

void RotatingFileSink::log(const LogMessage& msg) {
    if (!should_log(msg.level)) return;

    std::lock_guard<std::mutex> lock(mutex_);

    if (!file_.is_open()) return;

    // 获取时间戳
    auto time_t = std::chrono::system_clock::to_time_t(msg.timestamp);
    auto tm = *std::localtime(&time_t);

    std::ostringstream oss;
    oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
        << "[" << get_level_string(msg.level) << "] "
        << "[" << msg.thread_id << "] "
        << "[" << msg.logger_name << "]: "
        << msg.message << std::endl;

    std::string log_line = oss.str();

    // 检查是否需要轮转
    if (current_size_ + log_line.size() > max_size_) {
        rotate_file();
    }

    file_ << log_line;
    current_size_ += log_line.size();
}

void RotatingFileSink::flush() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (file_.is_open()) {
        file_.flush();
    }
}

void RotatingFileSink::rotate_file() {
    if (file_.is_open()) {
        file_.close();
    }

    // 轮转文件
    for (size_t i = max_files_ - 1; i > 0; --i) {
        std::string old_name = base_filename_ + "." + std::to_string(i);
        std::string new_name = base_filename_ + "." + std::to_string(i + 1);

        if (std::filesystem::exists(old_name)) {
            if (i == max_files_ - 1) {
                std::filesystem::remove(new_name); // 删除最老的文件
            }
            std::filesystem::rename(old_name, new_name);
        }
    }

    // 将当前文件重命名为 .1
    if (std::filesystem::exists(base_filename_)) {
        std::filesystem::rename(base_filename_, base_filename_ + ".1");
    }

    // 创建新文件
    file_.open(base_filename_, std::ios::out);
    current_size_ = 0;
}

std::string RotatingFileSink::get_level_string(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE:    return "TRACE";
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO ";
        case LogLevel::WARN:     return "WARN ";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT ";
        default:                 return "UNKN ";
    }
}

// ========== AsyncLogger 实现 ==========

AsyncLogger::AsyncLogger(const std::string& name)
    : name_(name), level_(LogLevel::TRACE), should_stop_(false) {
    worker_thread_ = std::thread(&AsyncLogger::worker_function, this);
}

AsyncLogger::~AsyncLogger() {
    should_stop_ = true;
    queue_cv_.notify_all();

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void AsyncLogger::add_sink(const std::shared_ptr<LogSink>& sink) {
    sinks_.push_back(sink);
}

void AsyncLogger::log(LogLevel level, const std::string& message) {
    if (!should_log(level)) return;

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        message_queue_.emplace(level, name_, message);
    }
    queue_cv_.notify_one();
}

void AsyncLogger::flush() {
    for (auto& sink : sinks_) {
        sink->flush();
    }
}

void AsyncLogger::trace(const std::string& message) {
    log(LogLevel::TRACE, message);
}

void AsyncLogger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void AsyncLogger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void AsyncLogger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void AsyncLogger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void AsyncLogger::critical(const std::string& message) {
    log(LogLevel::CRITICAL, message);
}

void AsyncLogger::worker_function() {
    while (!should_stop_) {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        queue_cv_.wait(lock, [this] { return !message_queue_.empty() || should_stop_; });

        while (!message_queue_.empty()) {
            LogMessage msg = message_queue_.front();
            message_queue_.pop();
            lock.unlock();

            // 将消息发送到所有sink
            for (auto& sink : sinks_) {
                try {
                    sink->log(msg);
                } catch (const std::exception& e) {
                    // 如果日志输出失败，输出到stderr
                    std::cerr << "Log sink error: " << e.what() << std::endl;
                }
            }

            lock.lock();
        }
    }
}

// ========== Log 类实现 ==========

void Log::Initialize() {
    if (s_Initialized) return;

    try {
        // 创建核心日志器
        s_CoreLogger = std::make_shared<AsyncLogger>("JFMEngine");
        s_CoreLogger->add_sink(std::make_shared<ConsoleSink>(true));
        s_CoreLogger->add_sink(std::make_shared<FileSink>("logs/engine.log"));

        // 创建客户端日志器
        s_ClientLogger = std::make_shared<AsyncLogger>("Client");
        s_ClientLogger->add_sink(std::make_shared<ConsoleSink>(true));
        s_ClientLogger->add_sink(std::make_shared<FileSink>("logs/client.log"));

        s_Initialized = true;

        JFM_CORE_INFO("日志系统初始化完成");
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logging system: " << e.what() << std::endl;
    }
}

void Log::Shutdown() {
    if (!s_Initialized) return;

    JFM_CORE_INFO("正在关闭日志系统...");

    if (s_CoreLogger) {
        s_CoreLogger->flush();
        s_CoreLogger.reset();
    }

    if (s_ClientLogger) {
        s_ClientLogger->flush();
        s_ClientLogger.reset();
    }

    s_Initialized = false;
}

} // namespace JFM
