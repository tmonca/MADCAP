/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef ns_tskcomsink_h
#define ns_tskcomsink_h

#include <tclcl.h>

#include "agent.h"
#include "config.h"
#include "packet.h"
#include "dchost.h"
#include "ip.h"
#include "rtp.h"
#include "tskobject.h"

class TskComSink : public Agent {
public:
	TskComSink();
	virtual ~TskComSink();
	void addDcHost(DcHost *newhost);
	virtual int command(int argc, const char*const* argv);
	virtual void recv(Packet* pkt, Handler*);
protected:
	int nlost_;
	int npkts_;
	int expected_;
	int bytes_;
	int seqno_;
	double last_packet_time_;

	DcHost *dchost_;
};

#endif 
