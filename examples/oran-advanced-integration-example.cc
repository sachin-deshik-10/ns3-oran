/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 *
 * Advanced O-RAN Integration Test Example
 *
 * This example demonstrates the integration of all advanced O-RAN features:
 * - Reinforcement Learning for intelligent handovers
 * - Digital Twin for network prediction and optimization
 * - Multi-access Edge Computing (MEC) for low-latency services
 * - Cloud-Native orchestration with Kubernetes
 * - Federated Learning across edge nodes
 * - Advanced analytics and monitoring
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"

// O-RAN includes
#include "ns3/oran-helper.h"
#include "ns3/oran-lm-reinforcement-learning.h"
#include "ns3/oran-digital-twin.h"
#include "ns3/oran-mec-framework.h"
#include "ns3/oran-cloud-native.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("OranAdvancedIntegrationExample");

/**
 * Network configuration parameters
 */
struct NetworkConfig
{
    uint32_t numEnbs = 7;
    uint32_t numUes = 20;
    double distance = 60.0;
    double speed = 20.0;
    Time simTime = Seconds(300.0);
    bool enableRL = true;
    bool enableDigitalTwin = true;
    bool enableMEC = true;
    bool enableCloudNative = true;
    bool enableFederatedLearning = true;
};

/**
 * Setup LTE network topology
 */
void SetupLteNetwork(NodeContainer &enbNodes, NodeContainer &ueNodes,
                     Ptr<LteHelper> &lteHelper, const NetworkConfig &config)
{
    NS_LOG_INFO("Setting up LTE network with " << config.numEnbs << " eNBs and " << config.numUes << " UEs");

    // Create nodes
    enbNodes.Create(config.numEnbs);
    ueNodes.Create(config.numUes);

    // Install LTE protocol stack
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice(enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice(ueNodes);

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(ueNodes);

    // Assign IP addresses
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = lteHelper->AssignUeIpv4Address(NetDeviceContainer(ueLteDevs));

    // Set up mobility
    MobilityHelper mobility;

    // eNB positions (stationary)
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();

    for (uint32_t i = 0; i < config.numEnbs; ++i)
    {
        double x = (i % 3) * config.distance;
        double y = (i / 3) * config.distance;
        enbPositionAlloc->Add(Vector(x, y, 0.0));
    }

    mobility.SetPositionAllocator(enbPositionAlloc);
    mobility.Install(enbNodes);

    // UE mobility (random walk)
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-50, config.distance * 2 + 50, -50, config.distance * 2 + 50)),
                              "Speed", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(config.speed) + "]"));
    mobility.Install(ueNodes);

    // Attach UEs to eNBs
    lteHelper->Attach(ueLteDevs);
}

/**
 * Setup O-RAN components with advanced features
 */
