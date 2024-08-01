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

        std::string ret = "";
        doudizhu_endgame::Pattern *last_hand = nullptr;
        doudizhu_endgame::DouDiZhuHand hand;
        doudizhu_endgame::CardSet ltcard;
        if (last != nullptr && last[0] != '\0')
        {
            ltcard.from_c_string(const_cast<char *>(last));
            last_hand = hand.check_hand(ltcard);
            if (!last_hand)
            {
                // 给的上手牌不合法
                return nullptr;
            }
        }
        // doudizhu_endgame::Negamax *engine = new doudizhu_endgame::Negamax();
        // doudizhu_endgame::TreeNode *root = nullptr;
        // if (last_hand)
        // {
        //     root = engine->search(c1, c2, last_hand);
        // }
        // else
        // {
        //     root = engine->search(c1, c2);
        // }
        // if (!root->child.empty())
        // {
        //     ret = root->child[0]->last_move->hand.str();
        // }

        // delete engine;

        // if (!ret.empty())
        // {
        // printf("ddzend_search win: %s %s\n", ret.data(), ret.c_str());
        // return ret.data();
        // }

        // 必输局, 需要用tip算法找个最优解
        doudizhu_endgame::AlphaBeta *ab = new doudizhu_endgame::AlphaBeta();
        std::string lastStr = last_hand ? last_hand->hand.str() : "";
        // printf("ddzend_search last %s \n", lastStr.c_str());
        ab->LoadCards(c1.str(), c2.str(), lastStr);
        std::string ret2 = ab->GetBestMove();
        // printf("ddzend_search win2: %s %s\n", ret2.data(), ret.c_str());
        delete ab;
        return ret2.data();
    }
#ifdef __cplusplus
};
#endif