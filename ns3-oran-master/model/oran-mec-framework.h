/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 * Multi-access Edge Computing (MEC) Framework for O-RAN
 * 
 * This module implements comprehensive MEC capabilities including:
 * - Edge application orchestration
 * - Service discovery and load balancing
 * - Edge-cloud continuum optimization
 * - Federated learning at the edge
 * - Low-latency service delivery
 */

#ifndef ORAN_MEC_FRAMEWORK_H
#define ORAN_MEC_FRAMEWORK_H

#include "ns3/object.h"
#include "ns3/node.h"
#include "ns3/application.h"
#include "ns3/traced-value.h"
#include "ns3/callback.h"
#include "oran-data-repository.h"
#include <map>
#include <vector>
#include <queue>
#include <memory>
#include <functional>

namespace ns3 {

/**
 * Edge service types
 */
enum class EdgeServiceType {
    AUGMENTED_REALITY = 0,
    VIRTUAL_REALITY = 1,
    AUTONOMOUS_VEHICLE = 2,
    INDUSTRIAL_IOT = 3,
    VIDEO_ANALYTICS = 4,
    GAMING = 5,
    HEALTHCARE = 6,
    SMART_CITY = 7,
    FEDERATED_LEARNING = 8
};

/**
 * Edge service requirements
 */
struct EdgeServiceRequirements {
    double maxLatency;           // Maximum acceptable latency (ms)
    double minBandwidth;         // Minimum bandwidth requirement (Mbps)
    double cpuRequirement;       // CPU requirement (cores)
    double memoryRequirement;    // Memory requirement (GB)
    double storageRequirement;   // Storage requirement (GB)
    bool gpuRequired;           // GPU acceleration required
    std::string serviceLevel;   // Service level agreement
    std::vector<std::string> dependencies; // Service dependencies
};

/**
 * Edge node capabilities
 */
struct EdgeNodeCapabilities {
    double cpuCapacity;         // Available CPU cores
    double memoryCapacity;      // Available memory (GB)
    double storageCapacity;     // Available storage (GB)
    bool gpuAvailable;          // GPU availability
    double networkBandwidth;    // Network bandwidth (Mbps)
    Vector3D location;          // Physical location
    std::vector<EdgeServiceType> supportedServices;
};

/**
 * Service instance
 */
struct EdgeServiceInstance {
    std::string serviceId;
    std::string instanceId;
    EdgeServiceType type;
    uint32_t nodeId;
    EdgeServiceRequirements requirements;
    Time creationTime;
    Time lastAccessTime;
    double currentLoad;
    std::map<std::string, double> metrics;
};

/**
 * Workload prediction
 */
struct WorkloadPrediction {
    Time predictionTime;
    std::map<EdgeServiceType, double> expectedLoad;
    std::map<uint32_t, double> nodeUtilization;
    double confidence;
};

/**
 * Edge application base class
 */
class EdgeApplication : public Application
{
public:
    static TypeId GetTypeId();
    
    EdgeApplication();
    ~EdgeApplication() override = default;
    
    /**
     * Set service requirements
     */
    void SetServiceRequirements(const EdgeServiceRequirements& requirements);
    
    /**
     * Get service requirements
     */
    EdgeServiceRequirements GetServiceRequirements() const;
    
    /**
     * Set service type
     */
    void SetServiceType(EdgeServiceType type);
    
    /**
     * Get current metrics
     */
    virtual std::map<std::string, double> GetMetrics() const;
    
    /**
     * Handle service migration
     */
    virtual void MigrateToNode(uint32_t targetNodeId);
    
    /**
     * Process edge request
     */
    virtual void ProcessEdgeRequest(const std::string& request) = 0;

protected:
    void StartApplication() override;
    void StopApplication() override;

private:
    EdgeServiceType m_serviceType;
    EdgeServiceRequirements m_requirements;
    std::map<std::string, double> m_metrics;
};

/**
 * Edge orchestrator for service management
 */
class EdgeOrchestrator : public Object
{
public:
    static TypeId GetTypeId();
    
    EdgeOrchestrator();
    ~EdgeOrchestrator() override = default;
    
    /**
     * Register edge node
     */
    void RegisterEdgeNode(uint32_t nodeId, const EdgeNodeCapabilities& capabilities);
    
    /**
     * Deploy service
     */
    std::string DeployService(const EdgeServiceRequirements& requirements, 
                             EdgeServiceType type);
    
    /**
     * Scale service
     */
    void ScaleService(const std::string& serviceId, uint32_t instances);
    
    /**
     * Migrate service
     */
    void MigrateService(const std::string& serviceId, uint32_t targetNodeId);
    
    /**
     * Terminate service
     */
    void TerminateService(const std::string& serviceId);
    
