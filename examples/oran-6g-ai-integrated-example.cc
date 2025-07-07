/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * O-RAN 6G Integrated Example with AI-Native Networks
 * 
 * This example demonstrates the integration of:
 * - 6G Terahertz communication
 * - AI-native transformer networks
 * - Advanced digital twin
 * - Federated learning
 * - Intelligent reflecting surfaces
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"

// O-RAN Enhanced Modules
#include "../model/oran-helper.h"
#include "../model/oran-cmm.h"
#include "../model/oran-lm-lte-2-lte-handover.h"
#include "../model/oran-data-repository-sqlite.h"
#include "../model/oran-digital-twin.h"
#include "../model/oran-6g-terahertz.h"
#include "../model/oran-ai-transformer.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Oran6gAiIntegratedExample");

/**
 * \brief Advanced 6G-AI O-RAN Network Scenario
 * 
 * This example creates a comprehensive 6G network with:
 * - THz communication links
 * - AI-native network intelligence
 * - Multi-agent reinforcement learning
 * - Federated learning across edge nodes
 * - Intelligent reflecting surfaces
 * - Digital twin real-time sync
 */
class Advanced6gOranScenario
{
public:
    Advanced6gOranScenario();
    ~Advanced6gOranScenario();

    /**
     * \brief Configure and run the 6G O-RAN scenario
     */
    void Run();

private:
    /**
     * \brief Setup 6G THz network infrastructure
     */
    void Setup6gInfrastructure();

    /**
     * \brief Deploy AI-native intelligence
     */
    void DeployAiNativeIntelligence();

    /**
     * \brief Configure advanced mobility and handovers
     */
    void ConfigureAdvancedMobility();

    /**
     * \brief Setup federated learning network
     */
    void SetupFederatedLearning();

    /**
     * \brief Deploy intelligent reflecting surfaces
     */
    void DeployIntelligentReflectingSurfaces();

    /**
     * \brief Configure real-time digital twin
     */
    void ConfigureDigitalTwin();

    /**
     * \brief Setup performance monitoring
     */
    void SetupPerformanceMonitoring();

    /**
     * \brief Callback for handover decisions
     */
    void HandoverDecisionCallback(uint32_t ueId, uint32_t sourceCell, uint32_t targetCell, std::string method);

    /**
     * \brief Callback for AI predictions
     */
    void AiPredictionCallback(OranAiTransformer::PredictionResult prediction);

    /**
     * \brief Callback for THz throughput updates
     */
    void ThzThroughputCallback(double throughput);

    /**
     * \brief Generate comprehensive results report
     */
    void GenerateResults();

    // Network components
    NodeContainer m_enbNodes;                     ///< 6G base stations
    NodeContainer m_ueNodes;                      ///< User equipment
    NodeContainer m_irsNodes;                     ///< Intelligent reflecting surfaces
    NodeContainer m_edgeNodes;                    ///< Edge computing nodes

    // LTE/6G network
    Ptr<LteHelper> m_lteHelper;
    NetDeviceContainer m_enbDevs;
    NetDeviceContainer m_ueDevs;

    // O-RAN components
    Ptr<OranHelper> m_oranHelper;
    Ptr<OranCmm> m_oranCmm;
    Ptr<OranDataRepositorySqlite> m_dataRepository;
    Ptr<OranDigitalTwin> m_digitalTwin;

    // 6G THz components
    std::vector<Ptr<Oran6gTerahertz>> m_thzModules;

    // AI components
    std::vector<Ptr<OranAiTransformer>> m_aiTransformers;

    // Performance tracking
    std::map<std::string, double> m_performanceMetrics;
    std::vector<double> m_throughputHistory;
    std::vector<double> m_latencyHistory;
    std::vector<double> m_aiAccuracyHistory;

    // Configuration
    uint32_t m_numEnbs;                           ///< Number of base stations
    uint32_t m_numUes;                            ///< Number of UEs
    uint32_t m_numIrsNodes;                       ///< Number of IRS nodes
    Time m_simulationTime;                        ///< Total simulation time
    std::string m_outputDir;                      ///< Output directory
};

Advanced6gOranScenario::Advanced6gOranScenario()
    : m_numEnbs(12),
      m_numUes(50),
      m_numIrsNodes(8),
      m_simulationTime(Seconds(600.0)),  // 10 minutes
      m_outputDir("output/6g_ai_results/")
{
    // Create output directory
    std::string createDirCmd = "mkdir -p " + m_outputDir;
    system(createDirCmd.c_str());
}

Advanced6gOranScenario::~Advanced6gOranScenario()
{
}

