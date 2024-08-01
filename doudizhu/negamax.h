//
// Created by deng on 18-11-28.
//

#ifndef DOUDIZHU_ENDGAME_NEGAMAX_H
#define DOUDIZHU_ENDGAME_NEGAMAX_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <atomic>

#include "cardset.h"
#include "hand.h"

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

    class Negamax
    {
    public:
        Negamax() = default;
        ~Negamax() = default;

        Pattern best_move{};

        bool search(const CardSet &lord, const CardSet &farmer, const Pattern &last);
        void reset_result();

    private:
        TranspositionTable transposition_table_;
        std::atomic<bool> finish_{false};

        int32_t negamax(const CardSet &lord, const CardSet &farmer, const Pattern &last_move, int32_t turn);
    };
} // namespace doudizhu_endgame

#endif // DOUDIZHU_ENDGAME_NEGAMAX_H
