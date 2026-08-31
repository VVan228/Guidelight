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
#include <chrono>
#include <cassert>

bool VERBOSE = false;


// TIME
chrono::steady_clock::time_point get_time()
{
    return std::chrono::steady_clock::now();
}

int time_diff(
    chrono::steady_clock::time_point begin,
    chrono::steady_clock::time_point end)
{
    return chrono::duration_cast<std::chrono::milliseconds> (end - begin).count();
}

void print_time(
    string category,
    chrono::steady_clock::time_point begin)
{
    cout<<"time::"<<category<<"="<<time_diff(begin, get_time())<<"\n";
}

// GET_OBS
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

// MAIN
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
    if (f.agent != "") {
        set<string> extra = set<string>();
        auto begin = get_time();
        set_difference(cur_obs.begin(), cur_obs.end(),
            sltl.visible_props[f.agent].begin(), sltl.visible_props[f.agent].end(),
            inserter(extra, extra.end()));
        if (extra.size() > 0) {
            ts = expand(ts, extra);
            if (VERBOSE) {
                print_time("expand", begin);
                print_ts(ts);
            }
        }        
    }
    if (f.children.size() == 0) {
        return ts;
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

        auto begin = get_time();
        auto sat = get_sat(a_ts, sub_f, PrismMode::exists);
        if (VERBOSE)
            print_time("sat", begin);

        auto dfa_nodes = map<set<int>, Node>();
        auto begin2 = get_time();
        Node init_node = build_dfa(a_ts, sat, dfa_nodes, child_obs);
        if (VERBOSE) {
            print_time("dfa_constr", begin2);
            cout<<sub_f.agent<<" DFA\n";
            for (auto iter: dfa_nodes) {
                print_node(iter.second);
            }
        }
        
        auto begin3 = get_time();
        parallel(ts, &init_node, sub_f.prop_name, child_obs);
        if (VERBOSE) {
            print_time("parallel", begin3);
            print_ts(ts);
        }
    }
    return ts;
}

// HELPER
set<int> apply(
    Sltl& sltl,
    Semantics sem)
{
    Ts res;
    auto begin = get_time();
    if (sem == Semantics::decr) {
        res = apply_i(sltl, sltl.main_ts, sltl.formula, sem, set<string>(), sltl.all_props);
    }
    res = apply_i(sltl, sltl.main_ts, sltl.formula, sem, set<string>(), set<string>());
    if (VERBOSE) {
        print_time("full", begin);
        print_formula(sltl.formula);
    }
    return get_sat(res, sltl.formula, PrismMode::always);
}

Semantics parse_sem(
    string sem_s)
{
    Semantics sem;
    if (sem_s == "pub") {
        sem = Semantics::pub;
    } else if (sem_s == "pobs") {
        sem = Semantics::pobs;
    } else if (sem_s == "step") {
        sem = Semantics::step;
    } else if (sem_s == "incr") {
        sem = Semantics::incr;
    } else if (sem_s == "decr") {
        sem = Semantics::decr;
    }
    return sem;
}

int main(int argc, char* argv[])
{
    for (int i = 1; i<argc; i++) {
        auto a = string(argv[i]);
        if (a == "-V") {
            VERBOSE = true;
            continue;
        }
        if (a == "-R") {
            assert(argc-i >= 4);

            int num_agents, num_states, num_props;
            num_agents = atoi(argv[i+1]);
            num_states = atoi(argv[i+2]);
            num_props = atoi(argv[i+3]);
            Semantics sem = parse_sem(string(argv[i+4]));
            Sltl sltl = generate_sltl(num_agents, num_states, num_props);
            auto sat = apply(sltl, sem);
            cout<<"sat: ";
            print_set(sat);
            cout<<"\n";

            i += 5;
            continue;
        }
        if (a == "-F") {
            assert(argc-i >= 2);

            //"resources/test_2.json"
            Sltl sltl = parse(string(argv[i+1]));
            Semantics sem = parse_sem(string(argv[i+2]));
            auto res = apply(sltl, sem);
            cout<<"sat for "<<sltl.formula.formula<<":";
            print_set(res);
            cout<<"\n";

            i += 3;
            continue;
        }
    }
}
