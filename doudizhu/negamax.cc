//
// Created by deng on 18-11-28.
//
#include <thread>
#include <functional>
#include <algorithm>
#include "utils.h"
#include "negamax.h"

namespace doudizhu_endgame
{
    int32_t Negamax::negamax(const CardSet &lord, const CardSet &farmer, const Pattern &last_move, int32_t turn)
    {
        if (finish_ || search_timeout(_start, MAX_TIME))
        {
            return 0;
        }

        if (lord.empty() || farmer.empty())
        {
            return -1;
        }

        uint32_t key = TranspositionTable::gen_key(lord, farmer, last_move.hand, turn);
        int32_t search = transposition_table_.get(key);
        if (search != 0)
        {
            return search;
        }

        int32_t score = -1;
        if (turn == FARMER_PLAY)
        { // farmer
            std::vector<Pattern> selections;
            DouDiZhuHand::next_hand(farmer, last_move, selections);
            for (Pattern &move : selections)
            {
                CardSet after_play;
                DouDiZhuHand::play(farmer, move.hand, after_play);
                int32_t val = -negamax(lord, after_play, move, LORD_PLAY);
                if (val > 0)
                {
                    score = val;
                    break;
                }
            }
        }
        else
        { // lord
            std::vector<Pattern> selections;
            DouDiZhuHand::next_hand(lord, last_move, selections);
            for (Pattern &move : selections)
            {
                CardSet after_play;
                DouDiZhuHand::play(lord, move.hand, after_play);
                int32_t val = -negamax(after_play, farmer, move, FARMER_PLAY);
                if (val > 0)
                {
                    score = val;
                    break;
                }
            }
        }

        transposition_table_.add(key, score);
        return score;
    }

    void Negamax::worker(CardSet lord, CardSet farmer, Pattern last_move, ThreadSafe_Queue<Pattern> &done_queue)
    {
        int32_t val = -negamax(lord, farmer, last_move, FARMER_PLAY);
        if (val > 0)
        {
            finish_.store(true);
            done_queue.push(last_move);
        }

        if (search_timeout(_start, MAX_TIME))
        {
            finish_.store(true);
        }
    }

    bool Negamax::search_multithreading(const CardSet &lord, const CardSet &farmer, const Pattern &last)
    {
        _start = std::chrono::steady_clock::now();
        std::vector<Pattern> selections;
        DouDiZhuHand::next_hand(lord, last, selections);
        std::vector<std::thread> threads;
        ThreadSafe_Queue<Pattern> done_queue;
        for (Pattern &move : selections)
        {
            CardSet after_play;
            DouDiZhuHand::play(lord, move.hand, after_play);
            threads.emplace_back(std::thread(&Negamax::worker, this, after_play, farmer, move, std::ref(done_queue)));
        }
        std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

        return done_queue.try_pop(best_move);
    }

    bool Negamax::search(const CardSet &lord, const CardSet &farmer, const Pattern &last)
    {
        _start = std::chrono::steady_clock::now();
        std::vector<Pattern> selections;
        DouDiZhuHand::next_hand(lord, last, selections);
        for (Pattern &move : selections)
        {
            CardSet after_play;
            DouDiZhuHand::play(lord, move.hand, after_play);
            int32_t val = -negamax(after_play, farmer, move, FARMER_PLAY);
            if (val > 0)
            {
                best_move = move;
                return true;
            }
            if (search_timeout(_start, MAX_TIME))
            {
                return false;
            }
        }
        return false;
    }

    void Negamax::reset_result()
    {
        finish_.store(false);
        best_move = Pattern();
    }

    TranspositionTable::TranspositionTable()
    {
        table_ = new HashItem[TRANSPOSITION_TABLE_SIZE];
    }

    TranspositionTable::~TranspositionTable()
    {
        delete[] table_;
    }

    void TranspositionTable::reset()
    {
        memset(table_, 0, sizeof(struct HashItem) * TRANSPOSITION_TABLE_SIZE);
    }

    void TranspositionTable::add(uint32_t key, int32_t score)
    {
        size_t index = key & TRANSPOSITION_TABLE_SIZE_MASK;
        table_[index].key = key ^ unsigned(score);
        table_[index].score = score;
    }

    int32_t TranspositionTable::get(uint32_t key)
    {
        size_t index = key & TRANSPOSITION_TABLE_SIZE_MASK;
        auto key_ = table_[index].key;
        auto score = table_[index].score;
        if ((key_ ^ unsigned(score)) == key)
        {
            return score;
        }
        else
        {
            return 0;
        }
    }

    uint32_t TranspositionTable::gen_key(const CardSet &lord, const CardSet &farmer, const CardSet &last_move, int32_t turn)
    {
        uint64_t key = 0;

        key ^= farmer.to_ullong() + 0x9e3779b9 + (key << 6) + (key >> 2);
        key ^= lord.to_ullong() + 0x9e3779b9 + (key << 6) + (key >> 2);
        key ^= last_move.to_ullong() + 0x9e3779b9 + (key << 6) + (key >> 2);
        key ^= turn + 0x9e3779b9 + (key << 6) + (key >> 2);

        // 64 bit to 32 bit hash
        key = (~key) + (key << 18);
        key = key ^ (key >> 31);
        key = (key + (key << 2)) + (key << 4);
        key = key ^ (key >> 11);
        key = key + (key << 6);
        key = key ^ (key >> 22);

        return (int)key;
    }
} // namespace doudizhu_endgame
