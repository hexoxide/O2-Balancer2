#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestEpn

// Boost headers
#include <boost/test/unit_test.hpp>
#include <boost/program_options.hpp>

// FairMQ headers
#include <fairmq/DeviceRunner.h>

// STL headers
#include <memory>
#include <string>
#include <thread>
#include <future>

// O2Integration headers
#include "epn.h"
#include "icn.h"

// O2 headers
#include "data/o2data.h"
#include "data/o2channel.h"

using namespace std;

using namespace fair::mq;
using namespace fair::mq::hooks;

using namespace O2::data;
using namespace O2::exception;

BOOST_AUTO_TEST_SUITE(Test_Epn_Icn)

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
    device.WaitForEndOfState("RUN");
    // Wait for 2 seconds so the EPN can send its initial configuration packet
    //this_thread::sleep_for(chrono::seconds(10));
	//device.ChangeState("STOP");

    device.ChangeState("RESET_TASK");
    device.WaitForEndOfState("RESET_TASK");
    device.ChangeState("RESET_DEVICE");
    device.WaitForEndOfState("RESET_DEVICE");

    device.ChangeState("END");
}

/**
 * These options will extend the default arguments that can be passed via the commandline
 * @warning Ensure this function is an exact copy of the definition in main.cxx from EPN
 */
void addCustomOptionsEPN(boost::program_options::options_description& options)
{
	options.add_options()
		(EventProcessingNode::pNumberOfFLP.c_str(), 
			boost::program_options::value<uint64_t>()->required(), 
			"Number of FLP's the EPN will expect to be in the network.")
		(EventProcessingNode::pPrimaryNetworkInterface.c_str(), 
			boost::program_options::value<std::string>()->required(), 
			"The primary network interface to use while performing load balancing experiments");
}

/**
 * These options will extend the default arguments that can be passed via the commandline
 * @warning Ensure this function is an exact copy of the definition in main.cxx from ICN
 */
void addCustomOptionsICN(boost::program_options::options_description& options)
{
    options.add_options()
        ("iterations", boost::program_options::value<uint64_t>()->required(), "Number of iterations for the duration of the experiment");
}

/**
 * Creates an instance of EventProcessingNode and configures it with the specified id
 * @param EPNId the ID that will be used to confgure the EPN
 * @return true if the device ran succesfully, false otherwise
 */
bool TestEPN(std::string EPNId) {
    // Create FairMQ DeviceRunner to enable extending commandline options
    char * const test[] = {nullptr};
    DeviceRunner EPNrunner{0, test, false};

    boost::program_options::options_description customOptionsEPN("Custom EPN options");
    addCustomOptionsEPN(customOptionsEPN);
    EPNrunner.fConfig.AddToCmdLineOptions(customOptionsEPN);

    /* The configuration for the EPN as found in the readme */
    vector<string> EPNArguments = {"dummy", 
        "--id", EPNId, 
        "--control", "static", 
        "--severity", "trace",
        "--verbosity", "high",
        "--primary-interface", "lo",
        "--num-flp", "1",
    };

    /* Set the transport method to zeromq*/
    EPNrunner.fConfig.SetValue<string>("transport", "zeromq");

    /* Parse the configuration*/
    if (EPNrunner.fConfig.ParseAll(EPNArguments, true))
    {
        throw O2Exception("Parsing EPN configuration failed", __FILE__, __LINE__);
    }

    /* Create the EventProcessingNode */
    EventProcessingNode EPNdevice;

    /* Pass the config to the device */
    EPNdevice.SetConfig(EPNrunner.fConfig);

    /* Reduce the startup delay for integration test */
    EPNdevice.initialDelay = 1;

    /* Create the first channel as found in the readme */
    FairMQChannel channelOne;
    channelOne.UpdateType("pull");
    channelOne.UpdateMethod("connect");
    channelOne.UpdateAddress("tcp://localhost:5555");
    channelOne.UpdateRateLogging(true);
    /* Add the channel with an appropriate name */
    EPNdevice.AddChannel(EPNId, channelOne);

    /* Create the second channel as found in the readme */
    FairMQChannel channelTwo;
    channelTwo.UpdateType("push");
    channelTwo.UpdateMethod("connect");
    channelTwo.UpdateAddress("tcp://localhost:5000");
    channelTwo.UpdateRateLogging(true);
    /* Add the channel with an appropriate name */
    EPNdevice.AddChannel("feedback", channelTwo);

    /* Perform the control sequence from another thread*/
    thread t(control, ref(EPNdevice));

    // Start the EventProcessingNode state machine
    EPNdevice.RunStateMachine();

    if (t.joinable())
    {
        t.join();
    }

    return true;
}

