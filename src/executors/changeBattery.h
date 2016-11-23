// This may look like C code, but it is really -*- C++ -*-

#ifndef _EXECUTORSBOX_CHANGEBATTERY_H
#define _EXECUTORSBOX_CHANGEBATTERY_H

#include "executor.h"
#include "lrs_msgs_tst/ConfirmReq.h"

#include "ros/ros.h"

namespace Exec {

  class ChangeBattery : public virtual Executor {
  private:
	int8_t direction;
	bool mission_succesfull;			//true if the command is accomplished
	int max_time;                //max time for execution (ms)

  public:
	ChangeBattery (std::string ns, int id);
	virtual ~ChangeBattery () {};

	bool prepare ();
	void start ();
	//virtual bool abort ();
	
  };

};

#endif
