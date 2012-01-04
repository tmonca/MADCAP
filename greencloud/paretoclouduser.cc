/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /cvsroot/nsnam/ns-2/tools/pareto.cc,v 1.9 2005/08/26 05:05:31 tomh Exp $";
#endif
 
#include "random.h"
#include "trafgen.h"
#include "clouduser.h"

class ParetoCloudUser : public TrafficGenerator, CloudUser {
 public:
	ParetoCloudUser();
	virtual double next_interval(int&);
	virtual void timeout();
	int on()  { return on_ ; }
 	// Added by Debojyoti Dutta October 12th 2000
	int command(int argc, const char*const* argv);
protected:
	void init();
	double ontime_;  /* average length of burst (sec) */
	double offtime_; /* average idle period (sec) */
	double rate_;    /* send rate during burst (bps) */
	double interval_; /* inter-packet time at burst rate */
	double burstlen_; /* average # packets/burst */
	double shape_;    /* pareto shape parameter */
	unsigned int rem_; /* number of packets remaining in current burst */
	double p1_;       /* parameter for pareto distribution to compute
			   * number of packets in burst.
		           */
	double p2_;       /* parameter for pareto distribution to compute
		     	   * length of idle period.
		     	   */
	int on_;          /* denotes whether in the on or off state */

	// Added by Debojyoti Dutta 13th October 2000
	RNG * rng_; /* If the user wants to specify his own RNG object */
};


static class POOTrafficClass : public TclClass {
 public:
	POOTrafficClass() : TclClass("Application/Traffic/ParetoCloudUser") {}
 	TclObject* create(int, const char*const*) {
		return (new ParetoCloudUser());
	}
} class_pareto_clouduser;

// Added by Debojyoti Dutta October 12th 2000
// This is a new command that allows us to use 
// our own RNG object for random number generation
// when generating application traffic

int ParetoCloudUser::command(int argc, const char*const* argv){
        
	Tcl& tcl = Tcl::instance();
        if(argc==3){
                if (strcmp(argv[1], "use-rng") == 0) {
			rng_ = (RNG*)TclObject::lookup(argv[2]);
			if (rng_ == 0) {
				tcl.resultf("no such RNG %s", argv[2]);
				return(TCL_ERROR);
			}                        
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

ParetoCloudUser::ParetoCloudUser() : rng_(NULL)
{
	bind_time("burst_time_", &ontime_);
	bind_time("idle_time_", &offtime_);
	bind_bw("rate_", &rate_);
	bind("shape_", &shape_);
	bind("packetSize_", &size_);
}

void ParetoCloudUser::init()
{
	interval_ = (double)(size_ << 3)/(double)rate_;
	burstlen_ = ontime_/interval_;
	rem_ = 0;
	on_ = 0;
	p1_ = burstlen_ * (shape_ - 1.0)/shape_;
	p2_ = offtime_ * (shape_ - 1.0)/shape_;
}

double ParetoCloudUser::next_interval(int& size)
{

	double t = interval_;

	on_ = 1;
	if (rem_ == 0) {
		/* compute number of packets in next burst */
		if(rng_ == 0){
			rem_ = int(Random::pareto(p1_, shape_) + .5);
		}
		else{
			// Added by Debojyoti Dutta 13th October 2000
			rem_ = int(rng_->pareto(p1_, shape_) + .5);
		}
		/* make sure we got at least 1 */
		if (rem_ == 0)
			rem_ = 1;	
		/* start of an idle period, compute idle time */
		if(rng_ == 0){
			t += Random::pareto(p2_, shape_);
		}
		else{
			// Added by Debojyoti Dutta 13th October 2000
			t += rng_->pareto(p2_, shape_);
		}
		on_ = 0;
	}	
	rem_--;

	size = size_;
	return(t);

}

void ParetoCloudUser::timeout()
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








