#pragma once

#include "src/common.h"
#include <mstd/types/list.h>
#include <mstd/types/dict.h>

enum language {
    LANG_C
};

typedef struct {
    // project
    string name;
    enum language lang;
    list* flags;

    // include
    list* paths;
    list* libs;
} build_file;

build_file* read_build_file();

typedef struct {
    string path;
    long time;
} cache_entry;

list* read_build_cache();
void write_build_cache(list* cache);