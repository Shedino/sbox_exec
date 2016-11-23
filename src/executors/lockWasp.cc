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

		mavlink_radio_t msg_cmd = {0,0,0,0,0,0,0}; //OK MINIMOSD
		msg_cmd.noise = 1;
		msg_cmd.rssi = pars;

		mavlink_message_t toSend;
		uint8_t txbuf[1024];
		int psize = mavlink_msg_radio_encode(1,1,&toSend,&msg_cmd);
		int sendsize = mavlink_msg_to_send_buffer(txbuf,&toSend);
		for(int i = 0; i<sendsize;i++){
			//SerialOSD.write(txbuf[i]);
		}
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
