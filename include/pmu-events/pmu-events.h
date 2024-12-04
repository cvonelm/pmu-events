/* SPDX-License-Identifier: GPL-2.0 */
#ifndef PMU_EVENTS_H
#define PMU_EVENTS_H

#include <pmu-events/kernel-defs.h>
#include <stdbool.h>
#include <stddef.h>

struct compact_pmu_event {
        int offset;
};

struct pmu_table_entry {
        const struct compact_pmu_event *entries;
        uint32_t num_entries;
        struct compact_pmu_event pmu_name;
};

struct pmu_events_table {
        const struct pmu_table_entry *pmus;
        uint32_t num_pmus;
};

/* Struct used to make the PMU metric table implementation opaque to callers. */
struct pmu_metrics_table {
        const struct pmu_table_entry *pmus;
        uint32_t num_pmus;
};

/*
 * Map a CPU to its table of PMU events. The CPU is identified by the
 * cpuid field, which is an arch-specific identifier for the CPU.
 * The identifier specified in tools/perf/pmu-events/arch/xxx/mapfile
 * must match the get_cpuid_str() in tools/perf/arch/xxx/util/header.c)
 *
 * The  cpuid can contain any character other than the comma.
 */
struct pmu_events_map {
        const char *arch;
        const char *cpuid;
        struct pmu_events_table event_table;
        struct pmu_metrics_table metric_table;
};

struct perf_pmu;

enum aggr_mode_class {
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

struct pmu_events_table;
struct pmu_metrics_table;

#define PMU_EVENTS__NOT_FOUND -1000

typedef int (*pmu_event_iter_fn)(const struct pmu_event *pe,
				 const struct pmu_events_table *table,
				 void *data);

typedef int (*pmu_metric_iter_fn)(const struct pmu_metric *pm,
				  const struct pmu_metrics_table *table,
				  void *data);

int pmu_events_table__for_each_event(const struct pmu_events_table *table,
				    struct perf_pmu *pmu,
				    pmu_event_iter_fn fn,
				    void *data);
/*
 * Search for table and entry matching with pmu__name_match. Each matching event
 * has fn called on it. 0 implies to success/continue the search while non-zero
 * means to terminate. The special value PMU_EVENTS__NOT_FOUND is used to
 * indicate no event was found in one of the tables which doesn't terminate the
 * search of all tables.
 */
int pmu_events_table__find_event(const struct pmu_events_table *table,
                                 struct perf_pmu *pmu,
                                 const char *name,
                                 pmu_event_iter_fn fn,
				 void *data);
size_t pmu_events_table__num_events(const struct pmu_events_table *table,
				    struct perf_pmu *pmu);

int pmu_metrics_table__for_each_metric(const struct pmu_metrics_table *table, pmu_metric_iter_fn fn,
				     void *data);

const struct pmu_events_table *perf_pmu__find_events_table(struct perf_pmu *pmu);
const struct pmu_metrics_table *pmu_metrics_table__find(void);
const struct pmu_events_table *find_core_events_table(const char *arch, const char *cpuid);
const struct pmu_metrics_table *find_core_metrics_table(const char *arch, const char *cpuid);
int pmu_for_each_core_event(pmu_event_iter_fn fn, void *data);
int pmu_for_each_core_metric(pmu_metric_iter_fn fn, void *data);

const struct pmu_events_table *find_sys_events_table(const char *name);
const struct pmu_metrics_table *find_sys_metrics_table(const char *name);
int pmu_for_each_sys_event(pmu_event_iter_fn fn, void *data);
int pmu_for_each_sys_metric(pmu_metric_iter_fn fn, void *data);

const char *describe_metricgroup(const char *group);
void decompress_event(int offset, struct pmu_event *pe);
#endif
