
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/perf_event.h>
enum {
	PERF_PMU_FORMAT_VALUE_CONFIG,
	PERF_PMU_FORMAT_VALUE_CONFIG1,
	PERF_PMU_FORMAT_VALUE_CONFIG2,
	PERF_PMU_FORMAT_VALUE_CONFIG3,
	PERF_PMU_FORMAT_VALUE_CONFIG_END,
};

struct perf_cpu {
	int cpu;
};

struct perf_cpu_map 
{
    int nr;
    struct perf_cpu map[];
};
struct perf_cpu perf_cpu_map__min(const struct perf_cpu_map *map);
/**
 * struct perf_pmu
 */
struct perf_pmu {
	/** @name: The name of the PMU such as "cpu". */
	const char *name;
	/**
	 * @alias_name: Optional alternate name for the PMU determined in
	 * architecture specific code.
	 */
	char *alias_name;
	/**
	 * @id: Optional PMU identifier read from
	 * <sysfs>/bus/event_source/devices/<name>/identifier.
	 */
	const char *id;
	/**
	 * @type: Perf event attributed type value, read from
	 * <sysfs>/bus/event_source/devices/<name>/type.
	 */
	uint32_t type;
	/**
	 * @selectable: Can the PMU name be selected as if it were an event?
	 */
	bool selectable;
	/**
	 * @is_core: Is the PMU the core CPU PMU? Determined by the name being
	 * "cpu" or by the presence of
	 * <sysfs>/bus/event_source/devices/<name>/cpus. There may be >1 core
	 * PMU on systems like Intel hybrid.
	 */
	bool is_core;
	/**
	 * @is_uncore: Is the PMU not within the CPU core? Determined by the
	 * presence of <sysfs>/bus/event_source/devices/<name>/cpumask.
	 */
	bool is_uncore;
	/**
	 * @auxtrace: Are events auxiliary events? Determined in architecture
	 * specific code.
	 */
	bool auxtrace;
	/**
	 * @formats_checked: Only check PMU's formats are valid for
	 * perf_event_attr once.
	 */
	bool formats_checked;
	/** @config_masks_present: Are there config format values? */
	bool config_masks_present;
	/** @config_masks_computed: Set when masks are lazily computed. */
	bool config_masks_computed;
	/**
	 * @max_precise: Number of levels of :ppp precision supported by the
	 * PMU, read from
	 * <sysfs>/bus/event_source/devices/<name>/caps/max_precise.
	 */
	int max_precise;
	/**
	 * @perf_event_attr_init_default: Optional function to default
	 * initialize PMU specific parts of the perf_event_attr.
	 */
	void (*perf_event_attr_init_default)(const struct perf_pmu *pmu,
					     struct perf_event_attr *attr);
	/**
	 * @cpus: Empty or the contents of either of:
	 * <sysfs>/bus/event_source/devices/<name>/cpumask.
	 * <sysfs>/bus/event_source/devices/<cpu>/cpus.
	 */
	struct perf_cpu_map *cpus;
	const struct pmu_events_table *events_table;
	/** @sysfs_aliases: Number of sysfs aliases loaded. */
	uint32_t sysfs_aliases;
	/** @cpu_json_aliases: Number of json event aliases loaded specific to the CPUID. */
	uint32_t cpu_json_aliases;
	/** @sys_json_aliases: Number of json event aliases loaded matching the PMU's identifier. */
	uint32_t sys_json_aliases;
	/** @sysfs_aliases_loaded: Are sysfs aliases loaded from disk? */
	bool sysfs_aliases_loaded;
	/**
	 * @cpu_aliases_added: Have all json events table entries for the PMU
	 * been added?
	 */
	bool cpu_aliases_added;
	/** @caps_initialized: Has the list caps been initialized? */
	bool caps_initialized;
	/** @nr_caps: The length of the list caps. */
	uint32_t nr_caps;
	/**
	 * @caps: Holds the contents of files read from
	 * <sysfs>/bus/event_source/devices/<name>/caps/.
	 *
	 * The contents are pairs of the filename with the value of its
	 * contents, for example, max_precise (see above) may have a value of 3.
	 */

	/**
	 * @config_masks: Derived from the PMU's format data, bits that are
	 * valid within the config value.
	 */
	uint64_t config_masks[PERF_PMU_FORMAT_VALUE_CONFIG_END];

	/**
	 * @missing_features: Features to inhibit when events on this PMU are
	 * opened.
	 */
	struct {
		/**
		 * @exclude_guest: Disables perf_event_attr exclude_guest and
		 * exclude_host.
		 */
		bool exclude_guest;
		/**
		 * @checked: Are the missing features checked?
		 */
		bool checked;
	} missing_features;

	/**
	 * @mem_events: List of the supported mem events
	 */
	struct perf_mem_event *mem_events;
};
#define ARRAY_SIZE(array) \
    (sizeof(array) / sizeof(*array))

bool pmu__name_match(const struct perf_pmu *pmu, const char *pmu_name);

char *get_cpuid_allow_env_override(struct perf_cpu cpu);

