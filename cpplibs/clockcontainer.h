#ifndef ns_clockcontainer_h
#define ns_clockcontainer_h

#include "object.h"
#include <tclcl.h>
#include "clocksdh.h"
#include <vector>
#include <math.h> 
#include <iostream> 
#include <numeric> 

class ClockContainer : public TclObject {
public:
	ClockContainer();
	virtual ~ClockContainer();
	virtual void tickN(int noTicks);
	virtual void addClock(ClockSdh *clk); //adds a clock from front to the linked list
	virtual int command(int argc, const char*const* argv); 
	
	int reportingInterval_;
	int recoveryType_; 		//2 types if 0 then immediate correction if 1 smooth
	int ticksForCorrection_;	//number of ticks to make correction

	/*----------Set of parameteres for Moving Averages---------*/ 
	vector <double> data_set;
	int currentIter_, windowSize_, emaFlag_;
	double av_, lastS_, currentS_, lastElem_, alpha_; 
	
	double MovAv(double data, int window);
	double ExpMovAv(double data, double smoothf);
	/*--------------------------------------------------------*/
		
protected:
	vector<ClockSdh*> vectorClk; //vector of pointers
//	ClockSdh *headClk;		//TEST FOR LINKED LIST
		
	FILE * pFile8;
	FILE * pFile9;
	FILE * pFile10;
	FILE * pFile11;
	double clkEdiff_, clkEreceived_, clkEMovAv_, clkEExpMovAv_;		
};

#endif
