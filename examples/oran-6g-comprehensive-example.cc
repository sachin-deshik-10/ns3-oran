/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "../model/oran-helper.h"
#include "../model/oran-6g-terahertz.h"
#include "../model/oran-ai-transformer.h"
#include "../model/oran-6g-network-slicing.h"
#include "../model/oran-data-repository.h"
#include "../model/oran-digital-twin.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Oran6gIntegratedExample");

/**
 * \brief Comprehensive 6G O-RAN Integration Example
 *
 * This example demonstrates the integration of:
 * - 6G Terahertz Communications
 * - AI Transformer for intelligent optimization
 * - Network Slicing for service differentiation
 * - Digital Twin for network monitoring
 * - Data Repository for analytics
 */

class Oran6gIntegratedDemo
{
public:
    Oran6gIntegratedDemo();
    ~Oran6gIntegratedDemo();

    void SetupTopology();
    void Configure6gTerahertz();
    void ConfigureAiTransformer();
    void ConfigureNetworkSlicing();
    void ConfigureDigitalTwin();
    void ConfigureDataRepository();
    void SetupApplications();
    void RunSimulation();
    void AnalyzeResults();

private:
    // Simulation parameters
    uint32_t m_numEnb;
    uint32_t m_numUe;
    double m_simTime;
    double m_interPacketInterval;

    // NS-3 objects
    NodeContainer m_enbNodes;
    NodeContainer m_ueNodes;
    NetDeviceContainer m_enbDevs;
    NetDeviceContainer m_ueDevs;
    Ptr<LteHelper> m_lteHelper;
    Ptr<EpcHelper> m_epcHelper;
    Ptr<Node> m_pgw;

    // O-RAN enhanced modules
    Ptr<Oran6gTerahertz> m_teraherzModule;
    Ptr<OranAiTransformer> m_aiTransformer;
    Ptr<Oran6gNetworkSlicing> m_networkSlicing;
    Ptr<OranDigitalTwin> m_digitalTwin;
    Ptr<OranDataRepository> m_dataRepository;

    // Network slices
    std::map<uint32_t, uint32_t> m_sliceIds; // UE ID -> Slice ID mapping

    // Metrics collection
    std::vector<double> m_throughputSamples;
    std::vector<double> m_latencySamples;
    std::vector<double> m_aiPredictions;

    // Callback functions
    void NotifyConnectionEstablishedUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);
    void NotifyHandoverStartUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId);
    void NotifyHandoverEndOkUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti);
    void CollectThroughputMetrics();
    void CollectLatencyMetrics();
    void UpdateAiModel();
    void UpdateNetworkSlices();
    void CollectAnalytics();

    // Trace sinks
    void TeraherzMetricsTrace(double frequency, double bandwidth, double dataRate);
    void AiPredictionTrace(double prediction);
    void SlicePerformanceTrace(uint32_t sliceId, double throughput, double latency);
};

Oran6gIntegratedDemo::Oran6gIntegratedDemo()
    : m_numEnb(3),
      m_numUe(9),
      m_simTime(60.0),
      m_interPacketInterval(0.1)
{
    NS_LOG_FUNCTION(this);
}

Oran6gIntegratedDemo::~Oran6gIntegratedDemo()
{
    NS_LOG_FUNCTION(this);
}

