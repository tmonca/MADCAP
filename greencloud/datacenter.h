#ifndef ns_datacenter_h
#define ns_datacenter_h

#include "object.h"
#include "dchost.h"
#include "tskagent.h"
#include <tclcl.h>
#include <vector>
#include <math.h> 
#include <iostream> 
#include <numeric> 

class DataCenter : public TclObject {
public:
	DataCenter();
	virtual ~DataCenter();
	void addHostPointer(DcHost *newhst); 
 	void addTaskAgentPointer(TskComAgent *newagnt);

	virtual void receivedTsk(int tsksize, unsigned int pTask, const char* flags = 0);	
	virtual int command(int argc, const char*const* argv);

	/* Stats */
	int tskSubmitted_;
	double avgLoad_;

protected:
	vector <DcHost*> host_list;
	vector <TskComAgent*> agent_list;
	
	DcHost *newhost_;

	/* Schedulers */
	int numTskAgents_;
	
	TskComAgent *scheduleRoundRobin(TskObject *tsk);
	TskComAgent *scheduleGreen(TskObject *tsk);
	void computeLoad();
};

#endif
