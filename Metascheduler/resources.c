/*
 * resources.c
 *
 * Author:Davide Corroppoli 
 *

*/
#include "resources.h"
#include "simulation.h"

int ResourceCount[N_TASKS];
int CurrentPriority[N_SEMAPHORES+1][N_TASKS]; 
/* ResourceCount has the number of resource that hold a task, CurrentPriority is a N x N array */


RAPName_t RAPName[6] = {
		"No Shared Resources",
		"No Resource Access Protocol",
		"Non-Preemptive Critical Section Protocol",
		"Highest Locker Protocol",
		"Priority Inheritance Protocol",
		"My Protocol"
};

void EntrySection(int semaphore, int task) {
	switch(ResourceAccessProtocol) {
	case NPP:
		if(task>0) {
			ResourceCount[task]++;
			if(ResourceCount[task]==1)
				taskPrioritySet(0,TOP_PRIORITY);
		}
		break;
	case HLP:
		if(task>0) {
			taskLock();
			ResourceCount[task]++;
			if(Ceiling[semaphore]<CurrentPriority[ResourceCount[task]-1][task]) {
				taskPrioritySet(0,Ceiling[semaphore]); /*The first parameter say which task has to modify the priority, if 0 
				modify its own priority  */
				CurrentPriority[ResourceCount[task]][task]=Ceiling[semaphore]; /*CurrentPriority is a data structure */
			}
			else
				CurrentPriority[ResourceCount[task]][task]=CurrentPriority[ResourceCount[task]-1][task];
			taskUnlock();
		}
		break;
	case NOP:
		semTake(Semaphore[semaphore],WAIT_FOREVER);
		break;
	case PIP:
		semTake(Semaphore[semaphore],WAIT_FOREVER);
		break;
	case MP:
		if(task>0) {
					
			ResourceCount[task]++;
			if(ResourceCount[task]==1)
				taskPrioritySet(0,TOP_PRIORITY+1); /* Top priority is reserved to computer vision which hold no resources, so no task
				will have priority larger than CV*/
			semTake(Semaphore[semaphore],WAIT_FOREVER); /*Semaphore is reduntant, but I like to see it in the system viewer, makes things more clear*/
		}
		break;
	case NOR:
	default:
		break;
	}
}

void ExitSection(int semaphore, int task) {
	switch(ResourceAccessProtocol) {
	case NPP:
		if(task>0) {
			ResourceCount[task]--;
			if(ResourceCount[task]==0)
				taskPrioritySet(0,TOP_PRIORITY+task);
			
		}
		break;
	case HLP:
		if(task>0) {
			taskLock();
			ResourceCount[task]--;
			if(CurrentPriority[ResourceCount[task]][task]>CurrentPriority[ResourceCount[task]+1][task])
				taskPrioritySet(0,CurrentPriority[ResourceCount[task]][task]);
			taskUnlock();
		}
		break;
	case NOP:
		semGive(Semaphore[semaphore]);
		break;
	case PIP:
		semGive(Semaphore[semaphore]);
		break;
	case MP:
		if(task>0) {
			semGive(Semaphore[semaphore]);
			ResourceCount[task]--;
			if(ResourceCount[task]==0)
				taskPrioritySet(0,TOP_PRIORITY+task);
		}
		break;
	case NOR:
	default:
		break;
	}
}
