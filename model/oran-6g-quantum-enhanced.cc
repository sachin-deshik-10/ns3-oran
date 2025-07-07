/*
 * Copyright (c) 2025 O-RAN 6G Research Consortium
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * O-RAN 6G Quantum-Enhanced Communication Implementation
 */

#include "oran-6g-quantum-enhanced.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/simulator.h"
#include <algorithm>
#include <cmath>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("Oran6gQuantumEnhanced");

    NS_OBJECT_ENSURE_REGISTERED(Oran6gQuantumEnhanced);

    TypeId
    Oran6gQuantumEnhanced::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::Oran6gQuantumEnhanced")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<Oran6gQuantumEnhanced>()
                                .AddAttribute("KeyLength",
                                              "Target quantum key length in bits",
                                              UintegerValue(256),
                                              MakeUintegerAccessor(&Oran6gQuantumEnhanced::m_keyLength),
                                              MakeUintegerChecker<uint32_t>())
                                .AddAttribute("NumQubits",
                                              "Number of qubits for quantum operations",
                                              UintegerValue(16),
                                              MakeUintegerAccessor(&Oran6gQuantumEnhanced::m_numQubits),
                                              MakeUintegerChecker<uint32_t>())
                                .AddAttribute("CoherenceTime",
                                              "Quantum coherence time in microseconds",
                                              DoubleValue(100.0),
                                              MakeDoubleAccessor(&Oran6gQuantumEnhanced::m_coherenceTime),
                                              MakeDoubleChecker<double>())
                                .AddAttribute("EntanglementFidelity",
                                              "Target entanglement fidelity",
                                              DoubleValue(0.95),
                                              MakeDoubleAccessor(&Oran6gQuantumEnhanced::m_entanglementFidelity),
                                              MakeDoubleChecker<double>())
                                .AddAttribute("NoiseLevel",
                                              "Quantum noise level (0.0 - 1.0)",
                                              DoubleValue(0.01),
                                              MakeDoubleAccessor(&Oran6gQuantumEnhanced::m_noiseLevel),
                                              MakeDoubleChecker<double>(0.0, 1.0))
                                .AddTraceSource("QuantumAdvantage",
                                                "Quantum advantage factor achieved",
                                                MakeTraceSourceAccessor(&Oran6gQuantumEnhanced::m_quantumAdvantageTrace),
                                                "ns3::TracedValueCallback::Double")
                                .AddTraceSource("EntanglementFidelity",
                                                "Current entanglement fidelity",
                                                MakeTraceSourceAccessor(&Oran6gQuantumEnhanced::m_entanglementTrace),
                                                "ns3::TracedValueCallback::Double")
                                .AddTraceSource("DecoherenceRate",
                                                "Current decoherence rate",
                                                MakeTraceSourceAccessor(&Oran6gQuantumEnhanced::m_decoherenceTrace),
                                                "ns3::TracedValueCallback::Double");
        return tid;
    }

    Oran6gQuantumEnhanced::Oran6gQuantumEnhanced()
        : m_qkdInitialized(false),
          m_postQuantumCryptoEnabled(false),
          m_quantumSensingEnabled(false),
          m_quantumMLEnabled(false),
          m_quantumErrorCorrectionEnabled(false),
          m_qkdProtocol("BB84"),
          m_postQuantumAlgorithm("Kyber"),
          m_quantumSensorType("magnetometer"),
          m_quantumMLAlgorithm("VQE"),
          m_noiseModel("depolarizing"),
          m_keyLength(256),
          m_numQubits(16),
          m_targetSensitivity(1e-15),
          m_errorCorrectionThreshold(0.01),
          m_noiseLevel(0.01),
          m_coherenceTime(100.0),
          m_entanglementFidelity(0.95)
    {
        NS_LOG_FUNCTION(this);

        // Initialize random number generators
        m_uniformRv = CreateObject<UniformRandomVariable>();
        m_normalRv = CreateObject<NormalRandomVariable>();
        m_normalRv->SetAttribute("Mean", DoubleValue(0.0));
        m_normalRv->SetAttribute("Variance", DoubleValue(1.0));

        InitializeQuantumStates();
    }

    Oran6gQuantumEnhanced::~Oran6gQuantumEnhanced()
    {
        NS_LOG_FUNCTION(this);
    }

    bool
    Oran6gQuantumEnhanced::InitializeQKD(const std::string &protocol, uint32_t keyLength)
    {
        NS_LOG_FUNCTION(this << protocol << keyLength);

        m_qkdProtocol = protocol;
        m_keyLength = keyLength;

        // Validate protocol
        if (protocol != "BB84" && protocol != "E91" && protocol != "SARG04")
        {
            NS_LOG_ERROR("Unsupported QKD protocol: " << protocol);
            return false;
        }

        // Initialize quantum states for QKD
        m_quantumStates.clear();
        for (uint32_t i = 0; i < keyLength; ++i)
        {
            std::string stateName = "qkd_qubit_" + std::to_string(i);
            m_quantumStates[stateName] = GenerateRandomQuantumState();
        }

        m_qkdInitialized = true;
        NS_LOG_INFO("QKD initialized with protocol " << protocol << " and key length " << keyLength);

        return true;
    }

    Oran6gQuantumEnhanced::QuantumKey
    Oran6gQuantumEnhanced::PerformKeyExchange(Ptr<Oran6gQuantumEnhanced> remoteNode)
    {
        NS_LOG_FUNCTION(this << remoteNode);

        if (!m_qkdInitialized)
        {
            NS_LOG_ERROR("QKD not initialized");
            return QuantumKey();
        }

        QuantumKey generatedKey;
        generatedKey.keyData = GenerateQuantumRandomBits(m_keyLength);
        generatedKey.timestamp = Simulator::Now();

        // Simulate quantum key distribution process
        double channelFidelity = 0.95 - m_noiseLevel;
        double errorRate = 1.0 - channelFidelity;

        // Apply quantum channel effects
        for (auto &bit : generatedKey.keyData)
        {
            if (m_uniformRv->GetValue() < errorRate)
            {
                bit = 1 - bit; // Flip bit due to channel error
            }
        }

        // Calculate key fidelity and security level
        generatedKey.fidelity = channelFidelity;
        generatedKey.securityLevel = std::min(channelFidelity, 0.99);

        // Store the key
        m_quantumKeys.push_back(generatedKey);

        NS_LOG_INFO("Quantum key exchange completed with fidelity " << generatedKey.fidelity);

        return generatedKey;
    }

    std::pair<bool, double>
    Oran6gQuantumEnhanced::VerifyKeyIntegrity(const QuantumKey &key)
    {
        NS_LOG_FUNCTION(this);

        // Simulate quantum key verification using error correction
        double errorRate = 1.0 - key.fidelity;
        double verificationSuccess = key.fidelity > 0.9;

        // Calculate security level based on key properties
        double securityLevel = key.securityLevel * (1.0 - errorRate);

        NS_LOG_INFO("Key verification: success=" << verificationSuccess << ", security=" << securityLevel);

        return std::make_pair(verificationSuccess, securityLevel);
    }

    bool
    Oran6gQuantumEnhanced::InitializePostQuantumCrypto(const std::string &algorithm)
    {
        NS_LOG_FUNCTION(this << algorithm);

        // Validate algorithm
        if (algorithm != "Kyber" && algorithm != "Dilithium" && algorithm != "FALCON")
        {
            NS_LOG_ERROR("Unsupported post-quantum algorithm: " << algorithm);
            return false;
        }

        m_postQuantumAlgorithm = algorithm;
        m_postQuantumCryptoEnabled = true;

        NS_LOG_INFO("Post-quantum cryptography initialized with " << algorithm);

        return true;
    }

    std::vector<uint8_t>
    Oran6gQuantumEnhanced::QuantumSafeEncrypt(const std::vector<uint8_t> &data,
                                              const std::vector<uint8_t> &publicKey)
    {
        NS_LOG_FUNCTION(this << data.size() << publicKey.size());

        if (!m_postQuantumCryptoEnabled)
        {
            NS_LOG_ERROR("Post-quantum cryptography not initialized");
            return std::vector<uint8_t>();
        }

        // Simulate post-quantum encryption
        std::vector<uint8_t> encryptedData = data;

        // Apply quantum-safe encryption transformation
        for (size_t i = 0; i < encryptedData.size(); ++i)
        {
            uint8_t keyByte = publicKey[i % publicKey.size()];
            encryptedData[i] ^= keyByte; // XOR with key (simplified)

            // Add post-quantum complexity simulation
            encryptedData[i] = static_cast<uint8_t>((encryptedData[i] + i) % 256);
        }

        NS_LOG_INFO("Data encrypted using " << m_postQuantumAlgorithm << " algorithm");

        return encryptedData;
    }

    std::vector<uint8_t>
    Oran6gQuantumEnhanced::QuantumSafeDecrypt(const std::vector<uint8_t> &encryptedData,
                                              const std::vector<uint8_t> &privateKey)
    {
        NS_LOG_FUNCTION(this << encryptedData.size() << privateKey.size());

        if (!m_postQuantumCryptoEnabled)
        {
            NS_LOG_ERROR("Post-quantum cryptography not initialized");
            return std::vector<uint8_t>();
        }

        // Simulate post-quantum decryption (reverse of encryption)
        std::vector<uint8_t> decryptedData = encryptedData;

        for (size_t i = 0; i < decryptedData.size(); ++i)
        {
            // Reverse post-quantum complexity
            decryptedData[i] = static_cast<uint8_t>((decryptedData[i] - i + 256) % 256);

            uint8_t keyByte = privateKey[i % privateKey.size()];
            decryptedData[i] ^= keyByte; // XOR with key
        }

        NS_LOG_INFO("Data decrypted using " << m_postQuantumAlgorithm << " algorithm");

        return decryptedData;
    }

    void
    Oran6gQuantumEnhanced::InitializeQuantumSensing(const std::string &sensorType, double sensitivity)
    {
        NS_LOG_FUNCTION(this << sensorType << sensitivity);

        m_quantumSensorType = sensorType;
        m_targetSensitivity = sensitivity;
        m_quantumSensingEnabled = true;

        NS_LOG_INFO("Quantum sensing initialized: " << sensorType << " with sensitivity " << sensitivity);
    }

    std::map<std::string, double>
    Oran6gQuantumEnhanced::PerformQuantumSensing(const Vector3D &position,
                                                 const std::map<std::string, double> &parameters)
    {
        NS_LOG_FUNCTION(this << position);

        std::map<std::string, double> sensingResults;

        if (!m_quantumSensingEnabled)
        {
            NS_LOG_ERROR("Quantum sensing not enabled");
            return sensingResults;
        }

        // Simulate quantum-enhanced sensing with improved sensitivity
        double quantumAdvantage = std::sqrt(static_cast<double>(m_numQubits)); // Heisenberg scaling
        double enhancedSensitivity = m_targetSensitivity / quantumAdvantage;

        // Generate sensing results based on sensor type
        if (m_quantumSensorType == "magnetometer")
        {
            sensingResults["magnetic_field_x"] = m_normalRv->GetValue() * enhancedSensitivity;
            sensingResults["magnetic_field_y"] = m_normalRv->GetValue() * enhancedSensitivity;
            sensingResults["magnetic_field_z"] = m_normalRv->GetValue() * enhancedSensitivity;
        }
        else if (m_quantumSensorType == "gravimeter")
        {
            sensingResults["gravitational_field"] = 9.81 + m_normalRv->GetValue() * enhancedSensitivity;
        }
        else if (m_quantumSensorType == "clock")
        {
            sensingResults["time_dilation"] = m_normalRv->GetValue() * enhancedSensitivity;
            sensingResults["frequency_stability"] = 1.0 + m_normalRv->GetValue() * enhancedSensitivity;
        }

        sensingResults["quantum_advantage"] = quantumAdvantage;
        sensingResults["sensitivity"] = enhancedSensitivity;

        NS_LOG_INFO("Quantum sensing performed with advantage factor " << quantumAdvantage);

        return sensingResults;
    }

    void
    Oran6gQuantumEnhanced::InitializeQuantumML(const std::string &algorithm, uint32_t qubits)
    {
        NS_LOG_FUNCTION(this << algorithm << qubits);

        m_quantumMLAlgorithm = algorithm;
        m_numQubits = qubits;
        m_quantumMLEnabled = true;

        NS_LOG_INFO("Quantum ML initialized: " << algorithm << " with " << qubits << " qubits");
    }

    std::map<std::string, double>
    Oran6gQuantumEnhanced::TrainQuantumNeuralNetwork(const std::vector<std::vector<double>> &trainingData,
                                                     const std::vector<int> &labels)
    {
        NS_LOG_FUNCTION(this << trainingData.size() << labels.size());

        std::map<std::string, double> trainingMetrics;

        if (!m_quantumMLEnabled)
        {
            NS_LOG_ERROR("Quantum ML not enabled");
            return trainingMetrics;
        }

        // Simulate quantum neural network training
        uint32_t numEpochs = 100;
        double learningRate = 0.01;
        double quantumAdvantage = std::sqrt(static_cast<double>(m_numQubits));

        // Simulate training convergence with quantum speedup
        double accuracy = 0.5; // Start with random accuracy
        for (uint32_t epoch = 0; epoch < numEpochs; ++epoch)
        {
            // Quantum-enhanced learning
            double improvement = learningRate * quantumAdvantage / (epoch + 1);
            accuracy = std::min(0.99, accuracy + improvement);
        }

        trainingMetrics["accuracy"] = accuracy;
        trainingMetrics["quantum_advantage"] = quantumAdvantage;
        trainingMetrics["training_time"] = numEpochs / quantumAdvantage; // Speedup
        trainingMetrics["convergence_rate"] = quantumAdvantage;

        NS_LOG_INFO("Quantum neural network training completed with accuracy " << accuracy);

        return trainingMetrics;
    }

    bool
    Oran6gQuantumEnhanced::EstablishQuantumEntanglement(Ptr<Oran6gQuantumEnhanced> remoteNode,
                                                        const std::string &entanglementType)
    {
        NS_LOG_FUNCTION(this << remoteNode << entanglementType);

        // Simulate quantum entanglement establishment
        double entanglementSuccess = m_entanglementFidelity * (1.0 - m_noiseLevel);

        if (m_uniformRv->GetValue() < entanglementSuccess)
        {
            // Create entangled quantum channel
            QuantumChannel channel;
            channel.channelId = m_quantumChannels.size();
            channel.transmissionFidelity = entanglementSuccess;
            channel.decoherenceRate = m_noiseLevel;
            channel.position = Vector3D(0, 0, 0); // Default position
            channel.isEntangled = true;

            m_quantumChannels.push_back(channel);

            // Update entanglement metrics
            m_entanglementTrace(entanglementSuccess);

            NS_LOG_INFO("Quantum entanglement established with fidelity " << entanglementSuccess);
            return true;
        }

        NS_LOG_WARN("Quantum entanglement establishment failed");
        return false;
    }

    std::map<std::string, double>
    Oran6gQuantumEnhanced::GetQuantumMetrics() const
    {
        NS_LOG_FUNCTION(this);

        std::map<std::string, double> metrics;

        metrics["qkd_initialized"] = m_qkdInitialized ? 1.0 : 0.0;
        metrics["post_quantum_crypto_enabled"] = m_postQuantumCryptoEnabled ? 1.0 : 0.0;
        metrics["quantum_sensing_enabled"] = m_quantumSensingEnabled ? 1.0 : 0.0;
        metrics["quantum_ml_enabled"] = m_quantumMLEnabled ? 1.0 : 0.0;
        metrics["num_qubits"] = static_cast<double>(m_numQubits);
        metrics["coherence_time"] = m_coherenceTime;
        metrics["entanglement_fidelity"] = m_entanglementFidelity;
        metrics["noise_level"] = m_noiseLevel;
        metrics["quantum_keys_generated"] = static_cast<double>(m_quantumKeys.size());
        metrics["quantum_channels"] = static_cast<double>(m_quantumChannels.size());

        // Calculate quantum advantage
        double quantumAdvantage = std::sqrt(static_cast<double>(m_numQubits)) * m_entanglementFidelity;
        metrics["quantum_advantage"] = quantumAdvantage;

        return metrics;
    }

    double
    Oran6gQuantumEnhanced::CalculateQuantumAdvantage(double classicalBenchmark) const
    {
        NS_LOG_FUNCTION(this << classicalBenchmark);

        // Calculate quantum advantage based on various factors
        double qubitAdvantage = std::sqrt(static_cast<double>(m_numQubits));
        double coherenceAdvantage = m_coherenceTime / 100.0; // Normalized
        double fidelityAdvantage = m_entanglementFidelity;
        double noiseReduction = 1.0 - m_noiseLevel;

        double quantumAdvantage = qubitAdvantage * coherenceAdvantage * fidelityAdvantage * noiseReduction;
        double advantageFactor = quantumAdvantage / classicalBenchmark;

        // Fire trace callback
        m_quantumAdvantageTrace(advantageFactor);

        return advantageFactor;
    }

    void
    Oran6gQuantumEnhanced::InitializeQuantumStates()
    {
        NS_LOG_FUNCTION(this);

        // Initialize default quantum states
        for (uint32_t i = 0; i < m_numQubits; ++i)
        {
            std::string stateName = "qubit_" + std::to_string(i);
            m_quantumStates[stateName] = GenerateRandomQuantumState();
        }

        UpdateQuantumMetrics();
    }

    void
    Oran6gQuantumEnhanced::UpdateQuantumMetrics()
    {
        NS_LOG_FUNCTION(this);

        // Update performance metrics
        m_quantumMetrics = GetQuantumMetrics();

        // Calculate and trace decoherence rate
        double decoherenceRate = m_noiseLevel * (1.0 / m_coherenceTime);
        m_decoherenceTrace(decoherenceRate);
    }

    Oran6gQuantumEnhanced::QuantumState
    Oran6gQuantumEnhanced::GenerateRandomQuantumState()
    {
        QuantumState state;

        // Generate random amplitudes (normalized)
        double real0 = m_normalRv->GetValue();
        double imag0 = m_normalRv->GetValue();
        double real1 = m_normalRv->GetValue();
        double imag1 = m_normalRv->GetValue();

        // Normalize
        double norm = std::sqrt(real0 * real0 + imag0 * imag0 + real1 * real1 + imag1 * imag1);
        state.amplitude0 = std::complex<double>(real0 / norm, imag0 / norm);
        state.amplitude1 = std::complex<double>(real1 / norm, imag1 / norm);

        state.coherenceTime = m_coherenceTime;
        state.entanglementLevel = m_entanglementFidelity;

        return state;
    }

    std::vector<uint8_t>
    Oran6gQuantumEnhanced::GenerateQuantumRandomBits(uint32_t numBits)
    {
        NS_LOG_FUNCTION(this << numBits);

        std::vector<uint8_t> randomBits;
        randomBits.reserve(numBits);

        for (uint32_t i = 0; i < numBits; ++i)
        {
            // Use quantum state measurement simulation
            uint8_t bit = (m_uniformRv->GetValue() < 0.5) ? 0 : 1;
            randomBits.push_back(bit);
        }

        return randomBits;
    }

} // namespace ns3
