#include "ros/ros.h"

#include <boost/thread.hpp>

#include "std_msgs/String.h"

#include <iostream>
#include <sstream>
#include <queue>

#include "lrs_msgs_tst/ConfirmReq.h"
//#include "mms_msgs/Cmd.h"

#include "lrs_srvs_exec/TSTCreateExecutor.h"

#include "executors/lockWasp.h"
#include "executors/moveRevolver.h"
#include "executors/moveLinActuator.h"
#include "executors/zeroRevolver.h"
#include "executors/moveBatClamps.h"
#include "executors/changeBattery.h"


#include "executil.h"

std::map<std::string, Executor *> execmap;
std::map<std::string, boost::thread *> threadmap;
boost::mutex mutex;
boost::mutex thread_map_lock;

ros::NodeHandle * global_nh;
ros::Publisher * global_confirm_pub;
ros::Publisher * global_cmd_pub;

int16_t global_seq = 1;

using namespace std;

bool create_executor (lrs_srvs_exec::TSTCreateExecutor::Request  &req,
		      lrs_srvs_exec::TSTCreateExecutor::Response &res ) {
  ROS_INFO("create executor start.");
  boost::mutex::scoped_lock lock(mutex);
  ROS_INFO("quadexecutor: create_executor: %s %d - %d", req.ns.c_str(), req.id, req.run_prepare);

  ostringstream os;
  os << req.ns << "-" << req.id;
  if (execmap.find (os.str()) != execmap.end()) {
    ROS_INFO ("Executor already exists, overwrites it: %s %d", req.ns.c_str(), req.id);
  }

  string type = get_node_type (req.ns, req.id);
  
  bool found = false;

  Executor * cres = check_exec(type, req.ns, req.id);
  if (cres) {
    execmap[os.str()] = cres;
    found = true;
  }

  //"lock-wasp", "move-revolver", "move-linactuator", "zero-revolver", "move-batclamps", "change-battery"
  if (type == "lock-wasp") {
	execmap[os.str()] = new Exec::LockWasp (req.ns, req.id);
	found = true;
  }
  if (type == "move-revolver") {
  	execmap[os.str()] = new Exec::MoveRevolver (req.ns, req.id);
  	found = true;
  }
  if (type == "move-linactuator") {
  	execmap[os.str()] = new Exec::MoveLinActuator (req.ns, req.id);
  	found = true;
  }
  if (type == "zero-revolver") {
  	execmap[os.str()] = new Exec::ZeroRevolver (req.ns, req.id);
  	found = true;
  }
  if (type == "move-batclamps") {
  	execmap[os.str()] = new Exec::MoveBatClamps (req.ns, req.id);
  	found = true;
  }
  if (type == "change-battery") {
   	execmap[os.str()] = new Exec::ChangeBattery (req.ns, req.id);
   	found = true;
  }
	
  if (found) {
    res.success = true;
    res.error = 0;
    if (req.run_prepare) {
      bool prep = execmap[os.str()]->prepare ();
      if (!prep) {
	res.success = false;
	res.error = 2;
      }
    }
  } else {
    ROS_ERROR ("Could not create executor of type: %s", type.c_str());
    res.success = false;
    res.error = 1;
  }

  return true;
}

int main(int argc, char **argv) {
  ROS_INFO("Executor main");
  ros::init(argc, argv, "sboxexecutor");

  ros::NodeHandle n;
  global_nh = &n;

  ros::Publisher confirm_pub = n.advertise<lrs_msgs_tst::ConfirmReq>("confirm_request", 1, true); // queue size 1 and latched
  global_confirm_pub = &confirm_pub;

  std::vector<ros::ServiceServer> services;

  std::string prefix = "tst_executor/";

  services.push_back (n.advertiseService(prefix + "destroy_executor", destroy_executor));
  services.push_back (n.advertiseService(prefix + "create_executor", create_executor));
  services.push_back (n.advertiseService(prefix + "executor_check", executor_check));
  services.push_back (n.advertiseService(prefix + "executor_continue", executor_continue));
  services.push_back (n.advertiseService(prefix + "executor_expand", executor_expand));
  services.push_back (n.advertiseService(prefix + "executor_is_delegation_expandable", executor_is_delegation_expandable));
  services.push_back (n.advertiseService(prefix + "executor_request_pause", executor_request_pause));
  services.push_back (n.advertiseService(prefix + "executor_get_constraints", executor_get_constraints));
  services.push_back (n.advertiseService(prefix + "start_executor", start_executor));
  services.push_back (n.advertiseService(prefix + "abort_executor", abort_executor));
  services.push_back (n.advertiseService(prefix + "pause_executor", pause_executor));
  services.push_back (n.advertiseService(prefix + "continue_executor", continue_executor));
  services.push_back (n.advertiseService(prefix + "enough_executor", enough_executor));

  ROS_INFO("Ready to be an executor factory");

  ros::spin();

  return 0;
}

