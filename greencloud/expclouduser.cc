#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /cvsroot/nsnam/ns-2/tools/clouduser.cc,v 1.15  Exp $";
#endif

#include <stdlib.h>
 
#include "random.h"
#include "datacenter.h"
#include "trafgen.h"
#include "ranvar.h"
#include "clouduser.h"


/* implement an on/off source with exponentially distributed on and
 * off times.  parameterized by average burst time, average idle time,
 * burst rate and packet size.
 */

class ExpCloudUser : public TrafficGenerator, public CloudUser {
 public:
	ExpCloudUser();
	virtual double next_interval(int&);
        virtual void timeout();
	// Added by Debojyoti Dutta October 12th 2000
	int command(int argc, const char*const* argv);
	//ADDED CODE
	void addDataCenterPointer(DataCenter *joindc_);

 protected:
	void init();
	double ontime_;   /* average length of burst (sec) */
	double offtime_;  /* average length of idle time (sec) */
	double rate_;     /* send rate during on time (bps) */
	double interval_; /* packet inter-arrival time during burst (sec) */
	unsigned int rem_; /* number of packets left in current burst */

	/* new stuff using RandomVariable */
	ExponentialRandomVariable burstlen_;
	ExponentialRandomVariable Offtime_;
};


static class ExpCloudUserClass : public TclClass {
 public:
	ExpCloudUserClass() : TclClass("Application/Traffic/ExpCloudUser") {}
	TclObject* create(int, const char*const*) {
		return (new ExpCloudUser());
	}
} class_exp_cloud_user;

// Added by Debojyoti Dutta October 12th 2000
// This is a new command that allows us to use 
// our own RNG object for random number generation
// when generating application traffic

int ExpCloudUser::command(int argc, const char*const* argv){
        
        if(argc==3){
                if (strcmp(argv[1], "use-rng") == 0) {
                        burstlen_.seed((char *)argv[2]);
                        Offtime_.seed((char *)argv[2]);
                        return (TCL_OK);
                }
		//ADDED CODE
		else if (strcmp(argv[1], "join-datacenter") == 0) { 
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

ExpCloudUser::ExpCloudUser() : burstlen_(0.0), Offtime_(0.0)
{
	bind_time("burst_time_", &ontime_);
	bind_time("idle_time_", Offtime_.avgp());
	bind_bw("rate_", &rate_);
	bind("packetSize_", &size_);

	// Bind CloudUser variables
	bind("tskmips_", &tskmips_);
	bind("tsksize_", &tsksize_);
	bind("tskmaxduration_", &tskmaxduration_);
	bind("toutputsize_", &toutputsize_);
	bind("tintercom_", &tintercom_);
}

void ExpCloudUser::init()
{
        /* compute inter-packet interval during bursts based on
	 * packet size and burst rate.  then compute average number
	 * of packets in a burst.
	 */
	interval_ = (double)(size_ << 3)/(double)rate_;
	burstlen_.setavg(ontime_/interval_);
	rem_ = 0;
}

double ExpCloudUser::next_interval(int& size)
{
	double t = interval_;

	if (rem_ == 0) {
		/* compute number of packets in next burst */
		rem_ = int(burstlen_.value() + .5);
		/* make sure we got at least 1 */
		if (rem_ == 0)
			rem_ = 1;
		/* start of an idle period, compute idle time */
		t += Offtime_.value();
	}	
	rem_--;

	size = size_;
	return(t);
}

void ExpCloudUser::timeout()
{
	if (! running_)
		return;

	if (nextPkttime_ != interval_ || nextPkttime_ == -1){
	        dc_->receivedTsk(size_, (unsigned int) createTask(), "NEW_BURST");
	}
	else {
		dc_->receivedTsk(size_, (unsigned int) createTask());
	}
	
	
	/* figure out when to send the next one */
	nextPkttime_ = next_interval(size_);
	/* schedule it */
	if (nextPkttime_ > 0)
		timer_.resched(nextPkttime_);
}


