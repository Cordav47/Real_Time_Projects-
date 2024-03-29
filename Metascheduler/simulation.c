/*
 * simulation.c
 *
 *  Author: Davide Corroppoli
 */

#include "simulation.h"
#include <stdio.h>
#include "stdlib.h"

/* Resource information */
SEM_ID Semaphore[N_SEMAPHORES];
int Ceiling[N_SEMAPHORES]=CEILINGS;

/* Semaphore for batch-mode simulation */
SEM_ID BatchSem;

/* Simulation parameters*/
int ResourceAccessProtocol = MP;
int TotalSimulationTicks = 300; /* total number of simulation ticks; defaults to hyperperiod */
WDOG_ID WdId; /* watch dog for terminating simulation after SimulationCounter ticks */
TASK_ID CleanUpId; /* clean up task upon simulation termination */
int AuxClkRate = 50;
int Verbose;
int BatchMode;
int Phased = 1;

/*in the end I didn't use the typedef defined below*/
typedef enum {BIG, SMALL} Size;
typedef enum {PLASTIC, GLASS, CARDBOARD} Kind;



/* Simulation functions */
void TaskShell(int task) {
	FOREVER {
		taskSuspend(0);
		Instance[task]++;
		APPLICATION(task);
		ShowJobNumber(task);
	}
}

void StartSimulation(int sec) {
	static int WorkerCounter = 1;
	int semaphore, SemOptions=SEM_Q_PRIORITY;
	int task;
	char task_name[15][N_TASKS];
	
	/* initialize simulation parameters */
	for(task=0;task<N_TASKS;task++) {
		NotFinished[task]=FALSE;
		Overruns[task]=0;
		Instance[task]=0;
		ResponseTime[task]=0;
		JobReleaseCounter[task]=1+(Phased?Phase[task]:0);
		availability[task] = 0;
		processed_trash = 0;
		bigt = 0;
		smallt = 0;
		glass_trash = 0;
		plastic_trash = 0;
		card_trash = 0;
	}
	

	/* initialize semaphores */

	/* 
	 * edit here to customize semaphore options...
	 *
	 */
    SemOptions = SEM_Q_PRIORITY;
    if (ResourceAccessProtocol == PIP) {
    	SemOptions = SemOptions | SEM_INVERSION_SAFE;
    	/* SEM_INVERSION_SAFE protects the system from priority inversion. Task owning the semaphore will execute at highest 
    	 * priority of the task pended on the semaphore. Task's priority is raised, so become higher than current priority
    	 */
    }
	for(semaphore=0;semaphore<N_SEMAPHORES;semaphore++)
		Semaphore[semaphore]=semMCreate(SemOptions);
	for(task=0;task<N_TASKS;task++) {
		ResourceCount[task]=0;
		CurrentPriority[0][task]=TOP_PRIORITY+task;
	}

	/* set up watch-dog for termination management */
	WdId = wdCreate();
	if(!BatchMode) {
		if(!sec)
			TotalSimulationTicks=Hyperperiod()+Phase[0]+1;
		else
			TotalSimulationTicks = sec*sysAuxClkRateGet();
	}
	wdStart(WdId, TotalSimulationTicks, (FUNCPTR) WdHandler, TotalSimulationTicks);
	
	/* create tasks */
	MetaschedulerId = taskSpawn("tMetascheduler", 101, 0, 4000, (FUNCPTR) MetaschedulerShell, 0,0,0,0,0,0,0,0,0,0);
	for(task=0;task<N_TASKS;task++) {
		sprintf(task_name[task],"tWork-%d",WorkerCounter++);
		WorkerId[task] = taskSpawn(task_name[task], TOP_PRIORITY+task, 0, 4000, (FUNCPTR) TaskShell, (_Vx_usr_arg_t) task,0,0,0,0,0,0,0,0,0);
	}
	CleanUpId = taskSpawn("tCleanUp", 100, 0, 4000, (FUNCPTR) CleanUp, 0,0,0,0,0,0,0,0,0,0);
	
	/* start simulation */
	sysAuxClkEnable();
	sysAuxClkRateSet(AuxClkRate);
	myLogMsg("sysAuxClk rate is %d\n", sysAuxClkRateGet(), 0, 0, 0, 0, 0 );
	sysAuxClkConnect((FUNCPTR) AuxClkHandler, 0); 
	myLogMsg("sysAuxClkConnect()\n", 0, 0, 0, 0, 0, 0 );
	
}
/* Function to see the results*/
void Results() {
	printf("\nTotal Results of the simulation, have been processed %d trash, of which %d big and %d small\n", processed_trash, bigt, smallt);
	printf("\nFor what concern the kind of processed trash: have been sorted %d cardboard trash, %d plastic trash and %d glass trash\n", card_trash, plastic_trash, glass_trash);
}
void BatchModeSimulation(int sec) {
	int RAP;
	int VerboseState=Verbose;
	Verbose=FALSE;
	BatchMode=TRUE;
	if(!sec)
		TotalSimulationTicks=Hyperperiod()+Phase[0]+1;
	else
		TotalSimulationTicks = sec*sysAuxClkRateGet();
	BatchSem=semBCreate(SEM_Q_PRIORITY,SEM_FULL);
	if(!sec)
		logMsg( "Starting hyperperiod batch simulations\n", 0, 0, 0, 0, 0, 0 );
	else if(sec%60)
		logMsg( "Starting %d-sec batch simulations\n", sec, 0, 0, 0, 0, 0 );
	else
		logMsg( "Starting %d-min batch simulations\n", sec/60, 0, 0, 0, 0, 0 );
	for(RAP=0;RAP<5;RAP++) {
		semTake(BatchSem,WAIT_FOREVER);
		taskDelay(Hyperperiod());
		ResourceAccessProtocol=RAP;
		logMsg( "%d: %s\n", RAP, (_Vx_usr_arg_t)RAPName[RAP], 0, 0, 0, 0);
		StartSimulation(0);
	}
	semTake(BatchSem,WAIT_FOREVER);
	semDelete(BatchSem);
	BatchMode=FALSE;
	Verbose=VerboseState;
}

