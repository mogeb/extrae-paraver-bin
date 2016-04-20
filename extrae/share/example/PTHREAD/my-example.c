/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                                   Extrae                                  *
 *              Instrumentation package for parallel applications            *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL: https://svn.bsc.es/repos/ptools/extrae/tags/3.2/3.2.1/example/LINUX/PTHREAD/pthread-example.c $
 | @last_commit: $Date: 2015-10-05 12:24:45 +0200 (Mon, 05 Oct 2015) $
 | @version:     $Revision: 3414 $
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>

#include "../../../../../git/benchtrace/ustbench/ustbench.h"
#include "extrae_user_events.h"

#define NCPUS 8
#define MAX_THREADS 1
#define METRIC_LEN 128
#define PER_CPU_ALLOC 100000

#define barrier() __asm__ volatile("" ::: "memory")

/* Barrier variable */
pthread_barrier_t barrier;
struct perf_event_attr attr1, attr2, attr3, attr4;
struct measurement_cpu_perf cpu_perf[NCPUS];

char metric1[METRIC_LEN];
char metric2[METRIC_LEN];
char metric3[METRIC_LEN];
char metric4[METRIC_LEN];

static inline pid_t gettid()
{
    return syscall(SYS_gettid);
}

void perf_init()
{
    int i;

    for(i = 0; i < NCPUS; i++) {
        cpu_perf[i].entries = (struct measurement_entry*) malloc(PER_CPU_ALLOC *
                    sizeof(struct measurement_entry));
        cpu_perf[i].pos = 0;
    }

    attr1.size = sizeof(struct perf_event_attr);
    attr1.pinned = 1;
    attr1.disabled = 0;
    attr1.type = PERF_TYPE_HW_CACHE;
    attr1.config = PERF_COUNT_HW_CACHE_L1D | \
               PERF_COUNT_HW_CACHE_OP_READ << 8 | \
               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
    attr1.read_format = PERF_FORMAT_GROUP|PERF_FORMAT_ID;
    strncat(metric1, "L1_misses", METRIC_LEN);

    /* attr2 = cache misses */
    attr2.size = sizeof(struct perf_event_attr);
    attr2.pinned = 1;
    attr2.disabled = 0;
    attr2.type = PERF_TYPE_HARDWARE;
    attr2.config = PERF_COUNT_HW_CACHE_MISSES;
    attr2.read_format = PERF_FORMAT_GROUP|PERF_FORMAT_ID;
    strncat(metric2, "Cache_misses", METRIC_LEN);

    attr3.size = sizeof(struct perf_event_attr);
    attr3.pinned = 1;
    attr3.disabled = 0;
    attr3.type = PERF_TYPE_HARDWARE;
    attr3.config = PERF_COUNT_HW_CPU_CYCLES;
    attr3.read_format = PERF_FORMAT_GROUP|PERF_FORMAT_ID;
    strncat(metric3, "CPU_cycles", METRIC_LEN);

    attr4.size = sizeof(struct perf_event_attr);
    attr4.pinned = 1;
    attr4.disabled = 0;
    attr4.type = PERF_TYPE_HARDWARE;
    attr4.config = PERF_COUNT_HW_INSTRUCTIONS;
    attr4.read_format = PERF_FORMAT_GROUP|PERF_FORMAT_ID;
    strncat(metric4, "Instructions", METRIC_LEN);


    /* attr4 = dTLB-load-misses */
//    attr3.size = sizeof(struct perf_event_attr);
//    attr3.pinned = 1;
//    attr3.disabled = 0;
//    attr3.type = PERF_TYPE_HW_CACHE;
//    attr3.config = PERF_COUNT_HW_CACHE_DTLB | \
//               PERF_COUNT_HW_CACHE_OP_READ << 8 | \
//               PERF_COUNT_HW_CACHE_RESULT_MISS << 16;
//    attr3.read_format = PERF_FORMAT_GROUP|PERF_FORMAT_ID;
//    strncat(metric3, "TLB_misses", METRIC_LEN);
}

void output_measurements()
{
    int i, cpu;
    FILE *outfile;

    printf("***********************************\n");
    printf("About to output\n");
    printf("***********************************\n");
    outfile = fopen("/tmp/out.csv", "w+");
    fprintf(outfile, "latency,%s,%s,%s,%s\n", metric1, metric2, metric3,
            metric4);
    for(cpu = 0; cpu < NCPUS; cpu++) {
        for(i = 0; i < cpu_perf[cpu].pos; i++) {
            fprintf(outfile, "%lu,%lu,%lu,%lu,%lu\n",
                    cpu_perf[cpu].entries[i].latency,
                    cpu_perf[cpu].entries[i].pmu1,
                    cpu_perf[cpu].entries[i].pmu2,
                    cpu_perf[cpu].entries[i].pmu3,
                    cpu_perf[cpu].entries[i].pmu4);
        }
    }
    fclose(outfile);
}

static unsigned long long rdpmc(unsigned int counter) {
  unsigned int low, high;

  __asm__ volatile("rdpmc" : "=a" (low), "=d" (high) : "c" (counter));

  return (unsigned long long)low | ((unsigned long long)high) <<32;
}