void SetupOranComponents(NodeContainer &enbNodes, NodeContainer &ueNodes,
                         Ptr<LteHelper> &lteHelper, const NetworkConfig &config)
{
    NS_LOG_INFO("Setting up advanced O-RAN components");

    // Create O-RAN helper
    OranHelper oranHelper;

    // Setup data repository
    oranHelper.SetDataRepository("ns3::OranDataRepositorySqlite",
                                 "DatabaseFile", StringValue("oran-advanced-integration.db"));

    // Configure reinforcement learning logic module
    if (config.enableRL)
    {
        NS_LOG_INFO("Configuring Reinforcement Learning Logic Module");
        oranHelper.SetLogicModule("ns3::OranLmReinforcementLearning",
                                  "LearningRate", DoubleValue(0.001),
                                  "DiscountFactor", DoubleValue(0.95),
                                  "ExplorationRate", DoubleValue(0.1),
                                  "ExperienceBufferSize", UintegerValue(10000),
                                  "BatchSize", UintegerValue(64),
                                  "UpdateFrequency", UintegerValue(100));
    }

    // Setup Digital Twin
    if (config.enableDigitalTwin)
    {
        NS_LOG_INFO("Setting up Digital Twin framework");
        Ptr<OranDigitalTwin> digitalTwin = CreateObject<OranDigitalTwin>();
        digitalTwin->SetAttribute("PredictionWindow", TimeValue(Seconds(60.0)));
        digitalTwin->SetAttribute("UpdateInterval", TimeValue(Seconds(5.0)));
        digitalTwin->SetAttribute("EnablePredictiveAnalytics", BooleanValue(true));
        digitalTwin->SetAttribute("EnableWhatIfAnalysis", BooleanValue(true));

        oranHelper.SetDigitalTwin(digitalTwin);
    }

    // Setup MEC Framework
    if (config.enableMEC)
    {
        NS_LOG_INFO("Setting up Multi-access Edge Computing framework");
        Ptr<OranMecFramework> mecFramework = CreateObject<OranMecFramework>();
        mecFramework->SetAttribute("MaxServices", UintegerValue(500));
        mecFramework->SetAttribute("EnableFederatedLearning", BooleanValue(config.enableFederatedLearning));
        mecFramework->SetAttribute("ServiceDiscoveryInterval", TimeValue(Seconds(10.0)));
        mecFramework->SetAttribute("LoadBalancingThreshold", DoubleValue(0.75));

        // Register edge nodes with different capabilities
        EdgeNodeCapabilities highPerf, mediumPerf, lowPerf;

        // High-performance edge node
        highPerf.cpuCores = 32;
        highPerf.memoryGB = 128;
        highPerf.storageGB = 2000;
        highPerf.gpuCores = 8;
        highPerf.networkBandwidthMbps = 10000;
        highPerf.latencyMs = 1.0;

        // Medium-performance edge node
        mediumPerf.cpuCores = 16;
        mediumPerf.memoryGB = 64;
        mediumPerf.storageGB = 1000;
        mediumPerf.gpuCores = 4;
        mediumPerf.networkBandwidthMbps = 5000;
        mediumPerf.latencyMs = 2.0;

        // Low-performance edge node
        lowPerf.cpuCores = 8;
        lowPerf.memoryGB = 32;
        lowPerf.storageGB = 500;
        lowPerf.gpuCores = 2;
        lowPerf.networkBandwidthMbps = 1000;
        lowPerf.latencyMs = 5.0;

        mecFramework->RegisterEdgeNode("edge-high-perf", highPerf);
        mecFramework->RegisterEdgeNode("edge-medium-perf", mediumPerf);
        mecFramework->RegisterEdgeNode("edge-low-perf", lowPerf);

        oranHelper.SetMecFramework(mecFramework);
    }

    // Setup Cloud-Native Framework
    if (config.enableCloudNative)
    {
        NS_LOG_INFO("Setting up Cloud-Native orchestration framework");
        Ptr<OranCloudNative> cloudNative = CreateObject<OranCloudNative>();
        cloudNative->SetAttribute("MaxPods", UintegerValue(2000));
        cloudNative->SetAttribute("EnableServiceMesh", BooleanValue(true));
        cloudNative->SetAttribute("AutoScalingEnabled", BooleanValue(true));
        cloudNative->SetAttribute("MonitoringInterval", TimeValue(Seconds(15.0)));
        cloudNative->SetAttribute("ScalingThreshold", DoubleValue(0.8));

        oranHelper.SetCloudNative(cloudNative);
    }

    // Install O-RAN on eNBs
    oranHelper.SetConflictMitigationModule("ns3::OranCmmHandover");
    oranHelper.InstallRic(enbNodes);

    // Install O-RAN terminators on UEs
    oranHelper.InstallTerminators(ueNodes, enbNodes);

    // Add reporters for comprehensive monitoring
    oranHelper.AddReporter("ns3::OranReporterLocation", ueNodes, Seconds(1.0));
    oranHelper.AddReporter("ns3::OranReporterLteUeRsrpRsrq", ueNodes, Seconds(1.0));
    oranHelper.AddReporter("ns3::OranReporterLteUeCellInfo", ueNodes, Seconds(2.0));

    NS_LOG_INFO("O-RAN advanced components setup completed");
}

/**
 * Deploy sample edge services
 */
void DeployEdgeServices(Ptr<OranMecFramework> mecFramework)
{
    if (!mecFramework)
        return;

    NS_LOG_INFO("Deploying sample edge services");

    // Deploy AR/VR service
    EdgeServiceRequirements arService;
    arService.serviceType = EdgeServiceType::AUGMENTED_REALITY;
    arService.cpuCores = 4.0;
    arService.memoryMB = 8192;
    arService.storageMB = 2048;
    arService.maxLatencyMs = 5.0;
    arService.bandwidthMbps = 1000;
    arService.requiresGpu = true;

    mecFramework->DeployService(arService);

    // Deploy autonomous vehicle service
    EdgeServiceRequirements avService;
    avService.serviceType = EdgeServiceType::AUTONOMOUS_VEHICLE;
    avService.cpuCores = 8.0;
    avService.memoryMB = 16384;
    avService.storageMB = 4096;
    avService.maxLatencyMs = 1.0;
    avService.bandwidthMbps = 2000;
    avService.requiresGpu = true;

    mecFramework->DeployService(avService);

    // Deploy video analytics service
    EdgeServiceRequirements vaService;
    vaService.serviceType = EdgeServiceType::VIDEO_ANALYTICS;
    vaService.cpuCores = 6.0;
    vaService.memoryMB = 12288;
    vaService.storageMB = 8192;
    avService.maxLatencyMs = 10.0;
    vaService.bandwidthMbps = 500;
    vaService.requiresGpu = true;

    mecFramework->DeployService(vaService);

    NS_LOG_INFO("Edge services deployment completed");
}

