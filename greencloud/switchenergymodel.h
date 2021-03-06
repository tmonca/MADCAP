/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef ns_switchenergymodel_h
#define ns_switchenergymodel_h

#include "tclcl.h"
#include "classifier.h"
#include "timer-handler.h"
#include "app.h"

class Classifier;

class SwitchEnergyModel;

class SwitchEnergyTimer : public TimerHandler {
public:
	SwitchEnergyTimer(SwitchEnergyModel* em) : em_(em) {}
protected:
	void expire(Event*);
	SwitchEnergyModel* em_;
};

class SwitchEnergyModel : public TclObject {
public:
	SwitchEnergyModel();
	virtual ~SwitchEnergyModel();
	
	virtual int command(int argc, const char*const* argv);
	virtual void timeout();
	void setClassifier(Classifier *classifier){classifier_ = classifier;};
	void updateEnergy(int curSlot, int nports);
	
	void start();
	void stop();
	
	double eConsumed_;
	
	double eChassis_;
	double eLineCard_;
	double ePort_;
	
	double eSimEnd_;
	
	int eDVFS_enabled_;
	int eDNS_enabled_;
	double eDNS_delay_;
	
protected:
	double computeCurrentRate();
  
	int eEnabled_;
	
	double eCurrentRate_;
	double eLastSample_;
	int eActivePorts_;
	
	double eSimDuration_;
	
	Classifier *classifier_;
	SwitchEnergyTimer energytimer_;
};

#endif
