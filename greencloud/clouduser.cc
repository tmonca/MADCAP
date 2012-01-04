/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
/*
 */

#include "clouduser.h"

CloudUser::CloudUser()
{ 
  tsksize_ = 0;
  tskmips_ = 0;
  tskmaxduration_ = 0.0;
  dc_ = NULL;
  toutputsize_ = tintercom_ = 0;
  
  taskcounter_ = 0;
}

CloudUser::~CloudUser()
{
}

TskObject *CloudUser::createTask()
{
  TskObject *pTskObj = new TskObject(tsksize_, tskmips_, tskmaxduration_);
  pTskObj->setID(taskcounter_);
  pTskObj->setOutput(toutputsize_);
  pTskObj->setIntercom(tintercom_);
  taskcounter_++;
  
  return pTskObj;
}
