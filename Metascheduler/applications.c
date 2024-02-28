/*
 * applications.c
 *
 *  Author: Davide Corroppoli
 */

#include "applications.h"
#include "time.h"
#include "tasklib.h"


void Application0(int task) {
	switch (task) {
	case 0: /* task 0: [:3 ] */
		get_busy(60*OPSMSEC);
		break;
	}
}

void Application1(int task) {
	switch (task) {
	case 0: /* task 1: [:2.5 [Ra: 0.5]] */
		get_busy(20*OPSMSEC);
			EntrySection(0,0);
			get_busy(10*OPSMSEC);
			ExitSection(0,0);
		get_busy(20*OPSMSEC);
		break;
	case 1: /* task 2: [:6 [Ra: 5]] */
		get_busy(10*OPSMSEC);
			EntrySection(0,1);
			get_busy(100*OPSMSEC);
			ExitSection(0,1);
		get_busy(10*OPSMSEC);
		break;
	}
}

void Application2(int task) {
	switch (task) {
	case 0: /* task 1: [:3 [Rb:2]] */
		get_busy(10*OPSMSEC);
			EntrySection(1,0);
			get_busy(40*OPSMSEC);
			ExitSection(1,0);
		get_busy(10*OPSMSEC);
		break;
	case 1: /* task 2: [:5 [Ra:4]] */
		get_busy(10*OPSMSEC);
			EntrySection(0,1);
			get_busy(80*OPSMSEC);
			ExitSection(0,1);
		get_busy(10*OPSMSEC);
		break;
	case 2: /* task 3: [:4 [Rb:3 [Ra:1]]] */
		get_busy(10*OPSMSEC);
			EntrySection(1,2);
			get_busy(20*OPSMSEC);
				EntrySection(0,2);
				get_busy(20*OPSMSEC);
				ExitSection(0,2);
			get_busy(20*OPSMSEC);
			ExitSection(1,2);
		get_busy(10*OPSMSEC);
		break;
	}
}

void Application3(int task) {
	switch (task) {
	case 0: /* task 1: [:3 [Rb:2]] */
		get_busy(10*OPSMSEC);
			EntrySection(1,0);
			get_busy(40*OPSMSEC);
			ExitSection(1,0);
		get_busy(10*OPSMSEC);
		break;
	case 1: /* task 2: [:5 [Ra:4]] */
		get_busy(10*OPSMSEC);
			EntrySection(0,1);
			get_busy(80*OPSMSEC);
			ExitSection(0,1);
		get_busy(10*OPSMSEC);
		break;
	case 2: /* task 3: [:4 [Ra:3 [Rb:1]]] */
		get_busy(10*OPSMSEC);
			EntrySection(0,2);
			get_busy(20*OPSMSEC);
				EntrySection(1,2);
				get_busy(20*OPSMSEC);
				ExitSection(1,2);
			get_busy(20*OPSMSEC);
			ExitSection(0,2);
		get_busy(10*OPSMSEC);
		break;
	}
}

void Application4(int task) {
	switch (task) {
	case 0: /* task 1: [:2 [Ra:1]] */
		get_busy(10*OPSMSEC);
			EntrySection(0,0);
			get_busy(20*OPSMSEC);
			ExitSection(0,0);
		get_busy(10*OPSMSEC);
		break;
	case 1: /* task 2: [:2 [Rb:1]] */
		get_busy(10*OPSMSEC);
			EntrySection(1,1);
			get_busy(20*OPSMSEC);
			ExitSection(1,1);
		get_busy(10*OPSMSEC);
		break;
	case 2: /* task 3: [:6 [Rb:6 [Ra:3]]] */
		get_busy(10*OPSMSEC);
			EntrySection(1,2);
			get_busy(20*OPSMSEC);
				EntrySection(0,2);
				get_busy(60*OPSMSEC);
				ExitSection(0,2);
			get_busy(40*OPSMSEC);
			ExitSection(1,2);
		get_busy(10*OPSMSEC);
		break;
	}
}


