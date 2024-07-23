#ifndef _DDZEND_ENGINE_H__
#define _DDZEND_ENGINE_H__

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
    extern const char *ddzend_search(const char *lord, const char *farmer, const char *last, int turn, int states);

#ifdef __cplusplus
};
#endif

#endif