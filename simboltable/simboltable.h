#pragma once

#include <stdio.h>
#include <string>
#include <unordered_map>

using namespace std;

class Env
{
private:
    std::unordered_map<string, string> table;
    Env *prev;
    int block = 0;

public:
    Env(int block = 0, Env *p = nullptr) : prev(p) {}
    void put(string s, const string sym);
    string get(string s);
};
