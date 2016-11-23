// This may look like C code, but it is really -*- C++ -*-

#ifndef _EXECUTORSBOX_MOVEBATCLAMPS_H
#define _EXECUTORSBOX_MOVEBATCLAMPS_H

#include "executor.h"
#include "lrs_msgs_tst/ConfirmReq.h"

#include "ros/ros.h"

namespace Exec {

  class MoveBatClamps : public virtual Executor {
  private:
	int8_t direction;
	bool mission_succesfull;			//true if the command is accomplished
	int max_time;                //max time for execution (ms)

  public:
	MoveBatClamps (std::string ns, int id);
	virtual ~MoveBatClamps () {};

	bool prepare ();
	void start ();
	//virtual bool abort ();
	
  };

};

#endif