void Oran6gIntegratedDemo::SetupTopology()
{
    NS_LOG_FUNCTION(this);

    // Create nodes
    m_enbNodes.Create(m_numEnb);
    m_ueNodes.Create(m_numUe);

    // Install mobility model
    MobilityHelper mobility;

    // eNB mobility (fixed positions)
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator>();
    enbPositionAlloc->Add(Vector(0.0, 0.0, 30.0));    // Central eNB
    enbPositionAlloc->Add(Vector(500.0, 0.0, 30.0));  // East eNB
    enbPositionAlloc->Add(Vector(-500.0, 0.0, 30.0)); // West eNB
    mobility.SetPositionAllocator(enbPositionAlloc);
    mobility.Install(m_enbNodes);

    // UE mobility (random walk)
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-800, 800, -800, 800)),
                              "Speed", StringValue("ns3::ConstantRandomVariable[Constant=5.0]"),
                              "Direction", StringValue("ns3::UniformRandomVariable[Min=0|Max=6.28]"));
    Ptr<RandomRectanglePositionAllocator> uePositionAlloc = CreateObject<RandomRectanglePositionAllocator>();
    uePositionAlloc->SetX(CreateObjectWithAttributes<UniformRandomVariable>("Min", DoubleValue(-300), "Max", DoubleValue(300)));
    uePositionAlloc->SetY(CreateObjectWithAttributes<UniformRandomVariable>("Min", DoubleValue(-300), "Max", DoubleValue(300)));
    mobility.SetPositionAllocator(uePositionAlloc);
    mobility.Install(m_ueNodes);

    NS_LOG_INFO("Network topology created with " << m_numEnb << " eNBs and " << m_numUe << " UEs");
}

void Oran6gIntegratedDemo::Configure6gTerahertz()
{
    NS_LOG_FUNCTION(this);

    // Create and configure THz module
    m_teraherzModule = CreateObject<Oran6gTerahertz>();
    m_teraherzModule->SetAttribute("CenterFrequency", DoubleValue(0.3));    // 300 GHz
    m_teraherzModule->SetAttribute("Bandwidth", DoubleValue(50.0));         // 50 GHz
    m_teraherzModule->SetAttribute("AntennaElements", UintegerValue(2048)); // Ultra-massive MIMO
    m_teraherzModule->SetAttribute("EnableIRS", BooleanValue(true));
    m_teraherzModule->SetAttribute("EnableBeamforming", BooleanValue(true));

    // Initialize THz communication parameters
    m_teraherzModule->Initialize();

    // Connect trace for metrics collection
    m_teraherzModule->TraceConnectWithoutContext("TeraherzMetrics",
                                                 MakeCallback(&Oran6gIntegratedDemo::TeraherzMetricsTrace, this));

    NS_LOG_INFO("6G Terahertz module configured: " << m_teraherzModule->GetCenterFrequency() << " THz, " << m_teraherzModule->GetBandwidth() << " GHz bandwidth");
}

void Oran6gIntegratedDemo::ConfigureAiTransformer()
{
    NS_LOG_FUNCTION(this);

    // Create and configure AI Transformer
    m_aiTransformer = CreateObject<OranAiTransformer>();
    m_aiTransformer->SetAttribute("NumHeads", UintegerValue(12)); // Multi-head attention
    m_aiTransformer->SetAttribute("EmbeddingDim", UintegerValue(768));
    m_aiTransformer->SetAttribute("FeedForwardDim", UintegerValue(3072));
    m_aiTransformer->SetAttribute("NumLayers", UintegerValue(8));
    m_aiTransformer->SetAttribute("LearningRate", DoubleValue(0.0001));
    m_aiTransformer->SetAttribute("EnableFederatedLearning", BooleanValue(true));
    m_aiTransformer->SetAttribute("FederationRounds", UintegerValue(20));

    // Connect traces
    m_aiTransformer->TraceConnectWithoutContext("PredictionAccuracy",
                                                MakeCallback(&Oran6gIntegratedDemo::AiPredictionTrace, this));

    NS_LOG_INFO("AI Transformer configured with " << m_aiTransformer->GetAttribute("NumLayers") << " layers and federated learning");
}

