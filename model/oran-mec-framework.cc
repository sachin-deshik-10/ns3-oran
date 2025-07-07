/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 * Multi-access Edge Computing (MEC) Framework for O-RAN - Implementation
 */

#include "oran-mec-framework.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("OranMecFramework");
    NS_OBJECT_ENSURE_REGISTERED(OranMecFramework);

    TypeId
    OranMecFramework::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::OranMecFramework")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<OranMecFramework>()
                                .AddAttribute("MaxServices",
                                              "Maximum number of edge services",
                                              UintegerValue(1000),
                                              MakeUintegerAccessor(&OranMecFramework::m_maxServices),
                                              MakeUintegerChecker<uint32_t>())
                                .AddAttribute("EnableFederatedLearning",
                                              "Enable federated learning capabilities",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&OranMecFramework::m_enableFederatedLearning),
                                              MakeBooleanChecker())
                                .AddAttribute("ServiceDiscoveryInterval",
                                              "Interval for service discovery updates",
                                              TimeValue(Seconds(5.0)),
                                              MakeTimeAccessor(&OranMecFramework::m_serviceDiscoveryInterval),
                                              MakeTimeChecker())
                                .AddAttribute("LoadBalancingThreshold",
                                              "Load balancing threshold (0-1)",
                                              DoubleValue(0.8),
                                              MakeDoubleAccessor(&OranMecFramework::m_loadBalancingThreshold),
                                              MakeDoubleChecker<double>(0.0, 1.0))
                                .AddTraceSource("ServiceDeployed",
                                                "A service was deployed to edge node",
                                                MakeTraceSourceAccessor(&OranMecFramework::m_serviceDeployedTrace),
                                                "ns3::OranMecFramework::ServiceDeployedTracedCallback")
                                .AddTraceSource("ServiceMigrated",
                                                "A service was migrated between edge nodes",
                                                MakeTraceSourceAccessor(&OranMecFramework::m_serviceMigratedTrace),
                                                "ns3::OranMecFramework::ServiceMigratedTracedCallback")
                                .AddTraceSource("LoadBalanced",
                                                "Load balancing occurred",
                                                MakeTraceSourceAccessor(&OranMecFramework::m_loadBalancedTrace),
                                                "ns3::OranMecFramework::LoadBalancedTracedCallback");
        return tid;
    }

    OranMecFramework::OranMecFramework()
        : m_maxServices(1000),
          m_enableFederatedLearning(true),
          m_serviceDiscoveryInterval(Seconds(5.0)),
          m_loadBalancingThreshold(0.8),
          m_serviceCounter(0),
          m_federatedRounds(0)
    {
        NS_LOG_FUNCTION(this);
        InitializeEdgeNodes();
        ScheduleServiceDiscovery();
    }

    OranMecFramework::~OranMecFramework()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    OranMecFramework::InitializeEdgeNodes()
    {
        NS_LOG_FUNCTION(this);

        // Initialize default edge nodes with different capabilities
        EdgeNodeCapabilities highPerf;
        highPerf.cpuCores = 32;
        highPerf.memoryGB = 128;
        highPerf.storageGB = 2000;
        highPerf.gpuCores = 8;
        highPerf.networkBandwidthMbps = 10000;
        highPerf.latencyMs = 1.0;

        EdgeNodeCapabilities mediumPerf;
        mediumPerf.cpuCores = 16;
        mediumPerf.memoryGB = 64;
        mediumPerf.storageGB = 1000;
        mediumPerf.gpuCores = 4;
        mediumPerf.networkBandwidthMbps = 5000;
        mediumPerf.latencyMs = 2.0;

        EdgeNodeCapabilities lowPerf;
        lowPerf.cpuCores = 8;
        lowPerf.memoryGB = 32;
        lowPerf.storageGB = 500;
        lowPerf.gpuCores = 2;
        lowPerf.networkBandwidthMbps = 1000;
        lowPerf.latencyMs = 5.0;

        // Register default edge nodes
        RegisterEdgeNode("edge-node-1", highPerf);
        RegisterEdgeNode("edge-node-2", mediumPerf);
        RegisterEdgeNode("edge-node-3", lowPerf);
    }

    void
    OranMecFramework::RegisterEdgeNode(const std::string &nodeId, const EdgeNodeCapabilities &capabilities)
    {
        NS_LOG_FUNCTION(this << nodeId);

        EdgeNodeInfo nodeInfo;
        nodeInfo.nodeId = nodeId;
        nodeInfo.capabilities = capabilities;
        nodeInfo.currentCpuUsage = 0.0;
        nodeInfo.currentMemoryUsage = 0.0;
        nodeInfo.currentStorageUsage = 0.0;
        nodeInfo.isActive = true;
        nodeInfo.lastHeartbeat = Simulator::Now();

        m_edgeNodes[nodeId] = nodeInfo;

        NS_LOG_INFO("Registered edge node: " << nodeId << " with " << capabilities.cpuCores << " CPU cores, " << capabilities.memoryGB << " GB memory");
    }

    std::string
    OranMecFramework::DeployService(const EdgeServiceRequirements &requirements)
    {
        NS_LOG_FUNCTION(this);

        // Find best edge node for service deployment
        std::string bestNode = SelectOptimalEdgeNode(requirements);
        if (bestNode.empty())
        {
            NS_LOG_WARN("No suitable edge node found for service deployment");
            return "";
        }

        // Create service instance
        EdgeServiceInstance service;
        service.serviceId = "service-" + std::to_string(++m_serviceCounter);
        service.requirements = requirements;
        service.deployedNode = bestNode;
        service.status = EdgeServiceStatus::RUNNING;
        service.deploymentTime = Simulator::Now();
        service.lastUpdate = Simulator::Now();

        // Update node resource usage
        auto &nodeInfo = m_edgeNodes[bestNode];
        nodeInfo.currentCpuUsage += requirements.cpuCores;
        nodeInfo.currentMemoryUsage += requirements.memoryMB / 1024.0; // Convert to GB
        nodeInfo.currentStorageUsage += requirements.storageMB / 1024.0;

        // Store service
        m_deployedServices[service.serviceId] = service;
        m_nodeServices[bestNode].push_back(service.serviceId);

        // Trigger trace
        m_serviceDeployedTrace(service.serviceId, bestNode, requirements.serviceType);

        NS_LOG_INFO("Deployed service " << service.serviceId << " to node " << bestNode);

        return service.serviceId;
    }

    std::string
    OranMecFramework::SelectOptimalEdgeNode(const EdgeServiceRequirements &requirements)
    {
        NS_LOG_FUNCTION(this);

        double bestScore = -1.0;
        std::string bestNode = "";

        for (const auto &[nodeId, nodeInfo] : m_edgeNodes)
        {
            if (!nodeInfo.isActive)
                continue;

            // Check if node can satisfy requirements
            if (!CanNodeSatisfyRequirements(nodeInfo, requirements))
                continue;

            // Calculate fitness score
            double score = CalculateNodeFitness(nodeInfo, requirements);

            if (score > bestScore)
            {
                bestScore = score;
                bestNode = nodeId;
            }
        }

        return bestNode;
    }

    bool
    OranMecFramework::CanNodeSatisfyRequirements(const EdgeNodeInfo &nodeInfo,
                                                 const EdgeServiceRequirements &requirements)
    {
        // Check resource availability
        double availableCpu = nodeInfo.capabilities.cpuCores - nodeInfo.currentCpuUsage;
        double availableMemory = nodeInfo.capabilities.memoryGB - nodeInfo.currentMemoryUsage;
        double availableStorage = nodeInfo.capabilities.storageGB - nodeInfo.currentStorageUsage;

        if (availableCpu < requirements.cpuCores ||
            availableMemory < (requirements.memoryMB / 1024.0) ||
            availableStorage < (requirements.storageMB / 1024.0))
        {
            return false;
        }

        // Check latency requirement
        if (nodeInfo.capabilities.latencyMs > requirements.maxLatencyMs)
        {
            return false;
        }

        // Check bandwidth requirement
        if (nodeInfo.capabilities.networkBandwidthMbps < requirements.bandwidthMbps)
        {
            return false;
        }

        return true;
    }

    double
    OranMecFramework::CalculateNodeFitness(const EdgeNodeInfo &nodeInfo,
                                           const EdgeServiceRequirements &requirements)
    {
        // Multi-criteria optimization considering:
        // 1. Resource utilization efficiency
        // 2. Latency optimization
        // 3. Load balancing
        // 4. Energy efficiency

        double cpuUtilization = nodeInfo.currentCpuUsage / nodeInfo.capabilities.cpuCores;
        double memoryUtilization = nodeInfo.currentMemoryUsage / nodeInfo.capabilities.memoryGB;

        // Prefer moderate utilization (not too low, not too high)
        double utilizationScore = 1.0 - std::abs(0.6 - (cpuUtilization + memoryUtilization) / 2.0);

        // Latency score (lower is better)
        double latencyScore = 1.0 - (nodeInfo.capabilities.latencyMs / 100.0);

        // Bandwidth score
        double bandwidthScore = std::min(1.0, nodeInfo.capabilities.networkBandwidthMbps / 10000.0);

        // Combined fitness score
        double fitness = 0.4 * utilizationScore + 0.4 * latencyScore + 0.2 * bandwidthScore;

        return std::max(0.0, std::min(1.0, fitness));
    }

    void
    OranMecFramework::MigrateService(const std::string &serviceId, const std::string &targetNodeId)
    {
        NS_LOG_FUNCTION(this << serviceId << targetNodeId);

        auto serviceIt = m_deployedServices.find(serviceId);
        if (serviceIt == m_deployedServices.end())
        {
            NS_LOG_WARN("Service not found: " << serviceId);
            return;
        }

        auto &service = serviceIt->second;
        std::string sourceNodeId = service.deployedNode;

        // Check if target node can accommodate the service
        auto targetNodeIt = m_edgeNodes.find(targetNodeId);
        if (targetNodeIt == m_edgeNodes.end() || !targetNodeIt->second.isActive)
        {
            NS_LOG_WARN("Target node not available: " << targetNodeId);
            return;
        }

        if (!CanNodeSatisfyRequirements(targetNodeIt->second, service.requirements))
        {
            NS_LOG_WARN("Target node cannot satisfy service requirements");
            return;
        }

        // Perform migration
        // 1. Release resources from source node
        auto &sourceNode = m_edgeNodes[sourceNodeId];
        sourceNode.currentCpuUsage -= service.requirements.cpuCores;
        sourceNode.currentMemoryUsage -= service.requirements.memoryMB / 1024.0;
        sourceNode.currentStorageUsage -= service.requirements.storageMB / 1024.0;

        // 2. Allocate resources on target node
        auto &targetNode = m_edgeNodes[targetNodeId];
        targetNode.currentCpuUsage += service.requirements.cpuCores;
        targetNode.currentMemoryUsage += service.requirements.memoryMB / 1024.0;
        targetNode.currentStorageUsage += service.requirements.storageMB / 1024.0;

        // 3. Update service information
        service.deployedNode = targetNodeId;
        service.lastUpdate = Simulator::Now();

        // 4. Update node service lists
        auto &sourceServices = m_nodeServices[sourceNodeId];
        sourceServices.erase(std::remove(sourceServices.begin(), sourceServices.end(), serviceId),
                             sourceServices.end());
        m_nodeServices[targetNodeId].push_back(serviceId);

        // Trigger trace
        m_serviceMigratedTrace(serviceId, sourceNodeId, targetNodeId);

        NS_LOG_INFO("Migrated service " << serviceId << " from " << sourceNodeId << " to " << targetNodeId);
    }

    void
    OranMecFramework::PerformLoadBalancing()
    {
        NS_LOG_FUNCTION(this);

        // Identify overloaded nodes
        std::vector<std::string> overloadedNodes;
        std::vector<std::string> underloadedNodes;

        for (const auto &[nodeId, nodeInfo] : m_edgeNodes)
        {
            if (!nodeInfo.isActive)
                continue;

            double cpuUtilization = nodeInfo.currentCpuUsage / nodeInfo.capabilities.cpuCores;
            double memoryUtilization = nodeInfo.currentMemoryUsage / nodeInfo.capabilities.memoryGB;
            double avgUtilization = (cpuUtilization + memoryUtilization) / 2.0;

            if (avgUtilization > m_loadBalancingThreshold)
            {
                overloadedNodes.push_back(nodeId);
            }
            else if (avgUtilization < 0.3) // Consider nodes with <30% utilization as underloaded
            {
                underloadedNodes.push_back(nodeId);
            }
        }

        // Migrate services from overloaded to underloaded nodes
        for (const std::string &overloadedNode : overloadedNodes)
        {
            if (underloadedNodes.empty())
                break;

            auto &services = m_nodeServices[overloadedNode];
            if (services.empty())
                continue;

            // Find a service to migrate (prefer smaller services first)
            std::string serviceToMigrate = "";
            double minResourceUsage = std::numeric_limits<double>::max();

            for (const std::string &serviceId : services)
            {
                const auto &service = m_deployedServices[serviceId];
                double resourceUsage = service.requirements.cpuCores +
                                       (service.requirements.memoryMB / 1024.0);

                if (resourceUsage < minResourceUsage)
                {
                    minResourceUsage = resourceUsage;
                    serviceToMigrate = serviceId;
                }
            }

            if (!serviceToMigrate.empty() && !underloadedNodes.empty())
            {
                // Select best underloaded node
                std::string targetNode = underloadedNodes[0]; // Simple selection for now
                MigrateService(serviceToMigrate, targetNode);

                m_loadBalancedTrace(overloadedNode, targetNode, serviceToMigrate);
            }
        }
    }

    void
    OranMecFramework::UpdateFederatedLearning()
    {
        NS_LOG_FUNCTION(this);

        if (!m_enableFederatedLearning)
            return;

        m_federatedRounds++;

        // Simulate federated learning round
        std::vector<std::string> participatingNodes;

        for (const auto &[nodeId, nodeInfo] : m_edgeNodes)
        {
            if (nodeInfo.isActive && HasMLCapabilities(nodeInfo))
            {
                participatingNodes.push_back(nodeId);
            }
        }

        if (participatingNodes.size() >= 2)
        {
            // Simulate model aggregation
            FederatedLearningRound round;
            round.roundId = m_federatedRounds;
            round.participatingNodes = participatingNodes;
            round.timestamp = Simulator::Now();
            round.modelAccuracy = SimulateModelTraining(participatingNodes);

            m_federatedRounds_history.push_back(round);

            NS_LOG_INFO("Completed federated learning round " << m_federatedRounds << " with " << participatingNodes.size() << " nodes, accuracy: " << round.modelAccuracy);
        }
    }

    bool
    OranMecFramework::HasMLCapabilities(const EdgeNodeInfo &nodeInfo)
    {
        // Consider nodes with GPU cores and sufficient resources as ML-capable
        return nodeInfo.capabilities.gpuCores > 0 &&
               nodeInfo.capabilities.cpuCores >= 8 &&
               nodeInfo.capabilities.memoryGB >= 16;
    }

    double
    OranMecFramework::SimulateModelTraining(const std::vector<std::string> &nodes)
    {
        // Simulate federated learning accuracy improvement
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.85, 0.98);

        // Base accuracy improves with more participating nodes
        double baseAccuracy = 0.7 + (nodes.size() * 0.05);
        double noiseAccuracy = dis(gen);

        return std::min(0.99, (baseAccuracy + noiseAccuracy) / 2.0);
    }

    void
    OranMecFramework::ScheduleServiceDiscovery()
    {
        NS_LOG_FUNCTION(this);

        // Update service discovery
        UpdateServiceDiscovery();

        // Perform load balancing
        PerformLoadBalancing();

        // Update federated learning
        UpdateFederatedLearning();

        // Schedule next update
        Simulator::Schedule(m_serviceDiscoveryInterval,
                            &OranMecFramework::ScheduleServiceDiscovery, this);
    }

    void
    OranMecFramework::UpdateServiceDiscovery()
    {
        NS_LOG_FUNCTION(this);

        // Update heartbeats and check node availability
        Time currentTime = Simulator::Now();

        for (auto &[nodeId, nodeInfo] : m_edgeNodes)
        {
            // Simulate heartbeat updates
            if (nodeInfo.isActive)
            {
                nodeInfo.lastHeartbeat = currentTime;

                // Simulate minor resource usage fluctuations
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> dis(-0.05, 0.05);

                nodeInfo.currentCpuUsage = std::max(0.0, nodeInfo.currentCpuUsage + dis(gen));
                nodeInfo.currentMemoryUsage = std::max(0.0, nodeInfo.currentMemoryUsage + dis(gen));
            }
        }

        // Update service registry
        for (auto &[serviceId, service] : m_deployedServices)
        {
            if (service.status == EdgeServiceStatus::RUNNING)
            {
                service.lastUpdate = currentTime;
            }
        }
    }

    std::vector<EdgeServiceInstance>
    OranMecFramework::GetServicesOnNode(const std::string &nodeId) const
    {
        std::vector<EdgeServiceInstance> services;

        auto nodeServicesIt = m_nodeServices.find(nodeId);
        if (nodeServicesIt != m_nodeServices.end())
        {
            for (const std::string &serviceId : nodeServicesIt->second)
            {
                auto serviceIt = m_deployedServices.find(serviceId);
                if (serviceIt != m_deployedServices.end())
                {
                    services.push_back(serviceIt->second);
                }
            }
        }

        return services;
    }

    EdgeNodeInfo
    OranMecFramework::GetNodeInfo(const std::string &nodeId) const
    {
        auto it = m_edgeNodes.find(nodeId);
        if (it != m_edgeNodes.end())
        {
            return it->second;
        }

        return EdgeNodeInfo{}; // Return empty struct if not found
    }

    std::vector<std::string>
    OranMecFramework::GetActiveEdgeNodes() const
    {
        std::vector<std::string> activeNodes;

        for (const auto &[nodeId, nodeInfo] : m_edgeNodes)
        {
            if (nodeInfo.isActive)
            {
                activeNodes.push_back(nodeId);
            }
        }

        return activeNodes;
    }

    void
    OranMecFramework::SetDataRepository(Ptr<OranDataRepository> dataRepository)
    {
        NS_LOG_FUNCTION(this << dataRepository);
        m_dataRepository = dataRepository;
    }

    Ptr<OranDataRepository>
    OranMecFramework::GetDataRepository() const
    {
        return m_dataRepository;
    }

} // namespace ns3
