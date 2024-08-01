#ifndef DOUDIZHU_ENDGAME_HASHMAP_H
#define DOUDIZHU_ENDGAME_HASHMAP_H

#include "datastructure.h"
namespace doudizhu_endgame
{
    class HashCache
    {
        struct Key
        {
            std::multiset<byte> card1, card2;
            Pattern style;
            Key(const std::multiset<byte> &c1, const std::multiset<byte> &c2, const Pattern &s = Pattern())
            {
                card1 = c1;
                card2 = c2;
                style = s;
            }
            bool operator==(const Key &k) const
            {
                return card1 == k.card1 && card2 == k.card2 &&
                       (style.type < k.style.type || style.type == k.style.type);
            }
        };
        struct hashcode
        {
            unsigned long long operator()(const Key &k) const
            {
                unsigned long long res = 0;
                for (auto x : k.card1)
                {
                    res = (res << 1) ^ x;
                }
                for (auto x : k.card2)
                {
                    res = (res << 1) ^ x;
                }
                return res;
            }
        };
        struct Value
        {
            std::list<Pattern *> path;
            int score;
            int depth;
            Value() {}
            Value(const std::list<Pattern *> &p, int s = 0, int d = 0)
            {
                path = p;
                score = s;
                depth = d;
            }
        };
        // typedef unordered_map<Key, Value, hashcode, hashequal> _Hashmap;
        typedef std::unordered_map<Key, Value, hashcode> _Hashmap;
        _Hashmap _table;
        _Hashmap::iterator _it;

    public:
        bool find(const std::multiset<byte> &c1, const std::multiset<byte> &c2,
                  const Pattern &last, int depth)
        {
            _it = _table.find(Key(c1, c2, last));
            if (_it == _table.end())
                return false;
            return true;
            // if (_it->second.depth >= depth) return true;
            // return false;
        }
        Value &get()
        {
            return _it->second;
        }
        void put(std::multiset<byte> c1, std::multiset<byte> c2,
                 Pattern style, std::list<Pattern *> path, int depth, int score)
        {
            if (!find(c1, c2, style, depth))
            {
                _table[Key(c1, c2, style)] = Value(path, score, depth);
            }
            else
            {
                if (depth >= _it->second.depth)
                {
                    _it->second = Value(path, score, depth);
                }
            }
        }
    };
}
#endif