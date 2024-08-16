#include <pmu-events/pmu-events.h>

#include <stdio.h>

int main(void)
{
    struct pmu_events_table *tbl  = find_core_events_table("x86", get_cpuid_str(NULL));
    printf("Num events: %d\n", tbl->num_pmus);
    int i = 0;

    for(; i < tbl->num_pmus; i++)
    {
        printf("Num entries in PMU %d:  %d\n", i, tbl->pmus[i].num_entries);;
        int j = 0;
        struct pmu_event pmu_ev;

        decompress_event(tbl->pmus[i].pmu_name.offset, &pmu_ev);
        printf("PMU Name: %s\n", pmu_ev.name);
        for(; j < tbl->pmus[i].num_entries; j++)
        {
            struct pmu_event ev;
            decompress_event(tbl->pmus[i].entries[j].offset, &ev);
            printf("=========================\n");
            printf("Name: %s\n", ev.name);
            printf("Event: %s\n", ev.event);
            printf("Unit: %s\n", ev.unit);
            printf("Unit: %s\n", ev.pmu);
        }
    }
    return 0;
}
