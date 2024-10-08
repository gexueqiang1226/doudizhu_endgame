#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ddzend_engine.h"

int *get_integer_array(lua_State *L, int index, int *size)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_pushvalue(L, index);
    lua_len(L, -1);
    int n = lua_tointeger(L, -1);
    lua_pop(L, 1);
    *size = n;
    int *array = malloc(n * sizeof(int));
    for (int i = 1; i <= n; ++i)
    {
        lua_pushinteger(L, i); // push key
        lua_gettable(L, -2);   // pop key,push value
        array[i - 1] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return array;
}

int get_integer_value_by_name(lua_State *L, int index, const char *name)
{
    lua_getfield(L, index, name);
    int value = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return value;
}

const char *get_string_value_by_name(lua_State *L, int index, const char *name)
{
    lua_getfield(L, index, name);
    const char *value = lua_tostring(L, -1);
    lua_pop(L, 1);
    return value;
}

/**
 * 需要的参数:
 * lord string 机器人的牌 A234567890JQKYZP(0-10, Y-小王, Z-大王, P-Pass)
 * farmer string 玩家的牌
 * last string 上把出的牌
 * turn int 谁出牌 0农民 1地主
 * states int  一些规则(保留字段)
 * return string 要出的牌 空串表示无法赢
 */
static int lua_ddz_endgame_call(lua_State *L)
{
    luaL_checktype(L, -1, LUA_TTABLE);
    const char *lord = get_string_value_by_name(L, -1, "lord");
    const char *farmer = get_string_value_by_name(L, -1, "farmer");
    const char *last = get_string_value_by_name(L, -1, "last");
    int turn = get_integer_value_by_name(L, -1, "turn");
    int states = get_integer_value_by_name(L, -1, "states");
    const char *result = NULL;
    int slen = 0;
    int check = ddzend_search(lord, farmer, last, turn, states, &result, &slen);
    printf("args lord: %s, farmer: %s, last: %s, turn: %d, states: %d, result:%s, slen:%d\n", lord, farmer, last, turn, states, result, slen);
    if (result)
    {
        lua_pushlstring(L, result, slen);
        free((void *)result);
    }
    else
    {
        lua_pushnil(L);
    }
    lua_pushinteger(L, check);
    return 2;
}

static const struct luaL_Reg reg[] = {
    {"call", lua_ddz_endgame_call},
    {NULL, NULL}};

int luaopen_ddz_endgame(lua_State *L)
{
    /* void luaL_newlib (lua_State *L, const luaL_Reg l[]);
     * 创建一个新的"table"，并将"l"中所列出的函数注册为"table"的域。
     */
    luaL_newlib(L, reg);

    return 1;
}
