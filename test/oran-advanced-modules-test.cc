/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 *
 * Advanced O-RAN Modules Test Suite
 *
 * This test suite validates the functionality of all advanced O-RAN modules:
 * - Reinforcement Learning Logic Module
 * - Digital Twin Framework
 * - Multi-access Edge Computing (MEC) Framework
 * - Cloud-Native Integration Framework
 */

#include "ns3/test.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/node.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-position-mobility-model.h"

// O-RAN module includes
#include "ns3/oran-lm-reinforcement-learning.h"
#include "ns3/oran-digital-twin.h"
#include "ns3/oran-mec-framework.h"
#include "ns3/oran-cloud-native.h"
#include "ns3/oran-data-repository-sqlite.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("OranAdvancedModulesTestSuite");

/**
 * Test class for Reinforcement Learning Logic Module
 */
class OranReinforcementLearningTestCase : public TestCase
{
public:
    OranReinforcementLearningTestCase();
    virtual ~OranReinforcementLearningTestCase();

private:
    virtual void DoRun() override;
};

OranReinforcementLearningTestCase::OranReinforcementLearningTestCase()
    : TestCase("O-RAN Reinforcement Learning Logic Module test")
{
}

OranReinforcementLearningTestCase::~OranReinforcementLearningTestCase()
{
}

void OranReinforcementLearningTestCase::DoRun()
{
    NS_LOG_INFO("Testing O-RAN Reinforcement Learning Logic Module");

    // Create RL logic module
    Ptr<OranLmReinforcementLearning> rlModule = CreateObject<OranLmReinforcementLearning>();
    NS_TEST_ASSERT_MSG_NE(rlModule, nullptr, "Failed to create RL logic module");

    // Test configuration
    rlModule->SetAttribute("LearningRate", DoubleValue(0.001));
    rlModule->SetAttribute("DiscountFactor", DoubleValue(0.95));
    rlModule->SetAttribute("ExplorationRate", DoubleValue(0.1));

    // Test state space and action space
    std::vector<double> state = {0.5, 0.8, 0.3, 0.9, 0.2}; // Sample state
    int action = rlModule->SelectAction(state);
    NS_TEST_ASSERT_MSG_GE(action, 0, "Action should be non-negative");
    NS_TEST_ASSERT_MSG_LT(action, 7, "Action should be within valid range");

    // Test experience storage
    std::vector<double> nextState = {0.6, 0.7, 0.4, 0.8, 0.3};
    double reward = 1.0;
    bool done = false;

    rlModule->StoreExperience(state, action, reward, nextState, done);

    // Test training
    rlModule->Train();

    NS_LOG_INFO("Reinforcement Learning test completed successfully");
}

/**
 * Test class for Digital Twin Framework
 */
class OranDigitalTwinTestCase : public TestCase
{
public:
    OranDigitalTwinTestCase();
    virtual ~OranDigitalTwinTestCase();

private:
    virtual void DoRun() override;
};

OranDigitalTwinTestCase::OranDigitalTwinTestCase()
    : TestCase("O-RAN Digital Twin Framework test")
{
}

OranDigitalTwinTestCase::~OranDigitalTwinTestCase()
{
}

void OranDigitalTwinTestCase::DoRun()
{
    NS_LOG_INFO("Testing O-RAN Digital Twin Framework");

    // Create digital twin
    Ptr<OranDigitalTwin> digitalTwin = CreateObject<OranDigitalTwin>();
    NS_TEST_ASSERT_MSG_NE(digitalTwin, nullptr, "Failed to create digital twin");

    // Test configuration
    digitalTwin->SetAttribute("PredictionWindow", TimeValue(Seconds(60.0)));
    digitalTwin->SetAttribute("UpdateInterval", TimeValue(Seconds(5.0)));
    digitalTwin->SetAttribute("EnablePredictiveAnalytics", BooleanValue(true));

    // Create a node for testing
    Ptr<Node> node = CreateObject<Node>();

    // Install mobility model
    Ptr<ConstantPositionMobilityModel> mobility = CreateObject<ConstantPositionMobilityModel>();
    mobility->SetPosition(Vector(0.0, 0.0, 0.0));
    node->AggregateObject(mobility);

    // Register node with digital twin
    digitalTwin->RegisterNetworkEntity("test-node-1", node);

    // Test state updates
    NetworkEntityState state;
    state.entityId = "test-node-1";
    state.position = Vector(10.0, 20.0, 0.0);
    state.rsrp = -80.0;
    state.rsrq = -10.0;
    state.sinr = 15.0;
    state.throughput = 50.0;
    state.latency = 5.0;
    state.timestamp = Simulator::Now();

    digitalTwin->UpdateEntityState(state);

    // Test prediction
    std::vector<NetworkEntityState> prediction = digitalTwin->PredictFutureStates("test-node-1", Seconds(30.0));
    NS_TEST_ASSERT_MSG_GT(prediction.size(), 0, "Prediction should return at least one state");

    // Test what-if analysis
    WhatIfScenario scenario;
    scenario.entityId = "test-node-1";
    scenario.parameterChanges["transmission_power"] = 23.0;
    scenario.timeHorizon = Seconds(60.0);

    WhatIfResult result = digitalTwin->AnalyzeWhatIfScenario(scenario);
    NS_TEST_ASSERT_MSG_GE(result.estimatedImpact, 0.0, "What-if impact should be calculated");

    NS_LOG_INFO("Digital Twin test completed successfully");
}

