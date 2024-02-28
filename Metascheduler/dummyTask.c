/*
 * dummyTask.c
 *
 * Author: Davide Corroppoli
 */

#include "dummyTask.h"
#include "math.h"
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "tasklib.h"


#define TPT 2

#define TCS 6
#define TT 3
#define TGB 4
void all_done(void) {}

void get_busy(int n) {
	int i, r=LOWER_LIMIT, going=UP;
	for(i=0;i<n;i++) {
		if(going==UP) {
			if(r<UPPER_LIMIT)
				r++;
			else
				going=DOWN;
		}
		else {
			if(r>LOWER_LIMIT)
				r--;
			else
				going=UP;
		}
	}
	all_done();
}


void trajectory(int size,int far) {
	get_busy(40*OPSMSEC);
	if (size == 1) {
		switch(far) {
		case 1:
			taskDelay(8);
			break;
		case 2: 
			taskDelay(10);
			break;
		case 3:
			taskDelay(12);
			break;
		}
		
	}
	else {
		taskDelay(TCS);
	}
			
}

void return_home(int size, int far) {
	get_busy(40*OPSMSEC);
	if (size == 1) {
			switch(far) {
			case 1:
				taskDelay(8);
				break;
			case 2: 
				taskDelay(10);
				break;
			case 3:
				taskDelay(12);
				break;
			}
			
		}
		else {
			taskDelay(TCS);
		}
	
}

int visionk() {
	get_busy(2*OPSMSEC); /*Time wasted to acced the camera, receive data from the camera and process the data */
	int kind;
	kind = rand()%3;
	
	return kind;
}

int visions(){
	get_busy(2*OPSMSEC);
	int size;
	size = rand()%2;
	return size;
}


void throw() {
	get_busy(20*OPSMSEC);
	taskDelay(TT);
}

void get_belt(int task){
	get_busy(20*OPSMSEC);
	taskDelay(TGB);
	/*printf("robot %d access the belt", task);*/
}

void pick_trash(int task){
	get_busy(20*OPSMSEC);
	taskDelay(TT);
	
}
