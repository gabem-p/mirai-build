#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define null ((void*)0)
#define string char*

#define sbyte int8_t
#define byte uint8_t
#define short int16_t
#define ushort uint16_t
#define int int32_t
#define uint uint32_t
#define long int64_t
#define ulong uint64_t

#define length(array) sizeof(array) / sizeof(array[0])

#define VERSION "0.10.0"

#define COLOR_MAIN "\033[38;5;43m"
#define COLOR_ERROR "\033[31m"
#define COLOR_WARN "\033[33m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RESET "\033[0m"
