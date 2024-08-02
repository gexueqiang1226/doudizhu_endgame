//
// Created by deng on 18-11-28.
//

#include <algorithm>
#include "hand.h"

namespace doudizhu_endgame
{

    void DouDiZhuHand::play(const CardSet &hand, const CardSet &toplay, CardSet &res)
    {
        size_t pos = toplay.find_first();
        res = hand;
        while (pos < BITSET_SIZE)
        {
            size_t i = ((pos >> 2) << 2) + 3;
            while (!res.test(i))
            {
                i -= 1;
            }
            res.set(i, false);
            pos = toplay.find_next(pos);
        }
    }

    void DouDiZhuHand::next_hand(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        next_moves.reserve(50);
        if (last.type != Pass)
        {
            switch (last.type)
            {
            case Pair:
            {
                get_pair(hand, last, next_moves);
                break;
            }
            case Single:
            {
                get_single(hand, last, next_moves);
                break;
            }
            case Triple_single:
            {
                get_triple_single(hand, last, next_moves);
                break;
            }
            case Triple_pair:
            {
                get_triple_pair(hand, last, next_moves);
                break;
            }
            case Triple:
            {
                get_triple(hand, last, next_moves);
                break;
            }
            case Straight_single:
            {
                get_straight_single(hand, last, next_moves);
                break;
            }
            case Straight_pair:
            {
                get_straight_pair(hand, last, next_moves);
                break;
            }
            case Bomb_single:
            {
                get_bomb_single(hand, last, next_moves);
                break;
            }
            case Bomb_pair:
            {
                get_bomb_pair(hand, last, next_moves);
                break;
            }
            case Plane:
            {
                get_plane(hand, last, next_moves);
                break;
            }
            case Plane_single:
            {
                get_plane_single(hand, last, next_moves);
                break;
            }
            case Plane_pair:
            {
                get_plane_pair(hand, last, next_moves);
                break;
            }
            default:
            {
            }
            }
            get_rocket(hand, next_moves);
            get_bomb(hand, last, next_moves);
            get_pass(hand, last, next_moves);
        }
        else
        {
            get_rocket(hand, next_moves);
            get_pair(hand, last, next_moves);
            get_single(hand, last, next_moves);

            // check card '7' and '10'
            if (hand.is_single(4) || hand.is_single(7))
            {
                get_straight_single(hand, last, next_moves);
            }
            get_straight_pair(hand, last, next_moves);

            bool get_move_continue = false;
            for (int8_t card = 0; card < 13; ++card)
            {
                if (hand.is_trio(card))
                {
                    get_move_continue = true;
                    break;
                }
            }
            if (get_move_continue)
            {
                get_triple_single(hand, last, next_moves);
                get_triple_pair(hand, last, next_moves);
                get_triple(hand, last, next_moves);
                get_bomb(hand, last, next_moves);
                get_bomb_single(hand, last, next_moves);
                get_bomb_pair(hand, last, next_moves);
                get_plane(hand, last, next_moves);
                get_plane_single(hand, last, next_moves);
                get_plane_pair(hand, last, next_moves);
            }
        }
        if (next_moves.size() > 1)
        {
            std::sort(
                next_moves.begin(),
                next_moves.end(),
                [](const Pattern &a, const Pattern &b)
                {
                    if (a.hand.size() != b.hand.size())
                    {
                        return a.hand.size() > b.hand.size();
                    }
                    if (a.count != b.count)
                    {
                        return a.count < b.count;
                    }
                    return a.power > b.power;
                });
        }
    }

    Pattern DouDiZhuHand::check_hand(const CardSet &hand)
    {
        std::vector<Pattern> next_moves;

        next_hand(hand, Pattern(), next_moves);

        Pattern ret;
        for (Pattern move : next_moves)
        {
            if (move.hand.size() == hand.size())
            {
                ret = move;
                break;
            }
        }
        return ret;
    }

