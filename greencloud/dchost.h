#ifndef ns_dchost_h
#define ns_dchost_h

#include "object.h"
#include "tskobject.h"
#include "config.h"
#include "scheduler.h"

#include <stdlib.h>
#include <limits.h>
#include <tclcl.h>
#include <vector>
#include <math.h> 
#include <iostream> 
#include <numeric>
#include "scheduler.h"
#include "agent.h"

class DcHost : public TclObject, public Handler {
public:
	DcHost();
	virtual ~DcHost();
	void recv(TskObject *tskobj);
	virtual int command(int argc, const char*const* argv);
	
	void computeCurrentLoad();	/* updates current load of the server after task is received or its execution completed */
	double getCurrentLoad();
	int trySchedulingTsk(TskObject *tskobj);

	int host_id_;
	int nominal_mips_;		/* maximum computing capacility of the server at the maximum frequency */
	int current_mips_;		/* current load of the server in mips */
	// int mips_infly_;		/* computing capacity already assigned by the scheduler for tasks in fly */
	// int current_load_mips_;	/* current load of the server in mips */

	/* Stats */
	int ntasks_;
	double currentLoad_;
	
	/* Energy model */
	double eConsumed_;			/* total W of energy consumed */
	double eNominalrate_;			/* nominal consumption rate at full load at max CPU frequency */
	double eCurrentConsumption_;		/* current consumption rate */
	
	int eDVFS_enabled_;
	int eDNS_enabled_;
	
	enum EventStatus { EVENT_IDLE, EVENT_PENDING, EVENT_HANDLING };
	
protected:
	vector <TskObject*> tasks_list_;	/* execution list */
	vector <TskObject*> tasks_assigned_;	/* tasks in fly */
	
	virtual void handle(Event* event);
	void updateEvent();
	void updateTskList();
	void updateTskComputingRates();
	void nextEvent(double delay);
	void setComputingRate();
	double getMostUrgentTaskRate();
	
	void setCurrentConsumption();
	void eUpdate();
	
	double eLastUpdateTime_;
	
	int status_;
	Event event_;
	
	Agent *agent_;
	
private:
	inline void _sched(double delay) {
		(void)Scheduler::instance().schedule(this, &event_, delay);
	}
	inline void _cancel() {
		(void)Scheduler::instance().cancel(&event_);
		// no need to free event_ since it's statically allocated
	}
	
};

#endif
