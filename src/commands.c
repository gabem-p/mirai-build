#include <glob.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include "subprocess.h"
#include "src/common.h"
#include "src/list.h"
#include "src/files.h"
#include "src/util.h"

void add_arg_list(m_list* items, string template, string command[], uint* j) {
    m_list_iterator* iterator = m_list_iter_new(items);
    for (uint i = 0; i < items->length; i++, (*j)++, m_list_iter_next(iterator)) {
        string item = m_list_iter_get(iterator);
        string arg = malloc(strlen(template) + strlen(item));
        sprintf(arg, template, item);
        command[*j] = arg;
    }
    m_list_iter_cleanup(iterator);
}

void add_args(build_file* buildFile, string command[], uint* j) {
    add_arg_list(buildFile->paths, "-I%s", command, j);
    add_arg_list(buildFile->libs, "-l%s", command, j);
    add_arg_list(buildFile->flags, "-%s", command, j);
}

enum compile_result {
    RESULT_NOCHANGE,
    RESULT_SUCCESS,
    RESULT_FAILURE
};

enum compile_result compile_file(string file, build_file* buildFile, m_list* cache, m_list* newCache) {
    struct stat stats;
    stat(file, &stats);
    long modified = stats.st_mtim.tv_nsec * 1e-9 + stats.st_mtim.tv_sec;

    long lastCompile = 0;
    m_list_iterator* iterator = m_list_iter_new(cache);
    for (uint j = 0; j < cache->length; j++, m_list_iter_next(iterator)) {
        cache_entry* entry = m_list_iter_get(iterator);
        if (strcmp(entry->path, file) == 0) {
            lastCompile = entry->time;
            break;
        }
    }
    m_list_iter_cleanup(iterator);

    cache_entry* entry = malloc(sizeof(cache_entry));
    entry->path = file;
    entry->time = modified;
    m_list_add(newCache, entry);

    if (modified <= lastCompile)
        return RESULT_NOCHANGE;

    char path[256];
    memset(path, '\0', sizeof(path));
    for (uint i = 0; file[i] != '\0'; i++) {
        path[i] = file[i] == '/' ? '-' : file[i];
    }

    char outPath[300];
    snprintf(outPath, 300, "build/obj/%s.o", path);

    string* command = calloc(
        6 + buildFile->paths->length + buildFile->libs->length + buildFile->flags->length,
        sizeof(string));
    uint j = 0;
    command[j++] = "/bin/cc";
    command[j++] = file;
    command[j++] = "-o";
    command[j++] = outPath;
    command[j++] = "-c";
    add_args(buildFile, command, &j);

    struct subprocess_s process;
    subprocess_create((const string*)command, subprocess_option_inherit_environment, &process);
    int result;
    subprocess_join(&process, &result);

    FILE* output = subprocess_stdout(&process);
    char buffer[1024];
    // while (!feof(output))
    //     printf("%s", fgets(buffer, sizeof(buffer), output));

    subprocess_destroy(&process);
    free(command);
    return result == 0 ? RESULT_SUCCESS : RESULT_FAILURE;
}

int build(build_file* buildFile, int argc, string argv[]) {
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    if (!isdir("build"))
        mkdir("build", 0755);
    if (!isdir("build/obj"))
        mkdir("build/obj", 0755);

    m_list* files = recurse_dir(".", "*.c");

    m_list* cache = read_build_cache();
    m_list* newCache = m_list_new();

    bool failed = false;
    bool modified = false;

    m_list_iterator* fileIterator = m_list_iter_new(files);
    for (int i = 0; i < files->length; i++, m_list_iter_next(fileIterator)) {
        string file = m_list_iter_get(fileIterator);

        enum compile_result result = compile_file(file, buildFile, cache, newCache);
        failed |= result == RESULT_FAILURE;
        modified |= result != RESULT_NOCHANGE;
    }
    m_list_iter_cleanup(fileIterator);

    write_build_cache(newCache);

    if (failed) {
        struct timespec end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        printf("%s%sBuild failed%s (%.2f seconds)\n", COLOR_BOLD, COLOR_ERROR, COLOR_RESET,
               (end.tv_nsec - start.tv_nsec) * 1e-9 + (end.tv_sec - start.tv_sec));
        return EXIT_FAILURE;
    }

    int result = 0;
    if (modified) {
        m_list* objects = recurse_dir("build/obj", "*.o");

        char template[] = "build/%s";
        string outPath = malloc(sizeof(template) + strlen(buildFile->name) - 1);
        sprintf(outPath, template, buildFile->name);

        string* command = calloc(
            4 + buildFile->paths->length + buildFile->libs->length + buildFile->flags->length + files->length,
            sizeof(string));
        uint j = 0;
        command[j++] = "/bin/cc";
        command[j++] = "-o";
        command[j++] = outPath;
        m_list_iterator* iterator = m_list_iter_new(objects);
        for (uint i = 0; i < objects->length; i++, m_list_iter_next(iterator)) {
            command[j++] = m_list_iter_get(iterator);
        }
        m_list_iter_cleanup(iterator);
        add_args(buildFile, command, &j);

        struct subprocess_s process;
        subprocess_create((const string*)command, subprocess_option_inherit_environment, &process);
        subprocess_join(&process, &result);
        subprocess_destroy(&process);
        free(command);
        free(outPath);
    }

    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    if (result != 0) {
        printf("%s%sBuild failed%s (%.2f seconds)\n", COLOR_BOLD, COLOR_ERROR, COLOR_RESET,
               (end.tv_nsec - start.tv_nsec) * 1e-9 + (end.tv_sec - start.tv_sec));
        write_build_cache(cache);
        return EXIT_FAILURE;
    }

    printf("%s%s%s ─> %sbuild/%s%s (%.2f seconds)\n", COLOR_BOLD, modified ? "Build success" : "No changes", COLOR_RESET,
           COLOR_MAIN, buildFile->name, COLOR_RESET,
           (end.tv_nsec - start.tv_nsec) * 1e-9 + (end.tv_sec - start.tv_sec));
    return EXIT_SUCCESS;
};

int run(build_file* buildFile, int argc, string argv[]) {
    build(buildFile, argc, argv);

    string* command = calloc(2, sizeof(string));
    char template[] = "build/%s";
    uint size = sizeof(template) + strlen(buildFile->name) - 1;
    command[0] = malloc(size);
    sprintf(command[0], template, buildFile->name);

    struct subprocess_s process;
    subprocess_create((const string*)command,
                      subprocess_option_inherit_environment | subprocess_option_combined_stdout_stderr | subprocess_option_enable_async,
                      &process);

    while (subprocess_alive(&process)) {
        char buffer[1024];
        subprocess_read_stdout(&process, buffer, 1024);
        printf("%s", buffer);
    }

    int result;
    subprocess_join(&process, &result);

    subprocess_destroy(&process);
    free(command[0]);
    free(command);

    return result;
}