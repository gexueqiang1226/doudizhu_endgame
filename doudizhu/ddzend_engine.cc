#include "ddzend_engine.h"
#include "negamax.h"

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
        doudizhu_endgame::Negamax *engine = new doudizhu_endgame::Negamax();
        doudizhu_endgame::TreeNode *root = nullptr;
        if (last_hand)
        {
            root = engine->search(c1, c2, last_hand);
        }
        else
        {
            root = engine->search(c1, c2);
        }
        if (!root->child.empty())
        {
            ret = root->child[0]->last_move->hand.str();
        }

        delete engine;

        return ret.data();
    }

#ifdef __cplusplus
};
#endif