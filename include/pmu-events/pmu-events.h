/* This file has been imported from the Linux Kernel, which is licensed under the GPL-2.0 */
/* SPDX-License-Identifier: GPL-2.0 */
#ifndef PMU_EVENTS_H
#define PMU_EVENTS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/perf_event.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


struct perf_pmu;
int get_cpuid(char *buffer, size_t sz);

char *get_cpuid_str(struct perf_pmu *pmu );
int strcmp_cpuid_str(const char *s1, const char *s2);
enum {
	PERF_PMU_FORMAT_VALUE_CONFIG,
	PERF_PMU_FORMAT_VALUE_CONFIG1,
	PERF_PMU_FORMAT_VALUE_CONFIG2,
	PERF_PMU_FORMAT_VALUE_CONFIG3,
	PERF_PMU_FORMAT_VALUE_CONFIG_END,
};

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
	 * @config_masks: Derived from the PMU's format data, bits that are
	 * valid within the config value.
	 */
	__u64 config_masks[PERF_PMU_FORMAT_VALUE_CONFIG_END];

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
	} missing_features;

	/**
	 * @mem_events: List of the supported mem events
	 */
	struct perf_mem_event *mem_events;
};
enum aggr_mode_class
{
    PerChip = 1,
    PerCore
};
/**
 * enum metric_event_groups - How events within a pmu_metric should be grouped.
 */
enum metric_event_groups {
	/**
	 * @MetricGroupEvents: Default, group events within the metric.
	 */
	MetricGroupEvents = 0,
	/**
	 * @MetricNoGroupEvents: Don't group events for the metric.
	 */
	MetricNoGroupEvents = 1,
	/**
	 * @MetricNoGroupEventsNmi: Don't group events for the metric if the NMI
	 *                          watchdog is enabled.
	 */
	MetricNoGroupEventsNmi = 2,
	/**
	 * @MetricNoGroupEventsSmt: Don't group events for the metric if SMT is
	 *                          enabled.
	 */
	MetricNoGroupEventsSmt = 3,
};

/*
 * Describe each PMU event. Each CPU has a table of PMU events.
 */
struct pmu_event {
	const char *name;
	const char *compat;
	const char *event;
	const char *desc;
	const char *topic;
	const char *long_desc;
	const char *pmu;
	const char *unit;
	bool perpkg;
	bool deprecated;
};

struct pmu_metric {
	const char *pmu;
	const char *metric_name;
	const char *metric_group;
	const char *metric_expr;
	const char *metric_threshold;
	const char *unit;
	const char *compat;
	const char *desc;
	const char *long_desc;
	const char *metricgroup_no_group;
	const char *default_metricgroup_name;
	enum aggr_mode_class aggr_mode;
	enum metric_event_groups event_grouping;
};


struct compact_pmu_event {
  int offset;
};

struct pmu_table_entry {
        const struct compact_pmu_event *entries;
        uint32_t num_entries;
        struct compact_pmu_event pmu_name;
};
/* Struct used to make the PMU event table implementation opaque to callers. */
struct pmu_events_table {
        const struct pmu_table_entry *pmus;
        uint32_t num_pmus;
};

/* Struct used to make the PMU metric table implementation opaque to callers. */
struct pmu_metrics_table {
        const struct pmu_table_entry *pmus;
        uint32_t num_pmus;
};



bool pmu__name_match(const struct perf_pmu *pmu, const char *pmu_name);
char *perf_pmu__getcpuid(struct perf_pmu *pmu);
typedef int (*pmu_event_iter_fn)(const struct pmu_event* pe, const struct pmu_events_table* table,
                                 void* data);

typedef int (*pmu_metric_iter_fn)(const struct pmu_metric* pm,
                                  const struct pmu_metrics_table* table, void* data);

int pmu_events_table_for_each_event(const struct pmu_events_table* table, pmu_event_iter_fn fn,
                                    void* data);
int pmu_metrics_table_for_each_metric(const struct pmu_metrics_table* table, pmu_metric_iter_fn fn,
                                      void* data);

const struct pmu_events_table* find_core_events_table(const char* arch, const char* cpuid);
const struct pmu_metrics_table* find_core_metrics_table(const char* arch, const char* cpuid);
int pmu_for_each_core_event(pmu_event_iter_fn fn, void* data);
int pmu_for_each_core_metric(pmu_metric_iter_fn fn, void* data);

const struct pmu_events_table* find_sys_events_table(const char* name);
const struct pmu_metrics_table* find_sys_metrics_table(const char* name);
int pmu_for_each_sys_event(pmu_event_iter_fn fn, void* data);
int pmu_for_each_sys_metric(pmu_metric_iter_fn fn, void* data);


void decompress_event(int offset, struct pmu_event *pe);
#endif