void Oran6gIntegratedDemo::ConfigureNetworkSlicing()
{
    NS_LOG_FUNCTION(this);

    // Create and configure Network Slicing
    m_networkSlicing = CreateObject<Oran6gNetworkSlicing>();
    m_networkSlicing->SetAttribute("MaxSlices", UintegerValue(32));
    m_networkSlicing->SetAttribute("DefaultSliceBandwidth", DoubleValue(200.0));
    m_networkSlicing->SetAttribute("EnableDynamicAllocation", BooleanValue(true));
    m_networkSlicing->SetAttribute("SliceIsolationLevel", UintegerValue(2)); // Full isolation
    m_networkSlicing->SetAttribute("QosThreshold", DoubleValue(0.99));

    // Set total available bandwidth from THz module
    double totalBandwidth = m_teraherzModule->GetBandwidth() * 1000; // Convert GHz to MHz
    m_networkSlicing->SetTotalBandwidth(totalBandwidth);

    // Create different slice types for different UE groups
    SliceRequirements eMbbReq;
    eMbbReq.bandwidth = 500.0; // Mbps
    eMbbReq.latency = 10.0;    // ms
    eMbbReq.reliability = 0.99;
    eMbbReq.priority = 2;

    SliceRequirements urllcReq;
    urllcReq.bandwidth = 100.0; // Mbps
    urllcReq.latency = 1.0;     // ms
    urllcReq.reliability = 0.99999;
    urllcReq.priority = 1;

    SliceRequirements xrReq;
    xrReq.bandwidth = 1000.0; // Mbps
    xrReq.latency = 5.0;      // ms
    xrReq.reliability = 0.999;
    xrReq.priority = 1;

    // Create slices and assign UEs
    uint32_t eMbbSlice = m_networkSlicing->CreateSlice(SLICE_EMBB, eMbbReq);
    uint32_t urllcSlice = m_networkSlicing->CreateSlice(SLICE_URLLC, urllcReq);
    uint32_t xrSlice = m_networkSlicing->CreateSlice(SLICE_XR, xrReq);

    // Assign UEs to slices
    for (uint32_t i = 0; i < m_numUe; ++i)
    {
        if (i < 3)
        {
            m_sliceIds[i] = eMbbSlice; // First 3 UEs to eMBB
        }
        else if (i < 6)
        {
            m_sliceIds[i] = urllcSlice; // Next 3 UEs to URLLC
        }
        else
        {
            m_sliceIds[i] = xrSlice; // Last 3 UEs to XR
        }
    }

    NS_LOG_INFO("Network slicing configured with " << m_networkSlicing->GetSliceCount() << " slices and " << totalBandwidth << " MHz total bandwidth");
}

void Oran6gIntegratedDemo::ConfigureDigitalTwin()
{
    NS_LOG_FUNCTION(this);

    // Create Digital Twin for network monitoring
    m_digitalTwin = CreateObject<OranDigitalTwin>();

    // Add network entities to digital twin
    for (uint32_t i = 0; i < m_numEnb; ++i)
    {
        m_digitalTwin->AddNetworkEntity(m_enbNodes.Get(i)->GetId(), "eNodeB");
    }

    for (uint32_t i = 0; i < m_numUe; ++i)
    {
        m_digitalTwin->AddNetworkEntity(m_ueNodes.Get(i)->GetId(), "UE");
    }

    NS_LOG_INFO("Digital Twin configured with " << (m_numEnb + m_numUe) << " network entities");
}

void Oran6gIntegratedDemo::ConfigureDataRepository()
{
    NS_LOG_FUNCTION(this);

    // Create Data Repository for analytics
    m_dataRepository = CreateObject<OranDataRepository>();

    // Initialize with SQLite backend
    std::string dbPath = "results/6g_integrated_simulation.db";
    m_dataRepository->SetAttribute("DatabaseFile", StringValue(dbPath));

    NS_LOG_INFO("Data Repository configured with database: " << dbPath);
}

