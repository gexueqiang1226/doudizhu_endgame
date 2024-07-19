#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddz_hand.h"
#include "cardset.h"
#include "negamax.h"

static char* public_buffer;
static int public_buffer_index;
static int debugprint=1;

void printCardGroup(const struct CardGroup* group) {
    if (!group) return;
    for (int i = 0; i < group->num_sequences; ++i) {
        printf("出牌序列 %d: ", i + 1);
        for (int j = 0; j < group->sequences[i].num_cards; ++j) {
            printf("%d ", group->sequences[i].cards[j]);
        }
        printf("\n");
    }
}

int* get_integer_array(lua_State *L,int index,int *size)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushvalue(L,index);
    lua_len(L, -1);
    int n=lua_tointeger(L,-1);
    lua_pop(L,1);
    *size=n;
    int *array=malloc(n*sizeof(int));
    for (int i = 1; i <= n; ++i)
    {
        lua_pushinteger(L, i);  //push key
        lua_gettable(L, -2);  //pop key,push value
        array[i-1]=lua_tointeger(L,-1);
        lua_pop(L, 1);
    }
    lua_pop(L,1);
    return array;
}
//读取一个CardSequence
void getCardSequence(lua_State *L,int index,struct CardSequence* sequence)
{
    sequence->cards=get_integer_array(L,index,&sequence->num_cards);
}

//释放一个CardSequence
void freeSequence(struct CardSequence* sequence)
{
    if (sequence->cards)
    {
        free(sequence->cards);
        sequence->cards=NULL;
        sequence->num_cards=0;
    }
}

//输出CardSequence序列
void formatCardSequence(struct CardSequence* sequence,int *index)
{
    public_buffer[*index]='{';
    public_buffer[*index+1]=0;
    (*index)++;
    for (int i=0;i<sequence->num_cards;i++) {
        sprintf(public_buffer+*index,i==sequence->num_cards-1?"%d":"%d,",sequence->cards[i]);
        *index=*index+strlen(public_buffer+*index);
    }
    public_buffer[*index]='}';
    public_buffer[*index+1]=0;
    (*index)++;
}

//读取一个CardGroup
void getCardGroup(lua_State *L,int index,struct CardGroup* group)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushvalue(L,index);
    lua_len(L, -1);
    int n=lua_tointeger(L,-1);
    lua_pop(L,1);
    group->num_sequences=n;
    group->sequences=malloc(n*sizeof(struct CardSequence));
    struct CardSequence* ptr=group->sequences;
    for (int i = 1; i <= n; ++i)
    {
        lua_pushinteger(L, i);  //push key
        lua_gettable(L, -2);  //pop key,push value
        getCardSequence(L,-1,ptr);
        ptr++;
        lua_pop(L, 1);
    }
    lua_pop(L,1);
}

//释放一个CardGroup
void freeGroup(struct CardGroup* group)
{
    if (group->sequences)
    {
        for (int i=0;i<group->num_sequences;i++)
            freeSequence(group->sequences+i);
        free(group->sequences);
        group->sequences=NULL;
        group->num_sequences=0;
    }
}

//输出CardGroup序列
void formatCardGroup(struct CardGroup* group,int *index)
{
    public_buffer[*index]='{';
    public_buffer[*index+1]=0;
    (*index)++;
    for (int i=0;i<group->num_sequences;i++) {
        formatCardSequence(group->sequences+i,index);
        if (i!=group->num_sequences-1)
        {
            public_buffer[*index]=',';
            public_buffer[*index+1]=0;
            (*index)++;
        }
    }
    public_buffer[*index]='}';
    public_buffer[*index+1]=0;
    (*index)++;
}

//读取一个CardGroup
void getCardGroups(lua_State *L,int index,struct CardGroups* groups)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushvalue(L,index);
    lua_len(L, -1);
    int n=lua_tointeger(L,-1);
    lua_pop(L,1);
    groups->num_groups=n;
    groups->groups=malloc(n*sizeof(struct CardGroup));
    struct CardGroup* ptr=groups->groups;
    for (int i = 1; i <= n; ++i)
    {
        lua_pushinteger(L, i);  //push key
        lua_gettable(L, -2);  //pop key,push value
        getCardGroup(L,-1,ptr);
        ptr++;
        lua_pop(L, 1);
    }
    lua_pop(L,1);
}

//释放一个CardGroup
void freeGroups(struct CardGroups* groups)
{
    if (groups->groups)
    {
        for (int i=0;i<groups->num_groups;i++)
            freeGroup(groups->groups+i);
        free(groups->groups);
        groups->groups=NULL;
        groups->num_groups=0;
    }
}

//输出CardGroups序列
void formatCardGroups(struct CardGroups* groups,int *index)
{
    public_buffer[*index]='{';
    public_buffer[*index+1]=0;
    (*index)++;
    for (int i=0;i<groups->num_groups;i++) {
        formatCardGroup(groups->groups+i,index);
        if (i!=groups->num_groups-1)
        {
            public_buffer[*index]=',';
            public_buffer[*index+1]=0;
            (*index)++;
        }
    }
    public_buffer[*index]='}';
    public_buffer[*index+1]=0;
    (*index)++;
}

