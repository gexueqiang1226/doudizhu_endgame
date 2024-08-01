//
// Created by deng on 18-11-28.
//

#ifndef DOUDIZHU_ENDGAME_NEGAMAX_H
#define DOUDIZHU_ENDGAME_NEGAMAX_H

#include <atomic>
#include <cstdint>
#include <cstring>
#include "cardset.h"
#include "datastructure.h"

namespace doudizhu_endgame
{

#define TRANSPOSITION_TABLE_SIZE (1024 * 1024 * 2)
#define TRANSPOSITION_TABLE_SIZE_MASK ((TRANSPOSITION_TABLE_SIZE) - 1)

//'127' should be better then '1'
#define FARMER_PLAY 127
#define LORD_PLAY 0

    class TranspositionTable
    {
    public:
        TranspositionTable();
        ~TranspositionTable();

        struct HashItem
        {
            uint32_t key{};
            int32_t score{};
        };

        void reset();
        void add(uint32_t key, int32_t score);
        int32_t get(uint32_t key);
        static uint32_t gen_key(const CardSet &lord, const CardSet &farmer, const CardSet &last_move, int32_t turn);

    private:
        HashItem *table_;
    };

    template <typename T>
    class ThreadSafe_Queue
    {
    public:
        ThreadSafe_Queue() = default;

        bool empty() const
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

        void push(T val)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(val);
            condition_.notify_one();
        }

        bool try_pop(T &val)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (queue_.empty())
            {
                return false;
            }
            val = std::move(queue_.front());
            queue_.pop();
            return true;
        }

        void wait_and_pop(T &val)
        {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]
                            { return !queue_.empty(); });
            val = std::move(queue_.front());
            queue_.pop();
        }

    private:
        mutable std::mutex mutex_;
        std::queue<T> queue_;
        std::condition_variable condition_;
    };

    class Negamax
    {
    public:
        Negamax() = default;
        ~Negamax() = default;

        std::chrono::time_point<std::chrono::steady_clock> _start;

        Pattern best_move{};

        bool search_multithreading(const CardSet &lord, const CardSet &farmer, const Pattern &last);

        bool search(const CardSet &lord, const CardSet &farmer, const Pattern &last);
        void reset_result();

    private:
        TranspositionTable transposition_table_;
        std::atomic<bool> finish_{false};

        void worker(CardSet lord, CardSet farmer, Pattern last_move, ThreadSafe_Queue<Pattern> &done_queue);

        int32_t negamax(const CardSet &lord, const CardSet &farmer, const Pattern &last_move, int32_t turn);
    };
} // namespace doudizhu_endgame

#endif // DOUDIZHU_ENDGAME_NEGAMAX_H
