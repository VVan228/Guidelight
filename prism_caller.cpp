#include "structs.h"
#include "debug.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <format>
#include <map>
#include <algorithm>
#include <regex>
#include <iterator>
// exec:
#include <cstdarg>
#include <fstream>
#include <memory>
#include <cstdio>
#include <unistd.h>

using namespace std;

// TODO: remove hardcoded
const char* PRISM = "~/Programming/study/sltl/prism-4.10.1-mac64-arm/bin/prism";

string exec(const char* cmd) {
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

set<int> get_results(
    string out)
{
    auto sat = set<int>();

    regex r("^(\\d:\\(\\d\\))$", regex::multiline);
    smatch m;

    auto sbegin = sregex_iterator(out.begin(), out.end(), r);
    auto send = sregex_iterator();
    
    for (auto i = sbegin; i != send; i++)
    {
        string state = (*i).str();
        int bpos = state.find("(");
        sat.insert(stoi(state.substr(bpos+1, state.length()-bpos)));
    }
    // init state is fake
    if (*sat.begin() == -1)
        sat.erase(sat.begin());
    return sat;
}

string write_to_file(
    string type,
    string s)
{
    char filename[] = "/tmp/prismexport.XXXXXX";
    int fd = mkstemp(filename);    
    if (fd == -1) {
        return "";
        cout<<format("Error creating temp {} file\n", type);
    }
    write(fd, s.c_str(), s.size());
    close(fd);
    return string(filename);
}

string export_ts(
    Ts& ts)
{
    string res = "";
    res += "mdp\n";
    res += "module M1\n";
    res += format("\ts: [-1..{}] init -1;\n", ts.transit.size()-1);
    for (int s: ts.init_states) {
        res += format("\t[] s=-1 -> (s'={});\n", s);
    }
    for (int i = 0; i<ts.transit.size(); i++) {
        for (int j = 0; j<ts.transit[i].size(); j++) {
            if (ts.transit[i][j])
                res += format("\t[] s={} -> (s'={});\n", i, j);
        }
    }
    res += "endmodule";
    return write_to_file("ts", res);
}

string export_formula(
    Ts& ts,
    Formula f)
{
    string res = "";
    auto labels = map<string, string>();
    for (int i = 0; i<ts.props.size(); i++) {
        for (string p: ts.props[i]) {
            if (!labels.contains(p)) {
                labels[p] = format("label \"{}\" = s={}", p, i);
            } else {
                labels[p] = labels[p] + format(" | s={}", i);
            }
        }
    }
    for (auto it: labels) {
        res += format("{};\n", it.second);
    }
    res += format("filter(printall, {});", f.text);
    return write_to_file("formula", res);
}

set<int> get_sat(
    Ts& ts,
    Formula f)
{
    string tsfile = export_ts(ts);
    string formulafile = export_formula(ts, f);

    string res = exec(format("{} {} {}", PRISM, tsfile, formulafile).c_str());
    set<int> sat = get_results(res);

    unlink(tsfile.c_str());
    unlink(formulafile.c_str());
    return sat;
}
