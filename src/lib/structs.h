#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;

#ifndef STRUCTS_H_
#define STRUCTS_H_

struct Node {
    set<int> states;
    map<set<string>, Node*> transitions;
    bool fresh;
    bool final;
};

struct Ts {
    vector<vector<bool>> transit;
    vector<set<string>> props;
    set<int> init_states;
};

struct Formula {
    set<Formula> children;
    string agent;
    string formula;
    string prop_name;
    friend bool operator<(const Formula& x, const Formula& y) 
    {
        return x.formula < y.formula;
    }
};

struct Sltl {
    map<string, Ts> agents;
    map<string, set<string>> visible_props;
    Ts main_ts;
    Formula formula;
};


#endif
