#include <pmu-events/pmu-events.h>
#include <string.h>
#include <ctype.h>
/**
* perf_pmu__match_ignoring_suffix - Does the pmu_name match tok ignoring any
 *                                   trailing suffix? The Suffix must be in form
 *                                   tok_{digits}, or tok{digits}.
 * @pmu_name: The pmu_name with possible suffix.
 * @tok: The possible match to pmu_name without suffix.
 */
char *perf_pmu__getcpuid(struct perf_pmu *pmu)
{
	char *cpuid;
	static bool printed;

	cpuid = getenv("PERF_CPUID");
	if (cpuid)
		cpuid = strdup(cpuid);
	if (!cpuid)
		cpuid = get_cpuid_str(pmu);
	if (!cpuid)
		return NULL;

	if (!printed) {
		printed = true;
	}
	return cpuid;
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

 /* pmu_uncore_alias_match - does name match the PMU name?
 * @pmu_name: the json struct pmu_event name. This may lack a suffix (which
 *            matches) or be of the form "socket,pmuname" which will match
 *            "socketX_pmunameY".
 * @name: a real full PMU name as from sysfs.
 */
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
