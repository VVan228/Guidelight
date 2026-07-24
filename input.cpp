#include "json.hpp"
#include "structs.h"
#include "debug.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>

using json = nlohmann::json;
using namespace std;

string turn_to_prism_prop(
    string prop)
{
    return "\"" + prop + "\"";
}

int parse_form_rec(
    string f,
    Formula& subf,
    int start,
    function<string()> prop_namer)
{
    int i = start;
    int after_agent = start;

    string f_w_replaced_prop = f; 
    int len_diff = 0;

    for (; i < f.length()-1; i++) {
        if (f.at(i) == '<' && f.at(i+1) == '<') {
            Formula child = {set<Formula>(), "", "", ""};
            int newi = parse_form_rec(f, child, i+2, prop_namer);
            subf.children.insert(child);

            f_w_replaced_prop.replace(
                i - len_diff,
                newi - i + 1,
                turn_to_prism_prop(child.prop_name));
            len_diff = f.length() - f_w_replaced_prop.length();

            i = newi;
        }
        else if (f.at(i) == '>' && f.at(i+1) == '>') {
            // need to remember that len has changed because of replacement
            subf.formula = f_w_replaced_prop.substr(after_agent, i - after_agent - len_diff);

            i += 1;
            return i;
        }
        else if(f.at(i) == ':') {
            subf.agent = f.substr(start, i - start);
            subf.prop_name = prop_namer();
            after_agent = i+1;
        }
    }
    // the first subformula, if reached -> no agent (no closing bracket)
    subf.formula = f_w_replaced_prop;
    return i;
}

Formula parse_formula(
    string f)
{
    Formula ff = {set<Formula>(), "", "", ""};
    int i = 0;
    auto namer = [&]() -> string{i++;return string(i, '#');};
    parse_form_rec(f, ff, 0, namer);
    return ff;
}

Sltl parse(
    string filename)
{
    Sltl res = {};
    ifstream f(filename);
    json data = json::parse(f);

    res.agents = map<string, Ts>();
    res.visible_props = map<string, set<string>>();
    for (json a: data["agents"]) {
        Ts ts = {};

        int s = a["transitions"].size();
        string name = a["name"];

        ts.transit = vector<vector<bool>>(s, vector<bool>(s));
        int i = 0, j = 0;
        for (json row: a["transitions"]) {
            for (json val: row) {
                ts.transit[i][j] = (val == 1);
                j++;
            }
            j = 0;
            i++;
        }

        ts.init_states = set<int>();
        for (json state: a["initial_states"]) {
            ts.init_states.insert((int)state);
        }

        ts.props = vector<set<string>>(s);
        for (json prop: a["properties"]) {
            ts.props[(int)prop["state"]].insert(prop["name"]);
        }
        
        if (name == "_") {
            res.main_ts = ts;
        }
        else {
            res.agents[name] = ts;
        }

        res.visible_props[name] = set<string>();
        for (json prop: a["visible_properties"]) {
            res.visible_props[name].insert(prop);
        }
    }

    string formula = data["formula"];
    res.formula = {parse_formula(formula)};

    return res;
}

//int main()
//{
//    Formula f = parse_formula("xxxx<<1:111111<<2:22<<4:4>>2222>>11<<3:33<<5:5>>33<<6:66>>3>><<7:777>>>>");
//    print_formula(f);
//    Formula ff = parse_formula("<<1:11>><<2:22>>");
//    print_formula(ff);
//}
