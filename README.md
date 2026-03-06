# mirai build

mirai is a simple build i made cause everything else that existed looked overly complicated. thats about it

## usage

projects are defined with a build file (`mirai.build`) in a categorized key-value format. a minimal build file for a project in C is as follows:
```
[project]
name:mirai
language:c

[include]
path:.
```
the project can be compiled by running `mirai build` in the same directory as the build file.

## build file keys
- **project**
    - *name* - the name of the outputed executable / library
    - *language* - the language of the project
        - c - compiled with `/bin/cc` (usually gcc or clang)
    - *flag* - add a custom flag to send to the compiler. the value is included verbatim with a dash prefix (e.g. `flag:static` becomes `-static`)
- **include**
    - *path* - includes a directory (either relative or absolute)
    - *lib* - includes a library