    /**
     * Get optimal node for service
     */
    uint32_t GetOptimalNode(const EdgeServiceRequirements& requirements, 
                           EdgeServiceType type);
    
    /**
     * Load balancing
     */
    void PerformLoadBalancing();
    
    /**
     * Resource monitoring
     */
    std::map<uint32_t, double> GetResourceUtilization();

private:
    std::map<uint32_t, EdgeNodeCapabilities> m_edgeNodes;
    std::map<std::string, EdgeServiceInstance> m_services;
    std::map<uint32_t, std::vector<std::string>> m_nodeServices;
    
    /**
     * Calculate placement score
     */
    double CalculatePlacementScore(uint32_t nodeId, 
                                  const EdgeServiceRequirements& requirements);
};

/**
 * Federated Learning Manager for edge ML
 */
class FederatedLearningManager : public Object
{
public:
    static TypeId GetTypeId();
    
    FederatedLearningManager();
    ~FederatedLearningManager() override = default;
    
    /**
     * Initialize federated learning round
     */
    void InitializeLearningRound(const std::string& modelType);
    
    /**
     * Register edge node for federated learning
     */
    void RegisterLearningNode(uint32_t nodeId, double dataSize);
    
    /**
     * Collect local model updates
     */
    void CollectModelUpdate(uint32_t nodeId, const std::vector<double>& modelWeights);
    
    /**
     * Aggregate model updates
     */
    std::vector<double> AggregateModels();
    
    /**
     * Distribute global model
     */
    void DistributeGlobalModel(const std::vector<double>& globalModel);
    
    /**
     * Set aggregation algorithm
     */
    void SetAggregationAlgorithm(const std::string& algorithm);

private:
    struct LearningNode {
        uint32_t nodeId;
        double dataSize;
        std::vector<double> modelWeights;
        bool updateReceived;
    };
    
    std::string m_modelType;
    std::string m_aggregationAlgorithm;
    std::vector<LearningNode> m_learningNodes;
    std::vector<double> m_globalModel;
    uint32_t m_currentRound;
};

/**
 * Comprehensive MEC Framework
 * 
 * This class provides a complete Multi-access Edge Computing framework
 * with advanced features:
 * - Intelligent service placement and orchestration
 * - Real-time load balancing and auto-scaling
 * - Federated learning coordination
 * - Edge-cloud continuum optimization
 * - Service mesh integration
 * - Performance monitoring and SLA enforcement
 */
class OranMecFramework : public Object
{
public:
    static TypeId GetTypeId();
    
    OranMecFramework();
    ~OranMecFramework() override;
    
    /**
     * Initialize MEC framework
     */
    void Initialize();
    
    /**
     * Start MEC operations
     */
    void Start();
    
    /**
     * Stop MEC operations
     */
    void Stop();
    
    /**
     * Register edge node
     */
    void RegisterEdgeNode(Ptr<Node> node, const EdgeNodeCapabilities& capabilities);
    
    /**
     * Deploy edge service
     */
    std::string DeployEdgeService(const EdgeServiceRequirements& requirements,
                                 EdgeServiceType type,
                                 Ptr<EdgeApplication> application);
    
    /**
     * Enable service discovery
     */
    void EnableServiceDiscovery(bool enable);
    
    /**
     * Configure load balancing strategy
     */
    void SetLoadBalancingStrategy(const std::string& strategy);
    
    /**
     * Enable auto-scaling
     */
    void EnableAutoScaling(bool enable, double threshold = 0.8);
    
    /**
     * Configure federated learning
     */
    void ConfigureFederatedLearning(const std::string& modelType,
                                   const std::string& aggregationAlgorithm);
    
    /**
     * Start federated learning round
     */
    void StartFederatedLearningRound();
    
    /**
     * Predict workload demand
     */
    WorkloadPrediction PredictWorkload(Time horizon);
    
    /**
     * Optimize resource allocation
     */
    void OptimizeResourceAllocation();
    
    /**
     * Monitor SLA compliance
     */
    std::map<std::string, bool> CheckSlaCompliance();
    
    /**
     * Get edge analytics
     */
    std::map<std::string, double> GetEdgeAnalytics();
    
    /**
     * Set data repository
     */
    void SetDataRepository(Ptr<OranDataRepository> repository);
    
    /**
     * Configure edge-cloud integration
     */
    void ConfigureCloudIntegration(const std::string& cloudEndpoint,
                                  const std::string& hybridStrategy);
    
    /**
     * Enable container orchestration
     */
    void EnableContainerOrchestration(bool enable);
    
    /**
     * Set service mesh configuration
     */
    void ConfigureServiceMesh(const std::string& meshType);

protected:
    void DoDispose() override;

private:
    /**
     * Periodic monitoring and optimization
     */
    void PeriodicOptimization();
    
    /**
     * Service discovery management
     */
    void UpdateServiceRegistry();
    
