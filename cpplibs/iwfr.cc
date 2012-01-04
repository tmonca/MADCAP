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

#ifndef lint
static const char rcsid[] =
    "@(#) $Header: /cvsroot/nsnam/ns-2/tools/loss-monitor.cc,v 1.18 2000/09/01 03:04:06 haoboy Exp $ (LBL)";
#endif

#include <tclcl.h>

#include "agent.h"
#include "config.h"
#include "packet.h"
#include "ip.h"
#include "rtp.h"
#include "iwfr.h"

static class IwfrClass : public TclClass {
public:
	IwfrClass() : TclClass("Agent/IWFR") {}
	TclObject* create(int, const char*const*) {
		return (new Iwfr());
	}
} class_iwfr;

Iwfr::Iwfr() : Agent(PT_NTYPE), outputTimer_(this)
{
	bytes_ = 0;
	nlost_ = 0;
	npkts_ = 0;
	expected_ = -1;
	last_packet_time_ = 0.;
	seqno_ = 0;
	
	/*------------------Added code----------------------*/
	totnpkts_ = 0;
	totpkts_ = 0;
	totbiterr_ = 0;
	buffSize_ = 0;
	maxbuffsize_ = 0;
	counter_ = 0;
	pks_per_frame_ = 1;
	end_to_end_delay_ = 0;
	bind("pktsForFrame_", &pks_per_frame_);
	bind("totnpkts_", &totnpkts_);
	bind("totpkts_", &totpkts_);
	bind("totbiterr_", &totbiterr_);
	bind("buffSize_", &buffSize_);
	bind("packetID_", &packetID_);
	bind("frames", &frames);
	bind("hdrRcstr_", &hdrreconstruct_);
	bind_bool("rcompensation_", &rcompensation_);
	bind("reconstructionTime_", &reconstructionTime_);
	bind("entoenDelay_", &end_to_end_delay_); //ADDED CODE
	bind("wirelessLatency_", &wirelessLatency_); //ADDED CODE
	bind("maxbuffsize_", &maxbuffsize_);
	/*--------------------------------------------------*/
	
	bind("nlost_", &nlost_);
	bind("npkts_", &npkts_);
	bind("bytes_", &bytes_);
	bind("lastPktTime_", &last_packet_time_);
	bind("expected_", &expected_);
}

/*-----------------------Added code---------------------------*/	

void Iwfr::packetFree(Packet* pkt)
{
	hdr_rtp* p = hdr_rtp::access(pkt);
	seqno_ = p->seqno();
	
	/*
	 * Check for lost packets
	 */
	if (expected_ >= 0) {
		int loss = seqno_ - expected_;
		if (loss > 0) {
			nlost_ += loss;
			Tcl::instance().evalf("%s log-loss", name());
		}
	}
	expected_ = seqno_ + 1;
	Packet::free(pkt);
}

void Iwfr::callGetSequence()
{
	// Extract the packet in the head of the queue
	Packet* pkt = q_.getSequence(packetID_);
	if(pkt != 0) {
		// Update the pointer for the next packet
		buffSize_ -= 1;
		packetID_++;
		packetFree(pkt);
	} else {
		// Update the missing packets value
		missingPackets_++;
		packetID_++;
	}
}

void IwfrTimer::expire(Event *e)
{
	// Execute operations on packet
	Timer_->callGetSequence();
	
	// Reschedule again for next packet
	resched(Timer_->reconstructionTime_);
}

void Iwfr::graphsData(Packet* pkt)
{
	bytes_ += hdr_cmn::access(pkt)->size();
	++npkts_;

	if (clkCnt_) clkCnt_->tickN(8*(hdr_cmn::access(pkt)->size())); //ADDED CODE 
			 	
	// Perform the count of the buffer size
	if (maxbuffsize_ < buffSize_)
		maxbuffsize_ = buffSize_;
		
	// Variables useful to build graphs in tcl code		
	++totnpkts_;	
	++totpkts_;	
	totbiterr_ += hdr_cmn::access(pkt)->errbitcnt();
	
	hdr_cmn::access(pkt)->size() = hdr_cmn::access(pkt)->size() + hdrreconstruct_;			
	//printf("Time %e\n", Scheduler::instance().clock()-last_packet_time_);
	last_packet_time_ = Scheduler::instance().clock();
	
	if (pks_per_frame_ > 0 ){
		counter_ ++;
		if (counter_ == 1){ 	
			time_sent_ = hdr_cmn::access(pkt)->pk_timestamp();
		}
		if(counter_ == pks_per_frame_){
			end_to_end_delay_ = Scheduler::instance().clock() - time_sent_;
			counter_ = 0;			
		}
	}


	wirelessLatency_ = hdr_cmn::access(pkt)->el_timestamp() - hdr_cmn::access(pkt)->sl_timestamp();	
	
	if (!rcompensation()) 		
		packetFree(pkt);
}

void Iwfr::recv(Packet* pkt, Handler*)
{
	// Check if the reordering compensation model is active
	if (rcompensation()) {
		buffSize_ += 1;
		
		if(IWFRQueue_->reordering_compensation(pkt, &q_)) {
		
	//printf("Time %e\n", Scheduler::instance().clock()-last_packet_time_);
			// Count graphs parameters
			graphsData(pkt);
			frames++;
			// Start with rescheduling (call once)
			if (oneTime_ == 0 & frames == 2) {
				oneTime_++;
				outputTimer_.resched(reconstructionTime_);
			} 	
		}
	} else {
		// Count graphs parameters
		graphsData(pkt);
	}
}

void Iwfr::assigncont(ClockContainer *clkContontainer)
{
	clkCnt_ = clkContontainer;
}
/*------------------------------------------------------------*/

/*
 * $proc interval $interval
 * $proc size $size
 */
int Iwfr::command(int argc, const char*const* argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "clear") == 0) {
			expected_ = -1;
			return (TCL_OK);
		} 
	} 
	else if(argc == 3) {
  		if (strcmp(argv[1], "assign-container") == 0) { 	//ADDED CODE
			ClockContainer *clkcont_ = dynamic_cast<ClockContainer *> (TclObject::lookup(argv[2]));
		  	if(clkcont_){
				assigncont(clkcont_);
				return (TCL_OK);
			}
		  	return (TCL_ERROR);	
 		}
	}
	return (Agent::command(argc, argv));
}
