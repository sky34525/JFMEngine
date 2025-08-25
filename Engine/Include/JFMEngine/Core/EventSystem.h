//
// Created by kw on 25-7-22.
// 底层事件系统核心 - 内核级事件处理
//

#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include "JFMEngine/Core/Core.h"
#include "JFMEngine/Events/Event.h"
#include "JFMEngine/Core/LockFreeQueue.h"
#include "JFMEngine/Core/MemoryPool.h"
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <unordered_map>
#include <chrono>
#include <functional>

namespace JFM {

    // 事件处理器接口
    class JFM_API IEventHandler {
    public:
        virtual ~IEventHandler() = default;
        virtual bool Handle(Event& event) = 0;
        virtual JFM::EventType GetHandledEventType() const = 0;
        virtual int GetPriority() const { return 0; } // 优先级，数字越大优先级越高
    };

    // 模板化事件处理器
    template<typename EventT>
    class JFM_API EventHandler : public IEventHandler {
    public:
        using HandlerFunc = std::function<bool(EventT&)>;

        EventHandler(HandlerFunc func, int priority = 0)
            : m_Handler(std::move(func)), m_Priority(priority) {}

        bool Handle(Event& event) override {
            if (event.GetEventType() == EventT::GetStaticType()) {
                return m_Handler(static_cast<EventT&>(event));
            }
            return false;
        }

        JFM::EventType GetHandledEventType() const override {
            return EventT::GetStaticType();
        }

        int GetPriority() const override { return m_Priority; }

    private:
        HandlerFunc m_Handler;
        int m_Priority;
    };

    // 事件统计信息
    struct EventStats {
        std::atomic<uint64_t> EventsProcessed{0};
        std::atomic<uint64_t> EventsDropped{0};
        std::atomic<uint64_t> MaxQueueSize{0};
        std::atomic<uint64_t> TotalProcessingTime{0}; // 微秒
    };

    // 底层事件系统
    class JFM_API EventSystem {
    public:
        static EventSystem& GetInstance() {
            static EventSystem instance;
            return instance;
        }

        // 初始化事件系统
        void Initialize(size_t workerThreads = std::thread::hardware_concurrency());

        // ���闭事件系统
        void Shutdown();

        // 发送事件（立即处理）
        void SendEvent(std::unique_ptr<Event> event);

        // 投递事件（异步处理）
        bool PostEvent(std::unique_ptr<Event> event);

        // 注册事件处理器
        template<typename EventT>
        void RegisterHandler(std::function<bool(EventT&)> handler, int priority = 0) {
            auto eventHandler = std::make_shared<EventHandler<EventT>>(std::move(handler), priority);
            RegisterHandler(eventHandler);
        }

        void RegisterHandler(std::shared_ptr<IEventHandler> handler);

        // 取消注册事件处理器
        void UnregisterHandler(std::shared_ptr<IEventHandler> handler);

        // 处理单个事件（同步）
        void ProcessEvent(Event& event);

        // 批量处理事件
        void ProcessEvents(size_t maxEvents = 100);

        // 获取统计信息
        const EventStats& GetStats() const { return m_Stats; }

        // 设置事件过滤器
        void SetEventFilter(std::function<bool(const Event&)> filter) {
            std::lock_guard<std::mutex> lock(m_FilterMutex);
            m_EventFilter = std::move(filter);
        }

    private:
        EventSystem() = default;
        ~EventSystem() { Shutdown(); }

        // 禁止拷贝和赋值
        EventSystem(const EventSystem&) = delete;
        EventSystem& operator=(const EventSystem&) = delete;

        // 工作线程函数
        void WorkerThreadFunc();

        // 处理单个事件的内部函数
        void ProcessEventInternal(Event& event);

        // 事件队列
        std::unique_ptr<EventQueue> m_EventQueue;

        // 工作线程
        std::vector<std::thread> m_WorkerThreads;
        std::atomic<bool> m_Running{false};
        std::condition_variable m_WorkerCV;
        std::mutex m_WorkerMutex;

        // 事件处理器管理
        std::unordered_map<JFM::EventType, std::vector<std::shared_ptr<IEventHandler>>> m_Handlers;
        std::mutex m_HandlersMutex;

        // 事件过滤器
        std::function<bool(const Event&)> m_EventFilter;
        std::mutex m_FilterMutex;

        // 统计信息
        EventStats m_Stats;

        // 性能监控
        std::chrono::high_resolution_clock::time_point m_LastStatsUpdate;
        std::mutex m_StatsMutex;
    };

    // 事件系统管理器的便捷宏
    #define EVENT_SYSTEM() EventSystem::GetInstance()

    // 发送事件的便捷宏
    #define SEND_EVENT(eventType, ...) \
        EVENT_SYSTEM().SendEvent(std::make_unique<eventType>(__VA_ARGS__))

    // 投递事件的便捷宏
    #define POST_EVENT(eventType, ...) \
        EVENT_SYSTEM().PostEvent(std::make_unique<eventType>(__VA_ARGS__))

}

#endif //EVENTSYSTEM_H
