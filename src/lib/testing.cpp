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

    uniform_int_distribution<int> dis2(0, 26);
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
