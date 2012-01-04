#ifndef ns_clocki_h
#define ns_clocki_h

#include "object.h"
#include "clocksdh.h"
#include <tclcl.h>



class ClockI : public ClockSdh {
public:
	ClockI();
	virtual ~ClockI();
	
	virtual int command(int argc, const char*const* argv);
	void doTick();	
	int numTickstoSynch_;
	
protected:
	ClockSdh *lockToMaster_;
};


#endif
