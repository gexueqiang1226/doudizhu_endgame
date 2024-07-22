#include "ddzend_engine.h"
#include "negamax.h"

#ifdef __cplusplus
extern "C"
{
#endif
    char *search(char *lord, char *farmer, char *last, int turn, int state)
    {

        if ((lord == nullptr || lord[0] != '\0') || (farmer == nullptr || farmer[0] != '\0'))
        {
            return nullptr;
        }
        doudizhu_endgame::CardSet c1, c2;
        c1.from_c_string(turn == 0 ? farmer : lord);
        c2.from_c_string(turn == 0 ? lord : farmer);
        doudizhu_endgame::Negamax *engine = new doudizhu_endgame::Negamax();
        char *ret = nullptr;
        if (last != nullptr && last[0] != '\0')
        {
            doudizhu_endgame::Pattern *last_hand = nullptr;
            doudizhu_endgame::DouDiZhuHand hand;
            doudizhu_endgame::CardSet ltcard;
            ltcard.from_c_string(last);
            last_hand = hand.check_hand(ltcard);
            if (!last_hand)
            {
                // 给的上手牌不合法
                return nullptr;
            }

            doudizhu_endgame::TreeNode *root = engine->search(c1, c2, last_hand);

            doudizhu_endgame::Pattern *last_move = nullptr;
            for (auto child : root->child)
            {
                if (child->last_move->hand.bitset_str() == last_hand->hand.bitset_str())
                {
                    last_move = child->last_move;
                    if (!child->child.empty())
                    {
                        ret = child->child[0]->last_move->hand.str().data();
                    }
                    break;
                }
            }

            if (!root->lord.empty() && last_move != nullptr)
            {
                c2.remove(last_move->hand);
                doudizhu_endgame::Pattern last_{last_move->power, last_move->type, last_move->hand};
                delete engine;
                engine = new doudizhu_endgame::Negamax();
                doudizhu_endgame::TreeNode *re = engine->search(c1, c2, &last_);
                if (!re->child.empty())
                {
                    ret = re->child[0]->last_move->hand.str().data();
                }
                else
                {
                    ret = "";
                }
            }
        }
        else
        {
            doudizhu_endgame::TreeNode *root = engine->search(c1, c2);
            if (root->score == 1 && !root->child.empty())
            {
                ret = root->child[0]->last_move->hand.str().data();
            }
            else
            {
                ret = "";
            }
        }
        if (engine)
        {
            delete engine;
        }
        return ret;
    }

#ifdef __cplusplus
};
#endif