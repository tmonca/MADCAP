/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 1994-1997 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef ns_iwfr_h
#define ns_iwfr_h

#include <tclcl.h>

#include <string.h>
#include "agent.h"
#include "config.h"
#include "packet.h"
#include "ip.h"
#include "rtp.h"
#include "queue.h"
#include "clockcontainer.h"
//#include "timer-handler.h"

/*------------------Added code----------------------*/

// Definition of class Queue
class IwfrQueue: public Queue {
public:
	inline IwfrQueue(Queue& q) : IWFRQueue_(q) {}
private:		
	Queue& IWFRQueue_;
};

class Iwfr;

// Definition of class Timer
class IwfrTimer : public TimerHandler {
public:
   IwfrTimer(Iwfr* Timer) : TimerHandler() { 
   	Timer_ = Timer;
   }
protected:
   virtual void expire(Event *e);
   Iwfr* Timer_;
};

/*--------------------------------------------------*/

class Iwfr : public Agent {
public:
	Iwfr();
	virtual int command(int argc, const char*const* argv);
	void recv(Packet* pkt, Handler*);
	
	/*------------------Added code----------------------*/
	virtual bool rcompensation() const { return (rcompensation_); }
	
	// Initialize the queue
	IwfrQueue* IWFRQueue_;
	PacketQueue q_;
	void callGetSequence();
	void graphsData(Packet*);
	void packetFree(Packet*);
	double reconstructionTime_;
	ClockContainer* clkCnt_; //ADDED CODE
	virtual void assigncont(ClockContainer *clkContontainer); //ADDED CODE
	/*--------------------------------------------------*/
		
protected:
	int nlost_;
	int npkts_;
	int expected_;
	int bytes_;
	
	/*------------------Added code----------------------*/
	int totnpkts_;     	 	// Total packet number
	int totpkts_;        		// Total packet number
	int totbiterr_;      		// Total bit errors number
	int hdrreconstruct_; 		// Reconstruction of stm frame
	int buffSize_;       		// Value of buffer size
	int rcompensation_;  		// Reordering compensation model
	int maxbuffsize_;    		// Max value for buffer size
	double end_to_end_delay_;	// End to end delay
	int pks_per_frame_;		// Number of packets per frame
	double wirelessLatency_;	// Wireless bitrate
	IwfrTimer outputTimer_;		// Timer in IWFR
	int oneTime_;
	int missingPackets_;		// Missing packets calculation
	int packetID_;			// Pointer to expected packet in the head of queue
	int frames;
	int counter_;
	double time_sent_;
	/*--------------------------------------------------*/
	
	int seqno_;
	double last_packet_time_;
};

#endif // ns_loss_monitor_h
