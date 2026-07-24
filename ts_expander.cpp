#include "structs.h"
#include "debug.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <ranges>

using namespace std;

vector<set<string>> get_subsets(
    vector<string> props,
    int n)
{
    if (n == 0) {
        auto res = vector<set<string>>(2, set<string>());
        res[1].insert(props[0]);
        return res;
    }
    auto last = get_subsets(props, n-1);
    auto res = vector<set<string>>();
    res.insert(res.end(), last.begin(), last.end());
    res.insert(res.end(), last.begin(), last.end());
    for (int i = last.size(); i < last.size()*2; i++) {
        res[i].insert(props[n]);
    }
    return res;
}
vector<set<string>> get_subsets(
    set<string> props)
{
    return get_subsets(vector<string>(props.begin(), props.end()), props.size() - 1);
}

Ts expand(
    Ts& ts,
    set<string> out_props) 
{
    if (out_props.size() == 0) {
        return ts;
    }
    int num = 1 << out_props.size();
    auto subsets = get_subsets(out_props);
    // assert subsets.size == num

    auto new_props = vector<set<string>>(ts.props.size() * num, set<string>());
    for (int i = 0; i < new_props.size(); i++) {
        int si = i%num;
        int oi = i/num;

        merge(subsets[si].begin(), subsets[si].end(),
              ts.props[oi].begin(), ts.props[oi].end(),
              inserter(new_props[i], new_props[i].begin()));
    }
    
    int transit_size = ts.transit.size();
    auto new_transit = vector<vector<bool>>(transit_size * num, vector<bool>(transit_size * num));
    for (int i = 0; i < transit_size * num; i++) {
        for (int j = 0; j < transit_size; j++) {
            fill(new_transit[i].begin() + j*num, new_transit[i].begin() + (j+1)*num, ts.transit[i/num][j]);
        }
    }

    auto new_init_states = set<int>();
    for (int s: ts.init_states) {
        for (int i = s; i < s + num; i++) {
            new_init_states.insert(i);
        }
    }

    return {
        new_transit,
        new_props,
        new_init_states
    };
}
