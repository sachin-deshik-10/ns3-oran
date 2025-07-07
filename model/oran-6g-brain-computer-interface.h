/*
 * Copyright (c) 2025 O-RAN Alliance
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Advanced O-RAN Research Team
 */

#ifndef ORAN_6G_BRAIN_COMPUTER_INTERFACE_H
#define ORAN_6G_BRAIN_COMPUTER_INTERFACE_H

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include "ns3/simulation-time.h"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <complex>

namespace ns3
{

    /**
     * @brief Brain signal types for different BCI applications
     */
    enum class BrainSignalType
    {
        EEG,          ///< Electroencephalography
        FMRI,         ///< Functional Magnetic Resonance Imaging
        FNIRS,        ///< Functional Near-Infrared Spectroscopy
        ECOG,         ///< Electrocorticography
        LFP,          ///< Local Field Potentials
        SPIKE_TRAINS, ///< Neural spike trains
        MULTIMODAL    ///< Combined brain signals
    };

    /**
     * @brief Neural interface types
     */
    enum class NeuralInterfaceType
    {
        INVASIVE,      ///< Implanted electrodes
        SEMI_INVASIVE, ///< Partially implanted sensors
        NON_INVASIVE,  ///< External sensors (EEG, fNIRS)
        OPTICAL,       ///< Optical neural interfaces
        WIRELESS,      ///< Wireless neural dust/motes
        HYBRID         ///< Combination of interface types
    };

    /**
     * @brief BCI application types
     */
    enum class BciApplicationType
    {
        MOTOR_CONTROL,         ///< Motor imagery and control
        COMMUNICATION,         ///< Thought-to-text/speech
        COGNITIVE_ENHANCEMENT, ///< Cognitive augmentation
        SENSORY_SUBSTITUTION,  ///< Sensory feedback systems
        MEMORY_AUGMENTATION,   ///< Memory enhancement
        EMOTION_REGULATION,    ///< Emotional state control
        ATTENTION_CONTROL,     ///< Attention and focus control
        NEURAL_PROSTHETICS,    ///< Neural prosthetic devices
        BRAIN_NETWORKING,      ///< Brain-to-brain communication
        METAVERSE_INTERFACE    ///< Metaverse neural interaction
    };

    /**
     * @brief Neural signal processing parameters
     */
    struct NeuralSignalProcessing
    {
        double sampling_rate;                  ///< Signal sampling rate (Hz)
        std::vector<double> frequency_bands;   ///< Frequency bands of interest
        double signal_to_noise_ratio;          ///< SNR of neural signals
        std::string preprocessing_pipeline;    ///< Signal preprocessing steps
        std::string feature_extraction_method; ///< Feature extraction algorithm
        std::string classification_algorithm;  ///< Classification/decoding algorithm
        double real_time_latency;              ///< Real-time processing latency (ms)
        bool adaptive_filtering;               ///< Enable adaptive filtering
    };

    /**
     * @brief Brain state representation
     */
    struct BrainState
    {
        std::map<std::string, double> cognitive_load;
        std::map<std::string, double> emotional_state;
        std::map<std::string, double> attention_levels;
        std::map<std::string, double> motor_intentions;
        std::vector<double> neural_features;
        double consciousness_level;
        Time timestamp;
        uint32_t confidence_score;
    };

    /**
     * @brief Neural network architecture for BCI
     */
    struct NeuralNetworkArchitecture
    {
        std::string network_type;          ///< Network type (CNN, LSTM, Transformer, etc.)
        std::vector<uint32_t> layer_sizes; ///< Layer sizes
        std::string activation_function;   ///< Activation function
        double learning_rate;              ///< Learning rate
        uint32_t epochs;                   ///< Training epochs
        std::string optimizer;             ///< Optimization algorithm
        bool transfer_learning;            ///< Enable transfer learning
        std::string pretrained_model;      ///< Pre-trained model path
    };

