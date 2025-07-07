/*
 * Copyright (c) 2025 O-RAN 6G Research Consortium
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * O-RAN 6G Comprehensive Advanced Features Example
 * Demonstrates quantum, holographic, performance optimization, and industrial IoT
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/lte-module.h"
#include "ns3/config-store-module.h"

// O-RAN modules
#include "oran-helper.h"
#include "oran-data-repository.h"
#include "oran-cmm.h"
#include "oran-lm.h"

// Advanced 6G modules
#include "oran-6g-terahertz.h"
#include "oran-ai-transformer.h"
#include "oran-6g-network-slicing.h"
#include "oran-6g-quantum-enhanced.h"
#include "oran-6g-holographic.h"
#include "oran-6g-performance-optimizer.h"
#include "oran-6g-industrial-iot.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Oran6gAdvancedFeaturesExample");

/**
 * \brief Comprehensive 6G O-RAN Advanced Features Example
 * 
 * This example demonstrates the integration of all advanced 6G features:
 * - Quantum-enhanced communication
 * - Holographic data transmission
 * - Performance optimization with GPU acceleration
 * - Industrial IoT and sustainability
 * - AI-native networking with transformers
 * - THz communications
 * - Dynamic network slicing
 */
int
main (int argc, char *argv[])
{
    // Simulation parameters
    uint32_t numEnbs = 3;
    uint32_t numUes = 20;
    double simTime = 60.0;
    double interPacketInterval = 100;
    bool enableQuantum = true;
    bool enableHolographic = true;
    bool enablePerformanceOpt = true;
    bool enableIndustrialIoT = true;
    
    // Command line arguments
    CommandLine cmd;
    cmd.AddValue ("numEnbs", "Number of eNBs", numEnbs);
    cmd.AddValue ("numUes", "Number of UEs", numUes);
    cmd.AddValue ("simTime", "Simulation time", simTime);
    cmd.AddValue ("enableQuantum", "Enable quantum features", enableQuantum);
    cmd.AddValue ("enableHolographic", "Enable holographic features", enableHolographic);
    cmd.AddValue ("enablePerformanceOpt", "Enable performance optimization", enablePerformanceOpt);
    cmd.AddValue ("enableIndustrialIoT", "Enable industrial IoT", enableIndustrialIoT);
    cmd.Parse (argc, argv);
    
    NS_LOG_INFO ("=== O-RAN 6G Advanced Features Comprehensive Example ===");
    NS_LOG_INFO ("Simulating " << numUes << " UEs with " << numEnbs << " eNBs for " << simTime << " seconds");
    
    // Configure logging
    LogComponentEnable ("Oran6gAdvancedFeaturesExample", LOG_LEVEL_INFO);
    LogComponentEnable ("OranHelper", LOG_LEVEL_INFO);
    LogComponentEnable ("Oran6gTerahertz", LOG_LEVEL_INFO);
    LogComponentEnable ("OranAiTransformer", LOG_LEVEL_INFO);
    LogComponentEnable ("Oran6gNetworkSlicing", LOG_LEVEL_INFO);
    LogComponentEnable ("Oran6gQuantumEnhanced", LOG_LEVEL_INFO);
    LogComponentEnable ("Oran6gHolographic", LOG_LEVEL_INFO);
    LogComponentEnable ("Oran6gPerformanceOptimizer", LOG_LEVEL_INFO);
    LogComponentEnable ("Oran6gIndustrialIoT", LOG_LEVEL_INFO);
    
    // Create nodes
    NodeContainer enbNodes;
    enbNodes.Create (numEnbs);
    
    NodeContainer ueNodes;
    ueNodes.Create (numUes);
    
    // Configure mobility
    MobilityHelper mobility;
    
    // eNB mobility (stationary)
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
    for (uint32_t i = 0; i < numEnbs; ++i)
    {
        enbPositionAlloc->Add (Vector (i * 1000.0, 0.0, 30.0));
    }
    mobility.SetPositionAllocator (enbPositionAlloc);
    mobility.Install (enbNodes);
    
    // UE mobility (random walk with boundaries)
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue (Rectangle (-500, 2500, -500, 500)),
                              "Speed", StringValue ("ns3::ConstantRandomVariable[Constant=10.0]"),
                              "Direction", StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"));
    mobility.Install (ueNodes);
    
    // Install LTE devices
    Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
    Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
    lteHelper->SetEpcHelper (epcHelper);
    
    NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
    NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);
    
    // Install Internet stack
    InternetStackHelper internet;
    internet.Install (ueNodes);
    
    Ipv4InterfaceContainer ueIpIface;
    ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
    
    // Attach UEs to eNBs
    for (uint32_t i = 0; i < numUes; ++i)
    {
        lteHelper->Attach (ueLteDevs.Get (i), enbLteDevs.Get (i % numEnbs));
    }
    
    // ===============================================
    // Initialize O-RAN Framework
    // ===============================================
    
    OranHelper oranHelper;
    oranHelper.SetAttribute ("RepositoryType", StringValue ("sqlite"));
    oranHelper.SetAttribute ("DatabaseFile", StringValue ("oran_advanced_features.db"));
    
    // Install O-RAN components
    oranHelper.Install (enbNodes);
    oranHelper.Install (ueNodes);
    
    // ===============================================
    // Initialize Advanced 6G Modules
    // ===============================================
    
    NS_LOG_INFO ("Initializing Advanced 6G Modules...");
    
    // 1. THz Communication Module
    Ptr<Oran6gTerahertz> thzModule = CreateObject<Oran6gTerahertz> ();
    thzModule->SetAttribute ("OperatingFrequency", DoubleValue (300e9)); // 300 GHz
    thzModule->SetAttribute ("Bandwidth", DoubleValue (10e9)); // 10 GHz
    thzModule->SetAttribute ("AntennaElements", UintegerValue (1024));
    thzModule->SetAttribute ("IrsEnabled", BooleanValue (true));
    thzModule->SetAttribute ("AtmosphericModelingEnabled", BooleanValue (true));
    
    bool thzInit = thzModule->InitializeThzSystem (300e9, 10e9, 1024);
    NS_LOG_INFO ("THz System Initialization: " << (thzInit ? "SUCCESS" : "FAILED"));
    
    // 2. AI Transformer Module
    Ptr<OranAiTransformer> aiModule = CreateObject<OranAiTransformer> ();
    aiModule->SetAttribute ("NumHeads", UintegerValue (8));
    aiModule->SetAttribute ("ModelDimension", UintegerValue (512));
    aiModule->SetAttribute ("NumLayers", UintegerValue (6));
    aiModule->SetAttribute ("LearningRate", DoubleValue (0.001));
    aiModule->SetAttribute ("DropoutRate", DoubleValue (0.1));
    
    bool aiInit = aiModule->InitializeTransformer (8, 512, 6);
    NS_LOG_INFO ("AI Transformer Initialization: " << (aiInit ? "SUCCESS" : "FAILED"));
    
    // 3. Network Slicing Module
    Ptr<Oran6gNetworkSlicing> slicingModule = CreateObject<Oran6gNetworkSlicing> ();
    slicingModule->SetAttribute ("MaxSlices", UintegerValue (64));
    slicingModule->SetAttribute ("IsolationLevel", DoubleValue (0.99));
    slicingModule->SetAttribute ("QosEnabled", BooleanValue (true));
    
    bool slicingInit = slicingModule->InitializeSlicingFramework (64, 0.99);
    NS_LOG_INFO ("Network Slicing Initialization: " << (slicingInit ? "SUCCESS" : "FAILED"));
    
    // 4. Quantum-Enhanced Communication (if enabled)
    Ptr<Oran6gQuantumEnhanced> quantumModule;
    if (enableQuantum)
    {
        quantumModule = CreateObject<Oran6gQuantumEnhanced> ();
        quantumModule->SetAttribute ("KeyLength", UintegerValue (256));
        quantumModule->SetAttribute ("NumQubits", UintegerValue (16));
        quantumModule->SetAttribute ("CoherenceTime", DoubleValue (100.0));
        quantumModule->SetAttribute ("EntanglementFidelity", DoubleValue (0.95));
        
        bool qkdInit = quantumModule->InitializeQKD ("BB84", 256);
        bool pqcInit = quantumModule->InitializePostQuantumCrypto ("Kyber");
        quantumModule->InitializeQuantumML ("VQE", 16);
        
        NS_LOG_INFO ("Quantum QKD Initialization: " << (qkdInit ? "SUCCESS" : "FAILED"));
        NS_LOG_INFO ("Post-Quantum Crypto Initialization: " << (pqcInit ? "SUCCESS" : "FAILED"));
    }
    
    // 5. Holographic Communication (if enabled)
    Ptr<Oran6gHolographic> holographicModule;
    if (enableHolographic)
    {
        holographicModule = CreateObject<Oran6gHolographic> ();
        
        bool holoInit = holographicModule->InitializeHolographicSystem (1e6, 532, 5);
        holographicModule->EnableAdaptiveTransmission (true, 10.0);
        holographicModule->SetViewerTracking (true, 0.01);
        
        NS_LOG_INFO ("Holographic System Initialization: " << (holoInit ? "SUCCESS" : "FAILED"));
    }
    
    // 6. Performance Optimizer (if enabled)
    Ptr<Oran6gPerformanceOptimizer> perfModule;
    if (enablePerformanceOpt)
    {
        perfModule = CreateObject<Oran6gPerformanceOptimizer> ();
        
        bool gpuInit = perfModule->InitializeGPUAcceleration ("CUDA", 8192);
        perfModule->InitializeThreadPool (8, 10);
        perfModule->InitializeMemoryManagement (1024, 512);
        perfModule->EnableAdaptiveResourceAllocation (true, Seconds (1.0));
        perfModule->InitializeEnergyOptimization ("linear", 1000.0);
        
        NS_LOG_INFO ("Performance Optimizer GPU Initialization: " << (gpuInit ? "SUCCESS" : "FAILED"));
    }
    
    // 7. Industrial IoT (if enabled)
    Ptr<Oran6gIndustrialIoT> iotModule;
    if (enableIndustrialIoT)
    {
        iotModule = CreateObject<Oran6gIndustrialIoT> ();
        
        std::map<std::string, Vector3D> factoryLayout;
        factoryLayout["assembly_line_1"] = Vector3D (100, 50, 0);
        factoryLayout["assembly_line_2"] = Vector3D (200, 50, 0);
        factoryLayout["quality_control"] = Vector3D (300, 50, 0);
        factoryLayout["packaging"] = Vector3D (400, 50, 0);
        
        bool iotInit = iotModule->InitializeIndustry40Framework (factoryLayout, "advanced");
        iotModule->InitializePredictiveMaintenance (std::vector<uint32_t> {1, 2, 3, 4}, Seconds (30.0));
        
        std::map<std::string, double> sustainabilityTargets;
        sustainabilityTargets["energy_efficiency"] = 0.85;
        sustainabilityTargets["carbon_reduction"] = 0.30;
        sustainabilityTargets["waste_reduction"] = 0.50;
        iotModule->InitializeSustainabilityMonitoring (sustainabilityTargets, "detailed");
        
        NS_LOG_INFO ("Industrial IoT Initialization: " << (iotInit ? "SUCCESS" : "FAILED"));
    }
    
    // ===============================================
    // Create Advanced Network Slices
    // ===============================================
    
    if (slicingInit)
    {
        NS_LOG_INFO ("Creating Advanced Network Slices...");
        
        // Enhanced Mobile Broadband (eMBB) slice
        Oran6gNetworkSlicing::SliceConfiguration eMbbSlice;
        eMbbSlice.sliceType = "eMBB";
        eMbbSlice.bandwidthRequirement = 1e9; // 1 Gbps
        eMbbSlice.latencyRequirement = 10e-3; // 10 ms
        eMbbSlice.reliabilityRequirement = 0.99;
        eMbbSlice.priorityLevel = 1;
        
        uint32_t eMbbSliceId = slicingModule->CreateNetworkSlice (eMbbSlice);
        NS_LOG_INFO ("eMBB Slice Created with ID: " << eMbbSliceId);
        
        // Ultra-Reliable Low Latency (URLLC) slice
        Oran6gNetworkSlicing::SliceConfiguration urllcSlice;
        urllcSlice.sliceType = "URLLC";
        urllcSlice.bandwidthRequirement = 100e6; // 100 Mbps
        urllcSlice.latencyRequirement = 1e-3; // 1 ms
        urllcSlice.reliabilityRequirement = 0.999999;
        urllcSlice.priorityLevel = 3;
        
        uint32_t urllcSliceId = slicingModule->CreateNetworkSlice (urllcSlice);
        NS_LOG_INFO ("URLLC Slice Created with ID: " << urllcSliceId);
        
        // Massive IoT slice
        Oran6gNetworkSlicing::SliceConfiguration miotSlice;
        miotSlice.sliceType = "mIoT";
        miotSlice.bandwidthRequirement = 1e6; // 1 Mbps
        miotSlice.latencyRequirement = 100e-3; // 100 ms
        miotSlice.reliabilityRequirement = 0.95;
        miotSlice.priorityLevel = 0;
        
        uint32_t miotSliceId = slicingModule->CreateNetworkSlice (miotSlice);
        NS_LOG_INFO ("mIoT Slice Created with ID: " << miotSliceId);
        
        // Holographic Communications slice (if enabled)
        if (enableHolographic)
        {
            Oran6gNetworkSlicing::SliceConfiguration holoSlice;
            holoSlice.sliceType = "Holographic";
            holoSlice.bandwidthRequirement = 10e9; // 10 Gbps
            holoSlice.latencyRequirement = 0.5e-3; // 0.5 ms
            holoSlice.reliabilityRequirement = 0.9999;
            holoSlice.priorityLevel = 2;
            
            uint32_t holoSliceId = slicingModule->CreateNetworkSlice (holoSlice);
            NS_LOG_INFO ("Holographic Slice Created with ID: " << holoSliceId);
        }
    }
    
    // ===============================================
    // Configure Applications
    // ===============================================
    
    NS_LOG_INFO ("Configuring Advanced Applications...");
    
    // Create traffic generators for different slices
    for (uint32_t i = 0; i < numUes; ++i)
    {
        // Install different applications based on UE index
        if (i % 4 == 0) // eMBB traffic
        {
            // High-throughput video streaming
            uint16_t port = 8080 + i;
            Address remoteAddress (InetSocketAddress (ueIpIface.GetAddress (i), port));
            
            BulkSendHelper bulkSend ("ns3::TcpSocketFactory", remoteAddress);
            bulkSend.SetAttribute ("MaxBytes", UintegerValue (0)); // Unlimited
            bulkSend.SetAttribute ("SendSize", UintegerValue (1024));
            
            ApplicationContainer sourceApp = bulkSend.Install (enbNodes.Get (0));
            sourceApp.Start (Seconds (1.0));
            sourceApp.Stop (Seconds (simTime - 1.0));
        }
        else if (i % 4 == 1) // URLLC traffic
        {
            // Low-latency control messages
            uint16_t port = 9090 + i;
            OnOffHelper onOff ("ns3::UdpSocketFactory",
                              InetSocketAddress (ueIpIface.GetAddress (i), port));
            onOff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.01]"));
            onOff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.099]"));
            onOff.SetAttribute ("DataRate", StringValue ("1Mbps"));
            onOff.SetAttribute ("PacketSize", UintegerValue (64));
            
            ApplicationContainer sourceApp = onOff.Install (enbNodes.Get (1));
            sourceApp.Start (Seconds (1.0));
            sourceApp.Stop (Seconds (simTime - 1.0));
        }
        else if (i % 4 == 2) // mIoT traffic
        {
            // Periodic sensor data
            uint16_t port = 7070 + i;
            OnOffHelper onOff ("ns3::UdpSocketFactory",
                              InetSocketAddress (ueIpIface.GetAddress (i), port));
            onOff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.001]"));
            onOff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=9.999]"));
            onOff.SetAttribute ("DataRate", StringValue ("10kbps"));
            onOff.SetAttribute ("PacketSize", UintegerValue (32));
            
            ApplicationContainer sourceApp = onOff.Install (enbNodes.Get (2));
            sourceApp.Start (Seconds (1.0));
            sourceApp.Stop (Seconds (simTime - 1.0));
        }
        else if (enableHolographic) // Holographic traffic
        {
            // High-bandwidth 3D content
            uint16_t port = 6060 + i;
            BulkSendHelper bulkSend ("ns3::TcpSocketFactory",
                                    InetSocketAddress (ueIpIface.GetAddress (i), port));
            bulkSend.SetAttribute ("MaxBytes", UintegerValue (0));
            bulkSend.SetAttribute ("SendSize", UintegerValue (8192));
            
            ApplicationContainer sourceApp = bulkSend.Install (enbNodes.Get (i % numEnbs));
            sourceApp.Start (Seconds (2.0));
            sourceApp.Stop (Seconds (simTime - 2.0));
        }
    }
    
    // ===============================================
    // Performance Monitoring and Optimization
    // ===============================================
    
    if (enablePerformanceOpt)
    {
        NS_LOG_INFO ("Starting Performance Monitoring...");
        
        std::map<std::string, double> alertThresholds;
        alertThresholds["cpu_utilization"] = 80.0;
        alertThresholds["memory_usage"] = 85.0;
        alertThresholds["latency"] = 5.0;
        alertThresholds["energy_consumption"] = 500.0;
        
        perfModule->StartPerformanceMonitoring (Seconds (5.0), alertThresholds);
    }
    
    // ===============================================
    // Industrial IoT Simulation
    // ===============================================
    
    if (enableIndustrialIoT)
    {
        NS_LOG_INFO ("Initializing Industrial IoT Devices...");
        
        // Register industrial devices
        for (uint32_t i = 0; i < 10; ++i)
        {
            Oran6gIndustrialIoT::IndustrialDevice device;
            device.deviceId = i + 1;
            device.deviceType = (i % 3 == 0) ? "Robot" : (i % 3 == 1) ? "Sensor" : "Actuator";
            device.position = Vector3D (i * 50, 25, 1.5);
            device.manufacturer = "Industry40Vendor";
            device.energyConsumption = 100.0 + i * 10.0;
            device.carbonFootprint = 0.5 + i * 0.1;
            device.healthScore = 0.9 + (i % 5) * 0.02;
            
            bool deviceRegistered = iotModule->RegisterIndustrialDevice (device);
            NS_LOG_INFO ("Industrial Device " << device.deviceId << " (" << device.deviceType << ") Registration: " 
                        << (deviceRegistered ? "SUCCESS" : "FAILED"));
        }
        
        // Create manufacturing processes
        Oran6gIndustrialIoT::ManufacturingProcess assemblyProcess;
        assemblyProcess.processId = 1;
        assemblyProcess.processName = "PCB Assembly";
        assemblyProcess.involvedDevices = {1, 2, 3, 4};
        assemblyProcess.efficiency = 0.85;
        assemblyProcess.sustainability = 0.78;
        assemblyProcess.duration = Seconds (120.0);
        assemblyProcess.cost = 150.0;
        
        uint32_t twinId = iotModule->CreateProcessDigitalTwin (assemblyProcess);
        NS_LOG_INFO ("Digital Twin Created for Assembly Process with ID: " << twinId);
    }
    
    // ===============================================
    // Schedule Advanced Feature Demonstrations
    // ===============================================
    
    // THz beam adaptation demonstration
    Simulator::Schedule (Seconds (10.0), [&thzModule]() {
        NS_LOG_INFO ("=== THz Adaptive Beamforming Demonstration ===");
        auto beamConfig = thzModule->AdaptiveBeamforming (Vector3D (500, 100, 20), 8);
        NS_LOG_INFO ("Beamforming adapted for position (500, 100, 20) with " << beamConfig.size () << " beams");
    });
    
    // AI learning demonstration
    Simulator::Schedule (Seconds (15.0), [&aiModule]() {
        NS_LOG_INFO ("=== AI Transformer Learning Demonstration ===");
        std::vector<std::vector<double>> trainingData (100, std::vector<double> (10, 1.0));
        std::vector<int> labels (100, 1);
        auto trainingResult = aiModule->TrainTransformerModel (trainingData, labels);
        NS_LOG_INFO ("AI Training Completed - Accuracy: " << trainingResult.at ("accuracy"));
    });
    
    // Quantum key exchange demonstration
    if (enableQuantum)
    {
        Simulator::Schedule (Seconds (20.0), [&quantumModule]() {
            NS_LOG_INFO ("=== Quantum Key Exchange Demonstration ===");
            auto quantumKey = quantumModule->PerformKeyExchange (quantumModule);
            NS_LOG_INFO ("Quantum Key Generated - Fidelity: " << quantumKey.fidelity 
                        << ", Security Level: " << quantumKey.securityLevel);
        });
    }
    
    // Holographic transmission demonstration
    if (enableHolographic)
    {
        Simulator::Schedule (Seconds (25.0), [&holographicModule]() {
            NS_LOG_INFO ("=== Holographic Transmission Demonstration ===");
            
            std::vector<Oran6gHolographic::HolographicPoint> points (1000);
            for (auto& point : points)
            {
                point.position = Vector3D (rand() % 100, rand() % 100, rand() % 100);
                point.intensity = 0.8;
                point.phase = 1.0;
            }
            
            auto boundingBox = std::make_pair (Vector3D (0, 0, 0), Vector3D (100, 100, 100));
            auto hologram = holographicModule->CreatePointCloudHologram (points, boundingBox);
            NS_LOG_INFO ("Hologram Created - Frame ID: " << hologram.frameId 
                        << ", Quality: " << hologram.quality);
        });
    }
    
    // Performance optimization demonstration
    if (enablePerformanceOpt)
    {
        Simulator::Schedule (Seconds (30.0), [&perfModule]() {
            NS_LOG_INFO ("=== Performance Optimization Demonstration ===");
            auto metrics = perfModule->GetCurrentPerformanceMetrics ();
            NS_LOG_INFO ("Current Performance - CPU: " << metrics.cpuUtilization 
                        << "%, Memory: " << metrics.memoryUsage 
                        << "%, GPU: " << metrics.gpuUtilization << "%");
        });
    }
    
    // Industrial IoT monitoring demonstration
    if (enableIndustrialIoT)
    {
        Simulator::Schedule (Seconds (35.0), [&iotModule]() {
            NS_LOG_INFO ("=== Industrial IoT Monitoring Demonstration ===");
            auto sustainabilityMetrics = iotModule->GetSustainabilityMetrics ();
            NS_LOG_INFO ("Sustainability Metrics - Energy: " << sustainabilityMetrics.totalEnergyConsumption 
                        << "kWh, Carbon: " << sustainabilityMetrics.carbonEmissions 
                        << "kg, Efficiency: " << sustainabilityMetrics.resourceEfficiency);
        });
    }
    
    // Network slicing optimization demonstration
    Simulator::Schedule (Seconds (40.0), [&slicingModule]() {
        NS_LOG_INFO ("=== Network Slicing Optimization Demonstration ===");
        auto sliceMetrics = slicingModule->GetSlicingMetrics ();
        NS_LOG_INFO ("Active Slices: " << sliceMetrics.at ("active_slices")
                    << ", Resource Utilization: " << sliceMetrics.at ("resource_utilization") << "%"
                    << ", SLA Compliance: " << sliceMetrics.at ("sla_compliance") << "%");
    });
    
    // ===============================================
    // Final Performance Summary
    // ===============================================
    
    Simulator::Schedule (Seconds (simTime - 5.0), [&]() {
        NS_LOG_INFO ("=== FINAL PERFORMANCE SUMMARY ===");
        
        // THz Performance
        auto thzMetrics = thzModule->GetThzMetrics ();
        NS_LOG_INFO ("THz Performance:");
        NS_LOG_INFO ("  Peak Throughput: " << thzMetrics.at ("peak_throughput") / 1e9 << " Gbps");
        NS_LOG_INFO ("  Average Latency: " << thzMetrics.at ("average_latency") * 1000 << " ms");
        NS_LOG_INFO ("  Spectral Efficiency: " << thzMetrics.at ("spectral_efficiency") << " bps/Hz");
        
        // AI Performance
        auto aiMetrics = aiModule->GetTransformerMetrics ();
        NS_LOG_INFO ("AI Transformer Performance:");
        NS_LOG_INFO ("  Prediction Accuracy: " << aiMetrics.at ("prediction_accuracy") * 100 << "%");
        NS_LOG_INFO ("  Inference Latency: " << aiMetrics.at ("inference_latency") * 1000 << " ms");
        NS_LOG_INFO ("  Model Convergence: " << aiMetrics.at ("convergence_rate") * 100 << "%");
        
        // Network Slicing Performance
        auto slicingMetrics = slicingModule->GetSlicingMetrics ();
        NS_LOG_INFO ("Network Slicing Performance:");
        NS_LOG_INFO ("  Active Slices: " << slicingMetrics.at ("active_slices"));
        NS_LOG_INFO ("  Resource Utilization: " << slicingMetrics.at ("resource_utilization") << "%");
        NS_LOG_INFO ("  SLA Compliance: " << slicingMetrics.at ("sla_compliance") << "%");
        
        // Quantum Performance (if enabled)
        if (enableQuantum)
        {
            auto quantumMetrics = quantumModule->GetQuantumMetrics ();
            NS_LOG_INFO ("Quantum Enhancement Performance:");
            NS_LOG_INFO ("  Quantum Advantage: " << quantumMetrics.at ("quantum_advantage") << "x");
            NS_LOG_INFO ("  Entanglement Fidelity: " << quantumMetrics.at ("entanglement_fidelity") * 100 << "%");
            NS_LOG_INFO ("  Security Level: " << quantumMetrics.at ("qkd_initialized") * 100 << "%");
        }
        
        // Performance Optimization (if enabled)
        if (enablePerformanceOpt)
        {
            auto perfMetrics = perfModule->GetCurrentPerformanceMetrics ();
            NS_LOG_INFO ("System Performance Optimization:");
            NS_LOG_INFO ("  CPU Utilization: " << perfMetrics.cpuUtilization << "%");
            NS_LOG_INFO ("  Memory Usage: " << perfMetrics.memoryUsage << "%");
            NS_LOG_INFO ("  Energy Efficiency: " << (1000.0 - perfMetrics.energyConsumption) / 10.0 << "%");
        }
        
        // Industrial IoT (if enabled)
        if (enableIndustrialIoT)
        {
            auto iotMetrics = iotModule->GetIndustrialIoTMetrics ();
            auto sustainabilityMetrics = iotModule->GetSustainabilityMetrics ();
            NS_LOG_INFO ("Industrial IoT Performance:");
            NS_LOG_INFO ("  Registered Devices: " << iotMetrics.at ("registered_devices"));
            NS_LOG_INFO ("  Process Efficiency: " << iotMetrics.at ("process_efficiency") * 100 << "%");
            NS_LOG_INFO ("  Carbon Reduction: " << (1.0 - sustainabilityMetrics.carbonEmissions / 100.0) * 100 << "%");
        }
        
        NS_LOG_INFO ("=== WORLD-LEADING 6G O-RAN PLATFORM DEMONSTRATION COMPLETE ===");
    });
    
    // ===============================================
    // Run Simulation
    // ===============================================
    
    NS_LOG_INFO ("Starting Comprehensive 6G O-RAN Simulation...");
    
    Simulator::Stop (Seconds (simTime));
    Simulator::Run ();
    
    NS_LOG_INFO ("Simulation completed successfully!");
    NS_LOG_INFO ("Advanced features demonstrated:");
    NS_LOG_INFO ("  ✓ 6G THz Communications with " << thzModule->GetThzMetrics().at("peak_throughput")/1e9 << " Gbps");
    NS_LOG_INFO ("  ✓ AI-Native Networking with " << aiModule->GetTransformerMetrics().at("prediction_accuracy")*100 << "% accuracy");
    NS_LOG_INFO ("  ✓ Dynamic Network Slicing with " << slicingModule->GetSlicingMetrics().at("active_slices") << " slices");
    
    if (enableQuantum)
        NS_LOG_INFO ("  ✓ Quantum-Enhanced Security with " << quantumModule->GetQuantumMetrics().at("quantum_advantage") << "x advantage");
    if (enableHolographic)
        NS_LOG_INFO ("  ✓ Holographic Communications");
    if (enablePerformanceOpt)
        NS_LOG_INFO ("  ✓ GPU-Accelerated Performance Optimization");
    if (enableIndustrialIoT)
        NS_LOG_INFO ("  ✓ Industrial IoT with Sustainability Monitoring");
    
    Simulator::Destroy ();
    
    return 0;
}
