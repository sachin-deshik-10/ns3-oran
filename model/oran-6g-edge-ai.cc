/**
 * @file oran-6g-edge-ai.cc
 * @brief Implementation of Advanced AI-ML Edge Computing Platform for 6G O-RAN
 */

#include "oran-6g-edge-ai.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/simulator.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("Oran6gEdgeAi");

    NS_OBJECT_ENSURE_REGISTERED(Oran6gEdgeAi);

    // Federated Learning Coordinator Implementation

    FederatedLearningCoordinator::FederatedLearningCoordinator()
        : m_currentRound(0),
          m_learningRate(0.01),
          m_privacyBudget(1.0)
    {
        NS_LOG_FUNCTION(this);
        m_globalModel.resize(1000, 0.0); // Initialize with 1000 parameters
        m_convergenceHistory.reserve(1000);
    }

    FederatedLearningCoordinator::~FederatedLearningCoordinator()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    FederatedLearningCoordinator::InitializeFederatedRound(uint32_t roundId)
    {
        NS_LOG_FUNCTION(this << roundId);
        m_currentRound = roundId;

        // Reset round-specific metrics
        m_convergenceHistory.push_back(0.0);

        NS_LOG_INFO("Initialized federated learning round " << roundId);
    }

    void
    FederatedLearningCoordinator::AggregateGradients(const std::vector<std::vector<double>> &gradients)
    {
        NS_LOG_FUNCTION(this << gradients.size());

        if (gradients.empty())
        {
            NS_LOG_WARN("No gradients to aggregate");
            return;
        }

        // Use FedAvg as default aggregation
        std::vector<double> aggregated = FedAvgAggregation(gradients);
        UpdateGlobalModel(aggregated);

        NS_LOG_INFO("Aggregated gradients from " << gradients.size() << " clients");
    }

    std::vector<double>
    FederatedLearningCoordinator::FedAvgAggregation(const std::vector<std::vector<double>> &gradients)
    {
        NS_LOG_FUNCTION(this << gradients.size());

        if (gradients.empty())
        {
            return std::vector<double>();
        }

        size_t modelSize = gradients[0].size();
        std::vector<double> aggregated(modelSize, 0.0);

        // Simple averaging
        for (const auto &gradient : gradients)
        {
            for (size_t i = 0; i < modelSize && i < gradient.size(); ++i)
            {
                aggregated[i] += gradient[i];
            }
        }

        // Normalize by number of clients
        double numClients = static_cast<double>(gradients.size());
        for (double &value : aggregated)
        {
            value /= numClients;
        }

        return aggregated;
    }

    void
    FederatedLearningCoordinator::UpdateGlobalModel(const std::vector<double> &aggregatedGradients)
    {
        NS_LOG_FUNCTION(this << aggregatedGradients.size());

        // Apply gradient descent update
        for (size_t i = 0; i < m_globalModel.size() && i < aggregatedGradients.size(); ++i)
        {
            m_globalModel[i] -= m_learningRate * aggregatedGradients[i];
        }

        NS_LOG_INFO("Updated global model with learning rate " << m_learningRate);
    }

    double
    FederatedLearningCoordinator::CalculateConvergenceMetric()
    {
        NS_LOG_FUNCTION(this);

        if (m_convergenceHistory.size() < 2)
        {
            return 1.0; // Not converged yet
        }

        // Calculate relative change in last few rounds
        size_t window = std::min(static_cast<size_t>(5), m_convergenceHistory.size());
        double recent_avg = 0.0;
        double previous_avg = 0.0;

        for (size_t i = 0; i < window / 2; ++i)
        {
            recent_avg += m_convergenceHistory[m_convergenceHistory.size() - 1 - i];
            if (m_convergenceHistory.size() > window / 2 + i)
            {
                previous_avg += m_convergenceHistory[m_convergenceHistory.size() - window / 2 - 1 - i];
            }
        }

        recent_avg /= (window / 2);
        previous_avg /= (window / 2);

        double convergence = std::abs(recent_avg - previous_avg) / (previous_avg + 1e-8);

        NS_LOG_INFO("Convergence metric: " << convergence);
        return convergence;
    }

    // Neuromorphic Engine Implementation

    NeuromorphicEngine::NeuromorphicEngine()
        : m_current_time(0.0),
          m_total_energy(0.0)
    {
        NS_LOG_FUNCTION(this);
    }

    NeuromorphicEngine::~NeuromorphicEngine()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    NeuromorphicEngine::InitializeSpikeNetwork(uint32_t neurons, uint32_t synapses)
    {
        NS_LOG_FUNCTION(this << neurons << synapses);

        m_neurons.clear();
        m_synapses.clear();

        // Initialize neurons
        for (uint32_t i = 0; i < neurons; ++i)
        {
            Neuron neuron;
            neuron.membrane_potential = -70.0; // Resting potential (mV)
            neuron.threshold = -55.0;          // Spike threshold (mV)
            neuron.refractory_time = 0.0;
            neuron.is_spiking = false;
            neuron.spike_history.reserve(1000);
            m_neurons.push_back(neuron);
        }

        // Initialize synapses randomly
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> neuron_dist(0, neurons - 1);
        std::uniform_real_distribution<> weight_dist(-1.0, 1.0);
        std::uniform_real_distribution<> delay_dist(0.1, 5.0);

        for (uint32_t i = 0; i < synapses; ++i)
        {
            Synapse synapse;
            synapse.pre_neuron = neuron_dist(gen);
            synapse.post_neuron = neuron_dist(gen);
            synapse.weight = weight_dist(gen);
            synapse.delay = delay_dist(gen);
            synapse.plasticity_factor = 1.0;
            m_synapses.push_back(synapse);
        }

        NS_LOG_INFO("Initialized spike network with " << neurons << " neurons and " << synapses << " synapses");
    }

    void
    NeuromorphicEngine::ProcessSpikePattern(const std::vector<double> &inputSpikes)
    {
        NS_LOG_FUNCTION(this << inputSpikes.size());

        // Apply input spikes to first neurons
        size_t inputSize = std::min(inputSpikes.size(), m_neurons.size());

        for (size_t i = 0; i < inputSize; ++i)
        {
            if (inputSpikes[i] > 0.5)
            {                                            // Threshold for spike
                m_neurons[i].membrane_potential += 20.0; // Input current
                m_neurons[i].spike_history.push_back(m_current_time);
            }
        }

        // Update all neurons
        UpdateNeuronStates(0.1); // 0.1ms time step

        NS_LOG_INFO("Processed spike pattern with " << inputSpikes.size() << " inputs");
    }

    void
    NeuromorphicEngine::UpdateNeuronStates(double timeStep)
    {
        NS_LOG_FUNCTION(this << timeStep);

        m_current_time += timeStep;

        for (auto &neuron : m_neurons)
        {
            // Update refractory period
            if (neuron.refractory_time > 0)
            {
                neuron.refractory_time -= timeStep;
                neuron.is_spiking = false;
                continue;
            }

            // Leaky integrate-and-fire dynamics
            double leak_factor = 0.95; // Membrane leak
            neuron.membrane_potential *= leak_factor;

            // Check for spike
            if (neuron.membrane_potential >= neuron.threshold && !neuron.is_spiking)
            {
                neuron.is_spiking = true;
                neuron.refractory_time = 2.0;      // 2ms refractory period
                neuron.membrane_potential = -80.0; // Reset potential
                neuron.spike_history.push_back(m_current_time);

                // Energy consumption for spike
                m_total_energy += 1e-12; // 1 pJ per spike
            }
        }
    }

    double
    NeuromorphicEngine::CalculatePowerConsumption()
    {
        NS_LOG_FUNCTION(this);

        // Calculate based on spike rate and number of neurons
        double total_spikes = 0;
        for (const auto &neuron : m_neurons)
        {
            total_spikes += neuron.spike_history.size();
        }

        double avg_spike_rate = total_spikes / (m_current_time + 1e-6) / m_neurons.size();
        double power = avg_spike_rate * m_neurons.size() * 1e-9; // nW per neuron per Hz

        NS_LOG_INFO("Neuromorphic power consumption: " << power << " W");
        return power;
    }

    // Quantum-Classical Hybrid AI Implementation

    QuantumClassicalHybridAI::QuantumClassicalHybridAI()
        : m_circuit_depth(0),
          m_decoherence_time(100.0), // 100 μs
          m_gate_fidelity(0.999)
    {
        NS_LOG_FUNCTION(this);
    }

    QuantumClassicalHybridAI::~QuantumClassicalHybridAI()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    QuantumClassicalHybridAI::InitializeQuantumCircuit(uint32_t qubits)
    {
        NS_LOG_FUNCTION(this << qubits);

        m_qubits.clear();
        m_qubits.resize(qubits);

        // Initialize all qubits in |0⟩ state
        for (auto &qubit : m_qubits)
        {
            qubit.amplitude0 = std::complex<double>(1.0, 0.0);
            qubit.amplitude1 = std::complex<double>(0.0, 0.0);
            qubit.is_measured = false;
        }

        m_circuit_depth = 0;

        NS_LOG_INFO("Initialized quantum circuit with " << qubits << " qubits");
    }

    void
    QuantumClassicalHybridAI::ApplyQuantumGate(const std::string &gate, const std::vector<uint32_t> &qubits)
    {
        NS_LOG_FUNCTION(this << gate << qubits.size());

        if (qubits.empty() || qubits[0] >= m_qubits.size())
        {
            NS_LOG_WARN("Invalid qubit index");
            return;
        }

        uint32_t qubit_idx = qubits[0];
        auto &qubit = m_qubits[qubit_idx];

        if (gate == "H")
        { // Hadamard gate
            std::complex<double> new_amp0 = (qubit.amplitude0 + qubit.amplitude1) / std::sqrt(2.0);
            std::complex<double> new_amp1 = (qubit.amplitude0 - qubit.amplitude1) / std::sqrt(2.0);
            qubit.amplitude0 = new_amp0;
            qubit.amplitude1 = new_amp1;
        }
        else if (gate == "X")
        { // Pauli-X gate
            std::swap(qubit.amplitude0, qubit.amplitude1);
        }
        else if (gate == "Z")
        { // Pauli-Z gate
            qubit.amplitude1 = -qubit.amplitude1;
        }
        else if (gate == "T")
        { // T gate
            qubit.amplitude1 *= std::complex<double>(std::cos(M_PI / 4), std::sin(M_PI / 4));
        }

        // Apply decoherence
        double fidelity = m_gate_fidelity * std::exp(-m_circuit_depth / m_decoherence_time);
        qubit.amplitude0 *= std::sqrt(fidelity);
        qubit.amplitude1 *= std::sqrt(fidelity);

        m_circuit_depth++;

        NS_LOG_INFO("Applied " << gate << " gate to qubit " << qubit_idx);
    }

    std::vector<std::complex<double>>
    QuantumClassicalHybridAI::MeasureQuantumState()
    {
        NS_LOG_FUNCTION(this);

        std::vector<std::complex<double>> state;
        for (const auto &qubit : m_qubits)
        {
            // Probability amplitudes
            state.push_back(qubit.amplitude0);
            state.push_back(qubit.amplitude1);
        }

        NS_LOG_INFO("Measured quantum state with " << m_qubits.size() << " qubits");
        return state;
    }

    // Main Oran6gEdgeAi Implementation

    TypeId
    Oran6gEdgeAi::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::Oran6gEdgeAi")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<Oran6gEdgeAi>()
                                .AddAttribute("MaxInferenceLatency",
                                              "Maximum allowed inference latency in milliseconds",
                                              DoubleValue(10.0),
                                              MakeDoubleAccessor(&Oran6gEdgeAi::m_maxInferenceLatency),
                                              MakeDoubleChecker<double>(0.0))
                                .AddAttribute("MaxEnergyBudget",
                                              "Maximum energy budget in Watts",
                                              DoubleValue(1000.0),
                                              MakeDoubleAccessor(&Oran6gEdgeAi::m_maxEnergyBudget),
                                              MakeDoubleChecker<double>(0.0))
                                .AddAttribute("MaxConcurrentTasks",
                                              "Maximum number of concurrent AI tasks",
                                              IntegerValue(100),
                                              MakeIntegerAccessor(&Oran6gEdgeAi::m_maxConcurrentTasks),
                                              MakeIntegerChecker<uint32_t>())
                                .AddAttribute("QuantumAccelerationEnabled",
                                              "Enable quantum acceleration for AI workloads",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&Oran6gEdgeAi::m_quantumAccelerationEnabled),
                                              MakeBooleanChecker())
                                .AddAttribute("NeuromorphicEnabled",
                                              "Enable neuromorphic processing for ultra-low power AI",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&Oran6gEdgeAi::m_neuromorphicEnabled),
                                              MakeBooleanChecker());

        return tid;
    }

    Oran6gEdgeAi::Oran6gEdgeAi()
        : m_maxInferenceLatency(10.0),
          m_maxEnergyBudget(1000.0),
          m_maxConcurrentTasks(100),
          m_quantumAccelerationEnabled(true),
          m_neuromorphicEnabled(true)
    {
        NS_LOG_FUNCTION(this);

        // Initialize core components
        m_fedLearningCoordinator = std::make_unique<FederatedLearningCoordinator>();
        m_neuromorphicEngine = std::make_unique<NeuromorphicEngine>();
        m_quantumHybridAI = std::make_unique<QuantumClassicalHybridAI>();

        // Initialize random variables
        InitializeRandomVariables();

        // Schedule periodic tasks
        SchedulePeriodicTasks();
    }

    Oran6gEdgeAi::~Oran6gEdgeAi()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    Oran6gEdgeAi::InitializePlatform()
    {
        NS_LOG_FUNCTION(this);

        // Initialize neuromorphic network
        if (m_neuromorphicEnabled)
        {
            m_neuromorphicEngine->InitializeSpikeNetwork(1000, 10000);
        }

        // Initialize quantum circuit
        if (m_quantumAccelerationEnabled)
        {
            m_quantumHybridAI->InitializeQuantumCircuit(16);
        }

        // Initialize federated learning
        m_fedLearningCoordinator->InitializeFederatedRound(0);

        NS_LOG_INFO("Edge AI platform initialized successfully");
    }

    void
    Oran6gEdgeAi::RegisterEdgeNode(const EdgeAiProcessingUnit &unit)
    {
        NS_LOG_FUNCTION(this << unit.unitId);

        m_edgeNodes[unit.unitId] = unit;

        NS_LOG_INFO("Registered edge node " << unit.unitId << " with " << unit.computeCapacity << " TOPS capacity");
    }

    uint32_t
    Oran6gEdgeAi::SelectOptimalNode(const std::string &workloadType, double complexity)
    {
        NS_LOG_FUNCTION(this << workloadType << complexity);

        if (m_edgeNodes.empty())
        {
            NS_LOG_WARN("No edge nodes available");
            return 0;
        }

        uint32_t bestNode = 0;
        double bestScore = -1.0;

        for (const auto &[nodeId, node] : m_edgeNodes)
        {
            // Calculate suitability score
            double utilization_factor = 1.0 - node.utilizationRate;
            double capacity_factor = node.computeCapacity / complexity;
            double energy_factor = 1.0 / (node.powerConsumption + 1.0);

            double score = utilization_factor * capacity_factor * energy_factor;

            // Bonus for specialized hardware
            if (workloadType == "quantum" && node.isQuantumEnabled)
            {
                score *= 2.0;
            }
            if (workloadType == "neuromorphic" && node.isNeuromorphicEnabled)
            {
                score *= 1.5;
            }

            if (score > bestScore)
            {
                bestScore = score;
                bestNode = nodeId;
            }
        }

        NS_LOG_INFO("Selected node " << bestNode << " for " << workloadType << " workload (score: " << bestScore << ")");

        return bestNode;
    }

    void
    Oran6gEdgeAi::DistributeInferenceWorkload(const std::vector<double> &input,
                                              const std::string &modelName)
    {
        NS_LOG_FUNCTION(this << input.size() << modelName);

        double complexity = CalculateComplexityScore(input);
        uint32_t selectedNode = SelectOptimalNode("inference", complexity);

        if (selectedNode == 0)
        {
            NS_LOG_WARN("No suitable node found for inference workload");
            return;
        }

        // Simulate inference latency
        double baseLatency = complexity / m_edgeNodes[selectedNode].computeCapacity;
        double latency = baseLatency + m_normalRandomVariable->GetValue(0.0, 1.0);

        m_inferenceLatencyHistory.push_back(latency);

        // Update node utilization
        m_edgeNodes[selectedNode].utilizationRate += 0.1;
        m_edgeNodes[selectedNode].utilizationRate =
            std::min(1.0, m_edgeNodes[selectedNode].utilizationRate);

        // Schedule callback
        Simulator::Schedule(MilliSeconds(latency),
                            &Oran6gEdgeAi::m_aiInferenceCompleteCallback,
                            this, selectedNode, latency);

        NS_LOG_INFO("Distributed inference workload to node " << selectedNode << " (estimated latency: " << latency << "ms)");
    }

    void
    Oran6gEdgeAi::StartFederatedLearning(const std::string &modelType, uint32_t participants)
    {
        NS_LOG_FUNCTION(this << modelType << participants);

        // Select participating nodes
        std::vector<uint32_t> participantNodes;
        for (const auto &[nodeId, node] : m_edgeNodes)
        {
            if (participantNodes.size() < participants && node.utilizationRate < 0.8)
            {
                participantNodes.push_back(nodeId);
            }
        }

        if (participantNodes.size() < 2)
        {
            NS_LOG_WARN("Insufficient nodes for federated learning");
            return;
        }

        // Initialize federated round
        static uint32_t roundCounter = 0;
        m_fedLearningCoordinator->InitializeFederatedRound(++roundCounter);

        // Simulate gradient generation from participants
        std::vector<std::vector<double>> gradients;
        for (uint32_t i = 0; i < participantNodes.size(); ++i)
        {
            std::vector<double> gradient(1000);
            for (auto &g : gradient)
            {
                g = m_normalRandomVariable->GetValue(0.0, 0.01);
            }
            gradients.push_back(gradient);
        }

        // Aggregate gradients
        m_fedLearningCoordinator->AggregateGradients(gradients);

        // Calculate and store accuracy
        double accuracy = 0.90 + 0.05 * m_uniformRandomVariable->GetValue(0.0, 1.0);
        m_accuracyHistory.push_back(accuracy);

        NS_LOG_INFO("Started federated learning round " << roundCounter << " with " << participantNodes.size() << " participants (accuracy: " << accuracy << ")");
    }

    double
    Oran6gEdgeAi::GetAverageInferenceLatency()
    {
        NS_LOG_FUNCTION(this);

        if (m_inferenceLatencyHistory.empty())
        {
            return 0.0;
        }

        double sum = std::accumulate(m_inferenceLatencyHistory.begin(),
                                     m_inferenceLatencyHistory.end(), 0.0);
        return sum / m_inferenceLatencyHistory.size();
    }

    double
    Oran6gEdgeAi::GetFederatedLearningAccuracy()
    {
        NS_LOG_FUNCTION(this);

        if (m_accuracyHistory.empty())
        {
            return 0.0;
        }

        return m_accuracyHistory.back(); // Return most recent accuracy
    }

    double
    Oran6gEdgeAi::GetEnergyEfficiency()
    {
        NS_LOG_FUNCTION(this);

        double totalPower = 0.0;
        double totalThroughput = 0.0;

        for (const auto &[nodeId, node] : m_edgeNodes)
        {
            totalPower += node.powerConsumption * node.utilizationRate;
            totalThroughput += node.computeCapacity * node.utilizationRate;
        }

        if (totalPower == 0.0)
        {
            return 0.0;
        }

        return totalThroughput / totalPower; // TOPS per Watt
    }

    std::map<std::string, double>
    Oran6gEdgeAi::GetDetailedMetrics()
    {
        NS_LOG_FUNCTION(this);

        std::map<std::string, double> metrics;

        metrics["average_inference_latency"] = GetAverageInferenceLatency();
        metrics["federated_learning_accuracy"] = GetFederatedLearningAccuracy();
        metrics["energy_efficiency"] = GetEnergyEfficiency();
        metrics["total_edge_nodes"] = static_cast<double>(m_edgeNodes.size());
        metrics["quantum_acceleration_enabled"] = m_quantumAccelerationEnabled ? 1.0 : 0.0;
        metrics["neuromorphic_enabled"] = m_neuromorphicEnabled ? 1.0 : 0.0;

        if (m_neuromorphicEnabled)
        {
            metrics["neuromorphic_power"] = m_neuromorphicEngine->CalculatePowerConsumption();
        }

        return metrics;
    }

    // Private Helper Methods

    void
    Oran6gEdgeAi::InitializeRandomVariables()
    {
        NS_LOG_FUNCTION(this);

        m_uniformRandomVariable = CreateObject<UniformRandomVariable>();
        m_normalRandomVariable = CreateObject<NormalRandomVariable>();
        m_exponentialRandomVariable = CreateObject<ExponentialRandomVariable>();
    }

    void
    Oran6gEdgeAi::SchedulePeriodicTasks()
    {
        NS_LOG_FUNCTION(this);

        // Schedule metrics update every 100ms
        Simulator::Schedule(MilliSeconds(100), &Oran6gEdgeAi::UpdateSystemMetrics, this);

        // Schedule load balancing every 1 second
        Simulator::Schedule(Seconds(1.0), &Oran6gEdgeAi::BalanceComputeLoad, this);
    }

    void
    Oran6gEdgeAi::UpdateSystemMetrics()
    {
        NS_LOG_FUNCTION(this);

        // Update node utilization decay
        for (auto &[nodeId, node] : m_edgeNodes)
        {
            node.utilizationRate *= 0.95; // Gradual decay
            node.utilizationRate = std::max(0.0, node.utilizationRate);
        }

        // Schedule next update
        Simulator::Schedule(MilliSeconds(100), &Oran6gEdgeAi::UpdateSystemMetrics, this);
    }

    void
    Oran6gEdgeAi::BalanceComputeLoad()
    {
        NS_LOG_FUNCTION(this);

        // Find heavily loaded and lightly loaded nodes
        std::vector<uint32_t> heavyNodes, lightNodes;

        for (const auto &[nodeId, node] : m_edgeNodes)
        {
            if (node.utilizationRate > 0.8)
            {
                heavyNodes.push_back(nodeId);
            }
            else if (node.utilizationRate < 0.3)
            {
                lightNodes.push_back(nodeId);
            }
        }

        // Migrate workloads if needed
        if (!heavyNodes.empty() && !lightNodes.empty())
        {
            uint32_t fromNode = heavyNodes[0];
            uint32_t toNode = lightNodes[0];
            MigrateAiWorkload(fromNode, toNode, "inference");
        }

        // Schedule next balancing
        Simulator::Schedule(Seconds(1.0), &Oran6gEdgeAi::BalanceComputeLoad, this);
    }

    double
    Oran6gEdgeAi::CalculateComplexityScore(const std::vector<double> &input)
    {
        NS_LOG_FUNCTION(this << input.size());

        // Simple complexity based on input size and variance
        double variance = 0.0;
        double mean = std::accumulate(input.begin(), input.end(), 0.0) / input.size();

        for (double value : input)
        {
            variance += (value - mean) * (value - mean);
        }
        variance /= input.size();

        return input.size() * (1.0 + variance); // Complexity score
    }

    void
    Oran6gEdgeAi::MigrateAiWorkload(uint32_t fromNode, uint32_t toNode, const std::string &workload)
    {
        NS_LOG_FUNCTION(this << fromNode << toNode << workload);

        if (m_edgeNodes.find(fromNode) == m_edgeNodes.end() ||
            m_edgeNodes.find(toNode) == m_edgeNodes.end())
        {
            NS_LOG_WARN("Invalid node IDs for migration");
            return;
        }

        // Simulate workload migration
        double migrationLoad = 0.2; // 20% load migration

        m_edgeNodes[fromNode].utilizationRate -= migrationLoad;
        m_edgeNodes[fromNode].utilizationRate = std::max(0.0, m_edgeNodes[fromNode].utilizationRate);

        m_edgeNodes[toNode].utilizationRate += migrationLoad;
        m_edgeNodes[toNode].utilizationRate = std::min(1.0, m_edgeNodes[toNode].utilizationRate);

        NS_LOG_INFO("Migrated " << workload << " workload from node " << fromNode << " to node " << toNode);
    }

} // namespace ns3