    /**
     * @brief Haptic feedback configuration
     */
    struct HapticFeedbackConfig
    {
        std::vector<std::string> feedback_modalities; ///< Types of haptic feedback
        double feedback_intensity;                    ///< Intensity level (0-1)
        double feedback_latency;                      ///< Feedback latency (ms)
        bool adaptive_feedback;                       ///< Adaptive feedback based on brain state
        std::string spatial_encoding;                 ///< Spatial encoding method
        double temporal_resolution;                   ///< Temporal resolution (ms)
    };

    /**
     * @brief Brain-to-brain communication protocol
     */
    struct BrainToBrainProtocol
    {
        std::string encoding_method;        ///< Neural encoding method
        std::string transmission_protocol;  ///< Communication protocol
        double transmission_rate;           ///< Transmission rate (bits/second)
        std::string error_correction;       ///< Error correction method
        bool bidirectional;                 ///< Bidirectional communication
        std::string synchronization_method; ///< Brain synchronization method
        double latency_requirement;         ///< Maximum allowable latency (ms)
    };

    /**
     * @brief Neurofeedback training parameters
     */
    struct NeurofeedbackTraining
    {
        std::string training_protocol;                        ///< Training protocol
        std::vector<std::string> target_brain_states;         ///< Target brain states
        double training_duration;                             ///< Training session duration (minutes)
        std::string feedback_type;                            ///< Type of feedback (visual, auditory, haptic)
        bool real_time_feedback;                              ///< Real-time feedback enabled
        double adaptation_rate;                               ///< Adaptation rate for training
        std::map<std::string, double> performance_thresholds; ///< Performance thresholds
    };

    /**
     * @brief 6G Brain-Computer Interface Framework
     *
     * This class implements an advanced brain-computer interface system leveraging
     * 6G networks for ultra-low latency, high-bandwidth neural communication,
     * enabling revolutionary human-AI interaction and brain-networked applications.
     *
     * Key Features:
     * - Multi-modal neural signal processing
     * - Real-time brain state decoding and prediction
     * - Brain-to-brain communication networks
     * - Neural prosthetics and motor control
     * - Cognitive enhancement and augmentation
     * - Metaverse neural interfaces
     * - Adaptive neurofeedback systems
     * - Privacy-preserving neural computing
     * - Distributed neural processing
     * - Quantum-enhanced neural security
     */
    class Oran6gBrainComputerInterface : public Object
    {
    public:
        /**
         * @brief Get the type ID
         * @return type ID
         */
        static TypeId GetTypeId(void);

        /**
         * @brief Constructor
         */
        Oran6gBrainComputerInterface();

        /**
         * @brief Destructor
         */
        virtual ~Oran6gBrainComputerInterface();

        // Core BCI Functionality

        /**
         * @brief Initialize BCI system
         * @param interface_type Type of neural interface
         * @param signal_processing Signal processing configuration
         * @param application_type Target BCI application
         */
        void InitializeBciSystem(NeuralInterfaceType interface_type,
                                 const NeuralSignalProcessing &signal_processing,
                                 BciApplicationType application_type);

        /**
         * @brief Acquire neural signals
         * @param node_id Node ID for signal acquisition
         * @param signal_type Type of brain signal to acquire
         * @param duration Duration of signal acquisition
         * @return Acquired neural signal data
         */
        std::vector<std::vector<double>> AcquireNeuralSignals(uint32_t node_id,
                                                              BrainSignalType signal_type,
                                                              Time duration);

        /**
         * @brief Process neural signals in real-time
         * @param raw_signals Raw neural signal data
         * @param processing_params Processing parameters
         * @return Processed neural features
         */
        std::vector<double> ProcessNeuralSignals(const std::vector<std::vector<double>> &raw_signals,
                                                 const NeuralSignalProcessing &processing_params);

        /**
         * @brief Decode brain state from neural signals
         * @param neural_features Processed neural features
         * @param decoding_model Brain state decoding model
         * @return Decoded brain state
         */
        BrainState DecodeBrainState(const std::vector<double> &neural_features,
                                    const std::string &decoding_model);

        // Real-Time Neural Processing

