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
            printf("ddzend_search last %s, %d, %d \n", last_hand->hand.str().c_str(), last_hand->type, last_hand->power);
            if (!last_hand)
            {
                // 给的上手牌不合法
                return nullptr;
            }
        }
        doudizhu_endgame::Negamax *engine = new doudizhu_endgame::Negamax();
        printf("c1 %s \n", c1.str_h().c_str());
        printf("c2 %s \n", c2.str_h().c_str());
        doudizhu_endgame::TreeNode *root = engine->search(c1, c2, last_hand);

        printf("child len %d \n", root->child.size());
        if (!root->child.empty())
        {
            printf("child %s \n", root->child[0]->last_move->hand.str().c_str());
            ret = root->child[0]->last_move->hand.str();
        }

        delete engine;

        return ret.c_str();
    }

#ifdef __cplusplus
};
#endif