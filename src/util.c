#include <dirent.h>
#include <string.h>
#include <fnmatch.h>
#include <stdarg.h>
#include <src/common.h>
#include <mstd/types/list.h>
#include <mstd/types/stack.h>

bool isdir(const string dir) {
    DIR* _dir = opendir(dir);
    if (_dir == null)
        return false;
    closedir(_dir);
    return true;
}

string path_concat(string prefix, string suffix) {
    string path = malloc(strlen(prefix) + strlen(suffix) + sizeof("/"));
    sprintf(path, "%s/%s", prefix, suffix);
    return path;
}

list* recurse_dir(string dir, string pattern) {
    stack* checkPaths = stack_new(64);
    stack_push(checkPaths, dir);

    list* matches = list_new();
    struct dirent* entry;

    while (true) {
        if (checkPaths->count == 0)
            break;

        string path = stack_pop(checkPaths);
        bool root = strcmp(path, ".") == 0;

        DIR* _dir = opendir(path);
        if (_dir == null)
            break;

        while ((entry = readdir(_dir)) != null) {

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            string entryPath = root ? entry->d_name : path_concat(path, entry->d_name);

            if (entry->d_type == DT_DIR) {
                if (entry->d_name[0] != '.' && strcmp(entry->d_name, "build") != 0)
                    stack_push(checkPaths, strdup(entryPath));
            } else if (fnmatch(pattern, entry->d_name, 0) == 0)
                list_add(matches, strdup(entryPath));
        }

        closedir(_dir);
    }

    stack_cleanup(checkPaths);
    return matches;
}