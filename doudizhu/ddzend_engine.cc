#include "ddzend_engine.h"
#include "negamax.h"
#include "alpha_beta.h"

#ifdef __cplusplus
extern "C"
{
#endif
    const char *ddzend_search(const char *lord, const char *farmer, const char *last, int turn, int states)
    {

        if ((lord == nullptr || lord[0] == '\0') || (farmer == nullptr || farmer[0] == '\0'))
        {
            return nullptr;
        }
        doudizhu_endgame::CardSet c1, c2;
        c1.from_c_string(const_cast<char *>(turn == 0 ? farmer : lord));
        c2.from_c_string(const_cast<char *>(turn == 0 ? lord : farmer));

        doudizhu_endgame::Pattern last_hand;
        doudizhu_endgame::DouDiZhuHand hand;
        doudizhu_endgame::CardSet ltcard;
        if (last != nullptr && last[0] != '\0')
        {
            ltcard.from_c_string(const_cast<char *>(last));
            last_hand = hand.check_hand(ltcard);
            if (last_hand.type == doudizhu_endgame::Pass)
            {
                // 给的上手牌不合法
                return nullptr;
            }
        }

        doudizhu_endgame::Negamax engine;
        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
        bool check = engine.search(c1, c2, last_hand);
        std::string ret = engine.best_move.hand.str();
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapse = end - start;
        printf("Negamax search: %f, check: %d, bet_move: %s\n", elapse.count(), check ? 1 : 0, check ? ret.c_str() : "not way");
        if (!check)
        {
            // 必输局, 需要用tip算法找个最优解
            doudizhu_endgame::AlphaBeta ab;
            ab.LoadCards(c1.str(), c2.str(), last_hand.hand.str());
            ret = ab.GetBestMove();
        }

        char *str = new char[ret.length()];
        strcpy(str, ret.c_str());
        return str;
    }
#ifdef __cplusplus
};
#endif