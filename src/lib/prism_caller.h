#include "structs.h"
#include <set>

using namespace std;

set<int> get_sat(
    Ts& ts,
    Formula& f,
    PrismMode m);
