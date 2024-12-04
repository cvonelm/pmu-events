#pragma once
#include <pmu-events/kernel-defs.h>

char *get_cpuid_str(struct perf_cpu cpu);
int strcmp_cpuid_str(const char *mapcpuid, const char *id);
