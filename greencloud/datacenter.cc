/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /cvsroot/nsnam/ns-2/common/datacenter.cc,v 1.43 $";
#endif

#include <stdlib.h>
#include "datacenter.h"
#include "tskobject.h"

static class DataCenterClass : public TclClass {
public:
	DataCenterClass() : TclClass("DataCenter") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new DataCenter());
	}
} class_datacenter;

DataCenter::DataCenter() : tskSubmitted_ (0), avgLoad_(0.0), numTskAgents_(0)
{
    bind("tskSubmitted_", &tskSubmitted_);
    bind("avgLoad_", &avgLoad_);
}


DataCenter::~DataCenter()
{
	host_list.~vector();
	agent_list.~vector();
}

void DataCenter::addHostPointer(DcHost *newhst)
{	
	host_list.push_back(newhst);	
}

void DataCenter::addTaskAgentPointer(TskComAgent *newagnt)
{
	newagnt->set_pkttype(PT_CLOUD_USER);
	agent_list.push_back(newagnt);
}

void DataCenter::receivedTsk(int tsksize, unsigned int pTask, const char* flags)
{
	/* Update Stats */
	tskSubmitted_ ++;
	
	/* Schedule it */
	TskComAgent *tagent_ = scheduleGreen((TskObject *) pTask);
	tagent_->sendmsg(tsksize, pTask, flags);
}

TskComAgent *DataCenter::scheduleRoundRobin(TskObject *tsk)
{
    int j = tskSubmitted_% numTskAgents_;
    
    return (agent_list.at(j));
}

TskComAgent *DataCenter::scheduleGreen(TskObject *tsk)
{
    vector <DcHost*>::iterator iter;
    
    for (iter = host_list.begin(); iter!=host_list.end(); iter++)
    {
      if ((*iter)->trySchedulingTsk(tsk)){
	return (agent_list.at((*iter)->host_id_));
	
      }
    }
    
    return (agent_list.at(0));
}

int DataCenter::command(int argc, const char*const* argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "compute-load") == 0) { 
		  computeLoad();
		  return (TCL_OK);
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "add-dchost") == 0) { 
		  DcHost *hst = dynamic_cast<DcHost*> (TclObject::lookup(argv[2]));
		  if(hst){
			  addHostPointer(hst);
     			  return (TCL_OK);
 		  }
		  return (TCL_ERROR);
	   }
           else if (strcmp(argv[1], "add-taskagent") == 0) { 
		  TskComAgent *agnt = dynamic_cast<TskComAgent*> (TclObject::lookup(argv[2]));
		  if(agnt){
			  addTaskAgentPointer(agnt);
			  numTskAgents_ = (int) agent_list.size();
     			  return (TCL_OK);
 		  }
		  return (TCL_ERROR);
	   }
	}
	return (DataCenter::command(argc, argv));	
}

void DataCenter::computeLoad()
{
  /* Traverse servers and compute their average load */
  vector <DcHost*>::iterator iter;
  
  double avgLoad = 0;
  for (iter = host_list.begin(); iter!=host_list.end(); iter++)
  {
    avgLoad += (*iter)->getCurrentLoad();
  }
  
  avgLoad_ = avgLoad / host_list.size();
}


