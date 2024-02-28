/*
 * metascheduler.h
 *
 *  Author: Davide Corroppoli
 */

#ifndef METASCHEDULER_H_
#define METASCHEDULER_H_

#include "applications.h"
#include "simulation.h"

/* Meta-scheduler function declarations */
void AuxClkHandler(void);
void MetaschedulerShell();
void Metascheduler();
void TaskShell(int task);

#endif /* METASCHEDULER_H_ */
