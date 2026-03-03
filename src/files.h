#pragma once

#include "src/common.h"
#include "src/list.h"

enum language {
    LANG_C
};

typedef struct {
    // project
    string name;
    enum language lang;
    m_list* flags;

    // include
    m_list* paths;
    m_list* libs;
} build_file;

build_file* read_build_file();

typedef struct {
    string path;
    long time;
} cache_entry;

m_list* read_build_cache();
void write_build_cache(m_list* cache);