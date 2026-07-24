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

int main()
{
    Sltl sltl = parse("_test.json");
    print_sltl(sltl);
}

//int main() 
//{
//    cout<<"IN\n";
//    Sltl res = parse("_example.json");
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
//    Node init_node = build_dfa(ts.transit, ts.init_states, sat, ts.props, all_nodes);
//    for (auto iter: all_nodes) {
//        print_node(iter.second);
//    }
//
//    cout<<"PAR\n";
//    parallel(nts, &init_node, "X", set<string>({"p"}));
//    print_ts(nts);
//}

