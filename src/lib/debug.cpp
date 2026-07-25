#include "structs.h"
#include <string>
#include <iostream>
#include <vector>
#include <set>

using namespace std;

template<typename T>
void print_set(
    set<T> s) 
{
    for (T i: s) {
        cout<<i<<" ";
    }
}
template<typename T>
void print_vector(
    vector<T> s) 
{
    for (T i: s) {
        cout<<i<<" ";
    }
}

void print_ts(
    Ts& ts)
{
    cout<<"init: ";
    print_set(ts.init_states);
    cout<<"\n";
    for (int i = 0; i<ts.transit.size(); i++) {
        print_vector(ts.transit[i]);
        cout<<"\n";
    }
    for (int i = 0; i<ts.props.size(); i++) {
        cout<<i<<": ";
        print_set(ts.props[i]);
        cout<<"\n";
    }
}


void print_node(
    Node* node) 
{
    cout<<"node: ";
    if (node->states.size() == 0) {
        cout<<"init_node";
    }
    else {
        print_set(node->states);
    }
    if (node->fresh) {
        cout<<"(fresh)";
    }
    if (node->final) {
        cout<<"(fin)";
    }
    cout<<"\n";
    for (auto iter: node->transitions)
    {
        cout<<"<";
        print_set(iter.first);
        cout<<"> -> ";
        print_set(iter.second->states);
        if (iter.second->fresh) {
            cout<<"(fresh)";
        }
        cout<<"\n";
    }
}

void print_node(
    Node node) 
{
    print_node(&node);
}

void print_formula_p(
    Formula f,
    int depth)
{
    for (int i = 0; i<depth; i++) {
        cout<<"   ";
    }
    if (f.agent != "") {
        cout<<f.agent<<"("<<f.prop_name<<")"<<"\n";
    }
    for (int i = 0; i<depth; i++) {
        cout<<"   ";
    }
    cout<<f.formula<<"\n";
    for (auto child: f.children) {
        print_formula_p(child, depth + 1);
    }
}

void print_formula(
    Formula f)
{
    print_formula_p(f, 0);
}

void print_sltl(
    Sltl sltl)
{
    if (sltl.main_ts.init_states.size() > 0) {
        cout<<"main ts:\n";
        print_ts(sltl.main_ts);
    }
    cout<<"\nagents ts'\n";
    for (auto a: sltl.agents) {
        cout<<a.first<<"\n";
        print_ts(a.second);
        cout<<"visible properties: ";
        print_set(sltl.visible_props[a.first]);
        cout<<"\n";
    }
    cout<<"\nall props: ";
    print_set(sltl.all_props);
    cout<<"\n\n";
    print_formula(sltl.formula);
}
