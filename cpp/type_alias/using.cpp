#include <iostream>
#include <map>
using namespace std;

typedef std::map<int, std::string>::const_iterator map_const_iter;
map_const_iter iter;

using map_const_iter = std::map<int, std::string>::const_iterator;
map_const_iter iter;