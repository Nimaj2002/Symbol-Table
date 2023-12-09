#include "simboltable.h"

// Env::Env(int b, Env *p) {
//     prev = p;
//     block = b;
// }

void Env::put(string s, string sym)
{
    table[s] = sym;
}

string Env::get(string s)
{
    for (const Env *e = this; e != nullptr; e = e->prev)
    {
        auto it = e->table.find(s);
        if (it != e->table.end())
        {
            return it->second;
        }
    }
    return "None";
}