bool TestICN(uint16_t numberOfExpectedChannels) {
    // Create FairMQ DeviceRunner to enable extending commandline options
    char * const test[] = {nullptr};
    DeviceRunner ICNrunner{0, test, false};

    boost::program_options::options_description customOptionsICN("Custom ICN options");
    addCustomOptionsICN(customOptionsICN);    
    ICNrunner.fConfig.AddToCmdLineOptions(customOptionsICN);

    /* The configuration for the ICN as found in the readme */
    vector<string> ICNArguments = {"dummy", 
        "--id", "1",
        "--control", "static", 
        "--severity", "trace",
        "--verbosity", "high",
        "--rate", "200",
        "--iterations", "0"
    };

    /* Set the transport method to zeromq*/
    ICNrunner.fConfig.SetValue<string>("transport", "zeromq");

    /* Parse the configuration*/
    if (ICNrunner.fConfig.ParseAll(ICNArguments, true))
    {
        throw O2Exception("Parsing ICN configuration failed", __FILE__, __LINE__);
    }

    /* Create the InformationControlNode */
    InformationControlNode ICNdevice;

    /* Pass the config to the device */
    ICNdevice.SetConfig(ICNrunner.fConfig);

    /* Create the first channel as found in the readme */
    FairMQChannel channelOne;
    channelOne.UpdateType("pub");
    channelOne.UpdateMethod("bind");
    channelOne.UpdateAddress("tcp://*:5005");
    channelOne.UpdateRateLogging(true);
    /* Add the channel with an appropriate name */
    ICNdevice.AddChannel("broadcast", channelOne);

    /* Create the second channel as found in the readme */
    FairMQChannel channelTwo;
    channelTwo.UpdateType("pull");
    channelTwo.UpdateMethod("bind");
    channelTwo.UpdateAddress("tcp://*:5000");
    channelTwo.UpdateRateLogging(true);
    /* Add the channel with an appropriate name */
    ICNdevice.AddChannel("feedback", channelTwo);

    /* Reduce the startup delay for integration test */
    ICNdevice.initialDelay = 1;

    thread t(control, ref(ICNdevice));

    // Start the InformationProcessingNode state machine
    ICNdevice.RunStateMachine();

    // Wait for the control sequence to finish
    if (t.joinable())
    {
        t.join();
    }

    // Number of received channels in ICN device should match expected result based on the number of created EPN's
    return (ICNdevice.getNumberOfChannels() == numberOfExpectedChannels) ? true : false;
}

BOOST_AUTO_TEST_CASE(TestEpnIcnSingleChannel) {
    std::future<bool> fut1 = std::async(std::launch::async, &TestEPN, "1");
    std::future<bool> fut2 = std::async(std::launch::async, &TestICN, 1);

    BOOST_CHECK_EQUAL(fut1.get(), true);
    BOOST_CHECK_EQUAL(fut2.get(), true);
}

BOOST_AUTO_TEST_CASE(TestEpnIcnMultiChannel) {
    std::future<bool> fut1 = std::async(std::launch::async, &TestEPN, "1");
    std::future<bool> fut2 = std::async(std::launch::async, &TestEPN, "2");
    std::future<bool> fut3 = std::async(std::launch::async, &TestEPN, "3");
    std::future<bool> fut4 = std::async(std::launch::async, &TestEPN, "4");
    std::future<bool> fut5 = std::async(std::launch::async, &TestICN, 4);

    BOOST_CHECK_EQUAL(fut1.get(), true);
    BOOST_CHECK_EQUAL(fut2.get(), true);
    BOOST_CHECK_EQUAL(fut3.get(), true);
    BOOST_CHECK_EQUAL(fut4.get(), true);
    BOOST_CHECK_EQUAL(fut5.get(), true);
}

BOOST_AUTO_TEST_CASE(TestEpnIcnMismatchChannel) {
    std::future<bool> fut1 = std::async(std::launch::async, &TestEPN, "1");
    std::future<bool> fut2 = std::async(std::launch::async, &TestICN, 2);

    BOOST_CHECK_EQUAL(fut1.get(), true);
    BOOST_CHECK_EQUAL(fut2.get(), false);
}

BOOST_AUTO_TEST_SUITE_END()
