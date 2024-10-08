function __G__TRACKBACK__(msg)
    if msg then print(tostring(msg)) end
    print(debug.traceback())
end

--- 获取表的内容长度
function table.nums(t)
    local count = 0
    for k, v in pairs(t) do count = count + 1 end
    return count
end

--- 字符串拆分
-- @param str 需要拆分的字符串
-- @param delimiter 拆分标记
-- @param toNum 是否将拆分出来的字符串转换成数字
function string.split(str, delimiter, toNum)
    str = tostring(str)
    delimiter = tostring(delimiter)
    if (delimiter == '') then return false end
    local pos, arr = 0, {}
    -- for each divider found
    for st, sp in function() return string.find(str, delimiter, pos, true) end do
        local ss = string.sub(str, pos, st - 1)
        if toNum then ss = tonumber(ss) end
        table.insert(arr, ss)
        pos = sp + 1
    end
    local ss = string.sub(str, pos)
    if toNum then ss = tonumber(ss) end
    table.insert(arr, ss)
    return arr
end

function string.trim(str)
    str = string.gsub(str, "^[ \t\n\r]+", "")
    return string.gsub(str, "[ \t\n\r]+$", "")
end

-- 去掉字符串左空白  
local function trim_left(s) return string.gsub(s, "^%s+", ""); end

-- 去掉字符串右空白  
local function trim_right(s) return string.gsub(s, "%s+$", ""); end

local function vs(v)
    if "string" == type(v) then
        v = string.gsub(v, '\n', '\\n') -- TODO remove me when no return key in game
        v = string.gsub(v, '\\', '\\\\')
        return '\'' .. string.gsub(v, '\'', '\\\'') .. '\''
    else
        return 'table' == type(v) and tts(v) or tostring(v)
    end
end

local function ks(k)
    if 'string' == type(k) and string.match(k, '^[_%a][_%a%d]*$') then
        return k
    else
        return '[' .. vs(k) .. ']'
    end
end

---将表内容转成字符串
function tts(t)
    local s, done = {}, {}
    for k, v in ipairs(t) do
        table.insert(s, vs(v))
        done[k] = true
    end
    for k, v in pairs(t) do
        if not done[k] then table.insert(s, ks(k) .. '=' .. vs(v)) end
    end
    return '{' .. table.concat(s, ',') .. '}'
end

local function save(str, path)
    local file = assert(io.open(path, "w"))
    file:write(str)
    io.close()
end

local function formatLua(t)
    local ret = {}
    for i, row in ipairs(t) do
        local keys = {}
        for k, _ in pairs(row) do table.insert(keys, k) end
        table.sort(keys)
        local str = {}
        for _, k in ipairs(keys) do
            local value = row[k]
            if type(value) == 'string' then
                value = table.concat({'"', value, '"'}, "")
            elseif type(value) == 'table' then
                value = table.concat({'{', table.concat(value, ", "), '}'}, "")
            end
            table.insert(str, table.concat({k, value}, " = "))
        end

        table.insert(ret, table.concat(
                         {"[", i, "] = {", table.concat(str, ", "), "}"}, ""))
    end
    return ret
end

local function formatLua2(t)
    local ret = {}
    for i, row in pairs(t) do
        local t = formatLua(row)
        local str = table.concat(t, ",\n\t\t")
        table.insert(ret,
                     table.concat({"[", i, "] = {\n\t\t", str, "\n\t}"}, ""))
    end
    return ret
end

local function float2num(amout)
    if not amout then return 0 end
    return math.floor(amout * 100 + 0.5) / 100
end

local function _splitScore(score, count)
    local scores = {}
    if count <= 1 then
        scores[1] = score
    else
        local pts = {}
        for i = 1, count - 1 do
            table.insert(pts, math.floor(math.random() * score))
        end
        table.insert(pts, 0)
        table.insert(pts, score)
        table.sort(pts)
        for i = 1, count do scores[i] = pts[i + 1] - pts[i] end
    end
    return scores
end

local function getCards(str)
    local cards = type(str) == "table" and str or string.split(str, '|', true)
    local card_list = {
        [1] = 'A',
        [2] = '2',
        [3] = '3',
        [4] = '4',
        [5] = '5',
        [6] = '6',
        [7] = '7',
        [8] = '8',
        [9] = '9',
        [10] = '0',
        [11] = 'J',
        [12] = 'Q',
        [13] = 'K',
        [53] = 'Y',
        [54] = 'Z'
    }
    local list = {}
    if cards and next(cards) then
        for i, v in ipairs(cards) do
            local mod = v > 52 and v or (v - 1) % 13 + 1
            table.insert(list, card_list[mod])
        end
    end

    return table.concat(list, '')
end

xpcall(function()
    -- 程序入口
    print("!!! BAT START !!!")
    local ddz_endgame = require("ddz_endgame")
    print("输入机器人的牌:")
    local lord = getCards(io.read())
    print("输入玩家的牌:")
    local farmer = getCards(io.read())
    print("输入上手出的牌:")
    local last = getCards(io.read())
    print("该谁出牌了(0-玩家 1-机器人):")
    local turn = tonumber(io.read()) or 0
    local win, ret, check
    local games = 1
    repeat
        if turn == 1 then
            print("请输入机器人出牌:")
            ret = io.read()
            last = ret ~= "P" and ret or ""
            for char in ret:gmatch(".") do
                -- 对每个字符执行操作
                lord = string.gsub(lord, "(.*)(" .. char .. ")(.*)", "%1%3")
            end
            print("=====" .. games .. "======")
            print("机器人出:", ret)
            print("玩家剩余牌:", farmer)
            print("机器人剩余牌:", lord)
            print("===========")
            if lord == "" then
                win = 1
                break
            end
        else
            ret, check = ddz_endgame.call({
                lord = lord,
                farmer = farmer,
                last = last,
                turn = turn,
                states = 0
            })
            print("ret:", ret, ";check:", check)
            if games == 1 and not ret then
                -- 输入参数有问题
                print("error: 输入的参数有问题")
                return
            end
            if ret == "" then
                print("必输牌, 是否还玩儿下去(y/n):")
                local input = io.read()
                if input == "n" then
                    win = 1
                    break
                end
                print("请输入玩家出牌:")
                ret = io.read()
            end

            last = ret ~= "P" and ret or ""
            for char in ret:gmatch(".") do
                -- 对每个字符执行操作
                farmer = string.gsub(farmer, "(.*)(" .. char .. ")(.*)", "%1%3")
            end
            print("=====" .. games .. "======")
            print("玩家出:", ret)
            print("玩家剩余牌:", farmer)
            print("机器人剩余牌:", lord)
            print("===========")
            if farmer == "" then
                win = 0
                break
            end
        end
        turn = 1 - turn
        games = games + 1
    until false
    print("谁赢了:", (win == 0 and "玩家" or "机器人"))
    print("!!! BAT END !!!")
end, __G__TRACKBACK__)