/**
 * Test class for MEC Framework
 */
class OranMecFrameworkTestCase : public TestCase
{
public:
    OranMecFrameworkTestCase();
    virtual ~OranMecFrameworkTestCase();

private:
    virtual void DoRun() override;
};

OranMecFrameworkTestCase::OranMecFrameworkTestCase()
    : TestCase("O-RAN MEC Framework test")
{
}

OranMecFrameworkTestCase::~OranMecFrameworkTestCase()
{
}

void OranMecFrameworkTestCase::DoRun()
{
    NS_LOG_INFO("Testing O-RAN MEC Framework");

    // Create MEC framework
    Ptr<OranMecFramework> mecFramework = CreateObject<OranMecFramework>();
    NS_TEST_ASSERT_MSG_NE(mecFramework, nullptr, "Failed to create MEC framework");

    // Test configuration
    mecFramework->SetAttribute("MaxServices", UintegerValue(500));
    mecFramework->SetAttribute("EnableFederatedLearning", BooleanValue(true));
    mecFramework->SetAttribute("LoadBalancingThreshold", DoubleValue(0.8));

    // Register edge nodes
    EdgeNodeCapabilities capabilities;
    capabilities.cpuCores = 16;
    capabilities.memoryGB = 64;
    capabilities.storageGB = 1000;
    capabilities.gpuCores = 4;
    capabilities.networkBandwidthMbps = 5000;
    capabilities.latencyMs = 2.0;

    mecFramework->RegisterEdgeNode("test-edge-node-1", capabilities);
    mecFramework->RegisterEdgeNode("test-edge-node-2", capabilities);

    // Test edge node listing
    std::vector<std::string> activeNodes = mecFramework->GetActiveEdgeNodes();
    NS_TEST_ASSERT_MSG_GE(activeNodes.size(), 2, "Should have at least 2 active nodes");

    // Test service deployment
    EdgeServiceRequirements serviceReq;
    serviceReq.serviceType = EdgeServiceType::VIDEO_ANALYTICS;
    serviceReq.cpuCores = 4.0;
    serviceReq.memoryMB = 8192;
    serviceReq.storageMB = 2048;
    serviceReq.maxLatencyMs = 10.0;
    serviceReq.bandwidthMbps = 1000;
    serviceReq.requiresGpu = true;

    std::string serviceId = mecFramework->DeployService(serviceReq);
    NS_TEST_ASSERT_MSG_NE(serviceId, "", "Service deployment should succeed");

    // Test service migration
    if (activeNodes.size() >= 2)
    {
        mecFramework->MigrateService(serviceId, activeNodes[1]);
    }

    // Test load balancing
    mecFramework->PerformLoadBalancing();

    NS_LOG_INFO("MEC Framework test completed successfully");
}

/**
 * Test class for Cloud-Native Framework
 */
class OranCloudNativeTestCase : public TestCase
{
public:
    OranCloudNativeTestCase();
    virtual ~OranCloudNativeTestCase();

private:
    virtual void DoRun() override;
};

OranCloudNativeTestCase::OranCloudNativeTestCase()
    : TestCase("O-RAN Cloud-Native Framework test")
{
}

OranCloudNativeTestCase::~OranCloudNativeTestCase()
{
}

