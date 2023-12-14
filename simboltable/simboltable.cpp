#include "simboltable.h"

Env::Env(int b, Env *p) : block(b), prev(p) {}

void Env::put(string s, Symbol sym)
{
    table[s] = sym;
}

Symbol Env::get(string s)
{
    for (const Env *e = this; e != nullptr; e = e->prev)
    {
        auto it = e->table.find(s);
        if (it != e->table.end())
        {
            return it->second;
        }
    }
    return null;
}

bool Env::isInCurrentTop(string s)
{
    auto it = table.find(s);
    if (it != table.end())
    {
        return 1;
    }
    else
    {
        return 0;
    }
};