#include <src/common.h>
#include <mstd/types/list.h>

bool isdir(const string dir);
list* recurse_dir(string dir, string pattern);