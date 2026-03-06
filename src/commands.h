#pragma once

#include "src/common.h"
#include "src/files.h"

int build(build_file* buildFile, int argc, string argv[]);
int run(build_file* buildFile, int argc, string argv[]);