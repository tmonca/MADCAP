/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 * Copyright (c) 1996 Regents of the University of California.
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
 * 	This product includes software developed by the MASH Research
 * 	Group at the University of California Berkeley.
 * 4. Neither the name of the University nor of the Research Group may be
 *    used to endorse or promote products derived from this software without
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
    "@(#) $Header: /cvsroot/nsnam/ns-2/common/clocksdh.cc,v 1.43 2008/11/03 $";
#endif

#define ABS(x) ((x)<0?-(x):(x))

#include <stdlib.h>
#include "clocksdh.h"
// this class was written by someone in a galaxy far far away...
static class ClockSdhClass : public TclClass {
public:
	ClockSdhClass() : TclClass("ClockSdh") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new ClockSdh());
	}
} class_clocksdh;

ClockSdh::ClockSdh()
{	
	time_ = 0.0;
	tick_ = 0;

	maxDrift_ = 0.0;	
	driftIncrement_ = 0.0; 
	drift_ = 0.0;
	
	stdTickTime_ = 0.0;
	StandBitTx = 6.4300411;
	
	bind("driftIncrement_",&driftIncrement_);
	bind("maxDrift_",&maxDrift_);
	bind("stdTime_", &stdTickTime_);
	bind("currTime_", &time_);
		
}


ClockSdh::~ClockSdh()
{
	
}

void ClockSdh::doTick()
{
	// incrementValue_ is a positive or negative interval triangle saw tooth wave
	if(fabs(drift_ + driftIncrement_) - maxDrift_ > 0.1 * driftIncrement_)
	{
		driftIncrement_ = -driftIncrement_;
	}
	drift_ += driftIncrement_;

	tick_ ++;
}

double ClockSdh::getTime()
{
	return tick_ * StandBitTx + time_ + drift_; 
}