/**
 * Deploy cloud-native microservices
 */
void DeployMicroservices(Ptr<OranCloudNative> cloudNative)
{
    if (!cloudNative)
        return;

    NS_LOG_INFO("Deploying cloud-native microservices");

    // Deploy O-RAN Near-RT RIC service
    MicroserviceSpec ricService;
    ricService.name = "oran-near-rt-ric";
    ricService.image = "oran/near-rt-ric";
    ricService.version = "latest";
    ricService.namespaceName = "oran-system";
    ricService.replicas = 3;
    ricService.cpuRequest = 2.0;
    ricService.cpuLimit = 4.0;
    ricService.memoryRequest = 4.0;
    ricService.memoryLimit = 8.0;
    ricService.labels["app"] = "near-rt-ric";
    ricService.labels["version"] = "v1.0";
    ricService.ports.push_back("8080:8080");
    ricService.ports.push_back("9090:9090");

    cloudNative->CreateService(ricService);

    // Deploy AI/ML service
    MicroserviceSpec aiService;
    aiService.name = "oran-ai-ml-service";
    aiService.image = "oran/ai-ml-service";
    aiService.version = "latest";
    aiService.namespaceName = "oran-apps";
    aiService.replicas = 2;
    aiService.cpuRequest = 4.0;
    aiService.cpuLimit = 8.0;
    aiService.memoryRequest = 8.0;
    aiService.memoryLimit = 16.0;
    aiService.labels["app"] = "ai-ml-service";
    aiService.labels["tier"] = "backend";
    aiService.ports.push_back("5000:5000");

    cloudNative->CreateService(aiService);

    // Deploy monitoring service
    MicroserviceSpec monitoringService;
    monitoringService.name = "oran-monitoring";
    monitoringService.image = "oran/monitoring";
    monitoringService.version = "latest";
    monitoringService.namespaceName = "monitoring";
    monitoringService.replicas = 1;
    monitoringService.cpuRequest = 1.0;
    monitoringService.cpuLimit = 2.0;
    monitoringService.memoryRequest = 2.0;
    monitoringService.memoryLimit = 4.0;
    monitoringService.labels["app"] = "monitoring";
    monitoringService.ports.push_back("3000:3000");

    cloudNative->CreateService(monitoringService);

    NS_LOG_INFO("Microservices deployment completed");
}

/**
 * Setup performance monitoring and analytics
 */
void SetupMonitoring()
{
    NS_LOG_INFO("Setting up performance monitoring and analytics");

    // Enable comprehensive logging
    LogComponentEnable("OranLmReinforcementLearning", LOG_LEVEL_INFO);
    LogComponentEnable("OranDigitalTwin", LOG_LEVEL_INFO);
    LogComponentEnable("OranMecFramework", LOG_LEVEL_INFO);
    LogComponentEnable("OranCloudNative", LOG_LEVEL_INFO);

    // Setup file outputs for analysis
    Config::SetDefault("ns3::FileHelper::Filename", StringValue("oran-advanced-integration"));
    Config::SetDefault("ns3::FileHelper::FileProbe::Filename", StringValue("oran-metrics.txt"));
}

/**
 * Main simulation function
 */
