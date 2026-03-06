#include <string.h>
#include <unistd.h>
#include "src/common.h"
#include "src/commands.h"
#include "src/files.h"

int help(build_file* buildFile, int argc, string argv[]);
typedef struct {
    string name;
    int (*function)(build_file*, int, string[]);
} command;
const command commands[] = {{"help", help}, {"build", build}};

int help(build_file* buildFile, int argc, string argv[]) {
    printf("%s%smirai build tool%s %s\n", COLOR_MAIN, COLOR_BOLD, COLOR_RESET, VERSION);
    printf("\n%sUsage:%s mirai <command> [args]\n", COLOR_BOLD, COLOR_RESET);
    printf("\n%sCommands:%s\n", COLOR_BOLD, COLOR_RESET);
    for (int i = 0; i < length(commands); i++) {
        printf("%s%s─%s %s\n", COLOR_MAIN, i == length(commands) - 1 ? "└" : "├", COLOR_RESET, commands[i].name);
    }
    return 0;
}

int main(int argc, string argv[]) {
    if (argc == 1)
        return help(null, argc, argv);

    build_file* buildFile = read_build_file();
    if (buildFile == null)
        return 1;

    for (int i = 0; i < length(commands); i++) {
        if (strcmp(argv[1], commands[i].name) == 0)
            return commands[i].function(buildFile, argc, argv);
    }
    return help(null, argc, argv);
}