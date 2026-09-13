/**
 * @file lockfree_queue.hpp
 * @author lxlx (1729649497@qq.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-13
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include <assert.h>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <type_traits>
#include <utility>

namespace Algorithm{

enum class QueueError : uint8_t { OK = 0, FULL, EMPTY};

template <typename Data, size_t Capacity> class alignas(32) MpscQueue {
    
    static_assert(Capacity >= 2, "Capacity must be >= 2");
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of two");
    static_assert(std::is_move_assignable<Data>::value ||
                  std::is_copy_assignable<Data>::value, "Capacity must be move_assignable or copy_assignable");

private:
    struct alignas(32) slot{
        Data data;
        std::atomic<size_t> sequence;
    };

};



}
