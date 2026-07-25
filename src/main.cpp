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
#include <algorithm>

Ts apply(
    Sltl& sltl,
    Ts ts,
    Formula& f)
{
    if (f.children.size() == 0) {
        return ts;
    }
    for (Formula sub_f: f.children) {
        cout<<"working on: "<<sub_f.agent<<"\n";
        Ts a_ts = apply(sltl, sltl.agents[sub_f.agent], sub_f);

        // TODO: different semantics
        set<string> observable = sltl.visible_props[sub_f.agent];

        set<string> extra = set<string>();
        set_difference(observable.begin(), observable.end(),
            sltl.visible_props[sub_f.agent].begin(), sltl.visible_props[sub_f.agent].end(),
            inserter(extra, extra.end()));
        if (extra.size() > 0) {
            cout<<"expansion\n";
            a_ts = expand(a_ts, extra);
        } else {
            cout<<"expansion skipped\n";
        }

        cout<<"getting sat...";
        auto sat = get_sat(a_ts, sub_f.formula);
        cout<<" done\n";

        auto dfa_nodes = map<set<int>, Node>();
        Node init_node = build_dfa(a_ts, sat, dfa_nodes, observable);

        parallel(ts, &init_node, sub_f.prop_name, observable);
    }
    return ts;
}

int main()
{
    cout<<"start\n";
    Sltl sltl = parse("resources/test.json");
    Ts res = apply(sltl, sltl.main_ts, sltl.formula);
    //print_ts(res);
    cout<<"sat for "<<sltl.formula.formula<<":\n";
    auto sat = get_sat(res, sltl.formula.formula);
    print_set(sat);
    cout<<"\n";
}

//int main() 
//{
//    cout<<"IN\n";
//    Sltl res = parse("resources/example.json");
//    print_sltl(res);
//    
//    Ts ts = res.agents["A"];
//
//    cout<<"EXPANDED TS\n";
//    Ts nts = expand(ts, {"a", "b", "c"});
//    print_ts(ts);
//    print_ts(nts);
//
//    cout<<"SAT\n";
//    string f = "E [ X \"p\" ]";
//    set<int> sat = get_sat(ts, f);
//    print_set(sat);
//    cout<<"\n";
//
//    cout<<"DFA\n";
//    auto all_nodes = map<set<int>, Node>();
//    Node init_node = build_dfa(nts, sat, all_nodes, set<string>({"p", "a", "b"}));
//    for (auto iter: all_nodes) {
//        print_node(iter.second);
//    }
//
//    cout<<"PAR\n";
//    parallel(nts, &init_node, "X", set<string>({"p", "a"}));
//    print_ts(nts);
//}