void WdHandler(int ticks) {
	taskResume(CleanUpId);
	myLogMsg("WdHandler() fired after %d system clock ticks (%d ticks/sec)\n", 
			ticks, sysClkRateGet(), 0, 0, 0, 0 );
}

void CleanUp() {
	int task, semaphore;
	taskSuspend(0);
	sysAuxClkDisable(); /* turn off system clock interrupts */
	myLogMsg("Terminating simulation\n", 0, 0, 0, 0, 0, 0 );
	taskDelete(MetaschedulerId);
	for(task=0;task<N_TASKS;task++)
		taskDelete(WorkerId[task]);
	for(semaphore=0;semaphore<N_SEMAPHORES;semaphore++)
		semDelete(Semaphore[semaphore]);
	logMsg(CLEAN_UP_STRING, CLEAN_UP_PARAMS);
	if(BatchMode)
		semGive(BatchSem);
}

/* Output functions, 8 output instead of six */
void myLogMsg(char* s, 
		_Vx_usr_arg_t a1, _Vx_usr_arg_t a2, _Vx_usr_arg_t a3, 
		_Vx_usr_arg_t a4, _Vx_usr_arg_t a5, _Vx_usr_arg_t a6) {
	if(Verbose)
		logMsg(s, a1, a2, a3, a4, a5, a6);
}

void ShowJobNumber(int task) {
	myLogMsg("%s: %d\n", (_Vx_usr_arg_t) taskName(0), Instance[task], 0, 0, 0, 0 );
}


/* Auxiliary functions */
int GCD(int a, int b) {
	while(a!=b) {
		if(a>b)
			a-=b;
		else
			b-=a;
	}
	return a;
}

int Hyperperiod() {
	int i, hyperperiod=Period[0];
	if(N_TASKS<1)
		return 1;
	for(i=1;i<N_TASKS;i++)
		hyperperiod *= Period[i]/GCD(hyperperiod,Period[i]);
	return hyperperiod;
}