void
Advanced6gOranScenario::Setup6gInfrastructure()
{
    NS_LOG_FUNCTION(this);
    
    // Create nodes
    m_enbNodes.Create(m_numEnbs);
    m_ueNodes.Create(m_numUes);
    m_irsNodes.Create(m_numIrsNodes);
    m_edgeNodes.Create(4); // Edge computing nodes

    // Setup mobility for base stations (fixed positions)
    MobilityHelper enbMobility;
    enbMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    enbMobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue(0.0),
                                   "MinY", DoubleValue(0.0),
                                   "DeltaX", DoubleValue(2000.0),
                                   "DeltaY", DoubleValue(2000.0),
                                   "GridWidth", UintegerValue(4),
                                   "LayoutType", StringValue("RowFirst"));
    enbMobility.Install(m_enbNodes);

    // Setup advanced mobility for UEs (6G scenarios)
    MobilityHelper ueMobility;
    ueMobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                               "Bounds", RectangleValue(Rectangle(-1000, 7000, -1000, 5000)),
                               "Speed", StringValue("ns3::UniformRandomVariable[Min=5.0|Max=25.0]"),
                               "Direction", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=6.28]"));
    ueMobility.Install(m_ueNodes);

    // LTE/6G network setup with enhanced capabilities
    m_lteHelper = CreateObject<LteHelper>();
    
    // Configure 6G-enhanced parameters
    m_lteHelper->SetAttribute("PathlossModel", StringValue("ns3::Cost231PropagationLossModel"));
    m_lteHelper->SetAttribute("FadingModel", StringValue("ns3::TraceFadingLossModel"));
    
    // Enable carrier aggregation for 6G
    m_lteHelper->SetAttribute("UseCa", BooleanValue(true));
    m_lteHelper->SetAttribute("NumberOfComponentCarriers", UintegerValue(5));

    // Install LTE/6G devices
    m_enbDevs = m_lteHelper->InstallEnbDevice(m_enbNodes);
    m_ueDevs = m_lteHelper->InstallUeDevice(m_ueNodes);

    // Setup 6G THz modules for each base station
    for (uint32_t i = 0; i < m_numEnbs; ++i) {
        Ptr<Oran6gTerahertz> thzModule = Oran6gTerahertz::CreateTzPhyLayer(
            0.1 + (i % 3) * 0.3,  // Different THz bands: 0.1, 0.4, 0.7 THz
            20.0,                 // 20 GHz bandwidth
            2048                  // Ultra-massive MIMO with 2048 elements
        );
        
        // Configure for different scenarios
        if (i < 4) {
            // Urban macro cells - high capacity
            thzModule->ConfigureTerahertzBand(0.1, 30.0);
        } else if (i < 8) {
            // Urban micro cells - balanced
            thzModule->ConfigureTerahertzBand(0.3, 20.0);
        } else {
            // Indoor/hotspot cells - extreme capacity
            thzModule->ConfigureTerahertzBand(0.7, 50.0);
        }
        
        m_thzModules.push_back(thzModule);
        
        // Connect throughput tracing
        thzModule->TraceConnectWithoutContext("ThroughputTrace",
            MakeCallback(&Advanced6gOranScenario::ThzThroughputCallback, this));
    }

    NS_LOG_INFO("6G THz infrastructure deployed: " << m_numEnbs << " base stations with THz");
}

void
Advanced6gOranScenario::DeployAiNativeIntelligence()
{
    NS_LOG_FUNCTION(this);

    // Create AI transformers for different network functions
    std::vector<OranAiTransformer::ModelType> aiModels = {
        OranAiTransformer::NETWORK_TRANSFORMER,
        OranAiTransformer::HANDOVER_PREDICTOR,
        OranAiTransformer::RESOURCE_OPTIMIZER,
        OranAiTransformer::ANOMALY_DETECTOR,
        OranAiTransformer::TRAFFIC_FORECASTER,
        OranAiTransformer::ENERGY_OPTIMIZER
    };

    for (auto modelType : aiModels) {
        Ptr<OranAiTransformer> aiTransformer = CreateObject<OranAiTransformer>();
        
        // Configure transformer architecture based on model type
        uint32_t modelDim = 512;
        uint32_t numHeads = 8;
        uint32_t numLayers = 6;
        
        if (modelType == OranAiTransformer::NETWORK_TRANSFORMER) {
            // Largest model for general intelligence
            modelDim = 1024;
            numHeads = 16;
            numLayers = 12;
        } else if (modelType == OranAiTransformer::HANDOVER_PREDICTOR) {
            // Optimized for real-time decisions
            modelDim = 256;
            numHeads = 8;
            numLayers = 4;
        }
        
        aiTransformer->InitializeModel(modelType, modelDim, numHeads, numLayers);
        aiTransformer->ConfigureAttention(OranAiTransformer::ADAPTIVE_ATTENTION, 256);
        
        // Enable federated learning
        aiTransformer->EnableFederatedLearning(m_aiTransformers.size(), Seconds(30.0));
        
        // Set prediction callback
        aiTransformer->SetPredictionCallback(
            MakeCallback(&Advanced6gOranScenario::AiPredictionCallback, this));
        
        m_aiTransformers.push_back(aiTransformer);
    }

    NS_LOG_INFO("AI-native intelligence deployed: " << m_aiTransformers.size() << " AI models");
}