        /**
         * @brief Enable real-time neural processing
         * @param processing_latency_target Target processing latency (ms)
         * @param bandwidth_requirement Bandwidth requirement (Mbps)
         * @param reliability_requirement Reliability requirement (0-1)
         */
        void EnableRealTimeNeuralProcessing(double processing_latency_target,
                                            double bandwidth_requirement,
                                            double reliability_requirement);

        /**
         * @brief Configure edge neural computing
         * @param edge_nodes Edge computing nodes for neural processing
         * @param processing_distribution Processing distribution strategy
         * @param load_balancing Load balancing algorithm
         */
        void ConfigureEdgeNeuralComputing(const NodeContainer &edge_nodes,
                                          const std::string &processing_distribution,
                                          const std::string &load_balancing);

        /**
         * @brief Optimize neural processing pipeline
         * @param optimization_objective Optimization objective
         * @param resource_constraints Resource constraints
         * @return Optimization success rate
         */
        double OptimizeNeuralProcessingPipeline(const std::string &optimization_objective,
                                                const std::map<std::string, double> &resource_constraints);

        // Brain-Computer Interaction

        /**
         * @brief Configure motor control interface
         * @param control_type Type of motor control
         * @param degrees_of_freedom Number of degrees of freedom
         * @param control_precision Control precision requirement
         */
        void ConfigureMotorControlInterface(const std::string &control_type,
                                            uint32_t degrees_of_freedom,
                                            double control_precision);

        /**
         * @brief Enable thought-to-text communication
         * @param language_model Language model for text generation
         * @param vocabulary_size Vocabulary size
         * @param typing_speed_target Target typing speed (words/minute)
         */
        void EnableThoughtToTextCommunication(const std::string &language_model,
                                              uint32_t vocabulary_size,
                                              double typing_speed_target);

        /**
         * @brief Configure cognitive enhancement
         * @param enhancement_type Type of cognitive enhancement
         * @param enhancement_protocols Enhancement protocols
         * @param adaptation_algorithm Adaptation algorithm
         */
        void ConfigureCognitiveEnhancement(const std::string &enhancement_type,
                                           const std::vector<std::string> &enhancement_protocols,
                                           const std::string &adaptation_algorithm);

        // Brain-to-Brain Communication

        /**
         * @brief Establish brain-to-brain connection
         * @param source_node Source brain node
         * @param target_node Target brain node
         * @param communication_protocol Communication protocol
         * @return Connection establishment success
         */
        bool EstablishBrainToBrainConnection(Ptr<Node> source_node,
                                             Ptr<Node> target_node,
                                             const BrainToBrainProtocol &communication_protocol);

        /**
         * @brief Transmit neural information
         * @param brain_state Brain state to transmit
         * @param destination_node Destination brain node
         * @param encoding_method Neural encoding method
         * @return Transmission success rate
         */
        double TransmitNeuralInformation(const BrainState &brain_state,
                                         Ptr<Node> destination_node,
                                         const std::string &encoding_method);

        /**
         * @brief Create brain network
         * @param participant_nodes Participating brain nodes
         * @param network_topology Network topology
         * @param synchronization_protocol Synchronization protocol
         */
        void CreateBrainNetwork(const NodeContainer &participant_nodes,
                                const std::string &network_topology,
                                const std::string &synchronization_protocol);

        // Neural Prosthetics and Assistive Technologies

        /**
         * @brief Configure neural prosthetic device
         * @param prosthetic_type Type of prosthetic device
         * @param control_interface Control interface configuration
         * @param sensory_feedback Sensory feedback configuration
         */
        void ConfigureNeuralProsthetic(const std::string &prosthetic_type,
                                       const std::map<std::string, std::string> &control_interface,
                                       const HapticFeedbackConfig &sensory_feedback);

        /**
         * @brief Enable sensory substitution
         * @param source_modality Source sensory modality
         * @param target_modality Target sensory modality
         * @param conversion_algorithm Sensory conversion algorithm
         */
        void EnableSensorySubstitution(const std::string &source_modality,
                                       const std::string &target_modality,
                                       const std::string &conversion_algorithm);

        /**
         * @brief Configure memory augmentation
         * @param memory_type Type of memory to augment
         * @param augmentation_strategy Memory augmentation strategy
         * @param storage_capacity Storage capacity requirement
         */
        void ConfigureMemoryAugmentation(const std::string &memory_type,
                                         const std::string &augmentation_strategy,
                                         uint64_t storage_capacity);

