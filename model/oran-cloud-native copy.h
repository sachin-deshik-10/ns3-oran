/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 * Cloud-Native O-RAN Integration Framework
 *
 * This module provides comprehensive cloud-native capabilities including:
 * - Kubernetes orchestration integration
 * - Microservices architecture support
 * - Service mesh implementation
 * - Container-based deployment
 * - DevOps and CI/CD integration
 * - Multi-cloud and hybrid cloud support
 */

#ifndef ORAN_CLOUD_NATIVE_H
#define ORAN_CLOUD_NATIVE_H

#include "ns3/object.h"
#include "ns3/traced-value.h"
#include "ns3/callback.h"
#include "oran-data-repository.h"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ns3
{

    /**
     * Container specification
     */
    struct ContainerSpec
    {
        std::string name;
        std::string image;
        std::string version;
        std::map<std::string, std::string> environmentVars;
        std::map<std::string, std::string> labels;
        std::vector<std::string> ports;
        double cpuRequest;
        double cpuLimit;
        double memoryRequest;
        double memoryLimit;
        std::vector<std::string> volumes;
    };

    /**
     * Kubernetes deployment specification
     */
    struct K8sDeploymentSpec
    {
        std::string name;
        std::string namespace_;
        uint32_t replicas;
        ContainerSpec containerSpec;
        std::map<std::string, std::string> nodeSelector;
        std::vector<std::string> tolerations;
        std::string serviceAccount;
        bool autoScaling;
        uint32_t minReplicas;
        uint32_t maxReplicas;
        double targetCpuUtilization;
    };

    /**
     * Service mesh configuration
     */
    struct ServiceMeshConfig
    {
        std::string meshType; // "istio", "linkerd", "consul"
        bool mtlsEnabled;
        std::string loadBalancingPolicy;
        std::map<std::string, std::string> circuitBreakerConfig;
        std::map<std::string, std::string> retryPolicy;
        std::vector<std::string> trafficPolicies;
    };

    /**
     * Cloud provider configuration
     */
    struct CloudProviderConfig
    {
        std::string provider; // "aws", "azure", "gcp", "openstack"
        std::string region;
        std::string credentials;
        std::map<std::string, std::string> providerSpecific;
    };

    /**
     * Microservice definition
     */
    class Microservice : public Object
    {
    public:
        static TypeId GetTypeId();

        Microservice();
        virtual ~Microservice() = default;

        /**
         * Set service specification
         */
        void SetServiceSpec(const std::string &name, const std::string &version);

        /**
         * Add API endpoint
         */
        void AddApiEndpoint(const std::string &path, const std::string &method,
                            const std::string &handler);

        /**
         * Set health check configuration
         */
        void SetHealthCheck(const std::string &path, uint32_t intervalSeconds);

        /**
         * Add dependency
         */
        void AddDependency(const std::string &serviceName, const std::string &version);

        /**
         * Get service configuration
         */
        nlohmann::json GetServiceConfig() const;

        /**
         * Handle service request
         */
        virtual std::string HandleRequest(const std::string &path,
                                          const std::string &method,
                                          const nlohmann::json &payload) = 0;

    private:
        std::string m_serviceName;
        std::string m_version;
        std::map<std::pair<std::string, std::string>, std::string> m_apiEndpoints;
        std::string m_healthCheckPath;
        uint32_t m_healthCheckInterval;
        std::vector<std::pair<std::string, std::string>> m_dependencies;
    };

    /**
     * Kubernetes orchestrator
     */
    class KubernetesOrchestrator : public Object
    {
    public:
        static TypeId GetTypeId();

        KubernetesOrchestrator();
        ~KubernetesOrchestrator() override = default;

        /**
         * Connect to Kubernetes cluster
         */
        bool ConnectToCluster(const std::string &kubeconfig);

        /**
         * Deploy microservice
         */
        std::string DeployMicroservice(const K8sDeploymentSpec &spec);

        /**
         * Scale deployment
         */
        void ScaleDeployment(const std::string &deploymentName, uint32_t replicas);

        /**
         * Update deployment
         */
        void UpdateDeployment(const std::string &deploymentName,
                              const ContainerSpec &newSpec);

        /**
         * Delete deployment
         */
        void DeleteDeployment(const std::string &deploymentName);

        /**
         * Get deployment status
         */
        nlohmann::json GetDeploymentStatus(const std::string &deploymentName);

        /**
         * Create service
         */
        std::string CreateService(const std::string &serviceName,
                                  const std::vector<std::string> &ports,
                                  const std::string &serviceType = "ClusterIP");

        /**
         * Create ingress
         */
        std::string CreateIngress(const std::string &ingressName,
                                  const std::map<std::string, std::string> &rules);

        /**
         * Monitor cluster health
         */
        std::map<std::string, std::string> GetClusterHealth();

    private:
        std::string m_kubeconfigPath;
        bool m_connected;
        std::map<std::string, K8sDeploymentSpec> m_deployments;
        std::string m_defaultNamespace;
    };

    /**
     * Service mesh manager
     */
    class ServiceMeshManager : public Object
    {
    public:
        static TypeId GetTypeId();

        ServiceMeshManager();
        ~ServiceMeshManager() override = default;

        /**
         * Initialize service mesh
         */
        void InitializeServiceMesh(const ServiceMeshConfig &config);

        /**
         * Add service to mesh
         */
        void AddServiceToMesh(const std::string &serviceName,
                              const std::string &version);

        /**
         * Configure traffic routing
         */
        void ConfigureTrafficRouting(const std::string &serviceName,
                                     const std::map<std::string, double> &weightedRouting);

        /**
         * Set circuit breaker
         */
        void SetCircuitBreaker(const std::string &serviceName,
                               uint32_t maxConnections,
                               uint32_t maxRequests,
                               double failureThreshold);

        /**
         * Configure mutual TLS
         */
        void ConfigureMutualTls(bool enabled, const std::string &certPath = "");

        /**
         * Get service mesh metrics
         */
        std::map<std::string, double> GetServiceMeshMetrics();

        /**
         * Generate service topology
         */
        nlohmann::json GenerateServiceTopology();

    private:
        ServiceMeshConfig m_config;
        std::map<std::string, std::string> m_services;
        std::map<std::string, std::map<std::string, double>> m_trafficRouting;
        bool m_mtlsEnabled;
    };

    /**
     * CI/CD Pipeline integration
     */
    class CiCdPipeline : public Object
    {
    public:
        static TypeId GetTypeId();

        CiCdPipeline();
        ~CiCdPipeline() override = default;

        /**
         * Configure pipeline
         */
        void ConfigurePipeline(const std::string &pipelineType,
                               const std::string &repositoryUrl,
                               const std::string &configFile);

        /**
         * Trigger build
         */
        std::string TriggerBuild(const std::string &branch,
                                 const std::map<std::string, std::string> &parameters);

        /**
         * Deploy to environment
         */
        void DeployToEnvironment(const std::string &buildId,
                                 const std::string &environment);

        /**
         * Run tests
         */
        std::map<std::string, bool> RunTests(const std::string &testSuite);

        /**
         * Get pipeline status
         */
        nlohmann::json GetPipelineStatus(const std::string &pipelineId);

        /**
         * Configure automated deployment
         */
        void ConfigureAutomatedDeployment(const std::string &strategy,
                                          const std::map<std::string, std::string> &rules);

    private:
        std::string m_pipelineType;
        std::string m_repositoryUrl;
        std::string m_configFile;
        std::map<std::string, std::string> m_environments;
        std::map<std::string, nlohmann::json> m_pipelineHistory;
    };

    /**
     * Comprehensive Cloud-Native O-RAN Framework
     *
     * This class provides complete cloud-native capabilities for O-RAN:
     * - Kubernetes orchestration and management
     * - Microservices architecture and deployment
     * - Service mesh configuration and management
     * - Multi-cloud and hybrid cloud support
     * - DevOps automation and CI/CD integration
     * - Container lifecycle management
     * - Cloud-native observability and monitoring
     */
    class OranCloudNative : public Object
    {
    public:
        static TypeId GetTypeId();

        OranCloudNative();
        ~OranCloudNative() override;

        /**
         * Initialize cloud-native framework
         */
        void Initialize();

        /**
         * Connect to cloud provider
         */
        bool ConnectToCloud(const CloudProviderConfig &config);

        /**
         * Setup Kubernetes cluster
         */
        void SetupKubernetesCluster(const std::string &clusterName,
                                    const std::string &nodePoolConfig);

        /**
         * Deploy O-RAN components as microservices
         */
        void DeployOranMicroservices(const std::vector<Ptr<Microservice>> &services);

        /**
         * Configure service mesh
         */
        void ConfigureServiceMesh(const ServiceMeshConfig &config);

        /**
         * Setup CI/CD pipeline
         */
        void SetupCiCdPipeline(const std::string &pipelineConfig);

        /**
         * Enable auto-scaling
         */
        void EnableAutoScaling(const std::string &serviceName,
                               uint32_t minReplicas,
                               uint32_t maxReplicas,
                               double targetCpuUtilization);

        /**
         * Configure monitoring and observability
         */
        void ConfigureObservability(const std::map<std::string, std::string> &config);

        /**
         * Deploy to multiple clouds
         */
        void DeployMultiCloud(const std::vector<CloudProviderConfig> &clouds);

        /**
         * Configure disaster recovery
         */
        void ConfigureDisasterRecovery(const std::string &strategy,
                                       const std::string &backupLocation);

        /**
         * Get cloud resource utilization
         */
        std::map<std::string, double> GetResourceUtilization();

        /**
         * Get deployment health
         */
        std::map<std::string, std::string> GetDeploymentHealth();

        /**
         * Perform rolling update
         */
        void PerformRollingUpdate(const std::string &serviceName,
                                  const std::string &newVersion);

        /**
         * Configure blue-green deployment
         */
        void ConfigureBlueGreenDeployment(const std::string &serviceName);

        /**
         * Implement canary deployment
         */
        void ImplementCanaryDeployment(const std::string &serviceName,
                                       const std::string &newVersion,
                                       double trafficPercentage);

        /**
         * Set cost optimization policies
         */
        void SetCostOptimization(const std::map<std::string, std::string> &policies);

        /**
         * Get cost analysis
         */
        std::map<std::string, double> GetCostAnalysis();

    protected:
        void DoDispose() override;

    private:
        /**
         * Initialize Kubernetes integration
         */
        void InitializeKubernetes();

        /**
         * Setup monitoring stack
         */
        void SetupMonitoring();

        /**
         * Configure networking
         */
        void ConfigureNetworking();

        /**
         * Setup security policies
         */
        void SetupSecurityPolicies();

        /**
         * Initialize service discovery
         */
        void InitializeServiceDiscovery();

        // Core components
        Ptr<KubernetesOrchestrator> m_k8sOrchestrator; ///< Kubernetes orchestrator
        Ptr<ServiceMeshManager> m_serviceMesh;         ///< Service mesh manager
        Ptr<CiCdPipeline> m_cicdPipeline;              ///< CI/CD pipeline

        // Configuration
        CloudProviderConfig m_cloudConfig; ///< Cloud provider config
        ServiceMeshConfig m_meshConfig;    ///< Service mesh config

        // Deployed services
        std::vector<Ptr<Microservice>> m_microservices;
        std::map<std::string, K8sDeploymentSpec> m_deployments;

        // Multi-cloud management
        std::vector<CloudProviderConfig> m_cloudProviders;
        std::map<std::string, std::string> m_cloudRegions;

        // Monitoring and observability
        std::map<std::string, std::string> m_observabilityConfig;
        std::map<std::string, double> m_metrics;

        // Security and compliance
        std::map<std::string, std::string> m_securityPolicies;
        bool m_complianceEnabled;

        // State management
        bool m_initialized;
        bool m_cloudConnected;
        std::string m_currentCluster;

        // Performance monitoring
        TracedValue<uint32_t> m_activePods;             ///< Active pods count
        TracedValue<double> m_clusterCpuUtilization;    ///< Cluster CPU utilization
        TracedValue<double> m_clusterMemoryUtilization; ///< Cluster memory utilization
        TracedValue<uint32_t> m_failedDeployments;      ///< Failed deployments count
        TracedValue<double> m_serviceMeshLatency;       ///< Service mesh latency
    };

    /**
     * O-RAN specific microservices
     */

    /**
     * Near-RT RIC microservice
     */
    class NearRtRicMicroservice : public Microservice
    {
    public:
        static TypeId GetTypeId();

        NearRtRicMicroservice();
        ~NearRtRicMicroservice() override = default;

        std::string HandleRequest(const std::string &path,
                                  const std::string &method,
                                  const nlohmann::json &payload) override;

    private:
        Ptr<OranDataRepository> m_dataRepository;
    };

    /**
     * E2 Terminator microservice
     */
    class E2TerminatorMicroservice : public Microservice
    {
    public:
        static TypeId GetTypeId();

        E2TerminatorMicroservice();
        ~E2TerminatorMicroservice() override = default;

        std::string HandleRequest(const std::string &path,
                                  const std::string &method,
                                  const nlohmann::json &payload) override;
    };

    /**
     * xApp microservice base
     */
    class XAppMicroservice : public Microservice
    {
    public:
        static TypeId GetTypeId();

        XAppMicroservice();
        ~XAppMicroservice() override = default;

        std::string HandleRequest(const std::string &path,
                                  const std::string &method,
                                  const nlohmann::json &payload) override;

        /**
         * Set xApp specific configuration
         */
        void SetXAppConfig(const std::string &xappName,
                           const std::string &algorithmType);

    protected:
        std::string m_xappName;
        std::string m_algorithmType;
    };

} // namespace ns3

#endif /* ORAN_CLOUD_NATIVE_H */