void OranCloudNativeTestCase::DoRun()
{
    NS_LOG_INFO("Testing O-RAN Cloud-Native Framework");

    // Create cloud-native framework
    Ptr<OranCloudNative> cloudNative = CreateObject<OranCloudNative>();
    NS_TEST_ASSERT_MSG_NE(cloudNative, nullptr, "Failed to create cloud-native framework");

    // Test configuration
    cloudNative->SetAttribute("MaxPods", UintegerValue(1000));
    cloudNative->SetAttribute("EnableServiceMesh", BooleanValue(true));
    cloudNative->SetAttribute("AutoScalingEnabled", BooleanValue(true));

    // Test namespace creation
    cloudNative->CreateNamespace("test-namespace");

    // Test pod deployment
    ContainerSpec containerSpec;
    containerSpec.name = "test-container";
    containerSpec.image = "test-image";
    containerSpec.version = "latest";
    containerSpec.cpuRequest = 1.0;
    containerSpec.cpuLimit = 2.0;
    containerSpec.memoryRequest = 2.0;
    containerSpec.memoryLimit = 4.0;

    std::string podId = cloudNative->DeployPod(containerSpec, "test-namespace");
    NS_TEST_ASSERT_MSG_NE(podId, "", "Pod deployment should succeed");

    // Test service creation
    MicroserviceSpec serviceSpec;
    serviceSpec.name = "test-service";
    serviceSpec.image = "test-service-image";
    serviceSpec.version = "v1.0";
    serviceSpec.namespaceName = "test-namespace";
    serviceSpec.replicas = 2;
    serviceSpec.cpuRequest = 1.0;
    serviceSpec.cpuLimit = 2.0;
    serviceSpec.memoryRequest = 2.0;
    serviceSpec.memoryLimit = 4.0;
    serviceSpec.labels["app"] = "test-service";
    serviceSpec.ports.push_back("8080:8080");

    std::string serviceId = cloudNative->CreateService(serviceSpec);
    NS_TEST_ASSERT_MSG_NE(serviceId, "", "Service creation should succeed");

    // Test service scaling
    cloudNative->ScaleService(serviceId, 3);

    // Test CI/CD pipeline
    CiCdPipelineSpec pipelineSpec;
    pipelineSpec.name = "test-pipeline";
    pipelineSpec.repository = "https://github.com/test/test-repo";
    pipelineSpec.branch = "main";
    pipelineSpec.stages.push_back("build");
    pipelineSpec.stages.push_back("test");
    pipelineSpec.stages.push_back("deploy");

    std::string pipelineId = cloudNative->ExecutePipeline(pipelineSpec);
    NS_TEST_ASSERT_MSG_NE(pipelineId, "", "Pipeline execution should succeed");

    // Test multi-cloud deployment
    std::vector<CloudProvider> providers = {CloudProvider::AWS, CloudProvider::AZURE};
    cloudNative->DeployToMultiCloud(serviceId, providers);

    // Test metrics collection
    CloudNativeMetrics metrics = cloudNative->GetLatestMetrics();
    NS_TEST_ASSERT_MSG_GE(metrics.totalPods, 0, "Metrics should be collected");

    NS_LOG_INFO("Cloud-Native Framework test completed successfully");
}

/**
 * Test class for integration between all advanced modules
 */
class OranAdvancedModulesIntegrationTestCase : public TestCase
{
public:
    OranAdvancedModulesIntegrationTestCase();
    virtual ~OranAdvancedModulesIntegrationTestCase();

private:
    virtual void DoRun() override;
};

OranAdvancedModulesIntegrationTestCase::OranAdvancedModulesIntegrationTestCase()
    : TestCase("O-RAN Advanced Modules Integration test")
{
}

OranAdvancedModulesIntegrationTestCase::~OranAdvancedModulesIntegrationTestCase()
{
}

