//
// Created by kw on 25-7-22.
// 事件系统实现 - 底层内核级事件处理实现
//

#include "JFMEngine/Core/EventSystem.h"
#include "JFMEngine/Utils/Log.h"
#include <algorithm>
#include <chrono>

namespace JFM {

    void EventSystem::Initialize(size_t workerThreads) {
        if (m_Running.load()) {
            return;
        }


        // 初始化事件队列
        m_EventQueue = std::make_unique<EventQueue>();

        // 注意：不需要为抽象Event类创建内存池
        // 具体的事件类型会使用EventMemoryManager

        // 启动工作线程
        m_Running.store(true);
        m_WorkerThreads.reserve(workerThreads);

        for (size_t i = 0; i < workerThreads; ++i) {
            m_WorkerThreads.emplace_back(&EventSystem::WorkerThreadFunc, this);
        }

        m_LastStatsUpdate = std::chrono::high_resolution_clock::now();
    }

    void EventSystem::Shutdown() {
        if (!m_Running.load()) {
            return;
        }


        // 停止工作线程
        m_Running.store(false);
        m_WorkerCV.notify_all();

        // 等待所有工作线程结束
        for (auto& thread : m_WorkerThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        m_WorkerThreads.clear();

        // 处理剩余事件
        ProcessEvents(UINT32_MAX);

        // 清理资源
        {
            std::lock_guard<std::mutex> lock(m_HandlersMutex);
            m_Handlers.clear();
        }

    }

    void EventSystem::SendEvent(std::unique_ptr<Event> event) {
        if (!event) {
            return;
        }

        // 应用事件过滤器
        {
            std::lock_guard<std::mutex> lock(m_FilterMutex);
            if (m_EventFilter && !m_EventFilter(*event)) {
                return; // 事件被过滤
            }
        }

        // 立即处理事件
        ProcessEventInternal(*event);
    }

    bool EventSystem::PostEvent(std::unique_ptr<Event> event) {
        if (!event) {
            return false;
        }

        // 应用事件过滤器
        {
            std::lock_guard<std::mutex> lock(m_FilterMutex);
            if (m_EventFilter && !m_EventFilter(*event)) {
                return true; // 事件被过滤，但不算错误
            }
        }

        // 尝试将事件加入队列
        Event* rawEvent = event.release();
        if (m_EventQueue->Enqueue(rawEvent)) {
            // 更新队列大小统计
            size_t currentSize = m_EventQueue->GetSize();
            uint64_t maxSize = m_Stats.MaxQueueSize.load();
            uint64_t currentSizeULL = static_cast<uint64_t>(currentSize);
            while (currentSizeULL > maxSize &&
                   !m_Stats.MaxQueueSize.compare_exchange_weak(maxSize, currentSizeULL)) {
                maxSize = m_Stats.MaxQueueSize.load();
            }

            // 通知工作线程
            m_WorkerCV.notify_one();
            return true;
        } else {
            // 队列已满，删除事件并记录统计
            delete rawEvent;
            m_Stats.EventsDropped.fetch_add(1);
            return false;
        }
    }

    void EventSystem::RegisterHandler(std::shared_ptr<IEventHandler> handler) {
        if (!handler) {
            return;
        }

        std::lock_guard<std::mutex> lock(m_HandlersMutex);
        EventType eventType = handler->GetHandledEventType();

        auto& handlers = m_Handlers[eventType];
        handlers.push_back(handler);

        // 按优先级排序（高优先级在前）
        std::sort(handlers.begin(), handlers.end(),
            [](const std::shared_ptr<IEventHandler>& a, const std::shared_ptr<IEventHandler>& b) {
                return a->GetPriority() > b->GetPriority();
            });
    }

    void EventSystem::UnregisterHandler(std::shared_ptr<IEventHandler> handler) {
        if (!handler) {
            return;
        }

        std::lock_guard<std::mutex> lock(m_HandlersMutex);
        EventType eventType = handler->GetHandledEventType();

        auto it = m_Handlers.find(eventType);
        if (it != m_Handlers.end()) {
            auto& handlers = it->second;
            handlers.erase(
                std::remove(handlers.begin(), handlers.end(), handler),
                handlers.end());
        }
    }

    void EventSystem::ProcessEvent(Event& event) {
        ProcessEventInternal(event);
    }

    void EventSystem::ProcessEvents(size_t maxEvents) {
        Event* event = nullptr;
        size_t processedCount = 0;

        while (processedCount < maxEvents && m_EventQueue->Dequeue(event)) {
            if (event) {
                ProcessEventInternal(*event);
                delete event;
                processedCount++;
            }
        }
    }

    void EventSystem::WorkerThreadFunc() {
        while (m_Running.load()) {
            Event* event = nullptr;

            // 尝试从队列中获取事件
            if (m_EventQueue->Dequeue(event)) {
                if (event) {
                    ProcessEventInternal(*event);
                    delete event;
                }
            } else {
                // 队列为空，等待通知
                std::unique_lock<std::mutex> lock(m_WorkerMutex);
                m_WorkerCV.wait_for(lock, std::chrono::milliseconds(10),
                    [this] { return !m_Running.load() || !m_EventQueue->Empty(); });
            }
        }
    }

    void EventSystem::ProcessEventInternal(Event& event) {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::lock_guard<std::mutex> lock(m_HandlersMutex);
        auto it = m_Handlers.find(event.GetEventType());

        if (it != m_Handlers.end()) {
            auto& handlers = it->second;

            // 调用所有有效的处理器
            for (auto& handler : handlers) {
                if (handler && handler->Handle(event)) {
                    // 如果处理器返回true，表示事件已被处理，停止传播
                    break;
                }
            }
        }

        // 更新统计信息
        m_Stats.EventsProcessed.fetch_add(1);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        m_Stats.TotalProcessingTime.fetch_add(duration.count());
    }

}
