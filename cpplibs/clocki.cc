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
    "@(#) $Header: /cvsroot/nsnam/ns-2/common/clocki.cc$";
#endif

#include <stdlib.h>
#include "clocki.h"

static class ClockIClass : public TclClass {
public:
	ClockIClass() : TclClass("ClockSdh/ClockI") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new ClockI());
	}
} class_clocki;

ClockI::ClockI() : ClockSdh()
{	
	lockToMaster_ = NULL;
	bind("ticksToSynch_", &numTickstoSynch_);
}


ClockI::~ClockI()
{
	
}

void ClockI::doTick()
{
	ClockSdh::doTick();
	
	if(lockToMaster_ && (tick_ % numTickstoSynch_ == 0))
		setTime(lockToMaster_->getTime());			

}

int ClockI::command(int argc, const char*const* argv)
{
	if(argc == 3) {
  		if(strcmp(argv[1], "locked") == 0) {
				
			ClockSdh *ref = dynamic_cast<ClockSdh *> (TclObject::lookup(argv[2]));
 			if(ref){
				lockToMaster_ = ref;
				return (TCL_OK);
			}
		return (TCL_ERROR);
		}
	}
	return (ClockI::command(argc, argv));
}