int main(int argc, char *argv[])
{
    NetworkConfig config;

    // Parse command line arguments
    CommandLine cmd(__FILE__);
    cmd.AddValue("numEnbs", "Number of eNBs", config.numEnbs);
    cmd.AddValue("numUes", "Number of UEs", config.numUes);
    cmd.AddValue("distance", "Distance between eNBs", config.distance);
    cmd.AddValue("speed", "UE speed", config.speed);
    cmd.AddValue("simTime", "Simulation time", config.simTime);
    cmd.AddValue("enableRL", "Enable Reinforcement Learning", config.enableRL);
    cmd.AddValue("enableDigitalTwin", "Enable Digital Twin", config.enableDigitalTwin);
    cmd.AddValue("enableMEC", "Enable MEC Framework", config.enableMEC);
    cmd.AddValue("enableCloudNative", "Enable Cloud-Native", config.enableCloudNative);
    cmd.AddValue("enableFederatedLearning", "Enable Federated Learning", config.enableFederatedLearning);
    cmd.Parse(argc, argv);

    // Configure simulation environment
    Time::SetResolution(Time::NS);
    Config::SetDefault("ns3::LteHelper::UseCa", BooleanValue(true));
    Config::SetDefault("ns3::LteHelper::NumberOfComponentCarriers", UintegerValue(2));

    NS_LOG_INFO("Starting O-RAN Advanced Integration Example");
    NS_LOG_INFO("Configuration:");
    NS_LOG_INFO("  eNBs: " << config.numEnbs);
    NS_LOG_INFO("  UEs: " << config.numUes);
    NS_LOG_INFO("  Distance: " << config.distance << " m");
    NS_LOG_INFO("  Speed: " << config.speed << " m/s");
    NS_LOG_INFO("  Simulation time: " << config.simTime.GetSeconds() << " s");
    NS_LOG_INFO("  Reinforcement Learning: " << (config.enableRL ? "Enabled" : "Disabled"));
    NS_LOG_INFO("  Digital Twin: " << (config.enableDigitalTwin ? "Enabled" : "Disabled"));
    NS_LOG_INFO("  MEC Framework: " << (config.enableMEC ? "Enabled" : "Disabled"));
    NS_LOG_INFO("  Cloud-Native: " << (config.enableCloudNative ? "Enabled" : "Disabled"));
    NS_LOG_INFO("  Federated Learning: " << (config.enableFederatedLearning ? "Enabled" : "Disabled"));

    // Setup monitoring
    SetupMonitoring();

    // Create LTE helper
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper>();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper>();
    lteHelper->SetEpcHelper(epcHelper);
    lteHelper->SetSchedulerType("ns3::PfFfMacScheduler");
    lteHelper->SetHandoverAlgorithmType("ns3::A3RsrpHandoverAlgorithm");
    lteHelper->SetHandoverAlgorithmAttribute("Hysteresis", DoubleValue(3.0));
    lteHelper->SetHandoverAlgorithmAttribute("TimeToTrigger", TimeValue(MilliSeconds(256)));

    // Create nodes
    NodeContainer enbNodes, ueNodes;

    // Setup LTE network
    SetupLteNetwork(enbNodes, ueNodes, lteHelper, config);

    // Setup O-RAN components
    SetupOranComponents(enbNodes, ueNodes, lteHelper, config);

    // Get framework instances for service deployment
    Ptr<OranMecFramework> mecFramework = nullptr;
    Ptr<OranCloudNative> cloudNative = nullptr;

    if (config.enableMEC)
    {
        mecFramework = CreateObject<OranMecFramework>();
        DeployEdgeServices(mecFramework);
    }

    if (config.enableCloudNative)
    {
        cloudNative = CreateObject<OranCloudNative>();
        DeployMicroservices(cloudNative);
    }

    // Setup applications (UDP echo for testing)
    uint16_t dlPort = 1234;
    uint16_t ulPort = 2000;
    ApplicationContainer clientApps, serverApps;

    for (uint32_t u = 0; u < ueNodes.GetN(); ++u)
    {
        PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), dlPort));
        PacketSinkHelper ulPacketSinkHelper("ns3::UdpSocketFactory",
                                            InetSocketAddress(Ipv4Address::GetAny(), ulPort));

        serverApps.Add(dlPacketSinkHelper.Install(ueNodes.Get(u)));

        UdpEchoClientHelper client(epcHelper->GetUeDefaultGatewayAddress(), ulPort);
        client.SetAttribute("MaxPackets", UintegerValue(1000000));
        client.SetAttribute("Interval", TimeValue(MilliSeconds(100)));
        client.SetAttribute("PacketSize", UintegerValue(1024));

        clientApps.Add(client.Install(ueNodes.Get(u)));
    }

    serverApps.Start(Seconds(0.0));
    clientApps.Start(Seconds(1.0));

    // Enable traces for analysis
    lteHelper->EnableTraces();

    NS_LOG_INFO("Starting simulation...");

    // Run simulation
    Simulator::Stop(config.simTime);
    Simulator::Run();

    NS_LOG_INFO("Simulation completed");
    NS_LOG_INFO("Results saved to oran-advanced-integration.db");

    // Cleanup
    Simulator::Destroy();

    return 0;
}
