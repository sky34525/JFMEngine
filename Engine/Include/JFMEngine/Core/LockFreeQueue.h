//
// Created by kw on 25-7-22.
// 无锁队列 - 用于高性能事件传递
//

#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

#include "JFMEngine/Core/Core.h"
#include <atomic>
#include <memory>

namespace JFM {

    // 无锁环形缓冲区
    template<typename T, size_t Size = 4096>
    class JFM_API LockFreeQueue {
        static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");

    public:
        LockFreeQueue() : m_Head(0), m_Tail(0) {
            // 初始化环形缓冲区
            for (size_t i = 0; i < Size; ++i) {
                m_Buffer[i].sequence.store(i, std::memory_order_relaxed);
            }
        }

        // 生产者 - 入队操作
        bool Enqueue(const T& item) {
            Node* node = nullptr;
            size_t pos = m_Head.load(std::memory_order_relaxed);

            while (true) {
                node = &m_Buffer[pos & (Size - 1)];
                size_t seq = node->sequence.load(std::memory_order_acquire);
                intptr_t diff = (intptr_t)seq - (intptr_t)pos;

                if (diff == 0) {
                    // 位置可用，尝试占据
                    if (m_Head.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                        break;
                    }
                } else if (diff < 0) {
                    // 队列已满
                    return false;
                } else {
                    // 其他线程在操作，重试
                    pos = m_Head.load(std::memory_order_relaxed);
                }
            }

            // 写入数据
            node->data = item;
            node->sequence.store(pos + 1, std::memory_order_release);
            return true;
        }

        // 消费者 - 出队操作
        bool Dequeue(T& item) {
            Node* node = nullptr;
            size_t pos = m_Tail.load(std::memory_order_relaxed);

            while (true) {
                node = &m_Buffer[pos & (Size - 1)];
                size_t seq = node->sequence.load(std::memory_order_acquire);
                intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

                if (diff == 0) {
                    // 数据可用，尝试读取
                    if (m_Tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed)) {
                        break;
                    }
                } else if (diff < 0) {
                    // 队列为空
                    return false;
                } else {
                    // 其他线程在操作，重试
                    pos = m_Tail.load(std::memory_order_relaxed);
                }
            }

            // 读取数据
            item = node->data;
            node->sequence.store(pos + Size, std::memory_order_release);
            return true;
        }

        // 获取队列大小（近似值）
        size_t GetSize() const {
            return m_Head.load() - m_Tail.load();
        }

        bool Empty() const {
            return m_Head.load() == m_Tail.load();
        }

    private:
        struct Node {
            std::atomic<size_t> sequence;
            T data;
        };

        static constexpr size_t CACHE_LINE_SIZE = 64;

        // 使用缓存行对齐避免伪共享
        alignas(CACHE_LINE_SIZE) std::atomic<size_t> m_Head;
        alignas(CACHE_LINE_SIZE) std::atomic<size_t> m_Tail;
        alignas(CACHE_LINE_SIZE) Node m_Buffer[Size];
    };

    // 事件指针的无锁队列
    using EventQueue = LockFreeQueue<class Event*, 8192>;

}

#endif //LOCKFREEQUEUE_H
