//
// Created by deng on 18-11-28.
//

#include "cardset.h"

namespace doudizhu_endgame
{

    void CardSet::set_rocket()
    {
        card_mask_.set((size_t)(14 << 2));
        card_mask_.set((size_t)(13 << 2));
    }

    void CardSet::set_single(int8_t card, bool val)
    {
        card_mask_.set((size_t)(card << 2), val);
    }

    void CardSet::set_pair(int8_t card, bool val)
    {
        card <<= 2;
        card_mask_.set((size_t)card, val);
        card_mask_.set((size_t)(card + 1), val);
    }

    void CardSet::set_trio(int8_t card)
    {
        card <<= 2;
        card_mask_.set((size_t)card);
        card_mask_.set((size_t)(card + 1));
        card_mask_.set((size_t)(card + 2));
    }

    void CardSet::set_trio_single(int8_t trio, int8_t single)
    {
        trio <<= 2;
        card_mask_.set((size_t)trio);
        card_mask_.set((size_t)(trio + 1));
        card_mask_.set((size_t)(trio + 2));

        card_mask_.set((size_t)(single << 2));
    }

    void CardSet::set_trio_pair(int8_t trio, int8_t pair)
    {
        trio <<= 2;
        card_mask_.set((size_t)trio);
        card_mask_.set((size_t)(trio + 1));
        card_mask_.set((size_t)(trio + 2));

        pair <<= 2;
        card_mask_.set((size_t)pair);
        card_mask_.set((size_t)(pair + 1));
    }

    void CardSet::set_bomb(int8_t bomb)
    {
        bomb <<= 2;
        card_mask_.set((size_t)bomb);
        card_mask_.set((size_t)(bomb + 1));
        card_mask_.set((size_t)(bomb + 2));
        card_mask_.set((size_t)(bomb + 3));
    }

    void CardSet::set_straight_s(int8_t start, int8_t end)
    {
        for (int8_t i = start; i <= end; ++i)
        {
            card_mask_.set((size_t)(i << 2));
        }
    }

    void CardSet::set_straight_p(int8_t start, int8_t end)
    {
        for (int8_t i = start; i <= end; ++i)
        {
            card_mask_.set((size_t)(i << 2));
            card_mask_.set((size_t)((i << 2) + 1));
        }
    }

    void CardSet::set_plane(int8_t start, int8_t end)
    {
        for (int8_t i = start; i <= end; ++i)
        {
            card_mask_.set((size_t)(i << 2));
            card_mask_.set((size_t)((i << 2) + 1));
            card_mask_.set((size_t)((i << 2) + 2));
        }
    }

    void CardSet::set_plane_single(int8_t start, int8_t end, std::vector<int8_t> &comb)
    {
        for (int8_t i = start; i <= end; ++i)
        {
            card_mask_.set((size_t)(i << 2));
            card_mask_.set((size_t)((i << 2) + 1));
            card_mask_.set((size_t)((i << 2) + 2));
        }

        for (int8_t i : comb)
        {
            card_mask_.set((size_t)(i << 2));
        }
    }

    void CardSet::set_plane_pair(int8_t start, int8_t end, std::vector<int8_t> &comb)
    {
        for (int8_t i = start; i <= end; ++i)
        {
            card_mask_.set((size_t)(i << 2));
            card_mask_.set((size_t)((i << 2) + 1));
            card_mask_.set((size_t)((i << 2) + 2));
        }

        for (int8_t i : comb)
        {
            card_mask_.set((size_t)(i << 2));
            card_mask_.set((size_t)((i << 2) + 1));
        }
    }

    bool CardSet::has_count(int8_t card, int8_t count) const
    {
        switch (count)
        {
        case 1:
            return card_mask_[card << 2];
        case 2:
            return card_mask_[(card << 2) + 1];
        case 3:
            return card_mask_[(card << 2) + 2];
        case 4:
            return card_mask_[(card << 2) + 3];
        default:
            return false;
        }
    }

    // 获取牌数, 为了控制AI拆牌(主要是拆炸弹, 做个特殊处理)
    int8_t CardSet::card_count(int8_t card) const
    {
        if (card >= 13)
        {
            if (card_mask_[card << 2])
            {
                if (card_mask_[52] && card_mask_[56])
                {
                    return 99;
                }
                return 1;
            }
        }
        else
        {
            for (size_t i = 4; i > 0; i--)
            {
                if (card_mask_[(card << 2) + (i - 1)])
                {
                    return i;
                }
            }
        }
        return 0;
    }

    std::string CardSet::str()
    {
        std::string string;
        string.clear();
        size_t pos = find_first();
        while (pos < 64)
        {
            size_t index = (pos >> 2);
            if (index < 16)
            {
                string.insert(0, val2card[index]);
            }
            pos = find_next(pos);
        }

        if (string.length() == 0)
        {
            string += val2card[15];
        }
        return string;
    }

    std::string CardSet::str_h()
    {
        std::string string;
        string.clear();
        size_t pos = find_first();
        while (pos < 64)
        {
            size_t index = (pos >> 2);
            if (index < 16)
            {
                string.insert(0, val2card_h[index]);
            }
            pos = find_next(pos);
        }

        if (string.length() == 0)
        {
            string += val2card[15];
        }
        string.pop_back();
        return string;
    }

    std::string CardSet::bitset_str()
    {
        return card_mask_.to_string();
    }

    bool CardSet::from_string(std::string string)
    {
        int8_t card;
        for (auto ch : string)
        {
            auto it = card2val.find(toupper(ch));
            if (it == card2val.end())
            {
                printf("invalid card:%c\n", ch);
                return false;
            }
            else
            {
                if (it->second < 15)
                { // valid card
                    card = (it->second) << 2;
                    while (card_mask_[card])
                    {
                        card += 1;
                    }
                    card_mask_.set((size_t)card);
                }
                else
                {
                    // input is Pass
                }
            }
        }
        return true;
    }

    void CardSet::from_c_string(char *string)
    {
        this->from_string(std::string(string));
    }

    void CardSet::remove(const CardSet &hand)
    {
        size_t pos = hand.find_first();
        while (pos < BITSET_SIZE)
        {
            size_t i = ((pos >> 2) << 2) + 3;
            while (!card_mask_.test(i))
            {
                i -= 1;
            }
            card_mask_.set(i, false);
            pos = hand.find_next(pos);
        }
    }

} // namespace doudizhu_endgame