/*
 * applications.h
 *
 *  Author: Davide Corroppoli
 */

#ifndef APPLICATIONS_H_
#define APPLICATIONS_H_

#include "resources.h"
#include "dummyTask.h"

extern int Overruns[], ResponseTime[];
int kind1, size1, kind2, size2;

/* --> Change this to select application <-- */
#define ROBOTIC_APPLICATION

/* Baseline for worker tasks */
#define TOP_PRIORITY 110

/* Application parameters */
#ifdef APPLICATION_0
	#define N_TASKS 1
	#define TASK_PERIODS {5}
	#define TASK_PHASES {0}
	#define N_SEMAPHORES 0
	#define CEILINGS {  }
	#define APPLICATION Application0
	#define CLEAN_UP_STRING "Simulation ended. Response time: %d. Overruns: %d.\n" 
	#define CLEAN_UP_PARAMS (_Vx_usr_arg_t) ResponseTime[0], (_Vx_usr_arg_t) Overruns[0], 0, 0, 0, 0
#endif

#ifdef APPLICATION_1
	#define N_TASKS 2
	#define TASK_PERIODS {5,25}
	#define TASK_PHASES {1,0}
	#define N_SEMAPHORES 1
	#define CEILINGS { TOP_PRIORITY }
	#define APPLICATION Application1
	#define CLEAN_UP_STRING "Simulation ended. Response times: %d, %d. Overruns: %d, %d.\n" 
	#define CLEAN_UP_PARAMS (_Vx_usr_arg_t) ResponseTime[0], (_Vx_usr_arg_t) ResponseTime[1], (_Vx_usr_arg_t) Overruns[0],  (_Vx_usr_arg_t) Overruns[1], 0, 0
#endif

#ifdef APPLICATION_2
	#define N_TASKS 3
	#define TASK_PERIODS {10,15,20}
	#define TASK_PHASES {2,1,0}
	#define N_SEMAPHORES 2
	#define CEILINGS { TOP_PRIORITY+1, TOP_PRIORITY }
	#define APPLICATION Application2
	#define CLEAN_UP_STRING "Simulation ended. Response times: %d, %d, %d. Overruns: %d, %d, %d.\n" 
	#define CLEAN_UP_PARAMS (_Vx_usr_arg_t) ResponseTime[0], (_Vx_usr_arg_t) ResponseTime[1], (_Vx_usr_arg_t) ResponseTime[2], (_Vx_usr_arg_t) Overruns[0],  (_Vx_usr_arg_t) Overruns[1], (_Vx_usr_arg_t) Overruns[2]
#endif

#ifdef APPLICATION_3
	#define N_TASKS 3
	#define TASK_PERIODS {10,15,20}
	#define TASK_PHASES {2,1,0}
	#define N_SEMAPHORES 2
	#define CEILINGS { TOP_PRIORITY+1, TOP_PRIORITY }
	#define APPLICATION Application3
	#define CLEAN_UP_STRING "Simulation ended. Response times: %d, %d, %d. Overruns: %d, %d, %d.\n" 
	#define CLEAN_UP_PARAMS (_Vx_usr_arg_t) ResponseTime[0], (_Vx_usr_arg_t) ResponseTime[1], (_Vx_usr_arg_t) ResponseTime[2], (_Vx_usr_arg_t) Overruns[0],  (_Vx_usr_arg_t) Overruns[1], (_Vx_usr_arg_t) Overruns[2]
#endif

#ifdef APPLICATION_4
	#define N_TASKS 3
	#define TASK_PERIODS {5,10,20}
	#define TASK_PHASES {3,2,0}
	#define N_SEMAPHORES 2
	#define CEILINGS { TOP_PRIORITY, TOP_PRIORITY+1 }
	#define APPLICATION Application4
	#define CLEAN_UP_STRING "Simulation ended. Response times: %d, %d, %d. Overruns: %d, %d, %d.\n" 
	#define CLEAN_UP_PARAMS (_Vx_usr_arg_t) ResponseTime[0], (_Vx_usr_arg_t) ResponseTime[1], (_Vx_usr_arg_t) ResponseTime[2], (_Vx_usr_arg_t) Overruns[0],  (_Vx_usr_arg_t) Overruns[1], (_Vx_usr_arg_t) Overruns[2]
#endif

#ifdef APPLICATION_5
#define N_TASKS 4
#define TASK_PERIODS {2,2,10,60}
#define TASK_PHASES {0,0,0}
#define N_SEMAPHORES 3
#define CEILINGS { TOP_PRIORITY, TOP_PRIORITY+1 }
#define APPLICATION Application4
#define CLEAN_UP_STRING "Simulation ended. Response times: %d, %d, %d. Overruns: %d, %d, %d.\n" 
#define CLEAN_UP_PARAMS (_Vx_usr_arg_t) ResponseTime[0], (_Vx_usr_arg_t) ResponseTime[1], (_Vx_usr_arg_t) ResponseTime[2], (_Vx_usr_arg_t) Overruns[0],  (_Vx_usr_arg_t) Overruns[1], (_Vx_usr_arg_t) Overruns[2]

#endif

#ifdef ROBOTIC_APPLICATION
#define N_TASKS 5
#define TASK_PERIODS {5, 60, 50, 60, 50} 
#define TASK_PHASES {0,0,10,10,0}
#define N_SEMAPHORES 6
#define CEILINGS {TOP_PRIORITY+1, TOP_PRIORITY+2} /* Top priority is always reserved to computer vision that hold no resources*/
#define APPLICATION Robot_Application
#define CLEAN_UP_STRING "Simulation ended. Response times: %d, %d, %d. Overruns: %d, %d, %d.\n" 
#define CLEAN_UP_PARAMS (_Vx_usr_arg_t) ResponseTime[1], (_Vx_usr_arg_t) ResponseTime[2], (_Vx_usr_arg_t) ResponseTime[3], (_Vx_usr_arg_t) Overruns[1], (_Vx_usr_arg_t) Overruns[2], (_Vx_usr_arg_t) Overruns[3]
/* We have excluded computer vision task from the clean_up string, it can't overrun due to scheduling*/

#endif

/* Defined applications */
void APPLICATION(int task);



#endif /* APPLICATIONS_H_ */
