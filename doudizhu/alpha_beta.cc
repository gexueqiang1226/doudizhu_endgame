//
// Created by deng on 18-11-28.
//

#include "utils.h"
#include "alpha_beta.h"

namespace doudizhu_endgame
{
    const char CARD_TYPES[15] = {'3', '4', '5', '6', '7', '8', '9', '0', 'J', 'Q', 'K', 'A', '2', 'Y', 'Z'};
    const byte CARD_VALUE[15] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18};
    AlphaBeta::AlphaBeta()
    {

        for (int i = 0; i < 15; i++)
        {
            char c = CARD_TYPES[i];
            byte v = CARD_VALUE[i];
            _name2id[c] = v;
            _id2name[v] = c;
        }
    }

    AlphaBeta::~AlphaBeta()
    {
    }

    void AlphaBeta::printCards(std::string pre, const std::multiset<byte> &pattern)
    {
        if (!pattern.size())
            printf("%s pass \n", pre.c_str());

        std::string str = "";
        for (auto x : pattern)
        {
            str += _id2name[x];
        }
        printf("%s card: %s\n", pre.c_str(), str.c_str());
    }

    bool AlphaBeta::LoadCards(const std::string &str1, const std::string &str2, const std::string &last)
    {
        // printf("LoadCards start \n");
        // printf("LoadCards str1: %s, str2: %s, last: %s\n", str1.c_str(), str2.c_str(), last.c_str());
        if (!last.empty() && last != "P")
        {
            CardSet card;
            card.from_string(last.c_str());
            DouDiZhuHand hand;
            // printf("ccc \n");
            Pattern ret = hand.check_hand(card);
            if (ret.type == Pass)
            {
                return false;
            }
            _last = ret;
        }
        _cards[0].clear();
        _cards[1].clear();
        for (auto x : str1)
        {
            _cards[0].insert(_name2id[x]);
        }
        // printCards("LoadCards", _cards[0]);
        for (auto x : str2)
        {
            _cards[1].insert(_name2id[x]);
        }
        // printCards("LoadCards", _cards[1]);
        _round = 0;
        _side = 0;
        // printf("LoadCards end \n");
        return true;
    }

    void AlphaBeta::Move(std::string hand)
    {
        // printCards("Move start:" + hand, _cards[_side]);
        if (!hand.empty() && hand != "P")
        {
            for (char c : hand)
            {
                _cards[_side].erase(_cards[_side].find(_name2id[c]));
            }
        }
        // printCards("Move end:" + hand, _cards[_side]);
        _side = 1 - _side;
        ++_round;
    }
    void AlphaBeta::UnMove(std::string hand)
    {
        _side = 1 - _side;
        --_round;
        // printCards("UnMove start:" + hand, _cards[_side]);
        if (!hand.empty() && hand != "P")
        {
            for (char c : hand)
            {
                _cards[_side].insert(_name2id[c]);
            }
        }
        // printCards("UnMove end:" + hand, _cards[_side]);
    }

    std::string AlphaBeta::GetCardSting()
    {
        std::string str = "";
        // printCards("GetCardSting", _cards[_side]);
        for (auto x : _cards[_side])
        {
            str += _id2name[x];
        }
        return str;
    }

    int AlphaBeta::Search(int depth, int alpha, int beta, std::list<Pattern> &pv)
    {
        if (search_timeout(_start, MAX_TIME) || !_cards[_side].size() || !_cards[1 - _side].size())
        {
            auto v = Evaluate();
            int score = _side ? -v : v;
            // printf("Search ret %d, %d, %d, %d, %d\n", depth, _side, _cards[_side].size(), _cards[1 - _side].size(), score);
            return score;
        }

        CardSet cs;
        cs.from_string(this->GetCardSting());
        std::vector<Pattern> selections;
        doudizhu_.next_hand(cs, _last, selections);

        std::sort(
            selections.begin(),
            selections.end(),
            [](const Pattern &a, const Pattern &b)
            {
                return a.hand.size() > b.hand.size();
            });

        /*
                std::string str = "";
                for (Pattern s : selections)
                {
                    if (s.hand.str() != "P")
                    {
                        str += " " + s.hand.str();
                    }
                }
                printf("%d, %d, last: %s, card: %s, cs: %s\n", depth, _side, _last.hand.str().c_str(), cs.str().c_str(), str.c_str());
        */

        if (depth <= 0 && selections.size() > 1)
        {
            auto v = Evaluate();
            int score = _side ? -v : v;
            // printf("Search ret %d, %d, %d, %d, %d, %d\n", depth, _side, _cards[_side].size(), _cards[1 - _side].size(), selections.size(), score);
            return score;
        }

        int value;
        Pattern last = _last;
        std::list<Pattern> line;
        for (Pattern s : selections)
        {
            this->Move(s.hand.str());
            _last = s;
            value = -Search(depth - 1, -beta, -alpha, line);
            this->UnMove(s.hand.str());
            if (value > beta)
            {
                return beta;
            }
            if (value > alpha)
            {
                alpha = value;
                pv = line;
                pv.push_front(s);
            }
            if (alpha > WIN_SCORE)
                break;
        }
        _last = last;
        return alpha;
    }

    int AlphaBeta::Evaluate() const
    {
        if (!_cards[0].size())
            return MAX_SCORE - _round;
        if (!_cards[1].size())
            return -MAX_SCORE + _round;

        return GetWeight();
    }

    int AlphaBeta::GetWeight() const
    {
        std::map<byte, int> count;
        for (int i : {0, 1})
        {
            for (auto x : _cards[i])
            {
                if (count.find(x) != count.end())
                {
                    ++count[x];
                }
                else
                {
                    count[x] = 1;
                }
            }
        }
        int sum = 0, index = 0;
        for (auto x : count)
        {
            sum += index * x.second;
            count[x.first] = index++;
        }

        double score1 = 0, score2 = 0, aver = ((double)sum) / (_cards[0].size() + _cards[1].size());
        for (auto x : _cards[0])
            score1 += count[x] - aver;
        for (auto x : _cards[1])
            score2 += count[x] - aver;
        return (score1 - score2) * 100;
    }

    std::string AlphaBeta::GetBestMove(int depth)
    {
        if (depth <= 0)
            depth = 128;
        _max_depth = depth;
        _start = std::chrono::steady_clock::now();
        int value = Search(depth, -MAX_SCORE, MAX_SCORE, _pv);
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapse = end - _start;

        Pattern ret = _pv.front();
        printf("GetBestMove time: %f, score: %d, cards: %s\n", elapse.count(), value, ret.hand.str().c_str());
        return ret.hand.str();
    }
} // namespace doudizhu_endgame
