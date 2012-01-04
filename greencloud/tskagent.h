/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#ifndef ns_tskagent_h
#define ns_tskagent_h

#include "agent.h"
#include "trafgen.h"
#include "packet.h"
#include "tskobject.h"

//"rtp timestamp" needs the samplerate
#define SAMPLERATE 8000
#define RTP_M 0x0080 // marker for significant events

class TskComAgent : public Agent {
public:
	TskComAgent();
	TskComAgent(packet_t);
	virtual void sendmsg(int nbytes, unsigned int pTaskObj, const char *flags = 0)
	{
		sendmsg(nbytes, NULL, pTaskObj, flags);
	}
	virtual void sendmsg(int nbytes, AppData* data, unsigned int pTaskObj, const char *flags = 0);
	virtual void recv(Packet* pkt, Handler*);
	virtual int command(int argc, const char*const* argv);
protected:
	int seqno_;
};

#endif