void Oran6gIntegratedDemo::SetupApplications()
{
    NS_LOG_FUNCTION(this);

    // Create LTE Helper and EPC Helper
    m_lteHelper = CreateObject<LteHelper>();
    m_epcHelper = CreateObject<PointToPointEpcHelper>();
    m_lteHelper->SetEpcHelper(m_epcHelper);

    // Get PGW node
    m_pgw = m_epcHelper->GetPgwNode();

    // Install LTE devices
    m_enbDevs = m_lteHelper->InstallEnbDevice(m_enbNodes);
    m_ueDevs = m_lteHelper->InstallUeDevice(m_ueNodes);

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(m_ueNodes);
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = m_epcHelper->AssignUeIpv4Address(NetDeviceContainer(m_ueDevs));

    // Attach UEs to eNBs
    for (uint32_t u = 0; u < m_ueNodes.GetN(); ++u)
    {
        m_lteHelper->Attach(m_ueDevs.Get(u), m_enbDevs.Get(u % m_numEnb));
    }

    // Create applications for different slice types
    uint16_t dlPort = 1234;
    uint16_t ulPort = 2000;

    ApplicationContainer clientApps;
    ApplicationContainer serverApps;

    for (uint32_t i = 0; i < m_numUe; ++i)
    {
        // Create different application types based on slice assignment
        uint32_t sliceId = m_sliceIds[i];
        NetworkSlice sliceInfo = m_networkSlicing->GetSliceInfo(sliceId);

        if (sliceInfo.type == SLICE_EMBB)
        {
            // High bandwidth application (video streaming)
            PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), dlPort));
            serverApps.Add(dlPacketSinkHelper.Install(m_ueNodes.Get(i)));

            UdpClientHelper dlClient(ueIpIface.GetAddress(i), dlPort);
            dlClient.SetAttribute("Interval", TimeValue(MilliSeconds(10)));
            dlClient.SetAttribute("MaxPackets", UintegerValue(10000));
            dlClient.SetAttribute("PacketSize", UintegerValue(1024));
            clientApps.Add(dlClient.Install(m_pgw));
        }
        else if (sliceInfo.type == SLICE_URLLC)
        {
            // Low latency application (control messages)
            PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), dlPort));
            serverApps.Add(dlPacketSinkHelper.Install(m_ueNodes.Get(i)));

            UdpClientHelper dlClient(ueIpIface.GetAddress(i), dlPort);
            dlClient.SetAttribute("Interval", TimeValue(MilliSeconds(1)));
            dlClient.SetAttribute("MaxPackets", UintegerValue(60000));
            dlClient.SetAttribute("PacketSize", UintegerValue(64));
            clientApps.Add(dlClient.Install(m_pgw));
        }
        else if (sliceInfo.type == SLICE_XR)
        {
            // XR application (high bandwidth, low latency)
            PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
                                                InetSocketAddress(Ipv4Address::GetAny(), dlPort));
            serverApps.Add(dlPacketSinkHelper.Install(m_ueNodes.Get(i)));

            UdpClientHelper dlClient(ueIpIface.GetAddress(i), dlPort);
            dlClient.SetAttribute("Interval", TimeValue(MilliSeconds(5)));
            dlClient.SetAttribute("MaxPackets", UintegerValue(12000));
            dlClient.SetAttribute("PacketSize", UintegerValue(2048));
            clientApps.Add(dlClient.Install(m_pgw));
        }

        dlPort++;
    }

    serverApps.Start(Seconds(1.0));
    clientApps.Start(Seconds(2.0));
    serverApps.Stop(Seconds(m_simTime));
    clientApps.Stop(Seconds(m_simTime - 1.0));

    NS_LOG_INFO("Applications configured for " << m_numUe << " UEs with slice-specific traffic patterns");
}

