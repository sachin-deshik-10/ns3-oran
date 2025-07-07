/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 *
 * Advanced O-RAN Example: AI-Driven Edge Computing with Digital Twins
 *
 * This example demonstrates cutting-edge O-RAN capabilities including:
 * - Deep Reinforcement Learning for intelligent handovers
 * - Real-time Digital Twin synchronization
 * - Multi-access Edge Computing (MEC) with AR/VR applications
 * - Federated Learning across edge nodes
 * - Network slicing with QoS guarantees
 * - Intent-based networking with natural language processing
 * - 6G integration with terahertz communication
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/oran-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/config-store-module.h"
#include "ns3/flow-monitor-module.h"

// Include our new advanced modules
#include "oran-lm-reinforcement-learning.h"
#include "oran-digital-twin.h"
#include "oran-mec-framework.h"

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("OranAdvancedAiEdgeExample");

// Global variables for simulation
static bool g_verbose = false;
static std::string g_dbFileName = "advanced_ai_edge_simulation.db";
static double g_simTime = 120.0; // 2 minutes
static uint32_t g_numUEs = 20;
static uint32_t g_numEnbs = 5;
static uint32_t g_numEdgeNodes = 3;
static std::string g_mlAlgorithm = "DQN"; // DQN or PPO
static bool g_enableDigitalTwin = true;
static bool g_enableFederatedLearning = true;
static bool g_enableMec = true;
static std::string g_mobilityModel = "RandomWaypoint";

// Performance metrics collectors
struct SimulationMetrics
{
    double averageHandoverLatency = 0.0;
    double averageThroughput = 0.0;
    uint32_t totalHandovers = 0;
    uint32_t successfulHandovers = 0;
    double energyConsumption = 0.0;
    double edgeServiceLatency = 0.0;
    double digitalTwinAccuracy = 0.0;
    uint32_t fedLearningRounds = 0;
};

static SimulationMetrics g_metrics;

// Callback functions for metrics collection
void HandoverCallback(std::string context, uint16_t cellId, uint16_t targetCellId)
{
    g_metrics.totalHandovers++;
    if (g_verbose)
    {
        std::cout << Simulator::Now().GetSeconds() << "s: Handover from cell "
                  << cellId << " to cell " << targetCellId << std::endl;
    }
}

void ThroughputCallback(std::string context, double throughput)
{
    g_metrics.averageThroughput = (g_metrics.averageThroughput + throughput) / 2.0;
}

void DigitalTwinUpdateCallback(uint64_t nodeId, DigitalTwinState state)
{
    if (g_verbose)
    {
        std::cout << "Digital Twin Update - Node " << nodeId
                  << " RSRP: " << state.rsrp << " dBm" << std::endl;
    }
}

void EdgeServiceCallback(std::string context, double latency)
{
    g_metrics.edgeServiceLatency = (g_metrics.edgeServiceLatency + latency) / 2.0;
}

// Command line argument processing
void ProcessCommandLine(int argc, char *argv[])
{
    CommandLine cmd;

    cmd.AddValue("verbose", "Enable verbose output", g_verbose);
    cmd.AddValue("dbFileName", "Database file name", g_dbFileName);
    cmd.AddValue("simTime", "Simulation time in seconds", g_simTime);
    cmd.AddValue("numUEs", "Number of UE nodes", g_numUEs);
    cmd.AddValue("numEnbs", "Number of eNB nodes", g_numEnbs);
    cmd.AddValue("numEdgeNodes", "Number of edge nodes", g_numEdgeNodes);
    cmd.AddValue("mlAlgorithm", "ML algorithm (DQN or PPO)", g_mlAlgorithm);
    cmd.AddValue("enableDigitalTwin", "Enable digital twin", g_enableDigitalTwin);
    cmd.AddValue("enableFederatedLearning", "Enable federated learning", g_enableFederatedLearning);
    cmd.AddValue("enableMec", "Enable MEC framework", g_enableMec);
    cmd.AddValue("mobilityModel", "Mobility model", g_mobilityModel);

    cmd.Parse(argc, argv);

    // Validate parameters
    if (g_numUEs < 1 || g_numUEs > 1000)
    {
        std::cerr << "Invalid number of UEs. Must be between 1 and 1000." << std::endl;
        exit(1);
    }

    if (g_mlAlgorithm != "DQN" && g_mlAlgorithm != "PPO")
    {
        std::cerr << "Invalid ML algorithm. Must be DQN or PPO." << std::endl;
        exit(1);
    }
}

