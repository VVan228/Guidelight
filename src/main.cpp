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

bool VERBOSE = false;

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
    set<string> cur_obs,
    set<string> q_in)
{
    if (VERBOSE)
        print_ts(ts);
    if (f.children.size() == 0) {
        return ts;
    }
    if (f.agent != "") {
        set<string> extra = set<string>();
        set_difference(cur_obs.begin(), cur_obs.end(),
            sltl.visible_props[f.agent].begin(), sltl.visible_props[f.agent].end(),
            inserter(extra, extra.end()));
        if (extra.size() > 0) {
            ts = expand(ts, extra);
            if (VERBOSE)
                print_ts(ts);
        }        
    }

    for (Formula sub_f: f.children) {

        set<string> child_obs;
        set<string> q_out;
        switch (sem) {
            case pobs:
                child_obs = sltl.visible_props[sub_f.agent];
                q_out = q_in;
                break;
            case pub:
                child_obs = sltl.all_props;
                q_out = q_in;
                break;
            case step:
                child_obs = set<string>();
                q_out = q_in;
                break;
            case incr:
                child_obs = get_q_incr(q_in, sltl.visible_props[sub_f.agent]);
                q_out = child_obs;
                break;
            case decr:
                child_obs = get_q_decr(q_in, sltl.visible_props[sub_f.agent]);
                q_out = child_obs;
                break;
        }

        Ts a_ts = apply_i(sltl, sltl.agents[sub_f.agent], sub_f, sem, child_obs, q_out);

        auto sat = get_sat(a_ts, sub_f);

        auto dfa_nodes = map<set<int>, Node>();
        Node init_node = build_dfa(a_ts, sat, dfa_nodes, child_obs);
        if (VERBOSE) {
            cout<<sub_f.agent<<" DFA\n";
            for (auto iter: dfa_nodes) {
                print_node(iter.second);
            }
        }
        
        parallel(ts, &init_node, sub_f.prop_name, child_obs);
        if (VERBOSE)
            print_ts(ts);
    }
    return ts;
}

set<int> apply(
    Sltl& sltl,
    Semantics sem)
{
    Ts res;
    if (sem == Semantics::decr) {
        res = apply_i(sltl, sltl.main_ts, sltl.formula, sem, set<string>(), sltl.all_props);
    }
    res = apply_i(sltl, sltl.main_ts, sltl.formula, sem, set<string>(), set<string>());
    return get_sat(res, sltl.formula);
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

int main(int argc, char* argv[])
{
    if (argc > 1 && argv[1][0] == 'V') {
        VERBOSE = true;
    }

    Sltl sltl = parse("resources/test_2.json");
    auto res = apply(sltl, Semantics::step);
    cout<<"sat for "<<sltl.formula.formula<<":";
    print_set(res);
    cout<<"\n";
}
