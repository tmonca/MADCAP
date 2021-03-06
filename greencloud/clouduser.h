/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef ns_clouduser_h
#define ns_clouduser_h

#include "tclcl.h"
#include "datacenter.h"
#include "tskobject.h"

class CloudUser {
public:
	CloudUser();
	virtual ~CloudUser();
	TskObject *createTask();
	
	/* Task properties */
	unsigned int tskmips_;
	unsigned int tsksize_;
	double tskmaxduration_;
	
	int toutputsize_;
	int tintercom_;

protected:
	DataCenter *dc_;
	int taskcounter_;
};

#endif