void init_public_buffer()
{
    if (public_buffer) free(public_buffer);
    public_buffer=malloc(65536);
    *public_buffer=0;
    public_buffer_index=0;
}

int get_integer_value_by_name(lua_State *L,int index,const char* name)
{
    lua_getfield(L,index,name);
    int value=lua_tointeger(L,-1);
    lua_pop(L,1);
    return value;
}

void CardSequence_by_name(lua_State *L,int index,const char* name,struct CardSequence *sequence)
{
    lua_getfield(L,index,name);
    getCardSequence(L,-1,sequence);
    lua_pop(L,1);
    if (debugprint)
    {
        init_public_buffer();
        formatCardSequence(sequence,&public_buffer_index);
        printf("%s=%s\n",name,public_buffer);
    }
}

void CardGroup_by_name(lua_State *L,int index,const char* name,struct CardGroup *group)
{
    lua_getfield(L,index,name);
    getCardGroup(L,-1,group);
    lua_pop(L,1);
    if (debugprint)
    {
        init_public_buffer();
        formatCardGroup(group,&public_buffer_index);
        printf("%s=%s\n",name,public_buffer);
    }
}

void CardGroups_by_name(lua_State *L,int index,const char* name,struct CardGroups *groups)
{
    lua_getfield(L,index,name);
    getCardGroups(L,-1,groups);
    lua_pop(L,1);
    if (debugprint)
    {
        init_public_buffer();
        formatCardGroups(groups,&public_buffer_index);
        printf("%s=%s\n",name,public_buffer);
    }
}

/**
 * 需要的参数:
 * lord Array(int) 地主的牌(AI的牌robot)
 * famer Array(int) 玩家的牌
 * last Array(int) 上把出的牌
 * states table  一些规则(保留字段)
 * 
 * return table 
 * {
 *  result int 结果标志 0-表示pass; 1-表示AI可以赢 -1-AI出啥牌都赢不了(这块儿硬解程序的返回没有意义)
 *  cards Array(int) 要出的牌
 * }
 */
static int call(lua_State *L) {
    luaL_checktype(L, -1, LUA_TTABLE);

    // 执行下面任意一个测试之前都需要下面这行初始化
    init_public_buffer();

    //取lord
    struct doudizhu_endgame player_cards;
    CardSequence_by_name(L,-1,"cards",&player_cards);

    //取other_hand_cards
    struct CardSequence other_hand_cards;
    CardSequence_by_name(L,-1,"other_hand_cards",&other_hand_cards);

    //取num_cards_left
    int size;
    lua_getfield(L,-1,"num_cards_left");
    int *num_cards_left=get_integer_array(L,-1,&size);
    lua_pop(L,1);
    if (debugprint)
    {
        printf("num_cards_left={");
        for (int i=0;i<size;i++)
        {
            printf("%d",num_cards_left[i]);
            if (i<size-1) printf(",");
        }
        printf("}\n");
    }

    //取played_out_cards
    struct CardGroups played_out_cards;
    CardGroups_by_name(L,-1,"played_out_cards",&played_out_cards);

    //取last_moves
    struct CardGroups last_moves;
    CardGroups_by_name(L,-1,"last_moves",&last_moves);

    //取card_play_action_seq
    struct CardGroup card_play_action_seq;
    CardGroup_by_name(L,-1,"card_play_action_seq",&card_play_action_seq);

    // 调用predict函数
    printf("调用predict函数\n");
    struct CardGroup* result = predict(player, &player_cards, num_cards_left, &other_hand_cards, &played_out_cards, &last_moves, bomb_num, &card_play_action_seq);

    // // 打印返回的出牌序列
    printf("返回的出牌序列:\n");
    printCardGroup(result);

    printf("函数调用成功.\n");
    lua_newtable(L);
    for (int i=0;i<result->num_sequences;i++)
    {
        lua_newtable(L);
        struct CardSequence *sequence=result->sequences+i;
        for (int j=0;j<sequence->num_cards;j++)
        {
            lua_pushinteger(L,sequence->cards[j]);
            lua_seti(L,-2,j+1);
        }
        lua_seti(L,-2,i+1);
    }
    freeCardGroup(result);
    // lua_pushstring(L,public_buffer);
    // lua_pushstring(L,"ok2");
    return 1;
}

static const struct luaL_Reg reg[] = {
    {"call", call},
    {NULL, NULL}
};

int luaopen_guandanai(lua_State* L)
{
    /* void luaL_newlib (lua_State *L, const luaL_Reg l[]);
     * 创建一个新的"table"，并将"l"中所列出的函数注册为"table"的域。
     */ 
    luaL_newlib(L, reg);

    return 1;
}

