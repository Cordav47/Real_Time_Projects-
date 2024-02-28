/*
 * resources.h
 *
 *  Author: Davide Corroppoli
 */

#ifndef RESOURCES_H_
#define RESOURCES_H_

#include "taskLib.h"
#include "semLib.h"

typedef const char RAPName_t[100];
extern int Ceiling[];
extern int ResourceAccessProtocol;
extern SEM_ID Semaphore[];


/* Resource access protocols */
#define NOR 0 /* no resources */
#define NOP 1 /* no protocol */
#define NPP 2 /* Non-Preemptive Critical Section Protocol modified*/
#define HLP 3 /* Highest Locker Protocol */
#define PIP 4 /* Priority Inheritance Protocol */
#define MP 5 /*My protocol*/
/* Resource access procedures */
void EntrySection(int semaphore, int task);
void ExitSection(int semaphore, int task);

#endif /* RESOURCES_H_ */
