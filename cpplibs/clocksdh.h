#ifndef ns_clocksdh_h
#define ns_clocksdh_h

#include "object.h"
#include "random.h"
#include <tclcl.h>
//#include <cmath>
#include <math.h>


class ClockSdh : public TclObject {
public:
	ClockSdh();
	virtual ~ClockSdh();
	virtual void doTick();
		
	double getTime();
	inline int getTicksNumber(){return tick_;};
	inline double getStandBitTx(){return StandBitTx;};
	inline void setTime(double tmToSet){time_ = tmToSet - tick_ * StandBitTx - drift_;};
 	inline void addToTime(double tmToAdd){time_ += tmToAdd;};

	double stdTickTime_;	// standard deviation
		
	FILE * file1;
	//	ClockSdh *nextClkp;	//Created as a performance test for linked list
protected:	
	double driftIncrement_; 	// drift increment user defined
	double drift_;	                // drift value dynamically modified
	double maxDrift_;		// maximum possible drift boundary for meanTickTime_ if overtaken the sign is changed
	double StandBitTx;

	double time_;
	int tick_;	//
};
#endif
