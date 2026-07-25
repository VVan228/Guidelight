#include "structs.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

void set_is_final(
    Node* node,
    set<int> final_states)
{
    bool res = false;
    auto ns = node->states.begin();
    auto ne = node->states.end();
    auto fs = final_states.begin();
    auto fe = final_states.end();
    while (ns != ne && fs != fe) {
        if (*ns < *fs) {
            ns++;
        }
        else if (*fs < *ns) {
            fs++;
        }
        else {
            res = true;
            break;
        }
    }
    node->final = res;
}

map<set<string>, Node*> group_states(
    set<int> states,
    vector<set<string>> props,
    map<set<int>, Node>& nodes)
{
    auto transitions = map<set<string>, set<int>>();
    for (int state: states) {
        // TODO: intersection with D goes here
        // upd: or we can just filter the ts?
        transitions[props[state]].insert(state);
    }
    auto res = map<set<string>, Node*>();
    for (auto iter: transitions) {
        if (nodes.contains(iter.second)) {
            res[iter.first] = &nodes[iter.second];
        } else {
            nodes[iter.second] = {iter.second, map<set<string>, Node*>(), true};
            res[iter.first] = &nodes[iter.second];
        }
    }
    return res;
}

set<int> get_states(
    set<int> states, 
    vector<vector<bool>> transit)
{
    set<int> res = set<int>();
    for (int i: states) {
        for (int j = 0; j<transit[i].size(); j++) {
            if (transit[i][j]) {
                res.insert(j);
            }
        }
    }
    return res;
}

void build(
    Node* node,
    vector<vector<bool>> transit,
    set<int> final_states,
    vector<set<string>> props,
    map<set<int>, Node>& nodes)
{
    for(auto iter: node->transitions) {
        if (!iter.second->fresh) {
            continue;
        }
        iter.second->transitions = 
            group_states(
                get_states(iter.second->states, transit),
                props,
                nodes);
        iter.second->fresh = false;
        set_is_final(iter.second, final_states);
        build(iter.second, transit, final_states, props, nodes);
    }
}

Node build_dfa(
    Ts& ts,
    set<int> final_states,
    map<set<int>, Node>& nodes)
{
    Node init_node = {
        set<int>(),
        group_states(ts.init_states, ts.props, nodes),
        false
    };
    nodes[set<int>()] = init_node;

    build(&init_node, ts.transit, final_states, ts.props, nodes);

    return init_node;
}
