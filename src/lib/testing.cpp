#include "structs.h"

#include <iostream>
#include <vector>
#include <random>
#include <set>

using namespace std;

Ts generate_ts(
    int s,
    int p)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

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

    uniform_int_distribution<int> dis2(0, 25);
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

Ts trim_ts(
    Ts ts)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    auto transit = ts.transit;
    for (int i = 0; i<transit.size(); i++) {
        for (int j = 0; j<transit.size(); j++) {
            if (transit[i][j] && dis(gen) < 0.3) {
                transit[i][j] = false;
            }
        }
    }

    auto all_props = set<string>();
    auto props = ts.props;
    for (int i = 0; i<props.size(); i++) {
        for (auto p: props[i]) {
            all_props.insert(p);
        }
    }
    for (auto iter = all_props.begin(); iter != all_props.end(); iter++) {
        if (dis(gen) < 0.5) {
            iter = all_props.erase(iter);
        }
    }
    for (int i = 0; i<props.size(); i++) {
        if (props[i].size() == 0)
            continue;
        for (auto iter = props[i].begin(); iter != props[i].end(); iter++) {
            if (!all_props.contains(*iter)) {
                iter = props[i].erase(iter);
            }
        }
    }

    return {transit, props, set<int>({0})};
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

Sltl generate_sltl(
    int agents,
    int size,
    int num_props)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis2(0, 25);
    auto all_agents = set<string>();
    while(all_agents.size() < agents) {
        all_agents.insert(string(1, 'A' + dis2(gen)));
    }

    Sltl res = {};
    Ts main_ts = generate_ts(size, num_props);
    res.main_ts = main_ts;
    res.all_props = get_props(main_ts);
    res.agents = map<string, Ts>();

    for (auto agent: all_agents) {
        Ts ts = trim_ts(main_ts);
        res.agents[agent] = ts;
        res.visible_props[agent] = get_props(ts);
    }
    return res;
}
