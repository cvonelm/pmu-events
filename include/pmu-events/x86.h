#pragma once

#include <stdbool.h>
#include <stddef.h>

int strcmp_cpuid_str(const char* mapcpuid, const char* id);

void get_cpuid_0(char* vendor, unsigned int* lvl);

int get_cpuid(char* buffer, size_t sz);