void Oran6gIntegratedDemo::RunSimulation()
{
    NS_LOG_FUNCTION(this);

    // Setup trace connections
    Config::Connect("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                    MakeCallback(&Oran6gIntegratedDemo::NotifyConnectionEstablishedUe, this));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                    MakeCallback(&Oran6gIntegratedDemo::NotifyHandoverStartUe, this));
    Config::Connect("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                    MakeCallback(&Oran6gIntegratedDemo::NotifyHandoverEndOkUe, this));

    // Schedule periodic metric collection
    for (double t = 5.0; t < m_simTime; t += 5.0)
    {
        Simulator::Schedule(Seconds(t), &Oran6gIntegratedDemo::CollectThroughputMetrics, this);
        Simulator::Schedule(Seconds(t + 1.0), &Oran6gIntegratedDemo::CollectLatencyMetrics, this);
        Simulator::Schedule(Seconds(t + 2.0), &Oran6gIntegratedDemo::UpdateAiModel, this);
        Simulator::Schedule(Seconds(t + 3.0), &Oran6gIntegratedDemo::UpdateNetworkSlices, this);
        Simulator::Schedule(Seconds(t + 4.0), &Oran6gIntegratedDemo::CollectAnalytics, this);
    }

    NS_LOG_INFO("Starting simulation for " << m_simTime << " seconds...");

    Simulator::Stop(Seconds(m_simTime));
    Simulator::Run();
    Simulator::Destroy();

    NS_LOG_INFO("Simulation completed");
}

void Oran6gIntegratedDemo::AnalyzeResults()
{
    NS_LOG_FUNCTION(this);

    // Calculate average metrics
    double avgThroughput = 0.0;
    double avgLatency = 0.0;
    double avgAiAccuracy = 0.0;

    if (!m_throughputSamples.empty())
    {
        for (double sample : m_throughputSamples)
        {
            avgThroughput += sample;
        }
        avgThroughput /= m_throughputSamples.size();
    }

    if (!m_latencySamples.empty())
    {
        for (double sample : m_latencySamples)
        {
            avgLatency += sample;
        }
        avgLatency /= m_latencySamples.size();
    }

    if (m_aiTransformer)
    {
        avgAiAccuracy = m_aiTransformer->GetModelAccuracy();
    }

    // Print comprehensive results
    std::cout << "\n=== 6G O-RAN Integrated Simulation Results ===" << std::endl;
    std::cout << "Simulation Time: " << m_simTime << " seconds" << std::endl;
    std::cout << "Number of eNBs: " << m_numEnb << std::endl;
    std::cout << "Number of UEs: " << m_numUe << std::endl;
    std::cout << "\n--- 6G Terahertz Performance ---" << std::endl;
    std::cout << "Center Frequency: " << m_teraherzModule->GetCenterFrequency() << " THz" << std::endl;
    std::cout << "Bandwidth: " << m_teraherzModule->GetBandwidth() << " GHz" << std::endl;
    std::cout << "Data Rate: " << m_teraherzModule->GetDataRate() << " Gbps" << std::endl;
    std::cout << "\n--- Network Slicing Performance ---" << std::endl;
    std::cout << "Active Slices: " << m_networkSlicing->GetSliceCount() << std::endl;
    std::cout << "Total Bandwidth: " << m_networkSlicing->GetTotalBandwidth() << " MHz" << std::endl;
    std::cout << "Available Bandwidth: " << m_networkSlicing->GetAvailableBandwidth() << " MHz" << std::endl;
    std::cout << "\n--- AI Transformer Performance ---" << std::endl;
    std::cout << "Model Accuracy: " << avgAiAccuracy * 100 << "%" << std::endl;
    std::cout << "Training Epochs: " << m_aiTransformer->GetTrainingEpoch() << std::endl;
    std::cout << "Learning Rate: " << m_aiTransformer->GetLearningRate() << std::endl;
    std::cout << "\n--- Overall Network Performance ---" << std::endl;
    std::cout << "Average Throughput: " << avgThroughput << " Mbps" << std::endl;
    std::cout << "Average Latency: " << avgLatency << " ms" << std::endl;
    std::cout << "Throughput Samples: " << m_throughputSamples.size() << std::endl;
    std::cout << "Latency Samples: " << m_latencySamples.size() << std::endl;

    // Print slice-specific performance
    std::cout << "\n--- Slice-Specific Performance ---" << std::endl;
    std::vector<uint32_t> activeSlices = m_networkSlicing->GetActiveSlices();
    for (uint32_t sliceId : activeSlices)
    {
        SliceMetrics metrics = m_networkSlicing->GetSliceMetrics(sliceId);
        NetworkSlice sliceInfo = m_networkSlicing->GetSliceInfo(sliceId);
        std::cout << "Slice " << sliceId << " (Type: " << static_cast<uint32_t>(sliceInfo.type) << "): " << "Throughput=" << metrics.throughput << " Mbps, " << "Latency=" << metrics.latency << " ms, " << "Reliability=" << metrics.reliability * 100 << "%" << std::endl;
    }

    // Export results to files
    std::string outputDir = "results/";

    // Export throughput data
    std::ofstream throughputFile(outputDir + "6g_throughput_results.csv");
    throughputFile << "Time,Throughput_Mbps\n";
    for (size_t i = 0; i < m_throughputSamples.size(); ++i)
    {
        throughputFile << (i * 5.0) << "," << m_throughputSamples[i] << "\n";
    }
    throughputFile.close();

    // Export latency data
    std::ofstream latencyFile(outputDir + "6g_latency_results.csv");
    latencyFile << "Time,Latency_ms\n";
    for (size_t i = 0; i < m_latencySamples.size(); ++i)
    {
        latencyFile << (i * 5.0) << "," << m_latencySamples[i] << "\n";
    }
    latencyFile.close();

    // Export AI predictions
    std::ofstream aiFile(outputDir + "6g_ai_predictions.csv");
    aiFile << "Time,Prediction\n";
    for (size_t i = 0; i < m_aiPredictions.size(); ++i)
    {
        aiFile << (i * 5.0) << "," << m_aiPredictions[i] << "\n";
    }
    aiFile.close();

    std::cout << "\nResults exported to " << outputDir << std::endl;
    std::cout << "=== Simulation Analysis Complete ===" << std::endl;
}

// Callback implementations
void Oran6gIntegratedDemo::NotifyConnectionEstablishedUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    NS_LOG_INFO("UE IMSI " << imsi << " connected to cell " << cellid << " with RNTI " << rnti);

    // Update digital twin
    if (m_digitalTwin)
    {
        m_digitalTwin->UpdateEntityState(imsi, "CONNECTED");
    }
}

void Oran6gIntegratedDemo::NotifyHandoverStartUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti, uint16_t targetCellId)
{
    NS_LOG_INFO("UE IMSI " << imsi << " starting handover from cell " << cellid << " to cell " << targetCellId);

    // Update digital twin
    if (m_digitalTwin)
    {
        m_digitalTwin->UpdateEntityState(imsi, "HANDOVER");
    }
}