void
Advanced6gOranScenario::SetupFederatedLearning()
{
    NS_LOG_FUNCTION(this);

    // Configure federated learning network topology
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("100Gbps"));  // 6G backhaul
    p2p.SetChannelAttribute("Delay", StringValue("1ms"));

    // Connect edge nodes for federated learning
    NetDeviceContainer federatedDevices;
    for (uint32_t i = 0; i < m_edgeNodes.GetN() - 1; ++i) {
        NetDeviceContainer link = p2p.Install(m_edgeNodes.Get(i), m_edgeNodes.Get(i + 1));
        federatedDevices.Add(link);
    }

    // Setup Internet stack for federated communication
    InternetStackHelper internet;
    internet.Install(m_edgeNodes);

    Ipv4AddressHelper ipv4;
    ipv4.SetBase("192.168.1.0", "255.255.255.0");
    ipv4.Assign(federatedDevices);

    NS_LOG_INFO("Federated learning network established across " << m_edgeNodes.GetN() << " edge nodes");
}

void
Advanced6gOranScenario::DeployIntelligentReflectingSurfaces()
{
    NS_LOG_FUNCTION(this);

    // Position IRS nodes strategically
    MobilityHelper irsMobility;
    irsMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    
    Ptr<ListPositionAllocator> irsPositions = CreateObject<ListPositionAllocator>();
    
    // Deploy IRS at building corners and high-traffic areas
    irsPositions->Add(Vector(1000.0, 1000.0, 20.0));  // Building corner
    irsPositions->Add(Vector(3000.0, 1000.0, 25.0));  // High-traffic area
    irsPositions->Add(Vector(5000.0, 1000.0, 20.0));  // Junction
    irsPositions->Add(Vector(1000.0, 3000.0, 30.0));  // Mall entrance
    irsPositions->Add(Vector(3000.0, 3000.0, 25.0));  // City center
    irsPositions->Add(Vector(5000.0, 3000.0, 20.0));  // Transport hub
    irsPositions->Add(Vector(2000.0, 2000.0, 35.0));  // Stadium
    irsPositions->Add(Vector(4000.0, 2000.0, 30.0));  // Conference center
    
    irsMobility.SetPositionAllocator(irsPositions);
    irsMobility.Install(m_irsNodes);

    // Configure IRS for each THz module
    for (uint32_t i = 0; i < m_thzModules.size() && i < m_irsNodes.GetN(); ++i) {
        // Each IRS has 512-1024 reflecting elements
        uint32_t irsElements = 512 + (i * 64);
        m_thzModules[i]->EnableIntelligentReflectingSurfaces(irsElements);
    }

    NS_LOG_INFO("Intelligent Reflecting Surfaces deployed: " << m_irsNodes.GetN() << " IRS nodes");
}

void
Advanced6gOranScenario::ConfigureDigitalTwin()
{
    NS_LOG_FUNCTION(this);

    // Enhanced digital twin with real-time synchronization
    m_digitalTwin = CreateObject<OranDigitalTwin>();
    
    // Configure for 6G scenarios
    m_digitalTwin->SetAttribute("SyncInterval", TimeValue(MilliSeconds(100)));  // 100ms sync
    m_digitalTwin->SetAttribute("PredictionHorizon", TimeValue(Seconds(10.0)));
    m_digitalTwin->SetAttribute("UncertaintyThreshold", DoubleValue(0.1));
    
    // Enhanced physics-informed modeling for 6G
    m_digitalTwin->EnablePhysicsInformedModeling(true);
    m_digitalTwin->SetEnvironmentalParameters(0.6, 293.15, 101325.0);  // Humidity, temp, pressure
    
    // Connect to AI transformers for enhanced predictions
    for (auto aiTransformer : m_aiTransformers) {
        m_digitalTwin->AddAiPredictor(aiTransformer);
    }

    NS_LOG_INFO("Enhanced Digital Twin configured with real-time 6G synchronization");
}

