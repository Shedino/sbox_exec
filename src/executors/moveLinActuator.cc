#include "moveLinActuator.h"

#include "common_inc.h"

extern ros::NodeHandle * global_nh;
extern ros::Publisher * global_confirm_pub;
extern ros::Publisher * global_cmd_pub;

extern std::map<std::string, boost::thread *> threadmap;

extern int16_t global_seq;

using namespace std;




Exec::MoveLinActuator::MoveLinActuator (std::string ns, int id) : Executor (ns, id) {

  lrs_msgs_tst::TSTExecInfo einfo;
  einfo.can_be_aborted = true;
  einfo.can_be_enoughed = false;
  einfo.can_be_paused = true;
  set_exec_info(ns, id, einfo);
  update_from_exec_info (einfo);

}

bool Exec::MoveLinActuator::prepare () {
  bool res = true;
  ROS_INFO ("Exec::MoveLinActuator::prepare");
  mission_succesfull = false;

  if (res) {
    set_active_flag (node_ns, node_id, true);
  }

  return res;
}


void Exec::MoveLinActuator::start () {
	ROS_INFO ("Exec::MoveLinActuator::start: %s - %d", node_ns.c_str(), node_id);
	try {

		if (!do_before_work ()) {
		  return;
		}

		
		ROS_INFO ("Exec::MoveLinActuator: Execution unit: %s", tni.execution_ns.c_str());

		//TODO: INVIO MESSAGGI MAVLINK A BASSO LIVELLO SHERPABOX PER ESEGUIRE OPERAZIONE RICHIESTA
		mission_succesfull=true;

		//
		// Replace the sleep with useful work.
		//

		max_time = 5000; //ms

		boost::this_thread::interruption_point();
		for (int i=0; i<max_time; i++) {
			usleep(1000);
			boost::this_thread::interruption_point();
			if (mission_succesfull) break;
		}

		//
		// When we reach this point the node execution whould be finished.
		//

		if (!mission_succesfull){
			fail ("MoveLinActuator: Timeout");
			return;
		} else {
			set_succeeded_flag (node_ns, node_id, true);
			set_aborted_flag (node_ns, node_id, false);
			set_finished_flag (node_ns, node_id, true);
		}

		ROS_INFO ("Exec::MoveLinActuator: FINISHED");

		wait_for_postwork_conditions ();
	}
	catch (boost::thread_interrupted) {
		ROS_ERROR("BOOST INTERUPTED IN MoveLinActuator");
		set_succeeded_flag (node_ns, node_id, false);
		set_aborted_flag (node_ns, node_id, true);
		set_finished_flag (node_ns, node_id, true);
	}
}

