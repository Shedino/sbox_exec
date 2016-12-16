#include "lockWasp.h"

#include "common_inc.h"

extern ros::NodeHandle * global_nh;
extern ros::Publisher * global_confirm_pub;
extern ros::Publisher * global_cmd_pub;

extern std::map<std::string, boost::thread *> threadmap;

extern int16_t global_seq;

using namespace std;




Exec::LockWasp::LockWasp (std::string ns, int id) : Executor (ns, id) {

  lrs_msgs_tst::TSTExecInfo einfo;
  einfo.can_be_aborted = true;
  einfo.can_be_enoughed = false;
  einfo.can_be_paused = true;
  set_exec_info(ns, id, einfo);
  update_from_exec_info (einfo);

}

bool Exec::LockWasp::prepare () {
  bool res = true;
  ROS_INFO ("Exec::LockWasp::prepare");
  mission_succesfull = false;

  if (res) {
    set_active_flag (node_ns, node_id, true);
  }

  return res;
}


void Exec::LockWasp::start () {
	ROS_INFO ("Exec::LockWasp::start: %s - %d", node_ns.c_str(), node_id);
	try {

		if (!do_before_work ()) {
		  return;
		}
		int pars = 0;
		if (!get_param("param", pars)) {
			fail ("Exec::LockWasp: parameter PARAM is missing");
			return;
		} else {
			if (pars==0){
				fail ("Exec::LockWasp: parameter PARAM is zero");
				return;
			}else{
				ROS_INFO ("Exec::LockWasp: PARAM is: %d", pars);
			}
		}
		
		ROS_INFO ("Exec::LockWasp: Execution unit: %s", tni.execution_ns.c_str());

		
		//INVIO TOPIC ROS SHERPABOX PER ESEGUIRE OPERAZIONE RICHIESTA
		//ros::Publisher cmds_pub;

		//ros::NodeHandle nh_;
		//cmds_pub = global_nh->advertise < sbox_msgs::Sbox_msg_commands > ("mavros/sbox_msg_commands", 0);

		sbox_msgs::Sbox_msg_commands cmds;
		cmds.executeId = 1;

		ROS_INFO("Publish new value!");
		cmds.parameters = pars; 
		//cmds_pub.publish( cmds );
		global_cmd_pub->publish(cmds);

		mission_succesfull=true;
		//TODO: MONITORARE LO STATUS PER VEDERE QUANDO FINISCO!

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
			fail ("LockWasp: Timeout");
			return;
		} else {
			set_succeeded_flag (node_ns, node_id, true);
			set_aborted_flag (node_ns, node_id, false);
			set_finished_flag (node_ns, node_id, true);
		}

		ROS_INFO ("Exec::LockWasp: FINISHED");

		wait_for_postwork_conditions ();
	}
	catch (boost::thread_interrupted) {
		ROS_ERROR("BOOST INTERUPTED IN LockWasp");
		set_succeeded_flag (node_ns, node_id, false);
		set_aborted_flag (node_ns, node_id, true);
		set_finished_flag (node_ns, node_id, true);
	}
}

bool Exec::LockWasp::abort () {
  bool res = false;
  ROS_INFO("Exec::LockWasp::abort");

  return res;
}
