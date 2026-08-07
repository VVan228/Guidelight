#include "structs.h"
#include "debug.h"
#include "input.h"

#include <iostream>
#include <vector>
#include <random>
#include <set>
#include <format>
#include <algorithm>
#include <stdlib.h>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<double> dis(0.0, 1.0);
uniform_int_distribution<int> dis2(0, 25);


Ts generate_ts(
    int s,
    int p)
{
    auto transit = vector<vector<bool>>(s, vector<bool>(s));
    auto visited = set<int>();
    for (int i = 0; i<s; i++) {
        if (visited.contains(i)) {
            continue;
        }
        visited.insert(i);
        for (int j = 0; j<s; j++) {
            if (dis(gen) < 0.7) {
                transit[i][j] = true;
            }
        }
        for (int j = 0; j<s; j++) {
            if (transit[i][j] && !visited.contains(j)) {
                i = j;
                break;
            }
        }
    }

    auto all_props = set<string>();
    auto props = vector<set<string>>(s);
    while(all_props.size() < p) {
        all_props.insert(string(1, 'a' + dis2(gen)));
    }
    for (int i = 0; i<s; i++) {
        bool skip = true;
        for (int j = 0; j<s; j++) {
            if (transit[i][j]) {
                skip = false;
                break;
            }
        }
        if (skip) {
            continue;
        }
        for (string p: all_props) {
            if (dis(gen) < 0.5) {
                props[i].insert(p);
            }
        }
    }
    return {transit, props, set<int>({0})};
}

Ts reduce_ts(
    Ts ts)
{
    int s = ts.transit.size();
    auto transit = vector<vector<bool>>(s, vector<bool>(s));
    for (int i = 0; i<s; i++) {
        for (int j = 0; j<s; j++) {
            if (ts.transit[i][j] && dis(gen) < 0.7) {
                transit[i][j] = true;
            }
        }
    }

    auto all_props = set<string>();
    auto props = vector<set<string>>(s);
    for (int i = 0; i<s; i++) {
        for (auto p: ts.props[i]) {
            all_props.insert(p);
        }
    }
    for (auto iter = all_props.begin(); iter != all_props.end(); iter++) {
        if (dis(gen) < 0.5) {
            iter = all_props.erase(iter);
        }
    }
    for (int i = 0; i<s; i++) {
        for (auto p: ts.props[i]) {
            if (all_props.contains(p)) {
                props[i].insert(p);
            }
        }
    }

    return {transit, props, set<int>({0})};
}

string generate_formula(
    string agent,
    Ts ts,
    vector<int> path)
{
    string res = "";
    for(int i = 0; i<path.size(); i++){
        if (ts.props[path[i]].size() == 0) {
            continue;
        }
        //cout<<ts.props[path[i]].size()<<" ";
        auto iter = ts.props[path[i]].begin();
        int n = rand()%ts.props[path[i]].size();
        for (int j = 0; j<n; j++) {
            iter++;
        }
        string prop = *iter;

        string sub_res = format("\"{}\"", prop);
        for (int j = 0; j<i; j++) {
            sub_res = format("(X {})", sub_res);
        }
        if (i != 0) {
            res += " & ";
        }
        res += sub_res;
    }
    if (agent != "") {
        res = format("<<{}:{}>>", agent, res);
    }
    return res;
}

set<string> get_props(
    Ts ts)
{
    set<string> all_props = set<string>();
    for (int i = 0; i<ts.props.size(); i++) {
        for (auto p: ts.props[i]) {
            all_props.insert(p);
        }
    }
    return all_props;
}

vector<int> find_path(
    Ts ts)
{
    vector<int> path = vector<int>();
    set<int> visited = set<int>();
    for (int i = 0; i<ts.transit.size(); i++) {
        if (visited.contains(i)) {
            continue;
        }
        visited.insert(i);
        path.push_back(i);
        for (int j = 0; j<ts.transit.size(); j++) {
            if (ts.transit[i][j] && !visited.contains(j)) {
                i = j;
                break;
            }
        }
    }
    return path;
}

Sltl generate_sltl(
    int agents,
    int size,
    int num_props)
{
    auto all_agents = set<string>();
    while(all_agents.size() < agents) {
        all_agents.insert(string(1, 'A' + dis2(gen)));
    }

    Sltl res = {};
    Ts main_ts = generate_ts(size, num_props);
    res.main_ts = main_ts;
    res.all_props = get_props(main_ts);
    res.agents = map<string, Ts>();

    string formula = "";
    int i = 0;
    for (auto agent: all_agents) {
        Ts ts = reduce_ts(main_ts);
        formula += i!=0?" & ": "";
        vector<int> path = find_path(ts);
        cout<<agent<<": ";
        for (int i: path) {
            cout<<i<<" ";
        }
        string f = generate_formula(agent, ts, path);
        cout<<" "<<f;
        cout<<"\n";
        formula += f;
        res.agents[agent] = ts;
        res.visible_props[agent] = get_props(ts);
        i++;
    }
    res.formula = parse_formula(formula, res.all_props);
    return res;
}
