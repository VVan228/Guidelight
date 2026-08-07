#include "structs.h"
#include "debug.h"
#include "dfa_builder.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <format>
#include <map>
#include <algorithm>
#include <regex>
#include <iterator>

bool transition_match(
    set<string> transition,
    set<string> props,
    set<string> visible)
{
    auto transit_b = transition.begin();
    auto transit_e = transition.end();
    auto props_b = props.begin();
    auto props_e = props.end();
    auto visib_b = visible.begin();
    auto visib_e = visible.end();
    while (props_b != props_e && visib_b != visib_e) {
        if (*props_b > *visib_b) {
            visib_b++;
            continue;
        } 
        if (*visib_b > *props_b) {
            props_b++;
            continue;
        }
        if (*transit_b == *visib_b) {
            visib_b++;
            props_b++;
            // TODO: risky?
            transit_b++;
            continue;
        }
        return false;
    }
    return transit_b == transit_e;
}

void parallel_rec(
    Ts& ts,
    Node* cur_node,
    int cur_s,
    string newprop,
    set<pair<int, Node*>>& visited,
    set<string> visible_props)
{
    if (cur_node->final) {
        ts.props[cur_s].insert(newprop);
    }
    for (int state = 0; state<ts.transit.size(); state++) {
        if (!ts.transit[cur_s][state])
            continue;
        for (auto transition: cur_node->transitions) {
            if (visited.contains({state, transition.second}))
                continue;
            if (transition_match(transition.first, ts.props[state], visible_props)) 
            {
                visited.insert({state, transition.second});
                //cout<<"match "<<state<<"=(";
                //print_set(cur_node->states);
                //cout<<")->(";
                //print_set(transition.second->states);
                //cout<<") props: ";
                //print_set(ts.props[state]);
                //cout<<"~ ";
                //print_set(transition.first);
                //cout<<"\n";
                parallel_rec(ts, transition.second, state, newprop, visited, visible_props);
            }
        }
    }
}

void parallel(
    Ts& ts,
    Node* init_node,
    string newprop,
    set<string> visible_props)
{
    set<pair<int, Node*>> visited = set<pair<int, Node*>>();
    for (int state: ts.init_states) {
        for (auto transition: init_node->transitions) {
            if (transition_match(transition.first, ts.props[state], visible_props)) {
                visited.insert({state, transition.second});
                //cout<<"imatch "<<state<<"=(";
                //print_set(transition.second->states);
                //cout<<")\n";
                parallel_rec(ts, transition.second, state, newprop, visited, visible_props);
            }
        }
    }
}


//int main() 
//{
//    const int s = 4;
//    auto transit = vector<vector<bool>>(s, vector<bool>(s));
//    auto props = vector<set<string>>(s);
//    auto init_states = set<int>({0});
//
//    transit[0][1] = true;
//    transit[0][2] = true;
//    transit[1][1] = true;
//    transit[1][2] = true;
//    transit[1][3] = true;
//    transit[2][0] = true;
//    transit[2][1] = true;
//    transit[3][2] = true;
//    transit[3][3] = true;
//    props[2].insert("s");
//
//    Ts ts = {
//        transit,
//        props,
//        init_states
//    };
//    auto all_nodes = map<set<int>, Node>();
//    Node init_node = build_dfa(ts.transit, ts.init_states, set<int>({3}), ts.props, all_nodes);
//
//
//    auto transit2 = vector<vector<bool>>(s, vector<bool>(s));
//    auto props2 = vector<set<string>>(s);
//    auto init_states2 = set<int>({0});
//
//    transit2[0][1] = true;
//    transit2[0][2] = true;
//    transit2[1][1] = true;
//    transit2[1][2] = true;
//    transit2[1][3] = true;
//    transit2[2][0] = true;
//    transit2[2][1] = true;
//    transit2[3][2] = true;
//    transit2[3][3] = true;
//    props2[1].insert("p");
//    props2[1].insert("q");
//    props2[2].insert("s");
//    props2[3].insert("p");
//
//    Ts ts2 = {
//        transit2,
//        props2,
//        init_states2
//    };
//
//    for (auto iter: all_nodes) {
//        print_node(iter.second);
//    }
//    print_ts(ts2);
//    parallel(ts2, &init_node, "x", set<string>({"s"}));
//    print_ts(ts2);
//}
