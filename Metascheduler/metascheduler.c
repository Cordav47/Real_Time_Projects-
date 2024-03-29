/*
 * metascheduler.c
 *
 *  Author: Davide Corroppoli
 */

#include "metascheduler.h"

/* Meta-scheduler variables */
int NotFinished[N_TASKS];
int InOverrun[N_TASKS];

/* Task information */
TASK_ID MetaschedulerId, WorkerId[N_TASKS];
int Period[N_TASKS] = TASK_PERIODS;
int Phase[N_TASKS] = TASK_PHASES;
int Instance[N_TASKS]; /* J[i,1], J[i,2], ... */
int JobReleaseCounter[N_TASKS]; /* J1, J2, ...*/
int Overruns[N_TASKS]; /* number of overruns of task in present simulation */
int ResponseTime[N_TASKS]; /* max response time of tasks in present simulation */

/* Meta-scheduler function definitions */
void AuxClkHandler() {
	taskResume(MetaschedulerId);
}

void MetaschedulerShell() {
	FOREVER {
		taskSuspend(0);
		Metascheduler();
	}
}

void Metascheduler() {
	int task, ResTime;
	for(task=0;task<N_TASKS;task++) {
		JobReleaseCounter[task]--;
		if(NotFinished[task]) {
			/* check if job has finished */
			if(taskIsSuspended(WorkerId[task]) && availability[task] == 0) { /* task can be suspended also due to non availability of correspondent size of the trash*/
				/* if job has finished, display response time and update statistics */
				NotFinished[task]=FALSE;
				ResTime = Period[task]*(1+InOverrun[task])-JobReleaseCounter[task];
				myLogMsg("Response Time [%d] = %d units\n", 
						(_Vx_usr_arg_t) task, 
						(_Vx_usr_arg_t) ResTime, 
						0, 0, 0, 0 );
				if(ResponseTime[task]<ResTime) {
					ResponseTime[task]=ResTime;
				}
				/*Restart the task as soon as possible, after the update of computer vision, so after a periodo of cv Period[0]*/
				InOverrun[task]=0;
				JobReleaseCounter[task] = Period[0];
			}
		}
		/*If there's no availability of little trash, task is supended, but we cannot afford to skip to the next period keeping the robot stopped.
		 * So we wait for the update of the computer vision, if we resume task after the period of a computer vision task we are sure the values has been updated
		 */
		if (availability[task] != 0) {
			myLogMsg("Task %d is currently not available to start\n", (_Vx_usr_arg_t) task, 0, 0, 0, 0, 0);
			JobReleaseCounter[task] = Period[0];
			availability[task] = 0;
			
		}
		/* if it is time to start a new period, reset parameters and check if overrun */
		if(JobReleaseCounter[task]==0) {
			NotFinished[task]=TRUE;
			JobReleaseCounter[task] = Period[task];
			if(taskIsSuspended(WorkerId[task]))
				/* If no overrun, resume task */
				taskResume(WorkerId[task]);
			else {
				myLogMsg("Task%d OVERRUN!!!\n", (_Vx_usr_arg_t) task, 0, 0, 0, 0, 0 );
				Overruns[task] ++;
				InOverrun[task]++;
			}
		}
	}
}