void
Advanced6gOranScenario::ConfigureAdvancedMobility()
{
    NS_LOG_FUNCTION(this);

    // Setup O-RAN helper with enhanced 6G features
    m_oranHelper = CreateObject<OranHelper>();
    m_oranHelper->SetAttribute("Verbose", BooleanValue(true));
    m_oranHelper->SetAttribute("LmQueryInterval", TimeValue(MilliSeconds(100)));  // Faster for 6G

    // Create enhanced data repository
    m_dataRepository = CreateObject<OranDataRepositorySqlite>();
    m_dataRepository->SetAttribute("DatabaseFile", StringValue(m_outputDir + "6g_oran_data.db"));
    
    // Enhanced CMM with AI integration
    m_oranCmm = CreateObject<OranCmm>();
    
    // Create advanced handover learning module
    Ptr<OranLmLte2LteHandover> handoverLm = CreateObject<OranLmLte2LteHandover>();
    handoverLm->SetAttribute("ReinforcementLearning", BooleanValue(true));
    handoverLm->SetAttribute("DigitalTwin", BooleanValue(true));
    handoverLm->SetAttribute("LearningRate", DoubleValue(0.001));
    handoverLm->SetAttribute("ExplorationRate", DoubleValue(0.1));

    // Install O-RAN components
    m_oranHelper->Install(m_lteHelper, m_enbDevs, m_ueDevs, m_oranCmm, handoverLm, m_dataRepository);
    
    // Connect handover callback
    handoverLm->SetHandoverCallback(
        MakeCallback(&Advanced6gOranScenario::HandoverDecisionCallback, this));

    NS_LOG_INFO("Advanced 6G mobility and handover configured");
}

void
Advanced6gOranScenario::SetupPerformanceMonitoring()
{
    NS_LOG_FUNCTION(this);

    // Setup comprehensive monitoring
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
        MakeCallback(&LteUeRrc::ReportUeMeasurements));

    // Monitor throughput, latency, and energy
    Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/LteUePhy/ReportCurrentCellRsrpSinr",
        MakeCallback(&LteUePhy::ReportUeMeasurements));

    NS_LOG_INFO("Performance monitoring configured for 6G metrics");
}

void
Advanced6gOranScenario::HandoverDecisionCallback(uint32_t ueId, uint32_t sourceCell, 
                                                uint32_t targetCell, std::string method)
{
    NS_LOG_INFO("6G Handover: UE " << ueId << " from Cell " << sourceCell 
                << " to Cell " << targetCell << " using " << method);
    
    // Update performance metrics
    m_performanceMetrics["total_handovers"]++;
    if (method.find("AI") != std::string::npos || method.find("Transformer") != std::string::npos) {
        m_performanceMetrics["ai_handovers"]++;
    }
}

void
Advanced6gOranScenario::AiPredictionCallback(OranAiTransformer::PredictionResult prediction)
{
    NS_LOG_DEBUG("AI Prediction: Confidence=" << prediction.confidence[0] 
                 << ", Uncertainty=" << prediction.uncertainty);
    
    // Track AI performance
    m_aiAccuracyHistory.push_back(prediction.confidence[0]);
    m_performanceMetrics["ai_predictions"]++;
}

void
Advanced6gOranScenario::ThzThroughputCallback(double throughput)
{
    NS_LOG_DEBUG("THz Throughput: " << throughput / 1e9 << " Gbps");
    
    // Track throughput history
    m_throughputHistory.push_back(throughput);
    m_performanceMetrics["peak_throughput"] = std::max(m_performanceMetrics["peak_throughput"], throughput);
}

