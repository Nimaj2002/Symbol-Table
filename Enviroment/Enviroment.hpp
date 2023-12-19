#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;
enum Symbol
{
    BOOL,
    INT,
    FLOAT,
    CHAR,
    null
};
class Env
{
private:
public:
    Env *prev;
    unordered_map<string, Symbol> table;
    int block = 0;
    Env(int block = 0, Env *p = nullptr);
    void put(string s, Symbol sym);
    Symbol get(string s);
    bool isInCurrentTop(string s);
    int getBlockNumber(string s);
};