        // Metaverse Neural Interfaces

        /**
         * @brief Initialize metaverse neural interface
         * @param virtual_environment Virtual environment configuration
         * @param immersion_level Target immersion level (0-1)
         * @param interaction_modalities Interaction modalities
         */
        void InitializeMetaverseNeuralInterface(const std::map<std::string, std::string> &virtual_environment,
                                                double immersion_level,
                                                const std::vector<std::string> &interaction_modalities);

        /**
         * @brief Enable neural avatar control
         * @param avatar_configuration Avatar configuration parameters
         * @param control_mapping Neural-to-avatar control mapping
         * @param synchronization_quality Quality of neural-avatar synchronization
         */
        void EnableNeuralAvatarControl(const std::map<std::string, std::string> &avatar_configuration,
                                       const std::map<std::string, std::string> &control_mapping,
                                       double synchronization_quality);

        /**
         * @brief Configure immersive neural feedback
         * @param feedback_modalities Feedback modalities (visual, auditory, haptic)
         * @param feedback_fidelity Feedback fidelity level
         * @param latency_requirement Maximum feedback latency (ms)
         */
        void ConfigureImmersiveNeuralFeedback(const std::vector<std::string> &feedback_modalities,
                                              double feedback_fidelity,
                                              double latency_requirement);

        // Learning and Adaptation

        /**
         * @brief Train neural decoding models
         * @param training_data Neural training data
         * @param network_architecture Neural network architecture
         * @param training_strategy Training strategy
         * @return Training success rate
         */
        double TrainNeuralDecodingModels(const std::vector<std::pair<std::vector<double>, BrainState>> &training_data,
                                         const NeuralNetworkArchitecture &network_architecture,
                                         const std::string &training_strategy);

        /**
         * @brief Enable adaptive BCI learning
         * @param adaptation_algorithm Adaptation algorithm
         * @param adaptation_rate Learning adaptation rate
         * @param personalization_level Level of personalization (0-1)
         */
        void EnableAdaptiveBciLearning(const std::string &adaptation_algorithm,
                                       double adaptation_rate,
                                       double personalization_level);

        /**
         * @brief Configure neurofeedback training
         * @param training_params Neurofeedback training parameters
         * @param user_profile User profile for personalized training
         */
        void ConfigureNeurofeedbackTraining(const NeurofeedbackTraining &training_params,
                                            const std::map<std::string, std::string> &user_profile);

        // Security and Privacy

        /**
         * @brief Enable neural data encryption
         * @param encryption_algorithm Encryption algorithm
         * @param key_management_system Key management system
         * @param homomorphic_computation Enable homomorphic computation
         */
        void EnableNeuralDataEncryption(const std::string &encryption_algorithm,
                                        const std::string &key_management_system,
                                        bool homomorphic_computation);

        /**
         * @brief Configure differential privacy for neural data
         * @param privacy_budget Privacy budget (epsilon)
         * @param noise_mechanism Noise mechanism
         * @param utility_preservation Utility preservation strategy
         */
        void ConfigureDifferentialPrivacy(double privacy_budget,
                                          const std::string &noise_mechanism,
                                          const std::string &utility_preservation);

        /**
         * @brief Enable biometric neural authentication
         * @param authentication_method Authentication method
         * @param security_level Security level requirement
         * @param false_acceptance_rate Maximum false acceptance rate
         */
        void EnableBiometricNeuralAuthentication(const std::string &authentication_method,
                                                 const std::string &security_level,
                                                 double false_acceptance_rate);

        // Performance Monitoring and Diagnostics

        /**
         * @brief Monitor BCI performance
         * @param performance_metrics Performance metrics to monitor
         * @return Real-time performance data
         */
        std::map<std::string, double> MonitorBciPerformance(const std::vector<std::string> &performance_metrics);

