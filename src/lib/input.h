#include "structs.h"
#include <string>

using namespace std;

Sltl parse(
    string filename);

Formula parse_formula(
    string f,
    set<string>& all_props);