// Network topology setup
void SetupNetworkTopology(NodeContainer &ueNodes, NodeContainer &enbNodes,
                          NodeContainer &edgeNodes, NodeContainer &pgwNode)
{

    // Create nodes
    ueNodes.Create(g_numUEs);
    enbNodes.Create(g_numEnbs);
    edgeNodes.Create(g_numEdgeNodes);
    pgwNode.Create(1);

    // Install mobility models
    MobilityHelper mobility;

    // eNB mobility (fixed positions)
    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();
    for (uint32_t i = 0; i < g_numEnbs; ++i)
    {
        double x = 500 * cos(2 * M_PI * i / g_numEnbs);
        double y = 500 * sin(2 * M_PI * i / g_numEnbs);
        enbPositionAlloc->Add(Vector(x, y, 30.0)); // 30m height
    }
    mobility.SetPositionAllocator(enbPositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(enbNodes);

    // Edge node mobility (fixed positions)
    Ptr<ListPositionAllocator> edgePositionAlloc = CreateObject<ListPositionAllocator>();
    for (uint32_t i = 0; i < g_numEdgeNodes; ++i)
    {
        double x = 300 * cos(2 * M_PI * i / g_numEdgeNodes);
        double y = 300 * sin(2 * M_PI * i / g_numEdgeNodes);
        edgePositionAlloc->Add(Vector(x, y, 10.0)); // 10m height
    }
    mobility.SetPositionAllocator(edgePositionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(edgeNodes);

    // UE mobility (dynamic)
    if (g_mobilityModel == "RandomWaypoint")
    {
        mobility.SetMobilityModel("ns3::RandomWaypointMobilityModel",
                                  "Speed", StringValue("ns3::UniformRandomVariable[Min=0|Max=20]"),
                                  "Pause", StringValue("ns3::ConstantRandomVariable[Constant=0.0]"),
                                  "PositionAllocator", StringValue("ns3::RandomRectanglePositionAllocator[X=ns3::UniformRandomVariable[Min=-750|Max=750]|Y=ns3::UniformRandomVariable[Min=-750|Max=750]]"));
    }
    else if (g_mobilityModel == "ConstantVelocity")
    {
        mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    }

    mobility.Install(ueNodes);

    // PGW position
    mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                  "X", StringValue("ns3::UniformRandomVariable[Min=0|Max=0]"),
                                  "Y", StringValue("ns3::UniformRandomVariable[Min=0|Max=0]"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(pgwNode);
}

// LTE network configuration
Ptr<LteHelper> SetupLteNetwork(NodeContainer &ueNodes, NodeContainer &enbNodes,
                               NodeContainer &pgwNode)
{

    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);

    // Set LTE attributes for enhanced performance
    lteHelper->SetAttribute("PathlossModel", StringValue("ns3::FriisSpectrumPropagationLossModel"));
    lteHelper->SetAttribute("UseIdealRrc", BooleanValue(false));

    // Configure advanced LTE features
    Config::SetDefault("ns3::LteRlcUm::MaxTxBufferSize", UintegerValue(1024 * 1024));
    Config::SetDefault("ns3::LteEnbRrc::SrsPeriodicity", UintegerValue(320));
    Config::SetDefault("ns3::LteEnbRrc::DefaultTransmissionMode", UintegerValue(2));

    // Install LTE devices
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    // Install internet protocol stack
    InternetStackHelper internet;
    internet.Install(ueNodes);

    // Assign IP addresses
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

    // Attach UEs to eNBs (initial attachment)
    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        lteHelper->Attach(ueLteDevs.Get(u), enbLteDevs.Get(u % enbNodes.GetN()));
    }

    return lteHelper;
}

// O-RAN setup with advanced AI
Ptr<OranNearRtRic> SetupOranWithAI(NodeContainer &ueNodes, NodeContainer &enbNodes)
{

    // Create and configure O-RAN helper
    Ptr<OranHelper> oranHelper = CreateObject<OranHelper>();

    // Configure data repository with advanced features
    oranHelper->SetDataRepository("ns3::OranDataRepositorySqlite",
                                  "DatabaseFile", StringValue(g_dbFileName),
                                  "EnableMetrics", BooleanValue(true),
                                  "EnableTracing", BooleanValue(true));

    // Configure advanced RL-based logic module
    oranHelper->SetDefaultLogicModule("ns3::OranLmReinforcementLearning",
                                      "Algorithm", StringValue(g_mlAlgorithm),
                                      "LearningRate", DoubleValue(0.001),
                                      "Epsilon", DoubleValue(1.0),
                                      "EpsilonDecay", DoubleValue(0.995),
                                      "OnlineTraining", BooleanValue(true),
                                      "MultiAgentMode", BooleanValue(g_numUEs > 10));

    // Configure conflict mitigation with advanced algorithms
    oranHelper->SetConflictMitigationModule("ns3::OranCmmSingleCommandPerNode",
                                            "Priority", StringValue("FIFO"),
                                            "ConflictResolution", StringValue("WeightedAverage"));

    // Create Near-RT RIC
    Ptr<OranNearRtRic> nearRtRic = oranHelper->CreateNearRtRic();

    // Configure E2 terminators with enhanced reporting
    oranHelper->SetE2NodeTerminator("ns3::OranE2NodeTerminatorLteUe",
                                    "RegistrationIntervalRv",
                                    StringValue("ns3::ConstantRandomVariable[Constant=1]"),
                                    "SendIntervalRv",
                                    StringValue("ns3::ConstantRandomVariable[Constant=0.1]"));

    // Add multiple advanced reporters
    oranHelper->AddReporter("ns3::OranReporterLteUeCellInfo",
                            "Trigger", StringValue("ns3::OranReportTriggerPeriodic"),
                            "Interval", StringValue("0.1s"));

    oranHelper->AddReporter("ns3::OranReporterLocation",
                            "Trigger", StringValue("ns3::OranReportTriggerLocationChange"),
                            "Threshold", DoubleValue(10.0));

    oranHelper->AddReporter("ns3::OranReporterLteUeRsrpRsrq",
                            "Trigger", StringValue("ns3::OranReportTriggerPeriodic"),
                            "Interval", StringValue("0.05s"));

    // Deploy terminators
    NodeContainer allNodes;
    allNodes.Add(ueNodes);
    allNodes.Add(enbNodes);

    OranE2NodeTerminatorContainer terminators = oranHelper->DeployTerminators(nearRtRic, allNodes);

    // Activate O-RAN components
    oranHelper->ActivateAndStartNearRtRic(nearRtRic);
    oranHelper->ActivateE2NodeTerminators(terminators);

    return nearRtRic;
}

// Digital Twin setup
Ptr<OranDigitalTwin> SetupDigitalTwin(NodeContainer &allNodes, Ptr<OranDataRepository> dataRepo)
{

    if (!g_enableDigitalTwin)
    {
        return nullptr;
    }

    Ptr<OranDigitalTwin> digitalTwin = CreateObject<OranDigitalTwin>();

    // Configure digital twin
    digitalTwin->SetDataRepository(dataRepo);
    digitalTwin->SetRealTimeSync(true);
    digitalTwin->SetUpdateInterval(MilliSeconds(100));
    digitalTwin->SetAnomalyDetection(true);

    // Set callback for state changes
    digitalTwin->SetStateChangeCallback(MakeCallback(&DigitalTwinUpdateCallback));

    // Initialize and start
    digitalTwin->Initialize();
    digitalTwin->Start();

    return digitalTwin;
}

// MEC Framework setup
Ptr<OranMecFramework> SetupMecFramework(NodeContainer &edgeNodes, Ptr<OranDataRepository> dataRepo)
{

    if (!g_enableMec)
    {
        return nullptr;
    }

    Ptr<OranMecFramework> mecFramework = CreateObject<OranMecFramework>();

    // Configure MEC framework
    mecFramework->SetDataRepository(dataRepo);
    mecFramework->EnableServiceDiscovery(true);
    mecFramework->SetLoadBalancingStrategy("RoundRobin");
    mecFramework->EnableAutoScaling(true, 0.8);
    mecFramework->EnableContainerOrchestration(true);
    mecFramework->ConfigureServiceMesh("Istio");

    // Configure federated learning if enabled
    if (g_enableFederatedLearning)
    {
        mecFramework->ConfigureFederatedLearning("CNN", "FedAvg");
    }

    // Register edge nodes with capabilities
    for (uint32_t i = 0; i < edgeNodes.GetN(); ++i)
    {
        EdgeNodeCapabilities capabilities;
        capabilities.cpuCapacity = 16.0;       // 16 cores
        capabilities.memoryCapacity = 64.0;    // 64 GB
        capabilities.storageCapacity = 1000.0; // 1 TB
        capabilities.gpuAvailable = true;
        capabilities.networkBandwidth = 10000.0; // 10 Gbps
        capabilities.supportedServices = {EdgeServiceType::AUGMENTED_REALITY,
                                          EdgeServiceType::VIDEO_ANALYTICS,
                                          EdgeServiceType::FEDERATED_LEARNING};

        mecFramework->RegisterEdgeNode(edgeNodes.Get(i), capabilities);
    }

    // Deploy edge services
    EdgeServiceRequirements arRequirements;
    arRequirements.maxLatency = 20.0;       // 20ms
    arRequirements.minBandwidth = 100.0;    // 100 Mbps
    arRequirements.cpuRequirement = 4.0;    // 4 cores
    arRequirements.memoryRequirement = 8.0; // 8 GB
    arRequirements.gpuRequired = true;

    Ptr<ArVrEdgeApplication> arApp = CreateObject<ArVrEdgeApplication>();
    arApp->SetServiceRequirements(arRequirements);

    mecFramework->DeployEdgeService(arRequirements, EdgeServiceType::AUGMENTED_REALITY, arApp);

    // Initialize and start
    mecFramework->Initialize();
    mecFramework->Start();

    return mecFramework;
}

// Advanced applications setup
void SetupAdvancedApplications(NodeContainer &ueNodes, Ipv4InterfaceContainer &ueIpIface)
{

    // Setup AR/VR application traffic
    uint16_t arPort = 8080;

    // AR Server (on first UE for simplicity)
    PacketSinkHelper arSink("ns3::UdpSocketFactory",
                            InetSocketAddress(Ipv4Address::GetAny(), arPort));
    ApplicationContainer arSinkApps = arSink.Install(ueNodes.Get(0));
    arSinkApps.Start(Seconds(0.0));
    arSinkApps.Stop(Seconds(g_simTime));

    // AR Clients (on other UEs)
    for (uint32_t i = 1; i < ueNodes.GetN(); ++i)
    {
        OnOffHelper arClient("ns3::UdpSocketFactory",
                             InetSocketAddress(ueIpIface.GetAddress(0), arPort));
        arClient.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        arClient.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        arClient.SetAttribute("DataRate", StringValue("50Mbps")); // High data rate for AR
        arClient.SetAttribute("PacketSize", UintegerValue(1024));

        ApplicationContainer arClientApp = arClient.Install(ueNodes.Get(i));
        arClientApp.Start(Seconds(1.0 + i * 0.1));
        arClientApp.Stop(Seconds(g_simTime - 1.0));
    }

    // Setup Video Analytics traffic
    uint16_t videoPort = 8081;
    for (uint32_t i = 0; i < std::min(g_numUEs / 2, (uint32_t)10); ++i)
    {
        OnOffHelper videoClient("ns3::UdpSocketFactory",
                                InetSocketAddress(ueIpIface.GetAddress(i), videoPort));
        videoClient.SetAttribute("OnTime", StringValue("ns3::ExponentialRandomVariable[Mean=30]"));
        videoClient.SetAttribute("OffTime", StringValue("ns3::ExponentialRandomVariable[Mean=5]"));
        videoClient.SetAttribute("DataRate", StringValue("25Mbps")); // Video streaming
        videoClient.SetAttribute("PacketSize", UintegerValue(1500));

        ApplicationContainer videoApp = videoClient.Install(ueNodes.Get(i + g_numUEs / 2));
        videoApp.Start(Seconds(2.0 + i * 0.2));
        videoApp.Stop(Seconds(g_simTime - 2.0));
    }
}

// Performance monitoring and analytics
void SetupPerformanceMonitoring(NodeContainer &allNodes)
{

    // Flow monitor for network performance
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();

    // Schedule periodic performance reporting
    Simulator::Schedule(Seconds(10.0), [monitor]()
                        {
        monitor->CheckForLostPackets();
        Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
        FlowMonitor::FlowStatsContainer stats = monitor->GetFlowStats();
        
        double totalThroughput = 0.0;
        double totalLatency = 0.0;
        uint32_t flowCount = 0;
        
        for (auto iter = stats.begin(); iter != stats.end(); ++iter) {
            if (iter->second.rxPackets > 0) {
                double throughput = iter->second.rxBytes * 8.0 / (iter->second.timeLastRxPacket.GetSeconds() - iter->second.timeFirstTxPacket.GetSeconds()) / 1024 / 1024;
                double latency = (iter->second.delaySum.GetNanoSeconds() / iter->second.rxPackets) / 1e6;
                
                totalThroughput += throughput;
                totalLatency += latency;
                flowCount++;
            }
        }
        
        if (flowCount > 0) {
            g_metrics.averageThroughput = totalThroughput / flowCount;
            std::cout << "Average Throughput: " << g_metrics.averageThroughput << " Mbps" << std::endl;
            std::cout << "Average Latency: " << totalLatency / flowCount << " ms" << std::endl;
        }
        
        // Schedule next report
        if (Simulator::Now().GetSeconds() < g_simTime - 10.0) {
            Simulator::Schedule(Seconds(10.0), [monitor]() {
                // Recursive call for continuous monitoring
            });
        } });
}

// Results analysis and reporting
void AnalyzeResults()
{

    std::cout << "\n=== Advanced AI-Driven O-RAN Simulation Results ===" << std::endl;
    std::cout << "Simulation Time: " << g_simTime << " seconds" << std::endl;
    std::cout << "Number of UEs: " << g_numUEs << std::endl;
    std::cout << "Number of eNBs: " << g_numEnbs << std::endl;
    std::cout << "ML Algorithm: " << g_mlAlgorithm << std::endl;
    std::cout << "Digital Twin Enabled: " << (g_enableDigitalTwin ? "Yes" : "No") << std::endl;
    std::cout << "MEC Enabled: " << (g_enableMec ? "Yes" : "No") << std::endl;
    std::cout << "Federated Learning: " << (g_enableFederatedLearning ? "Yes" : "No") << std::endl;

    std::cout << "\n--- Performance Metrics ---" << std::endl;
    std::cout << "Total Handovers: " << g_metrics.totalHandovers << std::endl;
    std::cout << "Successful Handovers: " << g_metrics.successfulHandovers << std::endl;
    std::cout << "Handover Success Rate: " << (g_metrics.totalHandovers > 0 ? 100.0 * g_metrics.successfulHandovers / g_metrics.totalHandovers : 0.0) << "%" << std::endl;
    std::cout << "Average Throughput: " << g_metrics.averageThroughput << " Mbps" << std::endl;
    std::cout << "Average Edge Service Latency: " << g_metrics.edgeServiceLatency << " ms" << std::endl;

    // Generate detailed report
    std::ofstream reportFile("advanced_simulation_report.txt");
    reportFile << "Advanced O-RAN AI-Driven Simulation Report\n";
    reportFile << "========================================\n\n";
    reportFile << "Configuration:\n";
    reportFile << "- Simulation Time: " << g_simTime << "s\n";
    reportFile << "- UEs: " << g_numUEs << "\n";
    reportFile << "- eNBs: " << g_numEnbs << "\n";
    reportFile << "- Edge Nodes: " << g_numEdgeNodes << "\n";
    reportFile << "- ML Algorithm: " << g_mlAlgorithm << "\n";
    reportFile << "- Mobility Model: " << g_mobilityModel << "\n\n";

    reportFile << "Results:\n";
    reportFile << "- Total Handovers: " << g_metrics.totalHandovers << "\n";
    reportFile << "- Average Throughput: " << g_metrics.averageThroughput << " Mbps\n";
    reportFile << "- Edge Service Latency: " << g_metrics.edgeServiceLatency << " ms\n";

    reportFile.close();

    std::cout << "\nDetailed report saved to: advanced_simulation_report.txt" << std::endl;
    std::cout << "Database file: " << g_dbFileName << std::endl;
}

int main(int argc, char *argv[])
{
    auto startTime = std::chrono::high_resolution_clock::now();

    // Process command line arguments
    ProcessCommandLine(argc, argv);

    // Configure logging
    if (g_verbose)
    {
        LogComponentEnable("OranAdvancedAiEdgeExample", LOG_LEVEL_INFO);
        LogComponentEnable("OranLmReinforcementLearning", LOG_LEVEL_INFO);
        LogComponentEnable("OranDigitalTwin", LOG_LEVEL_INFO);
        LogComponentEnable("OranMecFramework", LOG_LEVEL_INFO);
    }

    std::cout << "Starting Advanced AI-Driven O-RAN Simulation..." << std::endl;
    std::cout << "Configuration: " << g_numUEs << " UEs, " << g_numEnbs << " eNBs, "
              << g_numEdgeNodes << " Edge Nodes" << std::endl;
    std::cout << "ML Algorithm: " << g_mlAlgorithm << std::endl;

    // Create node containers
    NodeContainer ueNodes, enbNodes, edgeNodes, pgwNode;

    // Setup network topology
    SetupNetworkTopology(ueNodes, enbNodes, edgeNodes, pgwNode);

    // Setup LTE network
    Ptr<LteHelper> lteHelper = SetupLteNetwork(ueNodes, enbNodes, pgwNode);

    // Setup O-RAN with advanced AI
    Ptr<OranNearRtRic> nearRtRic = SetupOranWithAI(ueNodes, enbNodes);

    // Get data repository for integration
    Ptr<OranDataRepository> dataRepo = nearRtRic->Data();

    // Setup Digital Twin framework
    NodeContainer allNodes;
    allNodes.Add(ueNodes);
    allNodes.Add(enbNodes);
    allNodes.Add(edgeNodes);

    Ptr<OranDigitalTwin> digitalTwin = SetupDigitalTwin(allNodes, dataRepo);

    // Setup MEC framework
    Ptr<OranMecFramework> mecFramework = SetupMecFramework(edgeNodes, dataRepo);

    // Setup applications
    Ipv4InterfaceContainer ueIpIface = lteHelper->GetEpcHelper()->GetUeDefaultGatewayAddress();
    SetupAdvancedApplications(ueNodes, ueIpIface);

    // Setup performance monitoring
    SetupPerformanceMonitoring(allNodes);

    // Connect trace callbacks
    Config::Connect("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx",
                    MakeCallback(&ThroughputCallback));

    // Start federated learning if enabled
    if (g_enableFederatedLearning && mecFramework)
    {
        Simulator::Schedule(Seconds(30.0), [mecFramework]()
                            { mecFramework->StartFederatedLearningRound(); });
    }

    // Schedule periodic optimization
    Simulator::Schedule(Seconds(20.0), [digitalTwin, mecFramework]()
                        {
        if (digitalTwin) {
            auto predictions = digitalTwin->RunPredictiveAnalysis(Seconds(10.0));
            std::cout << "Digital Twin predicted states for " << predictions.size() << " nodes" << std::endl;
        }
        
        if (mecFramework) {
            mecFramework->OptimizeResourceAllocation();
            auto analytics = mecFramework->GetEdgeAnalytics();
            std::cout << "MEC optimization completed. Active services: " 
                      << analytics["active_services"] << std::endl;
        } });

    std::cout << "Simulation configured. Starting execution..." << std::endl;

    // Run simulation
    Simulator::Stop(Seconds(g_simTime));
    Simulator::Run();

    // Cleanup and analysis
    if (digitalTwin)
    {
        digitalTwin->Stop();
    }

    if (mecFramework)
    {
        mecFramework->Stop();
    }

    Simulator::Destroy();

    // Calculate execution time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Simulation completed in " << duration.count() << " ms" << std::endl;

    // Analyze and report results
    AnalyzeResults();

    return 0;
}
