// This may look like C code, but it is really -*- C++ -*-

#ifndef _EXECUTORSBOX_MOVEREVOLVER_H
#define _EXECUTORSBOX_MOVEREVOLVER_H

#include "executor.h"
#include "lrs_msgs_tst/ConfirmReq.h"

#include "ros/ros.h"

namespace Exec {

  class MoveRevolver : public virtual Executor {
  private:
	int8_t direction;
	bool mission_succesfull;			//true if the command is accomplished
	int max_time;                //max time for execution (ms)

  public:
	MoveRevolver (std::string ns, int id);
	virtual ~MoveRevolver () {};

	bool prepare ();
	void start ();
	//virtual bool abort ();
	
  };

};

#endif