void Oran6gIntegratedDemo::NotifyHandoverEndOkUe(std::string context, uint64_t imsi, uint16_t cellid, uint16_t rnti)
{
    NS_LOG_INFO("UE IMSI " << imsi << " completed handover to cell " << cellid);

    // Update digital twin
    if (m_digitalTwin)
    {
        m_digitalTwin->UpdateEntityState(imsi, "CONNECTED");
    }
}

void Oran6gIntegratedDemo::CollectThroughputMetrics()
{
    // Simulate throughput measurement
    double throughput = m_teraherzModule->GetDataRate() * 1000.0 * (0.7 + 0.3 * ((double)rand() / RAND_MAX));
    m_throughputSamples.push_back(throughput);

    NS_LOG_DEBUG("Collected throughput: " << throughput << " Mbps");
}

void Oran6gIntegratedDemo::CollectLatencyMetrics()
{
    // Simulate latency measurement based on slice types
    double latency = 2.0 + 5.0 * ((double)rand() / RAND_MAX); // 2-7 ms range
    m_latencySamples.push_back(latency);

    NS_LOG_DEBUG("Collected latency: " << latency << " ms");
}

void Oran6gIntegratedDemo::UpdateAiModel()
{
    if (!m_aiTransformer)
        return;

    // Create training data from recent metrics
    std::vector<std::vector<double>> trainingData;
    std::vector<std::vector<double>> targets;

    if (m_throughputSamples.size() >= 2 && m_latencySamples.size() >= 2)
    {
        // Use recent samples for training
        std::vector<double> input = {
            m_throughputSamples.back(),
            m_latencySamples.back(),
            static_cast<double>(m_networkSlicing->GetSliceCount()),
            m_teraherzModule->GetDataRate()};

        std::vector<double> target = {
            m_throughputSamples.back() * 1.1 // Target: 10% improvement
        };

        trainingData.push_back(input);
        targets.push_back(target);

        // Train the model
        m_aiTransformer->Train(trainingData, targets);
    }

    NS_LOG_DEBUG("AI model updated, accuracy: " << m_aiTransformer->GetModelAccuracy());
}

