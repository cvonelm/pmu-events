
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <pmu-events/pmu-events.h>
#include <pmu-events/util.h>
#include <pmu-events/kernel-defs.h>

#define perf_cpu_map__for_each_cpu_skip_any(_cpu, idx, cpus)	\
	for ((idx) = 0, (_cpu) = cpus->map[idx];	\
	     (idx) < cpus->nr;			\
	     (idx)++, (_cpu) = cpus->map[idx])	\
		if ((_cpu).cpu != -1)

struct perf_cpu perf_cpu_map__min(const struct perf_cpu_map *map)
{
	struct perf_cpu cpu, result = {
		.cpu = -1
	};
	int idx;

	perf_cpu_map__for_each_cpu_skip_any(cpu, idx, map) {
		result = cpu;
		break;
	}
	return result;
}

static bool perf_pmu__match_ignoring_suffix(const char *pmu_name, const char *tok)
{
	const char *p, *suffix;
	bool has_hex = false;

	if (strncmp(pmu_name, tok, strlen(tok)))
		return false;

	suffix = p = pmu_name + strlen(tok);
	if (*p == 0)
		return true;

	if (*p == '_') {
		++p;
		++suffix;
	}

	/* Ensure we end in a number */
	while (1) {
		if (!isxdigit(*p))
			return false;
		if (!has_hex)
			has_hex = !isdigit(*p);
		if (*(++p) == 0)
			break;
	}

	if (has_hex)
		return (p - suffix) > 2;

	return true;
}

static bool pmu_uncore_alias_match(const char *pmu_name, const char *name)
{
	char *tmp = NULL, *tok, *str;
	bool res;

	if (strchr(pmu_name, ',') == NULL)
		return perf_pmu__match_ignoring_suffix(name, pmu_name);

	str = strdup(pmu_name);
	if (!str)
		return false;

	/*
	 * uncore alias may be from different PMU with common prefix
	 */
	tok = strtok_r(str, ",", &tmp);
	if (strncmp(pmu_name, tok, strlen(tok))) {
		res = false;
		goto out;
	}

	/*
	 * Match more complex aliases where the alias name is a comma-delimited
	 * list of tokens, orderly contained in the matching PMU name.
	 *
	 * Example: For alias "socket,pmuname" and PMU "socketX_pmunameY", we
	 *	    match "socket" in "socketX_pmunameY" and then "pmuname" in
	 *	    "pmunameY".
	 */
	while (1) {
		char *next_tok = strtok_r(NULL, ",", &tmp);

		name = strstr(name, tok);
		if (!name ||
		    (!next_tok && !perf_pmu__match_ignoring_suffix(name, tok))) {
			res = false;
			goto out;
		}
		if (!next_tok)
			break;
		tok = next_tok;
		name += strlen(tok);
	}

	res = true;
out:
	free(str);
	return res;
}

bool pmu__name_match(const struct perf_pmu *pmu, const char *pmu_name)
{
	return !strcmp(pmu->name, pmu_name) ||
		(pmu->is_uncore && pmu_uncore_alias_match(pmu_name, pmu->name)) ||
		/*
		 * jevents and tests use default_core as a marker for any core
		 * PMU as the PMU name varies across architectures.
		 */
	        (pmu->is_core && !strcmp(pmu_name, "default_core"));
}

char *get_cpuid_allow_env_override(struct perf_cpu cpu)
{
	char *cpuid;
	static bool printed;

	cpuid = getenv("PERF_CPUID");
	if (cpuid)
		cpuid = strdup(cpuid);
	if (!cpuid)
		cpuid = get_cpuid_str(cpu);
	if (!cpuid)
		return NULL;

	if (!printed) {
		fprintf(stderr, "Using CPUID %s\n", cpuid);
		printed = true;
	}
	return cpuid;
}