/* Ingo's code for using rdpmc */
static inline unsigned long long mmap_read_self(void *addr) {

    struct perf_event_mmap_page *pc = addr;
    unsigned int seq,idx;

    unsigned long long count;

    do {
        seq = pc->lock;
        barrier();

        idx = pc->index;
        count = pc->offset;

        if (idx) {
            count += rdpmc(pc->index-1);
        }
        barrier();
    } while (pc->lock != seq);

    return count;
}

/* perf_event_open syscall wrapper */
static long
sys_perf_event_open(struct perf_event_attr *hw_event,
                    pid_t pid, int cpu, int group_fd, unsigned long flags)
{
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

struct perf_event_mmap_page *setup_perf(struct perf_event_attr *attr)
{
    void *perf_addr;
    int fd, ret;
    size_t pgsz;

    pgsz = sysconf(_SC_PAGESIZE);

    fd = sys_perf_event_open(attr, 0, -1, -1, 0);

    if (fd < 0)
        return NULL;

    perf_addr = mmap(NULL, pgsz, PROT_READ, MAP_SHARED, fd, 0);

    if (perf_addr == MAP_FAILED)
        return NULL;

    ret = close(fd);
    if (ret) {
        perror("Error closing perf memory mapping FD");
    }

    return perf_addr;
}

void *routine1 (void *a)
{
    int i, ret;
    unsigned long pmu1_start, pmu1_end;
    unsigned long pmu2_start, pmu2_end;
    unsigned long pmu3_start, pmu3_end;
    unsigned long pmu4_start, pmu4_end;

    struct timespec ts_start, ts_end, ts_diff;
    struct perf_event_mmap_page *perf_mmap1, *perf_mmap2, *perf_mmap3,
            *perf_mmap4;
    struct worker_thread_args *args;
    args = (struct worker_thread_args*) a;
    int cpu = args->id;
    int tid = gettid();

    perf_mmap1 = setup_perf(&attr1);
    if(!perf_mmap1) {
        printf("Couldn't allocate perf_mmap1\n");
    }
    perf_mmap2 = setup_perf(&attr2);
    if(!perf_mmap2) {
        printf("Couldn't allocate perf_mmap2\n");
    }
    perf_mmap3 = setup_perf(&attr3);
    if(!perf_mmap3) {
        printf("Couldn't allocate perf_mmap3\n");
    }
    perf_mmap4 = setup_perf(&attr4);
    if(!perf_mmap4) {
        printf("Couldn't allocate perf_mmap4\n");
    }

    for(i = 0; i < 5000; i++) {
        pmu1_start = mmap_read_self(perf_mmap1);
        pmu2_start = mmap_read_self(perf_mmap2);
        pmu3_start = mmap_read_self(perf_mmap3);
        pmu4_start = mmap_read_self(perf_mmap4);

        clock_gettime(CLOCK_MONOTONIC, &ts_start);
//        Extrae_user_function(1);
        Extrae_event(1000, i + 1); // 1000 just because and make loop start at 1
//        Extrae_counters();
        clock_gettime(CLOCK_MONOTONIC, &ts_end);

        pmu4_end = mmap_read_self(perf_mmap4);
        pmu1_end = mmap_read_self(perf_mmap1);
        pmu2_end = mmap_read_self(perf_mmap2);
        pmu3_end = mmap_read_self(perf_mmap3);

        ts_diff = do_ts_diff(ts_start, ts_end);
        cpu_perf[cpu].entries[i].pmu1 = pmu1_end - pmu1_start;
        cpu_perf[cpu].entries[i].pmu2 = pmu2_end - pmu2_start;
        cpu_perf[cpu].entries[i].pmu3 = pmu3_end - pmu3_start;
        cpu_perf[cpu].entries[i].pmu4 = pmu4_end - pmu4_start;
        cpu_perf[cpu].entries[i].latency = ts_diff.tv_sec * 1000000000 +
                ts_diff.tv_nsec;
        cpu_perf[cpu].pos++;
    }

}

int main (int argc, char *argv[])
{
    pthread_t t[MAX_THREADS];
    struct worker_thread_args *worker_args;
    int i;
    int ncpus = sysconf(_SC_NPROCESSORS_ONLN);

    worker_args = (struct worker_thread_args*)
            malloc(MAX_THREADS * sizeof(struct worker_thread_args));

    // Barrier initialization
    if(pthread_barrier_init(&barrier, NULL, MAX_THREADS))
    {
        printf("Could not create a barrier\n");
        return -1;
    }

    perf_init();
//    Extrae_init ();

    for (i = 0; i < MAX_THREADS; i++) {
        worker_args[i].id = i;
        cpu_set_t cpu_set;
        pthread_attr_t attr;
        CPU_ZERO(&cpu_set);
        CPU_SET(i % ncpus, &cpu_set);
//        pthread_create (&t[i], &attr, routine1, (void*) &worker_args[i]);
        pthread_create (&t[i], NULL, routine1, (void*) &worker_args[i]);
    }
    for (i = 0; i < MAX_THREADS; i++)
        pthread_join (t[i], NULL);
    output_measurements();
    Extrae_fini();

    return 0;
}
