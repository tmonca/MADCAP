#ifndef ns_tskobject_h
#define ns_tskobject_h

#define DBL_MAX (9.999999999999999e999)

#include "scheduler.h"

class TskObject {
public:
	TskObject();
	TskObject(unsigned int size, unsigned int mips, double duration);
	virtual ~TskObject();
	
	void updateMIPS();
	void setComputingRate(double rate);
	double execTime();
	
	int getID() {return id_;};
	unsigned int getSize() {return size_;};
	unsigned int getMIPS() {return mips_;};
	double getDeadline() {return deadline_;};
	int getOutput() {return output_;};

	void setSize(unsigned int size) {size_ = size;};
	void setMIPS(unsigned int mips) {mips_ = mips;};
	void setExecTime(double execTime) {executedSince_ = execTime;};
	void setID(int id) {id_ = id;};
	void setDeadline(double deadline) {deadline_ = deadline;};
	void setOutput(int output) {output_ = output;};
	void setIntercom(int intercom) {intercom_ = intercom;};
	
	int id_;
	
protected:
	void handler(Event *);

	unsigned int size_;	/* amount of bytes transferred to servers for task execution */
	unsigned int mips_;	/* computational requirement of the task */
	double deadline_;	/* task deadline */
	
	int output_;	/* amount of data in bytes sent out of the data center upon task completion */
	int intercom_;	/* amoutn of data in bytes to be transferred to another data center application */
	
	double currProcRate_;	/* current processing rate of the task (determinded by the server) */
	double executedSince_;	/* last time instance of task execution */
};

#endif
