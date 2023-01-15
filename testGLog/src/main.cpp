#include "main.h"
#include <string.h>
#include <memory>
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>

int main(int argc, char* argv[]) {
	//programe name:"HelenXR_glog_program"
	google::InitGoogleLogging("HelenXR_glog_program");
	LOG(INFO) << "google log first info level message!";
	//...... DoSomething
	//Shutdown google's logging library.
	google::ShutdownGoogleLogging();
}
