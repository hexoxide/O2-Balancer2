#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestIcn

// Boost headers
#include <boost/test/unit_test.hpp>
#include <boost/program_options.hpp>

// FairMQ headers
#include <fairmq/DeviceRunner.h>

// STL headers
#include <memory>
#include <string>
#include <thread>

// O2Integration headers
#include "icn.h"

// O2 headers
#include "data/o2data.h"
#include "data/o2channel.h"

using namespace std;

using namespace fair::mq;
using namespace fair::mq::hooks;

using namespace O2::data;
using namespace O2::exception;

BOOST_AUTO_TEST_SUITE(Test_Icn)

/**
 * Definition of state transistion that can be passed to a FairMQDevice to follow
 */
void control(FairMQDevice& device)
{
    device.ChangeState("INIT_DEVICE");
    device.WaitForEndOfState("INIT_DEVICE");
    device.ChangeState("INIT_TASK");
    device.WaitForEndOfState("INIT_TASK");

    device.ChangeState("RUN");
    // Wait for 3 seconds so the ICN can send its initial configuration packet
    this_thread::sleep_for(chrono::seconds(3));
	device.ChangeState("STOP");

    device.ChangeState("RESET_TASK");
    device.WaitForEndOfState("RESET_TASK");
    device.ChangeState("RESET_DEVICE");
    device.WaitForEndOfState("RESET_DEVICE");

    device.ChangeState("END");
}

/**
 * These options will extend the default arguments that can be passed via the commandline
 * @warning Ensure this function is an exact copy of the definition in main.cxx from ICN
 */
void addCustomOptions(boost::program_options::options_description& options)
{
    options.add_options()
        ("iterations", boost::program_options::value<uint64_t>()->required(), "Number of iterations for the duration of the experiment");
}

BOOST_AUTO_TEST_CASE(TestIcn) {

	// Create FairMQ DeviceRunner to enable extending commandline options
	char * const test[] = {NULL};
	DeviceRunner runner{0, test, false};

	// extend commandline options
	boost::program_options::options_description customOptions("Custom options");
    addCustomOptions(customOptions);
	runner.fConfig.AddToCmdLineOptions(customOptions);

	/* The configuration as found in the readme */
    vector<string> emptyArgs = {"dummy", 
    	"--id", "1", 
        "--rate", "200",
    	"--control", "static", 
    	"--severity", "trace",
    	"--verbosity", "high",
    	"--iterations", "200"
    };

    /* Set the transport method to zeromq*/
    runner.fConfig.SetValue<string>("transport", "zeromq");

    /* Parse the configuration*/
    if (runner.fConfig.ParseAll(emptyArgs, true))
    {
    	throw O2Exception("Parsing configuration failed", __FILE__, __LINE__);
    }

    /* Create the FirstLineProcessor */
	InformationControlNode device;

	/* Pass the config to the device */
	device.SetConfig(runner.fConfig);

    /* Reduce the startup delay for integration test */
    device.initialDelay = 1;

	/* Create the first channel as found in the readme */
	FairMQChannel channelOne;
    channelOne.UpdateType("sub");
    channelOne.UpdateMethod("connect");
    channelOne.UpdateAddress("tcp://localhost:5005");
    channelOne.UpdateRateLogging(true);
    /* Add the channel with an appropriate name */
	device.AddChannel("broadcast", channelOne);

    /* Create the second channel as found in the readme */
    FairMQChannel channelTwo;
    channelTwo.UpdateType("push");
    channelTwo.UpdateMethod("connect");
    channelTwo.UpdateAddress("tcp://localhost:5000");
    channelTwo.UpdateRateLogging(true);
    /* Add the channel with an appropriate name */
    device.AddChannel("feedback", channelTwo);

	/* Perform the control sequence from another thread*/
	thread t(control, ref(device));

	// Start the EventProcessingNode state machine
	device.RunStateMachine();

	// Wait for the control sequence to finish
    if (t.joinable())
    {
        t.join();
	}
}

BOOST_AUTO_TEST_SUITE_END()