//
// Created by kw on 25-7-22.
//

#include "JFMEngine/Core/MemoryPool.h"
#include "JFMEngine/Utils/Log.h"
#include <algorithm>
#include <cstdlib>

namespace JFM {

    // 内存块大小常量

    constexpr size_t BLOCK_SIZE = 64 * 1024;  // 64KB块
    constexpr size_t ALIGNMENT = 16;          // 16字节对齐

    // 内存块头部信息
    struct MemoryBlockHeader {
        size_t size;           // 块大小
        size_t used;           // 已使用字节数
        uint8_t* data;         // 数据起始地址

        // 空闲块链表
        struct FreeBlock {
            size_t size;
            FreeBlock* next;
        };
        FreeBlock* freeList;   // 空闲块链表头

        MemoryBlockHeader(size_t blockSize)
            : size(blockSize), used(0), freeList(nullptr) {
            data = reinterpret_cast<uint8_t*>(this) + sizeof(MemoryBlockHeader);
        }

        // 检查是否有足够空间
        bool HasSpace(size_t requestedSize) const {
            size_t alignedSize = AlignSize(requestedSize);

            // 首先检查空闲列表
            FreeBlock* current = freeList;
            while (current) {
                if (current->size >= alignedSize) {
                    return true;
                }
                current = current->next;
            }

            // 然后检查未使用的空间
            return (used + alignedSize) <= size;
        }

        // 分配内存
        void* Allocate(size_t requestedSize) {
            size_t alignedSize = AlignSize(requestedSize);

            // 首先尝试从空闲列表分配
            FreeBlock** current = &freeList;
            while (*current) {
                if ((*current)->size >= alignedSize) {
                    FreeBlock* block = *current;
                    *current = block->next; // 从空闲列表移除

                    // 如果块太大，分割它
                    if (block->size > alignedSize + sizeof(FreeBlock)) {
                        FreeBlock* remainder = reinterpret_cast<FreeBlock*>(
                            reinterpret_cast<uint8_t*>(block) + alignedSize
                        );
                        remainder->size = block->size - alignedSize;
                        remainder->next = freeList;
                        freeList = remainder;
                    }

                    return block;
                }
                current = &(*current)->next;
            }

            // 从未使用空间分配
            if (!HasSpace(alignedSize)) {
                return nullptr;
            }

            void* ptr = data + used;
            used += alignedSize;
            return ptr;
        }

        // 标记内存为空闲
        void MarkAsFree(void* ptr, size_t size) {
            if (!ptr) return;

            size_t alignedSize = AlignSize(size);
            FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr);
            newBlock->size = alignedSize;
            newBlock->next = freeList;
            freeList = newBlock;

            // 尝试合并相邻的空闲块
            CoalesceFreeBlocks();
        }

        // 检查块是否完全空闲
        bool IsCompletelyFree() const {
            // 简化检查：如果所有已使用的内存都在空闲列表中
            size_t totalFreeSize = 0;
            FreeBlock* current = freeList;
            while (current) {
                totalFreeSize += current->size;
                current = current->next;
            }

            return totalFreeSize >= used;
        }

    private:
        static size_t AlignSize(size_t size) {
            return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
        }

