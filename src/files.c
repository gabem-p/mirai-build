#include <string.h>
#include <mstd/types/list.h>
#include "src/util.h"

#include "files.h"

bool consume(FILE* stream, string query) {
    long seek = ftell(stream);
    for (uint i = 0; i < strlen(query); i++)
        if (fgetc(stream) != query[i]) {
            fseek(stream, seek, SEEK_SET);
            return false;
        }
    return true;
}

bool check(FILE* stream, string query) {
    long start = ftell(stream);
    bool consumed = consume(stream, query);
    fseek(stream, start, SEEK_SET);
    return consumed;
}

bool ws(char query) {
    return (query == ' ' ||
            query == '\t' ||
            query == '\n' ||
            query == '\r');
}

bool alpha(char query) {
    return (query >= 'A' && query <= 'Z') ||
           (query >= 'a' && query <= 'z');
}

bool alphanum(char query) {
    return alpha(query) || (query >= '0' && query <= '9');
}

string readword(FILE* stream) {
    long seek = ftell(stream);
    uint length = 0;
    while (!feof(stream)) {
        if (!alphanum(fgetc(stream)))
            break;
        length++;
    }
    fseek(stream, seek, SEEK_SET);
    string str = malloc(length);
    fread(str, length, 1, stream);
    str[length] = '\0';
    return str;
}

string readto(FILE* stream, char query) {
    long seek = ftell(stream);
    uint length = 0;
    while (!feof(stream)) {
        if (fgetc(stream) == query)
            break;
        length++;
    }
    fseek(stream, seek, SEEK_SET);
    string str = malloc(length);
    fread(str, length, 1, stream);
    str[length] = '\0';
    return str;
}

string readline(FILE* stream) {
    long seek = ftell(stream);
    uint length = 0;
    while (!feof(stream)) {
        char c = fgetc(stream);
        if (c == '\r' || c == '\n')
            break;
        length++;
    }
    fseek(stream, seek, SEEK_SET);
    string str = malloc(length);
    fread(str, length, 1, stream);
    str[length] = '\0';
    return str;
}

void skipto(FILE* stream, char query) {
    long seek = ftell(stream);
    while (!feof(stream)) {
        seek = ftell(stream);
        if (fgetc(stream) == query)
            break;
    }
    fseek(stream, seek, SEEK_SET);
}

void skipws(FILE* stream) {
    long seek = ftell(stream);
    while (!feof(stream)) {
        seek = ftell(stream);
        if (!ws(fgetc(stream)))
            break;
    }
    fseek(stream, seek, SEEK_SET);
}

#define error(info) \
    fprintf(stderr, "%s%sInvalid build file:%s %s\n", COLOR_BOLD, COLOR_ERROR, COLOR_RESET, info); \
    fclose(file); \
    return null;

#define keycheck(_key, action) \
    if(strcmp(key, _key) == 0) { \
        action; \
        break; \
    }

enum category {
    CATEGORY_PROJECT,
    CATEGORY_INCLUDE
};
struct category_name {
    enum category category;
    string name;
};
struct category_name categories[] = {{CATEGORY_PROJECT, "project"}, {CATEGORY_INCLUDE, "include"}};

void set_lang(build_file* buildFile, string lang) {
}

build_file* read_build_file() {
    FILE* file = fopen("mirai.build", "r");
    if (file == null) {
        fprintf(stderr, "%s%sNo build file in directory.%s Use %smirai init%s to generate one\n",
                COLOR_BOLD, COLOR_ERROR, COLOR_RESET, COLOR_MAIN, COLOR_RESET);
        return null;
    }

    build_file* buildFile = calloc(1, sizeof(build_file));
    buildFile->flags = list_new();
    buildFile->paths = list_new();
    buildFile->libs = list_new();

    enum category category;
    while (!feof(file)) {
        skipws(file);

        if (check(file, "[")) {
            consume(file, "[");
            if (check(file, "]")) {
                error("empty category name");
            }
            string name = readword(file);
            if (!consume(file, "]")) {
                error("missing category closing bracket");
            }

            bool valid = false;
            for (uint i = 0; i < length(categories); i++) {
                if (strcmp(categories[i].name, name) != 0)
                    continue;
                valid = true;
                category = categories[i].category;
                break;
            }
            if (!valid) {
                error("unknown category name");
            }
            free(name);

            continue;
        }

        if (check(file, ":")) {
            error("empty key name");
        }
        string key = readword(file);
        if (!consume(file, ":")) {
            error("missing colon after key");
        }
        string value = readline(file);

        switch (category) {
        case CATEGORY_PROJECT:
            keycheck("name", buildFile->name = value);
            keycheck("language", set_lang(buildFile, value));
            keycheck("flag", list_add(buildFile->flags, value));
            break;
        case CATEGORY_INCLUDE:
            keycheck("path", list_add(buildFile->paths, value));
            keycheck("lib", list_add(buildFile->libs, value));
            break;
        }
        free(key);
    }

    return buildFile;
}

list* read_build_cache() {
    list* cache = list_new();

    if (!isdir("build")) {
        return cache;
    }

    FILE* file = fopen("build/build.cache", "r");
    if (file == null) {
        return cache;
    }

    while (!feof(file)) {
        skipws(file);
        cache_entry* entry = malloc(sizeof(cache_entry));
        entry->path = readto(file, ':');
        consume(file, ":");
        string value = readto(file, '\n');
        entry->time = atol(value);
        free(value);
        list_add(cache, entry);
    }

    fclose(file);
    return cache;
}

void write_build_cache(list* cache) {
    FILE* file = fopen("build/build.cache", "w");
    list_iterator* iterator = list_iter_new(cache);
    for (uint i = 0; i < cache->length; i++, list_iter_next(iterator)) {
        cache_entry* entry = (cache_entry*)list_iter_get(iterator);
        fprintf(file, "%s:%li%s", entry->path, entry->time, i == cache->length - 1 ? "" : "\n");
    }
    list_iter_cleanup(iterator);

    fclose(file);
}