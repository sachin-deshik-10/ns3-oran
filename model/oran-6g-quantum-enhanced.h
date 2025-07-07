/*
 * Copyright (c) 2025 O-RAN 6G Research Consortium
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * O-RAN 6G Quantum-Enhanced Communication Module
 * Advanced quantum communication, security, and sensing capabilities
 */

#ifndef ORAN_6G_QUANTUM_ENHANCED_H
#define ORAN_6G_QUANTUM_ENHANCED_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/vector.h"
#include "ns3/traced-callback.h"
#include "ns3/random-variable-stream.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
#include <complex>
#include <vector>
#include <map>

namespace ns3
{

    /**
     * \ingroup oran
     * \brief Quantum-Enhanced 6G Communication Module
     *
     * This class implements quantum communication capabilities including:
     * - Quantum Key Distribution (QKD)
     * - Post-Quantum Cryptography
     * - Quantum-Enhanced Sensing
     * - Quantum Machine Learning
     * - Quantum Network Protocols
     */
    class Oran6gQuantumEnhanced : public Object
    {
    public:
        /**
         * \brief Get the type ID.
         * \return the object TypeId
         */
        static TypeId GetTypeId(void);

        /**
         * Constructor
         */
        Oran6gQuantumEnhanced();

        /**
         * Destructor
         */
        virtual ~Oran6gQuantumEnhanced();

        // Quantum Communication Structures
        struct QuantumKey
        {
            std::vector<uint8_t> keyData;
            double fidelity;
            double securityLevel;
            Time timestamp;
        };

        struct QuantumState
        {
            std::complex<double> amplitude0;
            std::complex<double> amplitude1;
            double coherenceTime;
            double entanglementLevel;
        };

        struct QuantumChannel
        {
            uint32_t channelId;
            double transmissionFidelity;
            double decoherenceRate;
            Vector3D position;
            bool isEntangled;
        };

        // Quantum Key Distribution (QKD)
        /**
         * \brief Initialize Quantum Key Distribution
         * \param protocol QKD protocol (BB84, E91, SARG04)
         * \param keyLength Target key length in bits
         * \return Success status
         */
        bool InitializeQKD(const std::string &protocol, uint32_t keyLength);

        /**
         * \brief Perform quantum key exchange
         * \param remoteNode Partner node for key exchange
         * \return Generated quantum key
         */
        QuantumKey PerformKeyExchange(Ptr<Oran6gQuantumEnhanced> remoteNode);

        /**
         * \brief Verify quantum key integrity
         * \param key Quantum key to verify
         * \return Verification result and security metrics
         */
        std::pair<bool, double> VerifyKeyIntegrity(const QuantumKey &key);

        // Post-Quantum Cryptography
        /**
         * \brief Initialize post-quantum cryptographic algorithms
         * \param algorithm Algorithm type (Kyber, Dilithium, FALCON)
         * \return Initialization success
         */
        bool InitializePostQuantumCrypto(const std::string &algorithm);

        /**
         * \brief Encrypt data using post-quantum algorithms
         * \param data Data to encrypt
         * \param publicKey Public key for encryption
         * \return Encrypted data
         */
        std::vector<uint8_t> QuantumSafeEncrypt(const std::vector<uint8_t> &data,
                                                const std::vector<uint8_t> &publicKey);

        /**
         * \brief Decrypt data using post-quantum algorithms
         * \param encryptedData Encrypted data
         * \param privateKey Private key for decryption
         * \return Decrypted data
         */
        std::vector<uint8_t> QuantumSafeDecrypt(const std::vector<uint8_t> &encryptedData,
                                                const std::vector<uint8_t> &privateKey);

        // Quantum-Enhanced Sensing
        /**
         * \brief Initialize quantum sensing capabilities
         * \param sensorType Type of quantum sensor (magnetometer, gravimeter, clock)
         * \param sensitivity Target sensitivity level
         */
        void InitializeQuantumSensing(const std::string &sensorType, double sensitivity);

        /**
         * \brief Perform quantum-enhanced environment sensing
         * \param position Sensing position
         * \param parameters Sensing parameters
         * \return Sensing results with quantum advantage
         */
        std::map<std::string, double> PerformQuantumSensing(const Vector3D &position,
                                                            const std::map<std::string, double> &parameters);

        /**
         * \brief Create quantum sensor network
         * \param sensorPositions Positions of quantum sensors
         * \param entanglementTopology Network entanglement structure
         */
        void CreateQuantumSensorNetwork(const std::vector<Vector3D> &sensorPositions,
                                        const std::string &entanglementTopology);

        // Quantum Machine Learning
        /**
         * \brief Initialize quantum machine learning module
         * \param algorithm QML algorithm (VQE, QAOA, QNN)
         * \param qubits Number of qubits to use
         */
        void InitializeQuantumML(const std::string &algorithm, uint32_t qubits);

        /**
         * \brief Train quantum neural network
         * \param trainingData Training dataset
         * \param labels Training labels
         * \return Training metrics
         */
        std::map<std::string, double> TrainQuantumNeuralNetwork(
            const std::vector<std::vector<double>> &trainingData,
            const std::vector<int> &labels);

