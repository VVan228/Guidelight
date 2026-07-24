#include "structs.h"
#include "debug.h"
#include "dfa_builder.h"
#include "prism_caller.h"
#include "ts_expander.h"
#include "parallel.h"
#include "input.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>

Ts get_ts()
{
    const int s = 3;
    auto transit = vector<vector<bool>>(s, vector<bool>(s));
    auto props = vector<set<string>>(s);
    auto init_states = set<int>({0});

    transit[0][0] = true;
    transit[0][1] = true;
    transit[0][2] = true;
    transit[1][0] = true;
    transit[2][1] = true;
    props[1].insert("p");

    return {
        transit,
        props,
        init_states
    };
}

int main() 
{
    Ts ts = get_ts();

    cout<<"EXPANDED TS\n";
    Ts nts = expand(ts, {"a", "b", "c"});
    print_ts(ts);
    print_ts(nts);

    cout<<"SAT\n";
    string f = "E [ X \"p\" ]";
    set<int> sat = get_sat(ts, f);
    print_set(sat);
    cout<<"\n";

    cout<<"DFA\n";
    auto all_nodes = map<set<int>, Node>();
    Node init_node = build_dfa(ts.transit, ts.init_states, sat, ts.props, all_nodes);
    for (auto iter: all_nodes) {
        print_node(iter.second);
    }

    cout<<"PAR\n";
    parallel(nts, &init_node, "X", set<string>({"p"}));
    print_ts(nts);

    cout<<"IN\n";
    Sltl res = parse("example.json");
    for (auto a: res.agents) {
        cout<<a.first<<"\n";
        print_ts(a.second);
        cout<<"visible properties: ";
        print_set(res.visible_props[a.first]);
    }
    cout<<"\n";
    print_formula(res.formula);
}

