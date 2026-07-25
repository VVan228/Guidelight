#include "structs.h"
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

Node build_dfa(
    Ts ts,
    set<int> final_states,
    map<set<int>, Node>& nodes,
    set<string> visible_props);
