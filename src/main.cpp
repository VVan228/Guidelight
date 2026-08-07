#include "structs.h"
#include "debug.h"
#include "dfa_builder.h"
#include "prism_caller.h"
#include "ts_expander.h"
#include "parallel.h"
#include "input.h"
#include "testing.h"

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

set<string> get_q_incr(
    set<string> q_in,
    set<string> visible_props)
{
    set<string> q_out = set<string>();
    set_union(
        q_in.begin(), q_in.end(),
        visible_props.begin(), visible_props.end(),
        inserter(q_out, q_out.end())
    );
    return q_out;
}

set<string> get_q_decr(
    set<string> q_in,
    set<string> visible_props)
{
    set<string> q_out = set<string>();
    set_intersection(
        q_in.begin(), q_in.end(),
        visible_props.begin(), visible_props.end(),
        inserter(q_out, q_out.end())
    );
    return q_out;
}

Ts apply_i(
    Sltl& sltl,
    Ts ts,
    Formula& f,
    Semantics sem,
    set<string> q_in)
{
    if (f.children.size() == 0) {
        return ts;
    }
    for (Formula sub_f: f.children) {

        set<string> observable;
        set<string> q_out;
        switch (sem) {
            case pobs:
                observable = sltl.visible_props[sub_f.agent];
                q_out = q_in;
                break;
            case pub:
                observable = sltl.all_props;
                q_out = q_in;
                break;
            case step:
                observable = set<string>();
                q_out = q_in;
                break;
            case incr:
                observable = get_q_incr(q_in, sltl.visible_props[sub_f.agent]);
                q_out = observable;
                break;
            case decr:
                observable = get_q_decr(q_in, sltl.visible_props[sub_f.agent]);
                q_out = observable;
                break;
        }

        cout<<"working on: "<<sub_f.agent<<"\n";
        Ts a_ts = apply_i(sltl, sltl.agents[sub_f.agent], sub_f, sem, q_out);

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
        auto sat = get_sat(a_ts, sub_f);
        cout<<" done\n";

        auto dfa_nodes = map<set<int>, Node>();
        Node init_node = build_dfa(a_ts, sat, dfa_nodes, observable);

        parallel(ts, &init_node, sub_f.prop_name, observable);
    }
    return ts;
}

Ts apply(
    Sltl& sltl,
    Semantics sem)
{
    if (sem == Semantics::decr) {
        return apply_i(sltl, sltl.main_ts, sltl.formula, sem, sltl.all_props);
    }
    return apply_i(sltl, sltl.main_ts, sltl.formula, sem, set<string>());
}

//int main()
//{
//    for (int i = 0; i<10; i++) {
//        Sltl sltl = generate_sltl(3, 5, 5);
//        Ts res = apply(sltl, Semantics::pobs);
//        auto sat = get_sat(res, sltl.formula.formula);
//        if (!sat.contains(0)) {
//            print_sltl(sltl);
//            print_ts(res);
//            break;
//        }
//        cout<<"one done;\n";
//    }
//}

int main()
{
    cout<<"start\n";
    Sltl sltl = parse("resources/test.json");
    print_sltl(sltl);
    Ts res = apply(sltl, Semantics::pobs);
    for (auto i = res.props[0].begin(); i!=res.props[0].end(); i++) {
        if (*i == "A0") {
            res.props[0].erase(i);
            break;
        }
    }
    print_ts(res);
    cout<<"sat for "<<sltl.formula.formula<<":\n";
    auto sat = get_sat(res, sltl.formula);
    print_set(sat);
    cout<<"\n";
    

}

//int main()
//{
//    Sltl sltl = parse("resources/q_test.json");
//    print_sltl(sltl);
//    Ts res = apply(sltl, sltl.main_ts, sltl.formula, Semantics::decr);
//}

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

