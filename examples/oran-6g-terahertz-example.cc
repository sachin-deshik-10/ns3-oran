/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Example: 6G THz Communication with O-RAN Integration
 *
 * This example demonstrates how to use the new 6G terahertz communication
 * features with the existing O-RAN framework.
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/lte-module.h"
#include "ns3/oran-module.h"
#include "oran-6g-terahertz.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Oran6gTeraherzExample");

int main(int argc, char *argv[])
{
    // Configuration parameters
    uint32_t numUes = 10;
    uint32_t numEnbs = 3;
    double simTime = 60.0; // seconds
    bool enableIrs = true;
    bool enableUmMimo = true;

    // Parse command line
    CommandLine cmd;
    cmd.AddValue("numUes", "Number of UE devices", numUes);
    cmd.AddValue("numEnbs", "Number of eNBs", numEnbs);
    cmd.AddValue("simTime", "Simulation time", simTime);
    cmd.AddValue("enableIrs", "Enable Intelligent Reflecting Surfaces", enableIrs);
    cmd.AddValue("enableUmMimo", "Enable Ultra-massive MIMO", enableUmMimo);
    cmd.Parse(argc, argv);

    NS_LOG_INFO("Starting 6G THz O-RAN Example");
    NS_LOG_INFO("UEs: " << numUes << ", eNBs: " << numEnbs);

    // Create nodes
    NodeContainer ueNodes;
    ueNodes.Create(numUes);

    NodeContainer enbNodes;
    enbNodes.Create(numEnbs);

    // Setup mobility
    MobilityHelper mobility;

    // eNB positions (fixed)
    Ptr<ListPositionAllocator> enbPositions = CreateObject<ListPositionAllocator>();
    enbPositions->Add(Vector(0.0, 0.0, 30.0));     // eNB 1
    enbPositions->Add(Vector(500.0, 0.0, 30.0));   // eNB 2
    enbPositions->Add(Vector(250.0, 433.0, 30.0)); // eNB 3

    mobility.SetPositionAllocator(enbPositions);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);

    // UE mobility (random walk)
    mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                  "X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=500.0]"),
                                  "Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=500.0]"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Speed", StringValue("ns3::UniformRandomVariable[Min=1.0|Max=3.0]"),
                              "Bounds", StringValue("0|500|0|500"));
    mobility.Install(ueNodes);

    // Configure 6G THz Communication System
    Ptr<Oran6gTerahertz> thzSystem = CreateObject<Oran6gTerahertz>();

    // Configure THz band (200-400 GHz)
    thzSystem->ConfigureTerahertzBand(ThzBand::BAND_200_400_GHZ, 50.0); // 50 GHz bandwidth

    // Set atmospheric conditions (clear weather)
    AtmosphericConditions conditions;
    conditions.temperature = 20.0; // 20°C
    conditions.humidity = 50.0;    // 50%
    conditions.pressure = 1013.25; // Standard pressure
    conditions.rainRate = 0.0;     // No rain
    conditions.visibility = 10.0;  // 10 km visibility
    thzSystem->SetAtmosphericConditions(conditions);

    // Configure Ultra-massive MIMO if enabled
    if (enableUmMimo)
    {
        UmMimoConfiguration umMimoConfig;
        umMimoConfig.numAntennaElements = 1024; // 1024 antenna elements
        umMimoConfig.numRfChains = 64;          // 64 RF chains
        umMimoConfig.strategy = BeamformingStrategy::HYBRID_BEAMFORMING;
        umMimoConfig.antennaSpacing = 0.5; // Half wavelength spacing
        umMimoConfig.numBeams = 8;         // 8 simultaneous beams

        thzSystem->DeployUltraMassiveMimo(umMimoConfig);
        NS_LOG_INFO("Deployed Ultra-massive MIMO with " << umMimoConfig.numAntennaElements << " elements");
    }

    // Configure Intelligent Reflecting Surfaces if enabled
    if (enableIrs)
    {
        std::vector<IrsConfiguration> irsConfigs;

        // IRS 1: Between eNB 1 and common UE area
        IrsConfiguration irs1;
        irs1.numElements = 256;
        irs1.position = Vector3D(125.0, 100.0, 15.0);
        irs1.orientation = Vector3D(0.707, 0.707, 0.0); // 45-degree angle
        irs1.elementSpacing = 0.5;
        irs1.phaseShifts.resize(256, std::complex<double>(1.0, 0.0));
        irsConfigs.push_back(irs1);

        // IRS 2: Between eNB 2 and common UE area
        IrsConfiguration irs2;
        irs2.numElements = 256;
        irs2.position = Vector3D(375.0, 100.0, 15.0);
        irs2.orientation = Vector3D(-0.707, 0.707, 0.0);
        irs2.elementSpacing = 0.5;
        irs2.phaseShifts.resize(256, std::complex<double>(1.0, 0.0));
        irsConfigs.push_back(irs2);

        thzSystem->DeployIntelligentReflectingSurfaces(irsConfigs);
        NS_LOG_INFO("Deployed " << irsConfigs.size() << " Intelligent Reflecting Surfaces");
    }

    // Enable advanced features
    thzSystem->EnableOamMultiplexing(4); // 4 OAM modes

    // Setup O-RAN framework with 6G integration
    Ptr<OranHelper> oranHelper = CreateObject<OranHelper>();

    // Create Near-RT RIC with enhanced AI for 6G
    Ptr<OranNearRtRic> nearRtRic = CreateObject<OranNearRtRic>();

    // Configure advanced AI logic module for 6G
    oranHelper->SetLogicModule("ns3::OranLmReinforcementLearning",
                               "Algorithm", StringValue("PPO"),
                               "LearningRate", DoubleValue(0.0001),
                               "Enable6gFeatures", BooleanValue(true),
                               "ThzOptimization", BooleanValue(true));

    // Add THz-specific reporters
    oranHelper->AddReporter("ns3::OranReporter6gThz",
                            "Trigger", StringValue("ns3::OranReportTriggerPeriodic"),
                            "Interval", StringValue("0.1s"));

    oranHelper->AddReporter("ns3::OranReporterBeamforming",
                            "Trigger", StringValue("ns3::OranReportTriggerBeamChange"));

    oranHelper->AddReporter("ns3::OranReporterIrsOptimization",
                            "Trigger", StringValue("ns3::OranReportTriggerPeriodic"),
                            "Interval", StringValue("1.0s"));

    // Setup LTE base network (will be enhanced with 6G)
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<EpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    // Install LTE devices
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    // Attach UEs to eNBs
    lteHelper->AttachToClosestEnb(ueLteDevs, enbLteDevs);

    // Deploy O-RAN terminators
    NodeContainer allNodes;
    allNodes.Add(ueNodes);
    allNodes.Add(enbNodes);

    OranE2NodeTerminatorContainer terminators =
        oranHelper->DeployTerminators(nearRtRic, allNodes);

    // Activate O-RAN with 6G enhancements
    oranHelper->ActivateAndStartNearRtRic(nearRtRic);
    oranHelper->ActivateE2NodeTerminators(terminators);

    // Schedule 6G optimizations
    Simulator::Schedule(Seconds(5.0), [thzSystem]()
                        {
        thzSystem->OptimizeNetworkThroughput();
        NS_LOG_INFO("6G THz network optimization completed"); });

    // Schedule periodic IRS optimization
    if (enableIrs)
    {
        Simulator::Schedule(Seconds(10.0), [thzSystem]()
                            {
            thzSystem->OptimizeNetworkThroughput();
            NS_LOG_INFO("IRS phase optimization updated"); });
    }

    // Performance monitoring callback
    Simulator::Schedule(Seconds(1.0), []()
                        {
        static uint32_t callCount = 0;
        callCount++;
        
        // Log 6G performance metrics every 10 seconds
        if (callCount % 10 == 0)
        {
            NS_LOG_INFO("=== 6G Performance Report ===");
            NS_LOG_INFO("Time: " << Simulator::Now().GetSeconds() << "s");
            // Add performance metrics logging here
        }
        
        // Reschedule
        Simulator::Schedule(Seconds(1.0), MakeCallback([]() {
            // This callback will be called recursively
        })); });

    // Setup applications
    uint16_t port = 9;
    ApplicationContainer apps;

    // Server applications on eNBs
    for (uint32_t i = 0; i < enbNodes.GetN(); ++i)
    {
        UdpEchoServerHelper echoServer(port + i);
        ApplicationContainer serverApp = echoServer.Install(enbNodes.Get(i));
        serverApp.Start(Seconds(1.0));
        serverApp.Stop(Seconds(simTime - 1.0));
        apps.Add(serverApp);
    }

    // Client applications on UEs
    for (uint32_t i = 0; i < ueNodes.GetN(); ++i)
    {
        uint32_t serverIndex = i % enbNodes.GetN();

        UdpEchoClientHelper echoClient(interfaces.GetAddress(serverIndex), port + serverIndex);
        echoClient.SetAttribute("MaxPackets", UintegerValue(1000));
        echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(100)));
        echoClient.SetAttribute("PacketSize", UintegerValue(1024));

        ApplicationContainer clientApp = echoClient.Install(ueNodes.Get(i));
        clientApp.Start(Seconds(2.0 + i * 0.1));
        clientApp.Stop(Seconds(simTime - 1.0));
        apps.Add(clientApp);
    }

    // Enable tracing
    AsciiTraceHelper ascii;
    lteHelper->EnableTraces();

    // Custom trace for 6G metrics
    std::ofstream thzTraceFile("6g-thz-metrics.txt");
    thzTraceFile << "Time,NodeId,DataRate,Latency,BeamGain,IrsGain\n";

    NS_LOG_INFO("Starting simulation for " << simTime << " seconds");

    // Run simulation
    Simulator::Stop(Seconds(simTime));
    Simulator::Run();

    // Cleanup
    thzTraceFile.close();
    Simulator::Destroy();

    NS_LOG_INFO("6G THz O-RAN simulation completed");
    NS_LOG_INFO("Results saved to 6g-thz-metrics.txt");

    return 0;
}
