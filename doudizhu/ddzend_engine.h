#ifndef _DDZEND_ENGINE_H_
#define _DDZEND_ENGINE_H_
#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * lord string 地主的牌
     * farmer string 农民的牌
     * last string 上手牌
     * state int 规则
     */
    extern int ddzend_search(const char *lord, const char *farmer, const char *last, int turn, int states, const char **ret);

#ifdef __cplusplus
};
#endif

#endif