void
Advanced6gOranScenario::GenerateResults()
{
    NS_LOG_FUNCTION(this);

    // Generate comprehensive results report
    std::ofstream resultsFile(m_outputDir + "6g_ai_results.md");
    
    resultsFile << "# 6G AI-Native O-RAN Simulation Results\n\n";
    resultsFile << "## Executive Summary\n\n";
    resultsFile << "**Simulation Date:** " << Simulator::Now().GetSeconds() << " seconds\n";
    resultsFile << "**Network Configuration:** " << m_numEnbs << " 6G base stations, " 
                << m_numUes << " UEs\n";
    resultsFile << "**AI Models Deployed:** " << m_aiTransformers.size() << " transformer models\n";
    resultsFile << "**THz Modules:** " << m_thzModules.size() << " active THz links\n\n";

    // Performance metrics
    resultsFile << "## Performance Achievements\n\n";
    resultsFile << "### 6G THz Communication\n";
    if (!m_throughputHistory.empty()) {
        double avgThroughput = 0.0;
        for (double tp : m_throughputHistory) avgThroughput += tp;
        avgThroughput /= m_throughputHistory.size();
        
        resultsFile << "- **Average Throughput:** " << avgThroughput / 1e9 << " Gbps\n";
        resultsFile << "- **Peak Throughput:** " << m_performanceMetrics["peak_throughput"] / 1e9 << " Gbps\n";
    }

    resultsFile << "\n### AI-Native Intelligence\n";
    if (!m_aiAccuracyHistory.empty()) {
        double avgAccuracy = 0.0;
        for (double acc : m_aiAccuracyHistory) avgAccuracy += acc;
        avgAccuracy /= m_aiAccuracyHistory.size();
        
        resultsFile << "- **AI Prediction Accuracy:** " << avgAccuracy * 100.0 << "%\n";
        resultsFile << "- **Total AI Predictions:** " << m_performanceMetrics["ai_predictions"] << "\n";
    }

    resultsFile << "\n### Handover Performance\n";
    resultsFile << "- **Total Handovers:** " << m_performanceMetrics["total_handovers"] << "\n";
    resultsFile << "- **AI-Enhanced Handovers:** " << m_performanceMetrics["ai_handovers"] << "\n";
    
    if (m_performanceMetrics["total_handovers"] > 0) {
        double aiRatio = m_performanceMetrics["ai_handovers"] / m_performanceMetrics["total_handovers"];
        resultsFile << "- **AI Enhancement Ratio:** " << aiRatio * 100.0 << "%\n";
    }

    resultsFile << "\n## Next Steps\n\n";
    resultsFile << "1. **Advanced Beamforming:** Implement AI-driven beamforming optimization\n";
    resultsFile << "2. **Network Slicing:** Deploy dynamic 6G network slicing\n";
    resultsFile << "3. **Edge Computing:** Integrate MEC with THz fronthaul\n";
    resultsFile << "4. **Sustainability:** Add energy optimization algorithms\n";
    resultsFile << "5. **Security:** Implement quantum-safe communication\n";

    resultsFile.close();

    NS_LOG_INFO("Comprehensive 6G AI results generated in: " << m_outputDir);
}

void
Advanced6gOranScenario::Run()
{
    NS_LOG_FUNCTION(this);

    NS_LOG_INFO("Starting Advanced 6G AI-Native O-RAN Simulation...");

    // Initialize performance metrics
    m_performanceMetrics["total_handovers"] = 0;
    m_performanceMetrics["ai_handovers"] = 0;
    m_performanceMetrics["ai_predictions"] = 0;
    m_performanceMetrics["peak_throughput"] = 0;

    // Setup simulation components
    Setup6gInfrastructure();
    DeployAiNativeIntelligence();
    SetupFederatedLearning();
    DeployIntelligentReflectingSurfaces();
    ConfigureDigitalTwin();
    ConfigureAdvancedMobility();
    SetupPerformanceMonitoring();

    NS_LOG_INFO("All 6G AI components deployed successfully");

    // Start applications for traffic generation
    uint16_t dlPort = 1100;
    uint16_t ulPort = 2000;

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(m_ueNodes);

    // Assign IP addresses
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("7.0.0.0", "255.0.0.0");
    Ipv4InterfaceContainer ueIpIface = m_lteHelper->AssignUeIpv4Address(NetDeviceContainer(m_ueDevs));

    // Attach UEs to eNBs
    m_lteHelper->Attach(m_ueDevs);

    NS_LOG_INFO("Starting simulation for " << m_simulationTime.GetSeconds() << " seconds");

    // Run simulation
    Simulator::Stop(m_simulationTime);
    Simulator::Run();

    // Generate results
    GenerateResults();

    Simulator::Destroy();

    NS_LOG_INFO("6G AI-Native O-RAN Simulation completed successfully!");
}

int
main(int argc, char *argv[])
{
    // Configure logging
    LogComponentEnable("Oran6gAiIntegratedExample", LOG_LEVEL_INFO);
    LogComponentEnable("OranHelper", LOG_LEVEL_INFO);
    LogComponentEnable("Oran6gTerahertz", LOG_LEVEL_INFO);
    LogComponentEnable("OranAiTransformer", LOG_LEVEL_INFO);

    // Create and run scenario
    Advanced6gOranScenario scenario;
    scenario.Run();

    return 0;
}
