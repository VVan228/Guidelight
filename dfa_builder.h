#include "structs.h"
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

Node build_dfa(
    vector<vector<bool>> transit,
    set<int> init_states,
    set<int> final_states,
    vector<set<string>> props,
    map<set<int>, Node>& nodes);
