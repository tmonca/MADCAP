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
    "@(#) $Header: /cvsroot/nsnam/ns-2/common/clockcontainer.cc,v 1.43 $";
#endif

#include <stdlib.h>
#include "clockcontainer.h"

static class ClockContainerClass : public TclClass {
public:
	ClockContainerClass() : TclClass("ClockContainer") {}
	TclObject* create(int argc, const char*const*argv) {
		return (new ClockContainer());
	}
} class_clockcontainer;

ClockContainer::ClockContainer()
{	
	emaFlag_ = 0;
	//headClk = NULL;	//linked list
	bind("reportingInterval_", &reportingInterval_);
	bind("recoveryType_", &recoveryType_);
	bind("ticksForCorrection_", &ticksForCorrection_);
	bind("maWindowSize_", &windowSize_);
	bind("emaAlpha_", &alpha_);
	bind("emaFlag_", &emaFlag_);
	
	clkEreceived_ = 0.0;		
	clkEdiff_ = 0.0;

	clkEMovAv_ = 0.0;	//moving average
	clkEExpMovAv_ = 0.0;	//exponential moving average
	av_ = 0.0;		//variables used in Moving Average
	lastS_ = 0.0;
	currentS_ = 0.0;
	lastElem_ = 0.0;
	
	currentIter_ = 0;

}


ClockContainer::~ClockContainer()
{
	data_set.~vector();
	vectorClk.~vector();
	
//OR Delete LINKED LIST
/*
     ClockSdh *tempClk;

     tempClk = headClk;
     headClk = headClk->nextClkp;
     delete tempClk;
				
*/	
}

void ClockContainer::addClock(ClockSdh* clk)
{	
// Linked List method
/*	ClockSdh* tempClk;
	clk->nextClkp = NULL;

	if (headClk == NULL)
		headClk = clk;
	else {
		tempClk = headClk;
                while (tempClk->nextClkp != NULL) {
			tempClk = tempClk->nextClkp;
                }
         	tempClk->nextClkp = clk;
       }
*/
//OR Vector method
	vectorClk.push_back(clk);		
}

void ClockContainer::tickN(int nTicks)
{
	int tickCount = 0;
	
//METHOD USING LINKED LIST
/*	ClockSdh* tempClk;

	while(tickCount < nTicks){
		tempClk = headClk;

		do {
			tempClk->doTick();
	       		tempClk = tempClk->nextClkp;			
   	   	}while (tempClk != NULL);

		tickCount ++;
	}
*/

//METHOD USING VECTOR STRUCTURE		

	// vectorClk(0) is clkB
	// vectorClk(1) is clkI
	// vectorClk(2) is clkIprime
	// vectorClk(3) is clkEnominal

	pFile8 = fopen ("DiffBI.tr","a");
	pFile9 = fopen ("DiffIPI.tr","a");
 	pFile10 = fopen ("DiffEI.tr","a");
 	pFile11 = fopen ("DiffEE.tr","a");

	while(tickCount < nTicks)
	{
		for(int i = 0; i < vectorClk.size(); i ++)
		{
			vectorClk.at(i)->doTick();
			
		}		
 		if(recoveryType_ == 1)
 			if(tickCount < ticksForCorrection_)
			{
				vectorClk.at(3)->addToTime(clkEdiff_);						
			}
		
		tickCount ++;
	
		if(vectorClk.at(0)->getTicksNumber() % reportingInterval_ == 0)
		{
			fprintf(pFile8,"%d\t%E\n", vectorClk.at(0)->getTicksNumber(), (vectorClk.at(0)->getTime() - vectorClk.at(1)->getTime())/1e+9);
			fprintf(pFile9,"%d\t%E\n", vectorClk.at(2)->getTicksNumber(), (vectorClk.at(2)->getTime() - vectorClk.at(1)->getTime())/1e+9);
			fprintf(pFile10,"%d\t%E\n", vectorClk.at(3)->getTicksNumber(), (vectorClk.at(3)->getTime() - vectorClk.at(1)->getTime())/1e+9);
			fprintf(pFile11,"%d\t%E\n", vectorClk.at(3)->getTicksNumber(), (vectorClk.at(2)->getTime() + vectorClk.at(0)->getTime() - vectorClk.at(1)->getTime() - vectorClk.at(3)->getTime())/1e+9);
		}

	}
	
	clkEreceived_ = vectorClk.at(2)->getTime() + vectorClk.at(0)->getTime() - vectorClk.at(1)->getTime();
	
	if (recoveryType_ == 0)
	{
		// immediate update clkE after N ticks	
		vectorClk.at(3)->setTime(clkEreceived_);			
	}
	clkEdiff_ = (clkEreceived_ - vectorClk.at(3)->getTime())/ticksForCorrection_;
//OR
//	clkEMovAv_ = MovAv(clkEreceived_ - vectorClk.at(3)->getTime(), windowSize_);
// 	clkEdiff_ = clkEMovAv_/ticksForCorrection_;  

	if(emaFlag_ == 1)
	{
	  	clkEExpMovAv_ = ExpMovAv(clkEreceived_ - vectorClk.at(3)->getTime(), alpha_);	
	 	clkEdiff_ = (clkEExpMovAv_)/ticksForCorrection_;
	}


	fclose (pFile8);
	fclose (pFile9);
	fclose (pFile10);
	fclose (pFile11);
	
	
	
}

/**********************************************************************************
MovAv : computes the moving average 
**********************************************************************************/
double ClockContainer::MovAv(double data, int windowSize)
{
	double sum = 0.0;
	int i, j;

	if ((int)data_set.size() < windowSize)
	{
		data_set.push_back(data);
		for(i = 0; i < (int)data_set.size(); i++)
			sum += data_set.at(i);
		av_ = sum/data_set.size();
	}
	
	else if((int)data_set.size() == windowSize)
	{
		for(j = 1; j < (int)data_set.size(); j++)
			data_set.at(j-1) = data_set.at(j);

		data_set.at(data_set.size()-1) = data;

		for(i = 0; i < (int)data_set.size(); i++)
			sum += data_set.at(i);
		av_ = sum/data_set.size();
	}

	return av_;
}

/**********************************************************************************
ExpMovAv : computes the exponential weighted moving average
S(n) = (1-C)*x(n) +C*S(n-1)
**********************************************************************************/
double ClockContainer::ExpMovAv(double data, double smoothf)
{
	alpha_ = smoothf;

	if(++currentIter_ == 1)
	{
	      currentS_ = lastS_ = data;
    	}
	else
	{
		currentS_ = alpha_ * lastElem_ + (1-alpha_) * lastS_;
      		lastS_ = currentS_;
    	}
    	lastElem_ = data;
	
	return currentS_;

}

int ClockContainer::command(int argc, const char*const* argv)
{
	if (argc == 3) {
		if (strcmp(argv[1], "add-clock") == 0) { 
		  ClockSdh *clk = dynamic_cast<ClockSdh *> (TclObject::lookup(argv[2]));
		  if(clk){
			  addClock(clk);
     			  return (TCL_OK);
 			 }
		  return (TCL_ERROR);
		}
}
	return (ClockContainer::command(argc, argv));	
}
