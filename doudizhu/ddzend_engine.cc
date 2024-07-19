#include "ddzend_engine.h"
#include "negamax.h"

#ifdef __cplusplus
extern "C"
{
#endif
    char *search(char *lord, char *farmer, char *last, int turn, int state)
    {
        doudizhu_endgame::CardSet lcard, fcard;
        lcard.from_c_string(lord);
        fcard.from_c_string(farmer);
        doudizhu_endgame::Negamax *engine = new doudizhu_endgame::Negamax();
        
        delete engine;
    }

#ifdef __cplusplus
};
#endif