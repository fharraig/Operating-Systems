/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

/** 
 * COSC 3250 - Project 6
 * resched.c implemtation
 * @author Matthew Covington Alex Alarcon
 * Instructor Sabirat Rubya
 * TA-BOT:MAILTO matthew.covington@marquette.edu alex.alarcon@marquette.edu
*/

#include <xinu.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate 
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */

syscall resched(void)
{
    int highest_prio;
    irqmask im;
    pcb *oldproc;               /* pointer to old process entry */
    pcb *newproc;               /* pointer to new process entry */

    uint cpuid = getcpuid();

    oldproc = &proctab[currpid[cpuid]];

    im = disable();
    pid_typ pid;

#if AGING

    // TODO: Implement aging.
    //       Reference include/clock.h to find more information
    //       about the quantums and how aging should behave.

    promote_medium[cpuid]--;
    if (promote_medium[cpuid] <= 0) {
        if (nonempty(readylist[cpuid][PRIORITY_MED])) {
            pid = dequeue(readylist[cpuid][PRIORITY_MED]);
            enqueue(pid, readylist[cpuid][PRIORITY_HIGH]);
        }
        promote_medium[cpuid] = QUANTUM;
        promote_low[cpuid]--;
        if (promote_low[cpuid] <= 0) {
            if (nonempty(readylist[cpuid][PRIORITY_LOW])) {
                pid = dequeue(readylist[cpuid][PRIORITY_LOW]);
                enqueue(pid, readylist[cpuid][PRIORITY_MED]);
            }
            promote_low[cpuid] = QUANTUM;
        }
    }
    
#endif

    /* place current process at end of ready queue */
    if (PRCURR == oldproc->state)
    {
        oldproc->state = PRREADY;
        enqueue(currpid[cpuid], readylist[cpuid][oldproc->priority]);
    }

    /* remove first process in highest priority ready queue */
    // determine queue to pick from
    if (nonempty(readylist[cpuid][PRIORITY_HIGH]))
    {
        highest_prio = PRIORITY_HIGH;
    }
    else if (nonempty(readylist[cpuid][PRIORITY_MED]))
    {
        highest_prio = PRIORITY_MED;
    }
    else
    {
        highest_prio = PRIORITY_LOW;
    }

    currpid[cpuid] = dequeue(readylist[cpuid][highest_prio]);
    newproc = &proctab[currpid[cpuid]];
    newproc->state = PRCURR;    /* mark it currently running    */

#if PREEMPT
    preempt[cpuid] = QUANTUM;
#endif

    ctxsw(&oldproc->regs, &newproc->regs);

    /* The OLD process returns here when resumed. */
    restore(im);
    return OK;
}
