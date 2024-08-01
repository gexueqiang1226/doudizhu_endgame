//
// Created by deng on 18-11-28.
//

#ifndef DOUDIZHU_ENDGAME_AlphaBeta_H
#define DOUDIZHU_ENDGAME_AlphaBeta_H

#include "datastructure.h"
#include "cardset.h"
#include "hashcache.hpp"

namespace doudizhu_endgame
{
    class AlphaBeta
    {
    public:
        AlphaBeta();
        ~AlphaBeta();

        bool LoadCards(const std::string &str1, const std::string &str2, const std::string &last);
        std::string GetBestMove(int depth = 128);

    private:
        std::unordered_map<char, byte> _name2id;
        std::unordered_map<byte, char> _id2name;

        std::multiset<byte> _cards[2];
        std::list<Pattern *> _pv; // 主要变例
        Pattern _last;
        std::chrono::time_point<std::chrono::steady_clock> _start;
        int _side;
        int _round, _max_depth;
        HashCache _hash;

        DouDiZhuHand doudizhu_;

        void printCard(std::string pre, const std::multiset<byte> &pattern);
        void Move(std::string hand);
        void UnMove(std::string hand);
        int Evaluate() const;
        int GetWeight() const;
        std::string GetCardSting();
        int Search(int depth, int alpha, int beta, std::list<Pattern *> &pv);
    };

} // namespace doudizhu_endgame

#endif // DOUDIZHU_ENDGAME_AlphaBeta_H
