//
// Created by deng on 19-5-21.
//

#ifndef DOUDIZHU_ENDGAME_UTILS_H
#define DOUDIZHU_ENDGAME_UTILS_H

#include <map>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace doudizhu_endgame
{

    static const std::map<int, int8_t> card2val =
        {
            {'3', 0},
            {'4', 1},
            {'5', 2},
            {'6', 3},
            {'7', 4},
            {'8', 5},
            {'9', 6},
            {'0', 7},
            {'J', 8},
            {'Q', 9},
            {'K', 10},
            {'A', 11},
            {'2', 12},
            {'Y', 13},
            {'Z', 14},
            {'\n', 15},
            {'P', 15}};

    static const std::string val2card[16] =
        {
            "3", "4", "5", "6", "7", "8", "9", "0",
            "J", "Q", "K", "A", "2", "Y", "Z", "P"};

    static const std::string val2card_h[16] =
        {
            "3 ", "4 ", "5 ", "6 ", "7 ",
            "8 ", "9 ", "10 ", "J ", "Q ",
            "K ", "A ", "2 ", "BlackJoker ", "RedJoker ",
            "Pass "};

    static const uint64_t MultiplyDeBruijnBitPosition[64] =
        {
            0, 1, 48, 2, 57, 49, 28, 3, 61, 58, 50, 42, 38, 29, 17, 4,
            62, 55, 59, 36, 53, 51, 43, 22, 45, 39, 33, 30, 24, 18, 12, 5,
            63, 47, 56, 27, 60, 41, 37, 16, 54, 35, 52, 21, 44, 32, 23, 11,
            46, 26, 40, 15, 34, 20, 31, 10, 25, 14, 19, 9, 13, 8, 7, 6};

    static size_t my_find_first(uint64_t v, size_t not_found)
    {
        if (v != 0)
        {
            return MultiplyDeBruijnBitPosition[((uint64_t)((v & -v) * 0x3f79d71b4cb0a89UL)) >> 58];
        }
        else
        {
            return not_found;
        }
    }

    static size_t my_find_next(size_t prev, uint64_t v, size_t not_found)
    {
        prev += 1;
        if (prev >= not_found)
        {
            return not_found;
        }

        v >>= prev;
        if (v != 0)
        {
            return my_find_first(v, not_found) + prev;
        }
        else
        {
            return not_found;
        }
    }

} // doudizhu_endgame
#endif // DOUDIZHU_ENDGAME_UTILS_H
