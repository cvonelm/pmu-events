/* This file has been imported from the Linux kernel, which is licensed under GPL-2.0
/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __PERF_HEADER_H
#define __PERF_HEADER_H

int get_cpuid(char *buffer, size_t sz);

char *get_cpuid_str(struct perf_pmu *pmu __maybe_unused);
int strcmp_cpuid_str(const char *s1, const char *s2);
#endif /* __PERF_HEADER_H */