        /**
         * @brief Diagnose neural interface health
         * @param diagnostic_tests Diagnostic tests to perform
         * @return Diagnostic results
         */
        std::map<std::string, std::string> DiagnoseNeuralInterfaceHealth(const std::vector<std::string> &diagnostic_tests);

        /**
         * @brief Assess neural signal quality
         * @param signal_data Neural signal data
         * @param quality_metrics Quality metrics to assess
         * @return Signal quality assessment
         */
        std::map<std::string, double> AssessNeuralSignalQuality(const std::vector<std::vector<double>> &signal_data,
                                                                const std::vector<std::string> &quality_metrics);

        // Integration and Interoperability

        /**
         * @brief Integrate with AI systems
         * @param ai_systems Available AI systems
         * @param integration_protocols Integration protocols
         */
        void IntegrateWithAiSystems(const std::map<std::string, Ptr<Object>> &ai_systems,
                                    const std::vector<std::string> &integration_protocols);

        /**
         * @brief Configure multi-modal sensor fusion
         * @param sensor_types Sensor types for fusion
         * @param fusion_algorithm Sensor fusion algorithm
         * @param confidence_weighting Confidence weighting strategy
         */
        void ConfigureMultiModalSensorFusion(const std::vector<std::string> &sensor_types,
                                             const std::string &fusion_algorithm,
                                             const std::string &confidence_weighting);

        /**
         * @brief Enable cross-platform compatibility
         * @param platform_standards Supported platform standards
         * @param data_formats Supported data formats
         * @param api_protocols API protocols for interoperability
         */
        void EnableCrossPlatformCompatibility(const std::vector<std::string> &platform_standards,
                                              const std::vector<std::string> &data_formats,
                                              const std::vector<std::string> &api_protocols);

        // Analytics and Reporting

        /**
         * @brief Get BCI performance statistics
         * @return BCI performance statistics
         */
        std::map<std::string, double> GetBciPerformanceStatistics() const;

        /**
         * @brief Get neural processing analytics
         * @return Neural processing analytics
         */
        std::map<std::string, std::vector<double>> GetNeuralProcessingAnalytics() const;

        /**
         * @brief Generate BCI system report
         * @param report_format Report format (JSON, XML, CSV)
         * @return Generated report
         */
        std::string GenerateBciSystemReport(const std::string &report_format) const;

    private:
        // Private member variables
        NeuralInterfaceType m_interface_type;
        BciApplicationType m_application_type;
        NeuralSignalProcessing m_signal_processing_params;
        std::map<uint32_t, BrainState> m_brain_states;
        std::map<std::string, double> m_performance_metrics;

        // Neural processing components
        std::map<std::string, std::vector<double>> m_neural_models;
        std::vector<BrainToBrainProtocol> m_brain_connections;
        std::map<uint32_t, HapticFeedbackConfig> m_haptic_configs;

        // Configuration flags
        bool m_real_time_processing_enabled;
        bool m_adaptive_learning_enabled;
        bool m_privacy_protection_enabled;
        bool m_metaverse_interface_enabled;
        bool m_brain_networking_enabled;

        // Performance tracking
        std::map<std::string, std::vector<double>> m_processing_analytics;
        std::vector<std::map<std::string, double>> m_performance_history;

        // Private helper methods
        void InitializeNeuralProcessingPipeline();
        std::vector<double> FilterNeuralSignals(const std::vector<double> &signals, const std::vector<double> &filter_params);
        std::vector<double> ExtractNeuralFeatures(const std::vector<std::vector<double>> &processed_signals);
        BrainState ClassifyBrainState(const std::vector<double> &features);
        void UpdateBrainStateHistory(uint32_t node_id, const BrainState &state);
        double CalculateProcessingLatency(Time start_time, Time end_time);
        void UpdatePerformanceMetrics(const std::map<std::string, double> &metrics);
        bool ValidateNeuralSignalQuality(const std::vector<std::vector<double>> &signals);
        void ApplyPrivacyProtection(std::vector<double> &neural_data);
        std::vector<double> PredictBrainState(const std::vector<BrainState> &history, Time prediction_horizon);
    };

} // namespace ns3

#endif /* ORAN_6G_BRAIN_COMPUTER_INTERFACE_H */