    /**
     * Auto-scaling logic
     */
    void PerformAutoScaling();
    
    /**
     * SLA monitoring
     */
    void MonitorSla();
    
    /**
     * Workload prediction logic
     */
    void UpdateWorkloadPrediction();
    
    /**
     * Edge-cloud decision making
     */
    uint32_t DecideEdgeOrCloud(const EdgeServiceRequirements& requirements);
    
    /**
     * Container lifecycle management
     */
    void ManageContainers();
    
    /**
     * Service mesh traffic management
     */
    void ManageServiceMeshTraffic();

    // Core components
    Ptr<EdgeOrchestrator> m_orchestrator;           ///< Service orchestrator
    Ptr<FederatedLearningManager> m_flManager;      ///< Federated learning manager
    Ptr<OranDataRepository> m_dataRepository;       ///< Data repository
    
    // Node and service management
    std::map<uint32_t, Ptr<Node>> m_edgeNodes;
    std::map<uint32_t, EdgeNodeCapabilities> m_nodeCapabilities;
    std::map<std::string, Ptr<EdgeApplication>> m_deployedServices;
    std::map<EdgeServiceType, std::vector<std::string>> m_serviceRegistry;
    
    // Configuration
    bool m_serviceDiscoveryEnabled;                  ///< Service discovery enabled
    bool m_autoScalingEnabled;                       ///< Auto-scaling enabled
    bool m_containerOrchestrationEnabled;            ///< Container orchestration
    std::string m_loadBalancingStrategy;             ///< Load balancing strategy
    std::string m_cloudEndpoint;                     ///< Cloud integration endpoint
    std::string m_hybridStrategy;                    ///< Hybrid edge-cloud strategy
    std::string m_serviceMeshType;                   ///< Service mesh type
    
    // Thresholds and parameters
    double m_autoScalingThreshold;                   ///< Auto-scaling threshold
    Time m_monitoringInterval;                       ///< Monitoring interval
    uint32_t m_maxServicesPerNode;                   ///< Max services per node
    
    // Prediction and analytics
    std::vector<WorkloadPrediction> m_workloadHistory;
    std::map<std::string, std::vector<double>> m_performanceHistory;
    
    // Events
    EventId m_optimizationEvent;                     ///< Periodic optimization event
    EventId m_monitoringEvent;                       ///< Monitoring event
    
    // Traced values for monitoring
    TracedValue<uint32_t> m_activeServices;          ///< Active services count
    TracedValue<double> m_averageLatency;            ///< Average service latency
    TracedValue<double> m_resourceUtilization;       ///< Average resource utilization
    TracedValue<uint32_t> m_slaViolations;           ///< SLA violations count
    TracedValue<uint32_t> m_serviceMigrations;       ///< Service migrations count
    
    // Federated learning state
    bool m_federatedLearningActive;                  ///< FL session active
    uint32_t m_currentFLRound;                       ///< Current FL round
    
    // Container and service mesh state
    std::map<std::string, std::string> m_containerRegistry;
    std::map<std::string, std::vector<std::string>> m_serviceMeshRoutes;
    
    // Performance counters
    std::map<std::string, uint64_t> m_performanceCounters;
};

/**
 * Specialized edge applications
 */

/**
 * AR/VR Edge Application
 */
class ArVrEdgeApplication : public EdgeApplication
{
public:
    static TypeId GetTypeId();
    
    ArVrEdgeApplication();
    ~ArVrEdgeApplication() override = default;
    
    void ProcessEdgeRequest(const std::string& request) override;
    std::map<std::string, double> GetMetrics() const override;

private:
    double m_frameRate;
    double m_motionToPhotonLatency;
    uint32_t m_renderingQuality;
};

/**
 * Autonomous Vehicle Edge Application
 */
class AutonomousVehicleEdgeApplication : public EdgeApplication
{
public:
    static TypeId GetTypeId();
    
    AutonomousVehicleEdgeApplication();
    ~AutonomousVehicleEdgeApplication() override = default;
    
    void ProcessEdgeRequest(const std::string& request) override;
    std::map<std::string, double> GetMetrics() const override;

private:
    double m_decisionLatency;
    double m_sensorFusionAccuracy;
    uint32_t m_safetyLevel;
};

/**
 * Video Analytics Edge Application
 */
class VideoAnalyticsEdgeApplication : public EdgeApplication
{
public:
    static TypeId GetTypeId();
    
    VideoAnalyticsEdgeApplication();
    ~VideoAnalyticsEdgeApplication() override = default;
    
    void ProcessEdgeRequest(const std::string& request) override;
    std::map<std::string, double> GetMetrics() const override;

private:
    double m_detectionAccuracy;
    double m_processingLatency;
    uint32_t m_videoStreams;
};

} // namespace ns3

#endif /* ORAN_MEC_FRAMEWORK_H */
