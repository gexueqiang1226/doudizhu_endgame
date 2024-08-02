//
// Created by deng on 18-11-28.
//

#ifndef DOUDIZHU_ENDGAME_HAND_H
#define DOUDIZHU_ENDGAME_HAND_H

#include "cardset.h"

namespace doudizhu_endgame
{

    enum Type
    {
        Pass,            // 不出
        Single,          // 单牌
        Pair,            // 对子
        Triple,          // 三个不带
        Bomb,            // 炸弹
        Triple_single,   // 三带一
        Triple_pair,     // 三带二
        Bomb_single,     // 四带两个单
        Bomb_pair,       // 四带两对
        Straight_single, // 顺子
        Straight_pair,   // 连对
        Plane,           // 飞机（不带）
        Plane_single,    // 飞机带单牌
        Plane_pair,      // 飞机带对子
        Rocket           // 王炸（火箭）
    };

    struct Pattern
    {
        int8_t power = -1;
        Type type = Pass;
        CardSet hand{};
        int8_t count = 0;

        Pattern(int8_t power, Type type, CardSet hand, int8_t count = 0) : power(power), type(type), hand(hand), count(count)
        {
        }

        Pattern() = default;

        ~Pattern() = default;
    };

    inline bool operator<(const Pattern &a, const Pattern &b)
    {
        if (a.type == Pass && b.type != Pass)
        {
            return true;
        }
        if (a.type == b.type && a.hand.size() == b.hand.size())
        {
            return a.power < b.power;
        }
        if (b.type == Rocket)
        {
            return true;
        }
        if (a.type != Rocket && b.type == Bomb)
        {
            return true;
        }
        return false;
    }

    inline bool operator==(const Pattern &a, const Pattern &b)
    {
        return a.type == b.type && a.power == b.power && a.hand.size() == b.hand.size();
    }

    class DouDiZhuHand
    {
    public:
        DouDiZhuHand() = default;

        ~DouDiZhuHand() = default;

        static void play(const CardSet &hand, const CardSet &toplay, CardSet &res);

        static void next_hand(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static Pattern check_hand(const CardSet &hand);

    private:
        static void combination(CardSet hand, CardSet used, size_t comb_len, int8_t size,
                                std::vector<std::vector<int8_t>> &combinations);

        static void create_straight(const CardSet &hand, int8_t min_len, int8_t size,
                                    std::vector<std::vector<int8_t>> &straights);

        static void get_rocket(const CardSet &hand, std::vector<Pattern> &next_moves);

        static void get_pass(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_triple(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_triple_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_triple_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_bomb(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_bomb_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_bomb_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_straight_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_straight_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_plane(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_plane_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

        static void get_plane_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);
    };

} // namespace doudizhu_endgame

#endif // DOUDIZHU_ENDGAME_HAND_H
