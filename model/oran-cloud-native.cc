/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 * Cloud-Native O-RAN Integration Framework - Implementation
 */

#include "oran-cloud-native.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/pointer.h"
#include "ns3/uinteger.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <sstream>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("OranCloudNative");
    NS_OBJECT_ENSURE_REGISTERED(OranCloudNative);

    TypeId
    OranCloudNative::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::OranCloudNative")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<OranCloudNative>()
                                .AddAttribute("MaxPods",
                                              "Maximum number of pods per cluster",
                                              UintegerValue(1000),
                                              MakeUintegerAccessor(&OranCloudNative::m_maxPods),
                                              MakeUintegerChecker<uint32_t>())
                                .AddAttribute("EnableServiceMesh",
                                              "Enable service mesh capabilities",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&OranCloudNative::m_enableServiceMesh),
                                              MakeBooleanChecker())
                                .AddAttribute("AutoScalingEnabled",
                                              "Enable automatic scaling",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&OranCloudNative::m_autoScalingEnabled),
                                              MakeBooleanChecker())
                                .AddAttribute("MonitoringInterval",
                                              "Interval for monitoring and metrics collection",
                                              TimeValue(Seconds(10.0)),
                                              MakeTimeAccessor(&OranCloudNative::m_monitoringInterval),
                                              MakeTimeChecker())
                                .AddAttribute("ScalingThreshold",
                                              "Resource utilization threshold for scaling",
                                              DoubleValue(0.8),
                                              MakeDoubleAccessor(&OranCloudNative::m_scalingThreshold),
                                              MakeDoubleChecker<double>(0.0, 1.0))
                                .AddTraceSource("PodDeployed",
                                                "A pod was deployed",
                                                MakeTraceSourceAccessor(&OranCloudNative::m_podDeployedTrace),
                                                "ns3::OranCloudNative::PodDeployedTracedCallback")
                                .AddTraceSource("ServiceScaled",
                                                "A service was scaled",
                                                MakeTraceSourceAccessor(&OranCloudNative::m_serviceScaledTrace),
                                                "ns3::OranCloudNative::ServiceScaledTracedCallback")
                                .AddTraceSource("PipelineExecuted",
                                                "A CI/CD pipeline was executed",
                                                MakeTraceSourceAccessor(&OranCloudNative::m_pipelineExecutedTrace),
                                                "ns3::OranCloudNative::PipelineExecutedTracedCallback");
        return tid;
    }

    OranCloudNative::OranCloudNative()
        : m_maxPods(1000),
          m_enableServiceMesh(true),
          m_autoScalingEnabled(true),
          m_monitoringInterval(Seconds(10.0)),
          m_scalingThreshold(0.8),
          m_podCounter(0),
          m_serviceCounter(0),
          m_pipelineCounter(0)
    {
        NS_LOG_FUNCTION(this);
        InitializeKubernetesCluster();
        ScheduleMonitoring();
    }

    OranCloudNative::~OranCloudNative()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    OranCloudNative::InitializeKubernetesCluster()
    {
        NS_LOG_FUNCTION(this);

        // Initialize default cluster configuration
        KubernetesCluster cluster;
        cluster.clusterId = "oran-cluster-1";
        cluster.masterNodes = 3;
        cluster.workerNodes = 5;
        cluster.totalCpuCores = 64;
        cluster.totalMemoryGB = 256;
        cluster.totalStorageGB = 2000;
        cluster.networkPolicy = "Calico";
        cluster.ingressController = "NGINX";
        cluster.isActive = true;

        m_clusters["oran-cluster-1"] = cluster;

        // Initialize default namespaces
        CreateNamespace("oran-system");
        CreateNamespace("oran-apps");
        CreateNamespace("monitoring");
        CreateNamespace("istio-system");

        NS_LOG_INFO("Initialized Kubernetes cluster: " << cluster.clusterId);
    }

    void
    OranCloudNative::CreateNamespace(const std::string &name)
    {
        NS_LOG_FUNCTION(this << name);

        KubernetesNamespace ns;
        ns.name = name;
        ns.labels["managed-by"] = "oran-cloud-native";
        ns.resourceQuotas["cpu"] = "10";
        ns.resourceQuotas["memory"] = "20Gi";
        ns.resourceQuotas["pods"] = "100";
        ns.creationTime = Simulator::Now();

        m_namespaces[name] = ns;

        NS_LOG_INFO("Created namespace: " << name);
    }

    std::string
    OranCloudNative::DeployPod(const ContainerSpec &containerSpec, const std::string &namespaceName)
    {
        NS_LOG_FUNCTION(this << containerSpec.name << namespaceName);

        // Validate namespace exists
        if (m_namespaces.find(namespaceName) == m_namespaces.end())
        {
            NS_LOG_WARN("Namespace does not exist: " << namespaceName);
            return "";
        }

        // Create pod specification
        KubernetesPod pod;
        pod.podId = "pod-" + std::to_string(++m_podCounter);
        pod.name = containerSpec.name + "-" + std::to_string(m_podCounter);
        pod.namespaceName = namespaceName;
        pod.containers.push_back(containerSpec);
        pod.status = PodStatus::RUNNING;
        pod.scheduledNode = SelectOptimalNode(containerSpec);
        pod.creationTime = Simulator::Now();
        pod.lastUpdate = Simulator::Now();

        // Set resource requests and limits
        pod.resourceRequests["cpu"] = std::to_string(containerSpec.cpuRequest);
        pod.resourceRequests["memory"] = std::to_string(containerSpec.memoryRequest) + "Gi";
        pod.resourceLimits["cpu"] = std::to_string(containerSpec.cpuLimit);
        pod.resourceLimits["memory"] = std::to_string(containerSpec.memoryLimit) + "Gi";

        // Store pod
        m_pods[pod.podId] = pod;
        m_namespacePods[namespaceName].push_back(pod.podId);

        // Update cluster resource usage
        UpdateClusterResourceUsage(containerSpec.cpuRequest, containerSpec.memoryRequest);

        // Trigger trace
        m_podDeployedTrace(pod.podId, pod.name, namespaceName);

        NS_LOG_INFO("Deployed pod " << pod.podId << " in namespace " << namespaceName);

        return pod.podId;
    }

    std::string
    OranCloudNative::SelectOptimalNode(const ContainerSpec &containerSpec)
    {
        // Simple node selection based on resource requirements
        // In a real implementation, this would use Kubernetes scheduler

        if (containerSpec.cpuRequest > 4.0 || containerSpec.memoryRequest > 8.0)
        {
            return "worker-node-1"; // High-performance node
        }
        else if (containerSpec.cpuRequest > 2.0 || containerSpec.memoryRequest > 4.0)
        {
            return "worker-node-2"; // Medium-performance node
        }
        else
        {
            return "worker-node-3"; // Standard node
        }
    }

    void
    OranCloudNative::UpdateClusterResourceUsage(double cpuRequest, double memoryRequest)
    {
        auto &cluster = m_clusters["oran-cluster-1"];
        cluster.usedCpuCores += cpuRequest;
        cluster.usedMemoryGB += memoryRequest;
    }

    std::string
    OranCloudNative::CreateService(const MicroserviceSpec &serviceSpec)
    {
        NS_LOG_FUNCTION(this << serviceSpec.name);

        KubernetesService service;
        service.serviceId = "svc-" + std::to_string(++m_serviceCounter);
        service.name = serviceSpec.name;
        service.namespaceName = serviceSpec.namespaceName;
        service.serviceType = ServiceType::CLUSTER_IP;
        service.selector = serviceSpec.labels;
        service.ports = serviceSpec.ports;
        service.creationTime = Simulator::Now();

        // Deploy pods for the service
        for (int i = 0; i < serviceSpec.replicas; ++i)
        {
            ContainerSpec containerSpec;
            containerSpec.name = serviceSpec.name;
            containerSpec.image = serviceSpec.image;
            containerSpec.version = serviceSpec.version;
            containerSpec.labels = serviceSpec.labels;
            containerSpec.environmentVars = serviceSpec.environmentVars;
            containerSpec.cpuRequest = serviceSpec.cpuRequest;
            containerSpec.cpuLimit = serviceSpec.cpuLimit;
            containerSpec.memoryRequest = serviceSpec.memoryRequest;
            containerSpec.memoryLimit = serviceSpec.memoryLimit;

            std::string podId = DeployPod(containerSpec, serviceSpec.namespaceName);
            if (!podId.empty())
            {
                service.podIds.push_back(podId);
            }
        }

        // Store service
        m_services[service.serviceId] = service;

        // Setup service mesh if enabled
        if (m_enableServiceMesh)
        {
            SetupServiceMesh(service.serviceId);
        }

        NS_LOG_INFO("Created service " << service.serviceId << " with " << service.podIds.size() << " pods");

        return service.serviceId;
    }

    void
    OranCloudNative::SetupServiceMesh(const std::string &serviceId)
    {
        NS_LOG_FUNCTION(this << serviceId);

        ServiceMeshConfiguration meshConfig;
        meshConfig.serviceId = serviceId;
        meshConfig.enableMutualTLS = true;
        meshConfig.enableTracing = true;
        meshConfig.enableMetrics = true;
        meshConfig.retryPolicy.maxRetries = 3;
        meshConfig.retryPolicy.retryTimeout = MilliSeconds(1000);
        meshConfig.circuitBreaker.enabled = true;
        meshConfig.circuitBreaker.maxConnections = 100;
        meshConfig.circuitBreaker.maxRequests = 200;

        m_serviceMeshConfigs[serviceId] = meshConfig;

        NS_LOG_INFO("Setup service mesh for service: " << serviceId);
    }

    void
    OranCloudNative::ScaleService(const std::string &serviceId, int targetReplicas)
    {
        NS_LOG_FUNCTION(this << serviceId << targetReplicas);

        auto serviceIt = m_services.find(serviceId);
        if (serviceIt == m_services.end())
        {
            NS_LOG_WARN("Service not found: " << serviceId);
            return;
        }

        auto &service = serviceIt->second;
        int currentReplicas = service.podIds.size();

        if (targetReplicas > currentReplicas)
        {
            // Scale up - add pods
            int podsToAdd = targetReplicas - currentReplicas;

            for (int i = 0; i < podsToAdd; ++i)
            {
                // Create new pod with same configuration
                ContainerSpec containerSpec;
                containerSpec.name = service.name;
                containerSpec.cpuRequest = 1.0; // Default values
                containerSpec.cpuLimit = 2.0;
                containerSpec.memoryRequest = 2.0;
                containerSpec.memoryLimit = 4.0;

                std::string podId = DeployPod(containerSpec, service.namespaceName);
                if (!podId.empty())
                {
                    service.podIds.push_back(podId);
                }
            }
        }
        else if (targetReplicas < currentReplicas)
        {
            // Scale down - remove pods
            int podsToRemove = currentReplicas - targetReplicas;

            for (int i = 0; i < podsToRemove && !service.podIds.empty(); ++i)
            {
                std::string podId = service.podIds.back();
                service.podIds.pop_back();

                // Remove pod
                auto podIt = m_pods.find(podId);
                if (podIt != m_pods.end())
                {
                    auto &pod = podIt->second;
                    pod.status = PodStatus::TERMINATED;

                    // Update resource usage
                    for (const auto &container : pod.containers)
                    {
                        UpdateClusterResourceUsage(-container.cpuRequest, -container.memoryRequest);
                    }
                }
            }
        }

        // Trigger trace
        m_serviceScaledTrace(serviceId, currentReplicas, targetReplicas);

        NS_LOG_INFO("Scaled service " << serviceId << " from " << currentReplicas << " to " << targetReplicas << " replicas");
    }

    void
    OranCloudNative::AutoScale()
    {
        NS_LOG_FUNCTION(this);

        if (!m_autoScalingEnabled)
            return;

        for (auto &[serviceId, service] : m_services)
        {
            // Calculate average resource utilization
            double totalCpuUsage = 0.0;
            double totalMemoryUsage = 0.0;
            int activePods = 0;

            for (const std::string &podId : service.podIds)
            {
                auto podIt = m_pods.find(podId);
                if (podIt != m_pods.end() && podIt->second.status == PodStatus::RUNNING)
                {
                    // Simulate resource usage
                    totalCpuUsage += SimulateResourceUsage();
                    totalMemoryUsage += SimulateResourceUsage();
                    activePods++;
                }
            }

            if (activePods == 0)
                continue;

            double avgCpuUsage = totalCpuUsage / activePods;
            double avgMemoryUsage = totalMemoryUsage / activePods;
            double maxUsage = std::max(avgCpuUsage, avgMemoryUsage);

            // Scale decision
            int currentReplicas = activePods;
            int targetReplicas = currentReplicas;

            if (maxUsage > m_scalingThreshold)
            {
                // Scale up
                targetReplicas = std::min(10, currentReplicas + 1);
            }
            else if (maxUsage < 0.3 && currentReplicas > 1)
            {
                // Scale down
                targetReplicas = std::max(1, currentReplicas - 1);
            }

            if (targetReplicas != currentReplicas)
            {
                ScaleService(serviceId, targetReplicas);
            }
        }
    }

    double
    OranCloudNative::SimulateResourceUsage()
    {
        // Simulate realistic resource usage patterns
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.2, 0.9);

        return dis(gen);
    }

    std::string
    OranCloudNative::ExecutePipeline(const CiCdPipelineSpec &pipelineSpec)
    {
        NS_LOG_FUNCTION(this << pipelineSpec.name);

        CiCdPipeline pipeline;
        pipeline.pipelineId = "pipeline-" + std::to_string(++m_pipelineCounter);
        pipeline.name = pipelineSpec.name;
        pipeline.repository = pipelineSpec.repository;
        pipeline.branch = pipelineSpec.branch;
        pipeline.stages = pipelineSpec.stages;
        pipeline.status = PipelineStatus::RUNNING;
        pipeline.startTime = Simulator::Now();

        // Simulate pipeline execution
        Simulator::Schedule(Seconds(60.0), &OranCloudNative::CompletePipeline, this, pipeline.pipelineId);

        m_pipelines[pipeline.pipelineId] = pipeline;

        // Trigger trace
        m_pipelineExecutedTrace(pipeline.pipelineId, pipelineSpec.name, PipelineStatus::RUNNING);

        NS_LOG_INFO("Started CI/CD pipeline: " << pipeline.pipelineId);

        return pipeline.pipelineId;
    }

    void
    OranCloudNative::CompletePipeline(const std::string &pipelineId)
    {
        NS_LOG_FUNCTION(this << pipelineId);

        auto pipelineIt = m_pipelines.find(pipelineId);
        if (pipelineIt != m_pipelines.end())
        {
            auto &pipeline = pipelineIt->second;
            pipeline.status = PipelineStatus::SUCCESS;
            pipeline.endTime = Simulator::Now();

            // Trigger trace
            m_pipelineExecutedTrace(pipelineId, pipeline.name, PipelineStatus::SUCCESS);

            NS_LOG_INFO("Completed CI/CD pipeline: " << pipelineId);
        }
    }

    void
    OranCloudNative::DeployToMultiCloud(const std::string &serviceId,
                                        const std::vector<CloudProvider> &providers)
    {
        NS_LOG_FUNCTION(this << serviceId);

        auto serviceIt = m_services.find(serviceId);
        if (serviceIt == m_services.end())
        {
            NS_LOG_WARN("Service not found: " << serviceId);
            return;
        }

        MultiCloudDeployment deployment;
        deployment.serviceId = serviceId;
        deployment.providers = providers;
        deployment.status = DeploymentStatus::ACTIVE;
        deployment.deploymentTime = Simulator::Now();

        // Simulate deployment to each cloud provider
        for (const auto &provider : providers)
        {
            CloudDeploymentInfo info;
            info.provider = provider;
            info.region = GetOptimalRegion(provider);
            info.endpoints.push_back("https://" + serviceId + "." + info.region + ".example.com");
            info.isActive = true;

            deployment.deploymentInfo[provider] = info;

            NS_LOG_INFO("Deployed service " << serviceId << " to " << static_cast<int>(provider) << " in region " << info.region);
        }

        m_multiCloudDeployments[serviceId] = deployment;
    }

    std::string
    OranCloudNative::GetOptimalRegion(CloudProvider provider)
    {
        // Simple region selection logic
        switch (provider)
        {
        case CloudProvider::AWS:
            return "us-east-1";
        case CloudProvider::AZURE:
            return "eastus";
        case CloudProvider::GCP:
            return "us-central1";
        default:
            return "default-region";
        }
    }

    void
    OranCloudNative::ScheduleMonitoring()
    {
        NS_LOG_FUNCTION(this);

        // Collect metrics
        CollectMetrics();

        // Perform auto-scaling
        AutoScale();

        // Update service mesh configurations
        UpdateServiceMeshConfigurations();

        // Schedule next monitoring cycle
        Simulator::Schedule(m_monitoringInterval, &OranCloudNative::ScheduleMonitoring, this);
    }

    void
    OranCloudNative::CollectMetrics()
    {
        NS_LOG_FUNCTION(this);

        CloudNativeMetrics metrics;
        metrics.timestamp = Simulator::Now();

        // Collect cluster metrics
        auto &cluster = m_clusters["oran-cluster-1"];
        metrics.clusterCpuUsage = cluster.usedCpuCores / cluster.totalCpuCores;
        metrics.clusterMemoryUsage = cluster.usedMemoryGB / cluster.totalMemoryGB;
        metrics.totalPods = m_pods.size();
        metrics.totalServices = m_services.size();

        // Count running pods
        metrics.runningPods = 0;
        for (const auto &[podId, pod] : m_pods)
        {
            if (pod.status == PodStatus::RUNNING)
            {
                metrics.runningPods++;
            }
        }

        m_metricsHistory.push_back(metrics);

        // Keep only last 100 metrics
        if (m_metricsHistory.size() > 100)
        {
            m_metricsHistory.erase(m_metricsHistory.begin());
        }

        NS_LOG_DEBUG("Collected metrics - CPU: " << metrics.clusterCpuUsage << "%, Memory: " << metrics.clusterMemoryUsage << "%, Pods: " << metrics.runningPods << "/" << metrics.totalPods);
    }

    void
    OranCloudNative::UpdateServiceMeshConfigurations()
    {
        NS_LOG_FUNCTION(this);

        if (!m_enableServiceMesh)
            return;

        // Update circuit breaker configurations based on service performance
        for (auto &[serviceId, meshConfig] : m_serviceMeshConfigs)
        {
            // Simulate adaptive configuration updates
            auto serviceIt = m_services.find(serviceId);
            if (serviceIt != m_services.end())
            {
                double errorRate = SimulateResourceUsage() * 0.1; // Simulate error rate

                if (errorRate > 0.05) // 5% error rate threshold
                {
                    meshConfig.circuitBreaker.maxConnections =
                        std::max(50, meshConfig.circuitBreaker.maxConnections - 10);
                }
                else
                {
                    meshConfig.circuitBreaker.maxConnections =
                        std::min(200, meshConfig.circuitBreaker.maxConnections + 5);
                }
            }
        }
    }

    std::vector<KubernetesPod>
    OranCloudNative::GetPodsInNamespace(const std::string &namespaceName) const
    {
        std::vector<KubernetesPod> pods;

        auto nsPodsIt = m_namespacePods.find(namespaceName);
        if (nsPodsIt != m_namespacePods.end())
        {
            for (const std::string &podId : nsPodsIt->second)
            {
                auto podIt = m_pods.find(podId);
                if (podIt != m_pods.end())
                {
                    pods.push_back(podIt->second);
                }
            }
        }

        return pods;
    }

    std::vector<KubernetesService>
    OranCloudNative::GetServicesInNamespace(const std::string &namespaceName) const
    {
        std::vector<KubernetesService> services;

        for (const auto &[serviceId, service] : m_services)
        {
            if (service.namespaceName == namespaceName)
            {
                services.push_back(service);
            }
        }

        return services;
    }

    CloudNativeMetrics
    OranCloudNative::GetLatestMetrics() const
    {
        if (!m_metricsHistory.empty())
        {
            return m_metricsHistory.back();
        }

        return CloudNativeMetrics{};
    }

    void
    OranCloudNative::SetDataRepository(Ptr<OranDataRepository> dataRepository)
    {
        NS_LOG_FUNCTION(this << dataRepository);
        m_dataRepository = dataRepository;
    }

    Ptr<OranDataRepository>
    OranCloudNative::GetDataRepository() const
    {
        return m_dataRepository;
    }

} // namespace ns3