        /**
         * \brief Quantum-enhanced network optimization
         * \param networkState Current network state
         * \return Optimized network configuration
         */
        std::map<std::string, double> OptimizeNetworkQuantum(
            const std::map<std::string, double> &networkState);

        // Quantum Network Protocols
        /**
         * \brief Establish quantum entanglement between nodes
         * \param remoteNode Target node for entanglement
         * \param entanglementType Type of entanglement (Bell, GHZ, cluster)
         * \return Entanglement establishment success
         */
        bool EstablishQuantumEntanglement(Ptr<Oran6gQuantumEnhanced> remoteNode,
                                          const std::string &entanglementType);

        /**
         * \brief Perform quantum teleportation
         * \param quantumState State to teleport
         * \param targetNode Destination node
         * \return Teleportation success and fidelity
         */
        std::pair<bool, double> QuantumTeleportation(const QuantumState &quantumState,
                                                     Ptr<Oran6gQuantumEnhanced> targetNode);

        /**
         * \brief Quantum error correction
         * \param corruptedState Quantum state with errors
         * \param errorCorrectionCode Error correction code to use
         * \return Corrected quantum state
         */
        QuantumState QuantumErrorCorrection(const QuantumState &corruptedState,
                                            const std::string &errorCorrectionCode);

        // Performance and Metrics
        /**
         * \brief Get quantum communication performance metrics
         * \return Performance metrics map
         */
        std::map<std::string, double> GetQuantumMetrics() const;

        /**
         * \brief Calculate quantum advantage factor
         * \param classicalBenchmark Classical performance baseline
         * \return Quantum advantage factor
         */
        double CalculateQuantumAdvantage(double classicalBenchmark) const;

        /**
         * \brief Monitor quantum decoherence
         * \return Current decoherence metrics
         */
        std::map<std::string, double> MonitorDecoherence() const;

        // Configuration and Control
        /**
         * \brief Set quantum communication parameters
         * \param parameters Configuration parameters
         */
        void SetQuantumParameters(const std::map<std::string, double> &parameters);

        /**
         * \brief Enable quantum error correction
         * \param enable Enable/disable error correction
         * \param threshold Error correction threshold
         */
        void EnableQuantumErrorCorrection(bool enable, double threshold = 0.01);

        /**
         * \brief Set quantum channel noise model
         * \param noiseModel Noise model type
         * \param noiseLevel Noise intensity
         */
        void SetQuantumNoiseModel(const std::string &noiseModel, double noiseLevel);

    private:
        // Private member variables
        bool m_qkdInitialized;                //< QKD initialization status
        bool m_postQuantumCryptoEnabled;      //< Post-quantum crypto status
        bool m_quantumSensingEnabled;         //< Quantum sensing status
        bool m_quantumMLEnabled;              //< Quantum ML status
        bool m_quantumErrorCorrectionEnabled; //< Error correction status

        std::string m_qkdProtocol;          //< Current QKD protocol
        std::string m_postQuantumAlgorithm; //< Post-quantum algorithm
        std::string m_quantumSensorType;    //< Quantum sensor type
        std::string m_quantumMLAlgorithm;   //< Quantum ML algorithm
        std::string m_noiseModel;           //< Quantum noise model

        uint32_t m_keyLength;              //< Target key length
        uint32_t m_numQubits;              //< Number of qubits
        double m_targetSensitivity;        //< Sensing sensitivity
        double m_errorCorrectionThreshold; //< Error correction threshold
        double m_noiseLevel;               //< Quantum noise level
        double m_coherenceTime;            //< Quantum coherence time
        double m_entanglementFidelity;     //< Entanglement fidelity

        std::vector<QuantumKey> m_quantumKeys;               //< Generated quantum keys
        std::vector<QuantumChannel> m_quantumChannels;       //< Quantum channels
        std::map<std::string, QuantumState> m_quantumStates; //< Quantum states
        std::map<std::string, double> m_quantumMetrics;      //< Performance metrics

        Ptr<UniformRandomVariable> m_uniformRv; //< Random variable for quantum processes
        Ptr<NormalRandomVariable> m_normalRv;   //< Normal random variable for noise

        // Traced callbacks
        TracedCallback<double> m_quantumAdvantageTrace; //< Quantum advantage trace
        TracedCallback<double> m_entanglementTrace;     //< Entanglement fidelity trace
        TracedCallback<double> m_decoherenceTrace;      //< Decoherence rate trace

        // Private helper methods
        void InitializeQuantumStates();
        void UpdateQuantumMetrics();
        double SimulateQuantumProcess(const std::string &processType);
        QuantumState GenerateRandomQuantumState();
        double CalculateEntanglementFidelity(const QuantumState &state1, const QuantumState &state2);
        void ApplyQuantumNoise(QuantumState &state);
        std::vector<uint8_t> GenerateQuantumRandomBits(uint32_t numBits);
    };

} // namespace ns3

#endif /* ORAN_6G_QUANTUM_ENHANCED_H */
