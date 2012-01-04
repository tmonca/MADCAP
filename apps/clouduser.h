#ifndef ns_clouduser_h
#define ns_clouduser_h

#include "object.h"
#include "tskobject.h"
#include "datacenter.h"
#include <tclcl.h>
#include <vector>
#include <math.h> 
#include <iostream> 
#include <numeric> 

class CloudUser : public TclObject {
public:
	CloudUser();
	virtual ~CloudUser();
	void addDataCenterPointer(DataCenter *joindc_);
	void addTask(TskObject *newtsk);
	void rmovTask(TskObject *tsk);
	virtual int command(int argc, const char*const* argv); 
	
protected:
//	double valueX_;
	FILE * pFile1;
	DataCenter *dc_; 
	vector <TskObject*> task_list;
};

#endif
