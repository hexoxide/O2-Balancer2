#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestFlp

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
#include "flp.h"

// O2 headers
#include "data/o2data.h"
#include "data/o2channel.h"

using namespace std;

using namespace fair::mq;
using namespace fair::mq::hooks;

using namespace O2::data;
using namespace O2::exception;

BOOST_AUTO_TEST_SUITE(Test_Flp)

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
    // Wait for 2 seconds so the FLP can send its initial configuration packet
    this_thread::sleep_for(chrono::seconds(2));
	device.ChangeState("STOP");

    device.ChangeState("RESET_TASK");
    device.WaitForEndOfState("RESET_TASK");
    device.ChangeState("RESET_DEVICE");
    device.WaitForEndOfState("RESET_DEVICE");

    device.ChangeState("END");
}

/**
 * These options will extend the default arguments that can be passed via the commandline
 * @warning Ensure this function is an exact copy of the definition in main.cxx from FLP
 */
void addCustomOptions(boost::program_options::options_description& options)
{
    options.add_options()
    	("bytes-per-message", boost::program_options::value<uint64_t>()->default_value(20480), "Number of bytes to transmit per message");
}

BOOST_AUTO_TEST_CASE(TestFlp) {

	// Create FairMQ DeviceRunner to enable extending commandline options
	char * const test[] = {nullptr};
	DeviceRunner runner{0, test, false};

	// extend commandline options
	boost::program_options::options_description customOptions("Custom options");
    addCustomOptions(customOptions);
	runner.fConfig.AddToCmdLineOptions(customOptions);

	/* The configuration as found in the readme */
    vector<string> emptyArgs = {"dummy", 
    	"--id", "1", 
    	"--control", "static", 
    	"--severity", "trace",
    	"--verbosity", "high",
    	"--bytes-per-message", "2097152"
    };

    /* Set the transport method to zeromq*/
    runner.fConfig.SetValue<string>("transport", "zeromq");

    /* Parse the configuration*/
    if (runner.fConfig.ParseAll(emptyArgs, true))
    {
    	throw O2Exception("Parsing configuration failed", __FILE__, __LINE__);
    }

    /* Create the FirstLineProcessor */
	FirstLineProcessor device;

	/* Pass the config to the device */
	device.SetConfig(runner.fConfig);

	/* Create the channel as found in the readme */
	FairMQChannel channelOne;
    channelOne.UpdateType("sub");
    channelOne.UpdateMethod("connect");
    channelOne.UpdateAddress("tcp://localhost:5005");
    channelOne.UpdateRateLogging(true);
    /* Add the channel with an appropriate name */
	device.AddChannel("broadcast", channelOne);

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