        // 合并相邻的空闲块
        void CoalesceFreeBlocks() {
            if (!freeList || !freeList->next) return;

            // 对空闲块按地址排序
            std::vector<FreeBlock*> blocks;
            FreeBlock* current = freeList;
            while (current) {
                blocks.push_back(current);
                current = current->next;
            }

            std::sort(blocks.begin(), blocks.end());

            // 合并相邻块
            freeList = nullptr;
            FreeBlock* lastAdded = nullptr;

            for (FreeBlock* block : blocks) {
                if (lastAdded &&
                    reinterpret_cast<uint8_t*>(lastAdded) + lastAdded->size ==
                    reinterpret_cast<uint8_t*>(block)) {
                    // 合并到前一个块
                    lastAdded->size += block->size;
                } else {
                    // 添加新块到链表
                    block->next = freeList;
                    freeList = block;
                    lastAdded = block;
                }
            }
        }
    };

    void* EventMemoryManager::AllocateRawMemory(size_t size) {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // 尝试从现有块中分配
        for (auto& block : m_MemoryBlocks) {
            MemoryBlockHeader* header = reinterpret_cast<MemoryBlockHeader*>(block.get());
            if (header->HasSpace(size)) {
                return header->Allocate(size);
            }
        }

        // 需要新的内存块
        size_t blockSize = std::max(BLOCK_SIZE, size + sizeof(MemoryBlockHeader) + ALIGNMENT);
        auto newBlock = std::make_unique<uint8_t[]>(blockSize);

        // 初始化块头部
        MemoryBlockHeader* header = new(newBlock.get()) MemoryBlockHeader(
            blockSize - sizeof(MemoryBlockHeader)
        );

        void* result = header->Allocate(size);
        m_MemoryBlocks.push_back(std::move(newBlock));

        return result;
    }

    void EventMemoryManager::DeallocateRawMemory(void* ptr, size_t size) {
        if (!ptr) return;

        std::lock_guard<std::mutex> lock(m_Mutex);

        // 根据释放策略处理
        switch (m_ReleasePolicy) {
            case ReleasePolicy::Immediate:
                DeallocateImmediate(ptr, size);
                break;

            case ReleasePolicy::Deferred:
                DeallocateDeferred(ptr, size);
                break;

            case ReleasePolicy::BatchDeferred:
                DeallocateBatchDeferred(ptr, size);
                break;
        }

        // 清理分配记录
        UnregisterAllocation(ptr);
    }

    void EventMemoryManager::DeallocateImmediate(void* ptr, size_t size) {
        // 立即释放策略：尝试将内存标记为可重用
        for (auto& block : m_MemoryBlocks) {
            MemoryBlockHeader* header = reinterpret_cast<MemoryBlockHeader*>(block.get());

            // 检查指针是否在此块范围内
            uint8_t* blockStart = reinterpret_cast<uint8_t*>(header);
            uint8_t* blockEnd = blockStart + sizeof(MemoryBlockHeader) + header->size;
            uint8_t* ptrBytes = reinterpret_cast<uint8_t*>(ptr);

            if (ptrBytes >= header->data && ptrBytes < blockEnd) {
                // 在简化实现中，我们将释放的内存添加到空闲列表
                header->MarkAsFree(ptr, size);
                return;
            }
        }
    }

    void EventMemoryManager::DeallocateDeferred(void* ptr, size_t size) {
        // 延迟释放策略：添加到延迟队列
        DeferredDeallocation deferred{
            ptr,
            size,
            std::chrono::steady_clock::now()
        };

        m_DeferredQueue.push(deferred);

        // 如果队列太长，处理一部分
        if (m_DeferredQueue.size() > 1000) {
            ProcessDeferredDeallocations();
        }
    }

    void EventMemoryManager::DeallocateBatchDeferred(void* ptr, size_t size) {
        // 批量延迟释放：先放入队列，定期批量处理
        DeferredDeallocation deferred{ptr, size, std::chrono::steady_clock::now()};
        m_DeferredQueue.push(deferred);

        // 每积累100个或超过100ms就批量处理
        static auto lastBatchTime = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastBatch = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastBatchTime
        ).count();

        if (m_DeferredQueue.size() >= 100 || timeSinceLastBatch > 100) {
            ProcessDeferredDeallocations();
            lastBatchTime = now;
        }
    }

    void EventMemoryManager::ProcessDeferredDeallocations() {
        // 处理延迟释放队列
        auto now = std::chrono::steady_clock::now();
        const auto delayThreshold = std::chrono::milliseconds(50); // 50ms延迟

        while (!m_DeferredQueue.empty()) {
            auto& deferred = m_DeferredQueue.front();

            // 检查是否到达释放时间
            if (now - deferred.deallocTime < delayThreshold) {
                break; // 还没到时间，停止处理
            }

            // 执行实际释放
            DeallocateImmediate(deferred.ptr, deferred.size);
            m_DeferredQueue.pop();
        }
    }

    void EventMemoryManager::RegisterAllocation(void* ptr, size_t size, size_t typeHash) {
        AllocationInfo info{
            size,
            typeHash,
            std::chrono::steady_clock::now()
        };

        m_Allocations[ptr] = info;
    }

    void EventMemoryManager::UnregisterAllocation(void* ptr) {
        auto it = m_Allocations.find(ptr);
        if (it != m_Allocations.end()) {
            m_Allocations.erase(it);
        }
    }

    EventMemoryManager::MemoryStats EventMemoryManager::GetStats() {
        std::lock_guard<std::mutex> lock(m_Mutex);

        MemoryStats stats;
        stats.activeAllocations = m_Allocations.size();
        stats.totalBlocks = m_MemoryBlocks.size();

        // 计算总分配内存
        for (const auto& pair : m_Allocations) {
            stats.totalAllocated += pair.second.size;
        }

        // 计算碎片化比例
        size_t totalBlockMemory = stats.totalBlocks * BLOCK_SIZE;
        if (totalBlockMemory > 0) {
            size_t wastedMemory = totalBlockMemory - stats.totalAllocated;
            stats.fragmentationRatio = (wastedMemory * 100) / totalBlockMemory;
        }

        return stats;
    }

    void EventMemoryManager::Compact() {
        std::lock_guard<std::mutex> lock(m_Mutex);

        // 首先处理所有延迟释放
        while (!m_DeferredQueue.empty()) {
            auto& deferred = m_DeferredQueue.front();
            DeallocateImmediate(deferred.ptr, deferred.size);
            m_DeferredQueue.pop();
        }

        // 内存压缩整理（简化版）
        // 在生产环境中，这里会实现复杂的内存迁移和整理算法

        // 移除完全空闲的内存块
        auto it = std::remove_if(m_MemoryBlocks.begin(), m_MemoryBlocks.end(),
            [](const auto& block) {
                MemoryBlockHeader* header = reinterpret_cast<MemoryBlockHeader*>(block.get());
                return header->IsCompletelyFree();
            });

        m_MemoryBlocks.erase(it, m_MemoryBlocks.end());
    }

}