void Oran6gIntegratedDemo::UpdateNetworkSlices()
{
    if (!m_networkSlicing)
        return;

    // Update slice metrics with simulated data
    std::vector<uint32_t> activeSlices = m_networkSlicing->GetActiveSlices();

    for (uint32_t sliceId : activeSlices)
    {
        SliceMetrics metrics;
        NetworkSlice sliceInfo = m_networkSlicing->GetSliceInfo(sliceId);

        // Simulate metrics based on slice type
        switch (sliceInfo.type)
        {
        case SLICE_EMBB:
            metrics.throughput = 400.0 + 100.0 * ((double)rand() / RAND_MAX);
            metrics.latency = 8.0 + 4.0 * ((double)rand() / RAND_MAX);
            metrics.reliability = 0.98 + 0.02 * ((double)rand() / RAND_MAX);
            break;
        case SLICE_URLLC:
            metrics.throughput = 80.0 + 40.0 * ((double)rand() / RAND_MAX);
            metrics.latency = 0.5 + 1.0 * ((double)rand() / RAND_MAX);
            metrics.reliability = 0.9999 + 0.0001 * ((double)rand() / RAND_MAX);
            break;
        case SLICE_XR:
            metrics.throughput = 800.0 + 200.0 * ((double)rand() / RAND_MAX);
            metrics.latency = 3.0 + 2.0 * ((double)rand() / RAND_MAX);
            metrics.reliability = 0.997 + 0.003 * ((double)rand() / RAND_MAX);
            break;
        default:
            break;
        }

        metrics.packetLoss = 0.001 * ((double)rand() / RAND_MAX);

        m_networkSlicing->UpdateSliceMetrics(sliceId, metrics);
    }

    NS_LOG_DEBUG("Network slice metrics updated for " << activeSlices.size() << " slices");
}

void Oran6gIntegratedDemo::CollectAnalytics()
{
    // Collect comprehensive analytics for data repository
    if (m_dataRepository)
    {
        // This would typically store complex analytics data
        // For this example, we just log the collection
        NS_LOG_DEBUG("Analytics collected and stored in data repository");
    }
}

void Oran6gIntegratedDemo::TeraherzMetricsTrace(double frequency, double bandwidth, double dataRate)
{
    NS_LOG_INFO("THz Metrics - Frequency: " << frequency << " THz, Bandwidth: " << bandwidth << " GHz, Data Rate: " << dataRate << " Gbps");
}

void Oran6gIntegratedDemo::AiPredictionTrace(double prediction)
{
    m_aiPredictions.push_back(prediction);
    NS_LOG_INFO("AI Prediction: " << prediction);
}

void Oran6gIntegratedDemo::SlicePerformanceTrace(uint32_t sliceId, double throughput, double latency)
{
    NS_LOG_INFO("Slice " << sliceId << " Performance - Throughput: " << throughput << " Mbps, Latency: " << latency << " ms");
}

int main(int argc, char *argv[])
{
    // Set random seed for reproducible results
    RngSeedManager::SetSeed(12345);

    // Enable logging
    LogComponentEnable("Oran6gIntegratedExample", LOG_LEVEL_INFO);
    LogComponentEnable("Oran6gTerahertz", LOG_LEVEL_INFO);
    LogComponentEnable("OranAiTransformer", LOG_LEVEL_INFO);
    LogComponentEnable("Oran6gNetworkSlicing", LOG_LEVEL_INFO);

    // Parse command line arguments
    CommandLine cmd;
    cmd.Parse(argc, argv);

    // Create output directory
    std::string command = "mkdir -p results";
    int result = std::system(command.c_str());
    if (result != 0)
    {
        NS_LOG_WARN("Failed to create results directory");
    }

    // Create and run the integrated demo
    Oran6gIntegratedDemo demo;

    std::cout << "=== 6G O-RAN Integrated Simulation Starting ===" << std::endl;

    demo.SetupTopology();
    demo.Configure6gTerahertz();
    demo.ConfigureAiTransformer();
    demo.ConfigureNetworkSlicing();
    demo.ConfigureDigitalTwin();
    demo.ConfigureDataRepository();
    demo.SetupApplications();
    demo.RunSimulation();
    demo.AnalyzeResults();

    return 0;
}
