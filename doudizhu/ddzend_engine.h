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
    extern char *search(char *lord, char *farmer, char *last, int turn, int state);

#ifdef __cplusplus
};
#endif

#endif