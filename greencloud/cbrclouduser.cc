/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#include <stdlib.h>
 
#include "random.h"
#include "trafgen.h"
#include "ranvar.h"
#include "clouduser.h"


/* 
 * Constant bit rate traffic source.   Parameterized by interval, (optional)
 * random noise in the interval, and packet size.  
 */

class CBRCloudUser : public TrafficGenerator, public CloudUser {
 public:
	CBRCloudUser();
	virtual double next_interval(int&);
	//HACK so that udp agent knows interpacket arrival time within a burst
	inline double interval() { return (interval_); }
	virtual void timeout();
	int command(int argc, const char*const* argv);
	
	//ADDED CODE
	void addDataCenterPointer(DataCenter *joindc_);
 protected:
	virtual void start();
	void init();
	double rate_;     /* send rate during on time (bps) */
	double interval_; /* packet inter-arrival time during burst (sec) */
	double random_;
	int seqno_;
	int maxpkts_;
};


static class CBRCloudUserClass : public TclClass {
 public:
	CBRCloudUserClass() : TclClass("Application/Traffic/CBRCloudUser") {}
	TclObject* create(int, const char*const*) {
		return (new CBRCloudUser());
	}
} class_cbr_clouduser;

int CBRCloudUser::command(int argc, const char*const* argv){
        
        if(argc==3){
		if (strcmp(argv[1], "join-datacenter") == 0) { 
		  DataCenter *dc = dynamic_cast<DataCenter*> (TclObject::lookup(argv[2]));
		  if(dc){
			  dc_ = dc;
     			  return (TCL_OK);
 			 }
		  return (TCL_ERROR);
		}
		//ADDED CODE
        }
        return Application::command(argc,argv);
}

CBRCloudUser::CBRCloudUser() : seqno_(0)
{
	bind_bw("rate_", &rate_);
	bind("random_", &random_);
	bind("packetSize_", &size_);
	bind("maxpkts_", &maxpkts_);
	
	// Bind CloudUser variables
	bind("tskmips_", &tskmips_);
	bind("tsksize_", &tsksize_);
	bind("tskmaxduration_", &tskmaxduration_);
}

void CBRCloudUser::init()
{
        // compute inter-packet interval 
	interval_ = (double)(size_ << 3)/(double)rate_;
	if (agent_)
		if (agent_->get_pkttype() != PT_TCP &&
 		    agent_->get_pkttype() != PT_TFRC)
			agent_->set_pkttype(PT_CBR);
}

void CBRCloudUser::start()
{
        init();
        running_ = 1;
        timeout();
}

double CBRCloudUser::next_interval(int& size)
{
	// Recompute interval in case rate_ or size_ has changes
	interval_ = (double)(size_ << 3)/(double)rate_;
	double t = interval_;
	if (random_)
		t += interval_ * Random::uniform(-0.5, 0.5);	
	size = size_;
	if (++seqno_ < maxpkts_)
		return(t);
	else
		return(-1); 
}

void CBRCloudUser::timeout()
{
        if (! running_)
		return;

	/* send a packet */
	dc_->receivedTsk(size_, (unsigned int) createTask());
	/* figure out when to send the next one */
	nextPkttime_ = next_interval(size_);
	/* schedule it */
	if (nextPkttime_ > 0)
		timer_.resched(nextPkttime_);
	else
		running_ = 0;
}