    void DouDiZhuHand::get_pass(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        if (last.type != Pass)
        {
            CardSet res;
            next_moves.emplace_back(-1, Pass, res);
        }
        else
        {
            // can not pass twice
        }
    }

    void DouDiZhuHand::get_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        for (int8_t i = 0; i < 15; ++i)
        {
            if (hand.is_single(i) && i > last.power)
            {
                CardSet res;
                res.set_single(i);
                next_moves.emplace_back(i, Single, res, hand.card_count(i));
            }
        }
    }

    void DouDiZhuHand::get_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        for (int8_t i = 0; i < 15; ++i)
        {
            if (hand.is_pair(i) && i > last.power)
            {
                CardSet res;
                res.set_pair(i);
                next_moves.emplace_back(i, Pair, res, hand.card_count(i));
            }
        }
    }

    void DouDiZhuHand::get_triple(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        for (int8_t i = 0; i < 15; ++i)
        {
            if (hand.is_trio(i) && i > last.power)
            {
                CardSet res;
                res.set_trio(i);
                next_moves.emplace_back(i, Triple, res, hand.card_count(i));
            }
        }
    }

    void DouDiZhuHand::get_triple_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        for (int8_t i = 0; i < 15; ++i)
        {
            if (hand.is_trio(i) && i > last.power)
            {
                for (int8_t j = 0; j < 15; ++j)
                {
                    if (hand.is_single(j) && j != i)
                    {
                        CardSet res;
                        res.set_trio_single(i, j);
                        next_moves.emplace_back(i, Triple_single, res);
                    }
                }
            }
        }
    }

    void DouDiZhuHand::get_triple_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        for (int8_t i = 0; i < 15; ++i)
        {
            if (hand.is_trio(i) && i > last.power)
            {
                for (int8_t j = 0; j < 15; ++j)
                {
                    if (hand.is_pair(j) && j != i)
                    {
                        CardSet res;
                        res.set_trio_pair(i, j);
                        next_moves.emplace_back(i, Triple_pair, res);
                    }
                }
            }
        }
    }

    void DouDiZhuHand::get_bomb(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        // powerx4 !!!!
        if (last.type != Rocket && hand.size() >= 4)
        {
            for (int8_t i = 0; i < 13; ++i)
            {
                if ((hand.is_bomb(i) && last.type == Bomb && i > last.power) || (hand.is_bomb(i) && last.type != Bomb))
                {
                    CardSet res;
                    res.set_bomb(i);
                    next_moves.emplace_back((i << 2), Bomb, res);
                }
            }
        }
    }

    void DouDiZhuHand::combination(CardSet hand, CardSet used, size_t comb_len, int8_t size,
                                   std::vector<std::vector<int8_t>> &combinations)
    {
        hand.remove_taken(used);
        size_t n = 0;
        int8_t _hand[15];

        for (int8_t i = 0; i < 15; ++i)
        {
            if (hand.has_count(i, size))
            {
                _hand[n++] = i;
            }
        }

        if (comb_len <= n)
        {
            std::vector<bool> v(n);
            std::fill(v.end() - comb_len, v.end(), true);

            do
            {
                std::vector<int8_t> tmp;
                for (size_t i = 0; i < v.size(); ++i)
                {
                    if (v[i])
                    {
                        tmp.emplace_back(_hand[i]);
                    }
                }
                combinations.emplace_back(tmp);
            } while (std::next_permutation(v.begin(), v.end()));
        }
        else
        {
            // too few cards
        }
    }

    void DouDiZhuHand::get_bomb_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        for (int8_t i = 0; i < 13; ++i)
        {
            if (hand.is_bomb(i) && i > last.power)
            {
                CardSet res;
                res.set_bomb(i);
                std::vector<std::vector<int8_t>> comb;
                combination(hand, res, 2, 1, comb); // res->uesd

                for (std::vector<int8_t> &j : comb)
                {
                    res.set_single(j[0]);
                    res.set_single(j[1]);
                    next_moves.emplace_back(i, Bomb_single, res);

                    // reset
                    res.set_single(j[0], false);
                    res.set_single(j[1], false);
                }

                // bomb + one pair
                for (int8_t k = 0; k < 13; ++k)
                {
                    if (hand.is_pair(k) && k != i)
                    {
                        res.set_pair(k);
                        next_moves.emplace_back(i, Bomb_single, res);

                        // reset
                        res.set_pair(k, false);
                    }
                }
            }
        }
    }

    void DouDiZhuHand::get_bomb_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        for (int8_t i = 0; i < 13; ++i)
        {
            if (hand.is_bomb(i) && i > last.power)
            {
                CardSet res;
                res.set_bomb(i);

                std::vector<std::vector<int8_t>> comb;
                combination(hand, res, 2, 2, comb); // res->used

                for (std::vector<int8_t> &j : comb)
                {
                    res.set_pair(j[0]);
                    res.set_pair(j[1]);

                    next_moves.emplace_back(i, Bomb_pair, res);

                    // res reset
                    res.set_pair(j[0], false);
                    res.set_pair(j[1], false);
                }
            }
        }
    }

    void DouDiZhuHand::create_straight(const CardSet &hand, int8_t min_len, int8_t size,
                                       std::vector<std::vector<int8_t>> &straights)
    {
        straights.clear();
        // 3 ~ A
        int8_t count = 0;
        int8_t set[12]{};
        for (int8_t i = 0; i < 12; ++i)
        {
            if (hand.has_count(i, size))
            {
                set[count] = i;
                count += 1;
            }
        }

        if (count < min_len)
        {
            return;
        }

        for (int8_t start = 0; start < count; ++start)
        {
            for (int8_t end = start; end < count && set[end] - set[start] == end - start; ++end)
            {
                if (end - start >= min_len - 1)
                {
                    std::vector<int8_t> tmp;
                    for (int8_t i = set[start]; i <= set[end]; ++i)
                    {
                        tmp.emplace_back(i);
                    }
                    straights.emplace_back(tmp);
                }
            }
        }
    }

    void DouDiZhuHand::get_straight_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        if (last.type == Pass && hand.size() >= 5)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 5, 1, straights);

            for (std::vector<int8_t> &i : straights)
            {
                CardSet res;
                res.set_straight_s(i[0], i.back());
                next_moves.emplace_back(i[0], Straight_single, res);
            }
        }
        else if (last.type == Straight_single && hand.size() >= 5)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 5, 1, straights);
            for (std::vector<int8_t> &i : straights)
            {
                if (i.size() == last.hand.size() && i[0] > last.power)
                {
                    CardSet res;
                    res.set_straight_s(i[0], i.back());
                    next_moves.emplace_back(i[0], Straight_single, res);
                }
            }
        }
        else
        {
            // not move
        }
    }

    void DouDiZhuHand::get_straight_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        if (last.type == Pass && hand.size() >= 6)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 3, 2, straights);
            for (std::vector<int8_t> &i : straights)
            {
                CardSet res;
                res.set_straight_p(i[0], i.back());

                next_moves.emplace_back(i[0], Straight_pair, res);
            }
        }
        else if (last.type == Straight_pair && hand.size() >= 6)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 3, 2, straights);
            for (std::vector<int8_t> &i : straights)
            {
                if (i.size() * 2 == last.hand.size() && i[0] > last.power)
                {
                    CardSet res;
                    res.set_straight_p(i[0], i.back());

                    next_moves.emplace_back(i[0], Straight_pair, res);
                }
            }
        }
    }

    void DouDiZhuHand::get_plane(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        if (last.type == Pass && hand.size() >= 6)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 2, 3, straights);

            for (std::vector<int8_t> &i : straights)
            {
                CardSet res;
                res.set_plane(i[0], i.back());

                next_moves.emplace_back(i[0], Plane, res);
            }
        }
        else if (last.type == Plane && hand.size() >= 6)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 2, 3, straights);

            for (std::vector<int8_t> &i : straights)
            {
                if (i.size() * 3 == last.hand.size() && i[0] > last.power)
                {
                    CardSet res;
                    res.set_plane(i[0], i.back());

                    next_moves.emplace_back(i[0], Plane, res);
                }
            }
        }
    }

    void DouDiZhuHand::get_plane_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        int8_t power;
        if (last.type == Pass && hand.size() >= 8)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 2, 3, straights);

            for (std::vector<int8_t> &i : straights)
            {
                power = i[0];
                size_t comb_len = i.size();
                CardSet used;
                for (int8_t k : i)
                {
                    used.set_bomb(k);
                }

                std::vector<std::vector<int8_t>> comb;
                combination(hand, used, comb_len, 1, comb);

                for (std::vector<int8_t> &j : comb)
                {
                    CardSet res;
                    res.set_plane_single(i[0], i.back(), j);

                    next_moves.emplace_back(power, Plane_single, res);
                }
            }
        }
        else if (last.type == Plane_single && hand.size() >= 8)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 2, 3, straights);

            size_t plane_len = (last.hand.size() / 4);
            for (std::vector<int8_t> &i : straights)
            {
                power = i[0];
                if (i.size() == plane_len && power > last.power)
                {
                    size_t comb_len = i.size();
                    CardSet used;

                    for (int8_t k : i)
                    {
                        used.set_bomb(k);
                    }

                    std::vector<std::vector<int8_t>> comb;
                    combination(hand, used, comb_len, 1, comb);

                    for (std::vector<int8_t> &j : comb)
                    {
                        CardSet res;
                        res.set_plane_single(i[0], i.back(), j);

                        next_moves.emplace_back(power, Plane_single, res);
                    }
                }
            }
        }
    }

    void DouDiZhuHand::get_plane_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves)
    {
        int8_t power;
        if (last.type == Pass && hand.size() >= 10)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 2, 3, straights);

            for (std::vector<int8_t> &i : straights)
            {
                power = i[0];
                size_t comb_len = i.size();
                CardSet used;
                for (int8_t k : i)
                {
                    used.set_bomb(k);
                }

                std::vector<std::vector<int8_t>> comb;
                combination(hand, used, comb_len, 2, comb);

                for (std::vector<int8_t> &j : comb)
                {
                    CardSet res;
                    res.set_plane_pair(i[0], i.back(), j);

                    next_moves.emplace_back(power, Plane_pair, res);
                }
            }
        }
        else if (last.type == Plane_pair && hand.size() >= 10)
        {
            std::vector<std::vector<int8_t>> straights;
            create_straight(hand, 2, 3, straights);

            size_t plane_len = (last.hand.size() / 5);
            for (std::vector<int8_t> &i : straights)
            {
                power = i[0];
                if (i.size() == plane_len && power > last.power)
                {
                    size_t comb_len = i.size();
                    CardSet used;
                    for (int8_t k : i)
                    {
                        used.set_bomb(k);
                    }

                    std::vector<std::vector<int8_t>> comb;
                    combination(hand, used, comb_len, 2, comb);

                    for (std::vector<int8_t> &j : comb)
                    {
                        CardSet res;
                        res.set_plane_pair(i[0], i.back(), j);

                        next_moves.emplace_back(power, Plane_single, res);
                    }
                }
            }
        }
    }

    void DouDiZhuHand::get_rocket(const CardSet &hand, std::vector<Pattern> &next_moves)
    {
        if (hand.is_single(13) && hand.is_single(14))
        {
            CardSet res;
            res.set_rocket();
            next_moves.emplace_back(99, Rocket, res);
        }
    }
} // namespace doudizhu_endgame