void Application5(int task) {
	switch (task) {
	case 0: 
		EntrySection(0,0);
		get_busy(0.35*OPSMSEC);
		ExitSection(0,0);
		get_busy(0.15*OPSMSEC);
		break;
	case 1:
		EntrySection(0,1);
		get_busy(0.4*OPSMSEC);
		ExitSection(0,0);
		get_busy(0.10*OPSMSEC);
		break;
	case 2:
		EntrySection(1,2);
		get_busy(3*OPSMSEC);
		EntrySection(0,2);
		get_busy(1*OPSMSEC);
		ExitSection(0,2);
		ExitSection(1,2);
		break;
	case 3:
		EntrySection(2,3);
		get_busy(2*OPSMSEC);
		ExitSection(2,3);
		break;
		
		
	}
	
}



/* not used in the end
typedef struct{
	Size s;
	Kind k;
}trash;

trash tr[2][2];
*/
int beltk[2][2]; /* array containing the kind of the two pieces at bottom and two pieces at top of the conveyor belt*/
int belts[2][2]; /* array containing the size of the two pieces at bottom and two pieces at top of the conveyor belt*/

void Robot_Application(int task) {
	int kindA, kindB, kindC, kindD;
	switch(task) {
	case 0:
		/*We update the value generated randomly 2 by 2*/
		kind1 = visionk();
		kind2 = visionk();
		beltk[0][0] = kind1;
		beltk[0][1] = kind2;
		kind1 = visionk();
		kind2 = visionk();
		beltk[1][0] = kind1;
		beltk[1][1] = kind2;
		size1 = visions();
		size2 = visions();
		belts[1][0] = size1;
		belts[1][1] = size2;
		size1 = visions();
		size2 = visions();
		belts[0][0] = size1;
		belts[0][1] = size2;
		
	break;

	case 1: /* Robot A: big robot, provide pick up at bottom */
		/*semaphore 0: belt top, sem 1: belt bottom, sem 2: common workspace, sem 3: card, sem 4: plastic, sem 5: cardboard*/
		if (belts[0][1]!= 1 || belts[0][0] != 1) {
			/*If  there isn't a big trash at top, the task is supended and availibity is raised to 1, so metascheduler reactivate the task the next Cv cycle */
			availability[1] = 1;
			taskSuspend(0);
		}
		
		if (belts[0][0] == 1) { /* If we arrive here there's at least a big trash, then I assign to kind the correspondent kind
		Let's assume for ease that is picked up the closest piece, the one at zero in position, if it's big of course*/
		    kindA = beltk[0][0];
		}
		else if (belts[0][1] == 1){
			kindA = beltk[0][1];
		}
		EntrySection(0,1);
		get_belt(1);
	    
	    pick_trash(1);
	    ExitSection(0,1);
	    switch(kindA) { /*Three different cases corresponding to the kind of thrash*/
	       case 0: /* cardboard*/
		        EntrySection(2,1);
	            EntrySection(3,1);
	            trajectory(1,3);
	            ExitSection(3,1);
	            card_trash++;
	            return_home(1,3);
	            ExitSection(2,1);
	            break;
	       case 1: /*plastic*/
	    		EntrySection(2,1);
	            EntrySection(4,1);
	            trajectory(1,2);
	            ExitSection(4,1);
	            plastic_trash++;
	            return_home(1,2);
	            ExitSection(2,1);
	            break;
	       case 2: /*glass*/
	    		trajectory(1,1);
	    		glass_trash++;
	    		return_home(1,1);
	            break;
	    
	    }
	    bigt++;
	    processed_trash++;
	    
	    
	    break;
	    
	 
	 case 2: /* Robot B: small robot, provide pick up at bottom */
		/*semaphore 0: belt top, sem 1: belt bottom, sem 2: common workspace, sem 3: card, sem 4: plastic, sem 5: glass*/
		if (belts[0][1]!= 0 || belts[0][0] != 0) {
			availability[2] = 1;
		 	taskSuspend(0);
		}
		if (belts[0][1] == 0) { /* If we arrive here there's at least a small trash, then I assign to kind the correspondent kind
				Let's assume for ease that is pick up the closest piece the one in position, if it's small of course*/
			kindB = beltk[0][1];
		}
		else if (belts[0][0] == 0){ /* given that the closest for the big robot is the one in zero position, for the small should be the one in one position*/
			kindB = beltk[0][0];
		}
		EntrySection(0,2);
	    get_belt(2);
	    
	    pick_trash(2);
	    ExitSection(0,2);
	    trajectory(0,1); /* kind is unrelevant for the small robot, we assume the same duration */
	    throw();
	    		
	    return_home(0,1);
	    switch(kindB) { /* Just for statistics, it doesn't affect the duration f trajectory/task the kind of trash in small robot picking*/
	    case(0):
	    		card_trash++;
	    break;
	    case(1):
	    		plastic_trash++;
	    break;
	    case(2):
	    		glass_trash++;
	    }
	    smallt++;
	    processed_trash++;
	    break;
	 case 3: /* Robot C: big robot, provide pick up at bottom */
		    if (belts[1][1]!= 1 || belts[1][0] != 1) { /*if there are now big trash available suspend the task*/
		    	availability[3] = 1;
		 		taskSuspend(0);
		 	}
		    
		    if (belts[1][0] == 1) { /* If we arrive here there's at least a big trash, then I assign to kind the correspondent kind*/
		    	kindC = beltk[1][0];
		    }
		    else {
		    	kindC = beltk[1][1];
		    }
		    EntrySection(1,3);
	    	get_belt(3);
	    	
	    	pick_trash(3);
	    	ExitSection(1,3);
	    	switch (kindC) {
	    	     case 0: /* cardboard*/
	    		            	            
	    	        trajectory(1,1);
	    	        return_home(1,1);
	    	        card_trash++;
	    	            
	    	     break;
	    	     case 1: /*plastic*/
	    	    	EntrySection(2,3);
	    	        EntrySection(4,3);
	    	        trajectory(1,2);
	    	        ExitSection(4,3);
	    	        return_home(1,2);
	    	        ExitSection(2,3);
	    	        plastic_trash++;
	    	        
	    	     break;
	    	     case 2: /*glass*/
		            EntrySection(2,3);
			    	EntrySection(5,3);
			    	trajectory(1, 3);
			    	ExitSection(5,3);
			    	return_home(1,3);
			    	ExitSection(2,3);
			    	glass_trash++;

	    	     break;
	    	    
	    	    }
	    bigt++;
	    processed_trash++;
	    break;
	    case 4: /* Robot D: small robot, provide pick up at bottom */
	    			/*semaphore 0: belt top, sem 1: belt bottom, sem 2: common workspace, sem 3: card, sem 4: plastic, sem 5: glass*/
	    	if (belts[1][1]!= 0 || belts[1][0] != 0) {
	    		    availability[4] = 1;
	    			taskSuspend(0);
	    	}
	    	if (belts[0][1] == 0) { /* If we arrive here there's at least a small trash, then I assign to kind the correspondent kind
	    					Let's assume for ease that is pick up the closest piece the one in position, if it's small of course*/
	    		kindD = beltk[1][1];
	    	}
	    	else { /* given that the closest for the big robot is the one in zero position, for the small should be the one in one position*/
	    		kindD = beltk[1][0];
	    	}
	    	EntrySection(1,4);
	    	get_belt(4);
	    	
	    	pick_trash(4);
	    	ExitSection(1,4);
	    	trajectory(0, 1); /*kind actually is not important in small robot, we assume the same duration for every trajectory*/
	    	throw();
	    		    		
	    	return_home(0,1);
	    	
	    	switch(kindD) { /* Just for statistics, it doesn't affect the duration f trajectory/task the kind of trash in small robot picking*/
	    		case(0):
	    		    	card_trash++;
	    		    break;
	    	    case(1):
	    		    	plastic_trash++;
	    		    break;
	    	    case(2):
	    		    	glass_trash++;
	    	}
	    	smallt++;
	    	processed_trash++;
	    	break;
	    	
	    	}
	    	
	
}
	    
	    