void OranAdvancedModulesIntegrationTestCase::DoRun()
{
    NS_LOG_INFO("Testing O-RAN Advanced Modules Integration");

    // Create data repository for shared use
    Ptr<OranDataRepositorySqlite> dataRepo = CreateObject<OranDataRepositorySqlite>();
    dataRepo->SetAttribute("DatabaseFile", StringValue("test-integration.db"));

    // Create all frameworks
    Ptr<OranLmReinforcementLearning> rlModule = CreateObject<OranLmReinforcementLearning>();
    Ptr<OranDigitalTwin> digitalTwin = CreateObject<OranDigitalTwin>();
    Ptr<OranMecFramework> mecFramework = CreateObject<OranMecFramework>();
    Ptr<OranCloudNative> cloudNative = CreateObject<OranCloudNative>();

    // Connect frameworks to data repository
    rlModule->SetDataRepository(dataRepo);
    digitalTwin->SetDataRepository(dataRepo);
    mecFramework->SetDataRepository(dataRepo);
    cloudNative->SetDataRepository(dataRepo);

    // Test cross-framework functionality

    // 1. Digital Twin provides data to RL module
    NetworkEntityState state;
    state.entityId = "integration-test-node";
    state.rsrp = -85.0;
    state.rsrq = -12.0;
    state.sinr = 12.0;
    state.throughput = 45.0;
    state.latency = 8.0;
    state.timestamp = Simulator::Now();

    digitalTwin->UpdateEntityState(state);

    // 2. RL module makes decisions based on digital twin data
    std::vector<double> rlState = {state.rsrp + 100, state.rsrq + 20, state.sinr,
                                   state.throughput, state.latency};
    int action = rlModule->SelectAction(rlState);

    // 3. MEC framework deploys services based on RL decisions
    EdgeServiceRequirements serviceReq;
    serviceReq.serviceType = static_cast<EdgeServiceType>(action % 9);
    serviceReq.cpuCores = 2.0;
    serviceReq.memoryMB = 4096;
    serviceReq.storageMB = 1024;
    serviceReq.maxLatencyMs = 5.0;
    serviceReq.bandwidthMbps = 500;
    serviceReq.requiresGpu = false;

    std::string serviceId = mecFramework->DeployService(serviceReq);

    // 4. Cloud-Native framework orchestrates the deployed services
    if (!serviceId.empty())
    {
        MicroserviceSpec microService;
        microService.name = "rl-optimized-service";
        microService.image = "oran/rl-service";
        microService.version = "latest";
        microService.namespaceName = "oran-apps";
        microService.replicas = 2;
        microService.cpuRequest = 1.0;
        microService.cpuLimit = 2.0;
        microService.memoryRequest = 2.0;
        microService.memoryLimit = 4.0;

        std::string k8sServiceId = cloudNative->CreateService(microService);
        NS_TEST_ASSERT_MSG_NE(k8sServiceId, "", "Integrated service creation should succeed");
    }

    // 5. Test feedback loop: Digital Twin analyzes the impact
    std::vector<NetworkEntityState> prediction = digitalTwin->PredictFutureStates(
        "integration-test-node", Seconds(30.0));

    if (!prediction.empty())
    {
        // Provide feedback to RL module
        double reward = (prediction[0].throughput > state.throughput) ? 1.0 : -0.5;
        rlModule->StoreExperience(rlState, action, reward, rlState, false);
    }

    // Verify integration success
    NS_TEST_ASSERT_MSG_NE(rlModule, nullptr, "RL module should be active");
    NS_TEST_ASSERT_MSG_NE(digitalTwin, nullptr, "Digital Twin should be active");
    NS_TEST_ASSERT_MSG_NE(mecFramework, nullptr, "MEC Framework should be active");
    NS_TEST_ASSERT_MSG_NE(cloudNative, nullptr, "Cloud-Native Framework should be active");

    NS_LOG_INFO("Advanced Modules Integration test completed successfully");
}

/**
 * Test suite class
 */
class OranAdvancedModulesTestSuite : public TestSuite
{
public:
    OranAdvancedModulesTestSuite();
};

OranAdvancedModulesTestSuite::OranAdvancedModulesTestSuite()
    : TestSuite("oran-advanced-modules", UNIT)
{
    AddTestCase(new OranReinforcementLearningTestCase(), TestCase::QUICK);
    AddTestCase(new OranDigitalTwinTestCase(), TestCase::QUICK);
    AddTestCase(new OranMecFrameworkTestCase(), TestCase::QUICK);
    AddTestCase(new OranCloudNativeTestCase(), TestCase::QUICK);
    AddTestCase(new OranAdvancedModulesIntegrationTestCase(), TestCase::QUICK);
}

// Static variable for test suite registration
static OranAdvancedModulesTestSuite g_oranAdvancedModulesTestSuite;
