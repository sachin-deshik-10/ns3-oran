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

#include "oran-6g-brain-computer-interface.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("Oran6gBrainComputerInterface");

    NS_OBJECT_ENSURE_REGISTERED(Oran6gBrainComputerInterface);

    TypeId
    Oran6gBrainComputerInterface::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::Oran6gBrainComputerInterface")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<Oran6gBrainComputerInterface>()
                                .AddAttribute("SamplingRate",
                                              "Neural signal sampling rate in Hz",
                                              DoubleValue(1000.0),
                                              MakeDoubleAccessor(&Oran6gBrainComputerInterface::m_samplingRate),
                                              MakeDoubleChecker<double>())
                                .AddAttribute("ProcessingLatency",
                                              "Real-time processing latency in milliseconds",
                                              DoubleValue(10.0),
                                              MakeDoubleAccessor(&Oran6gBrainComputerInterface::m_processingLatency),
                                              MakeDoubleChecker<double>())
                                .AddAttribute("SignalToNoiseRatio",
                                              "Signal-to-noise ratio of neural signals",
                                              DoubleValue(20.0),
                                              MakeDoubleAccessor(&Oran6gBrainComputerInterface::m_snr),
                                              MakeDoubleChecker<double>())
                                .AddAttribute("EnableAdaptiveFiltering",
                                              "Enable adaptive signal filtering",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&Oran6gBrainComputerInterface::m_adaptiveFiltering),
                                              MakeBooleanChecker())
                                .AddAttribute("NumberOfChannels",
                                              "Number of neural recording channels",
                                              UintegerValue(64),
                                              MakeUintegerAccessor(&Oran6gBrainComputerInterface::m_numChannels),
                                              MakeUintegerChecker<uint32_t>());
        return tid;
    }

    Oran6gBrainComputerInterface::Oran6gBrainComputerInterface()
        : m_samplingRate(1000.0),
          m_processingLatency(10.0),
          m_snr(20.0),
          m_adaptiveFiltering(true),
          m_numChannels(64),
          m_isCalibrated(false),
          m_isActive(false),
          m_totalProcessedSignals(0),
          m_avgDecodingAccuracy(0.85),
          m_neuralNetworkLayers(3)
    {
        NS_LOG_FUNCTION(this);
        InitializeNeuralProcessing();
        InitializeBrainStates();
        SetupDefaultFrequencyBands();
    }

    Oran6gBrainComputerInterface::~Oran6gBrainComputerInterface()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    Oran6gBrainComputerInterface::DoDispose()
    {
        NS_LOG_FUNCTION(this);
        m_bcis.clear();
        m_brainStates.clear();
        m_neuralSignals.clear();
        m_applicationConnections.clear();
        Object::DoDispose();
    }

    void
    Oran6gBrainComputerInterface::InitializeNeuralProcessing()
    {
        NS_LOG_FUNCTION(this);

        // Initialize neural signal processing pipeline
        m_signalProcessing.sampling_rate = m_samplingRate;
        m_signalProcessing.signal_to_noise_ratio = m_snr;
        m_signalProcessing.real_time_latency = m_processingLatency;
        m_signalProcessing.adaptive_filtering = m_adaptiveFiltering;
        m_signalProcessing.preprocessing_pipeline = "bandpass->notch->car->ica";
        m_signalProcessing.feature_extraction_method = "csp+psd+connectivity";
        m_signalProcessing.classification_algorithm = "ensemble_ml+deep_learning";

        // Initialize machine learning models
        InitializeMLModels();

        NS_LOG_INFO("Neural processing pipeline initialized");
    }

    void
    Oran6gBrainComputerInterface::InitializeBrainStates()
    {
        NS_LOG_FUNCTION(this);

        // Initialize default brain state templates
        BrainState defaultState;
        defaultState.cognitive_load["attention"] = 0.5;
        defaultState.cognitive_load["working_memory"] = 0.4;
        defaultState.cognitive_load["executive_control"] = 0.3;

        defaultState.emotional_state["valence"] = 0.0;
        defaultState.emotional_state["arousal"] = 0.5;
        defaultState.emotional_state["stress"] = 0.2;

        defaultState.attention_levels["focused"] = 0.6;
        defaultState.attention_levels["sustained"] = 0.5;
        defaultState.attention_levels["divided"] = 0.3;

        defaultState.motor_intentions["left_hand"] = 0.0;
        defaultState.motor_intentions["right_hand"] = 0.0;
        defaultState.motor_intentions["feet"] = 0.0;
        defaultState.motor_intentions["tongue"] = 0.0;

        defaultState.consciousness_level = 0.8;
        defaultState.confidence_score = 75;
        defaultState.timestamp = Simulator::Now();

        m_currentBrainState = defaultState;

        NS_LOG_INFO("Brain state templates initialized");
    }

    void
    Oran6gBrainComputerInterface::SetupDefaultFrequencyBands()
    {
        NS_LOG_FUNCTION(this);

        // Setup standard EEG frequency bands
        m_signalProcessing.frequency_bands = {
            1.0, // Delta (0.5-4 Hz)
            4.0,
            8.0,  // Theta (4-8 Hz)
            12.0, // Alpha (8-12 Hz)
            30.0, // Beta (12-30 Hz)
            100.0 // Gamma (30-100 Hz)
        };

        NS_LOG_INFO("Default frequency bands configured");
    }

    void
    Oran6gBrainComputerInterface::InitializeMLModels()
    {
        NS_LOG_FUNCTION(this);

        // Initialize different ML models for various BCI tasks
        MLModel motorImageryModel;
        motorImageryModel.model_type = "CNN-LSTM";
        motorImageryModel.accuracy = 0.92;
        motorImageryModel.latency_ms = 15.0;
        motorImageryModel.is_trained = true;
        motorImageryModel.training_samples = 50000;
        m_mlModels["motor_imagery"] = motorImageryModel;

        MLModel p300Model;
        p300Model.model_type = "SVM-Ensemble";
        p300Model.accuracy = 0.89;
        p300Model.latency_ms = 8.0;
        p300Model.is_trained = true;
        p300Model.training_samples = 30000;
        m_mlModels["p300_speller"] = p300Model;

        MLModel cognitiveLoadModel;
        cognitiveLoadModel.model_type = "Random-Forest";
        cognitiveLoadModel.accuracy = 0.85;
        cognitiveLoadModel.latency_ms = 5.0;
        cognitiveLoadModel.is_trained = true;
        cognitiveLoadModel.training_samples = 75000;
        m_mlModels["cognitive_load"] = cognitiveLoadModel;

        NS_LOG_INFO("ML models initialized for BCI tasks");
    }

    Ptr<BrainComputerInterface>
    Oran6gBrainComputerInterface::CreateBCI(BrainSignalType signalType,
                                            NeuralInterfaceType interfaceType,
                                            BciApplicationType applicationType)
    {
        NS_LOG_FUNCTION(this << static_cast<int>(signalType) << static_cast<int>(interfaceType) << static_cast<int>(applicationType));

        auto bci = CreateObject<BrainComputerInterface>();
        bci->signal_type = signalType;
        bci->interface_type = interfaceType;
        bci->application_type = applicationType;
        bci->bci_id = m_bcis.size();
        bci->is_active = false;
        bci->is_calibrated = false;
        bci->processing_latency = CalculateProcessingLatency(signalType, interfaceType);
        bci->decoding_accuracy = CalculateDecodingAccuracy(signalType, applicationType);
        bci->signal_quality = 0.8;
        bci->adaptation_rate = 0.1;
        bci->creation_time = Simulator::Now();

        // Configure signal processing parameters based on BCI type
        ConfigureBCIProcessing(bci);

        m_bcis.push_back(bci);

        NS_LOG_INFO("Created BCI with ID: " << bci->bci_id);
        return bci;
    }

    void
    Oran6gBrainComputerInterface::ConfigureBCIProcessing(Ptr<BrainComputerInterface> bci)
    {
        NS_LOG_FUNCTION(this << bci->bci_id);

        bci->signal_processing = m_signalProcessing;

        // Customize processing based on signal type
        switch (bci->signal_type)
        {
        case BrainSignalType::EEG:
            bci->signal_processing.sampling_rate = 250.0;
            bci->signal_processing.frequency_bands = {1.0, 4.0, 8.0, 12.0, 30.0, 45.0};
            break;
        case BrainSignalType::FMRI:
            bci->signal_processing.sampling_rate = 2.0;
            bci->signal_processing.real_time_latency = 1000.0; // Higher latency for fMRI
            break;
        case BrainSignalType::ECOG:
            bci->signal_processing.sampling_rate = 1000.0;
            bci->signal_processing.frequency_bands = {1.0, 4.0, 8.0, 12.0, 30.0, 100.0, 200.0};
            break;
        case BrainSignalType::SPIKE_TRAINS:
            bci->signal_processing.sampling_rate = 30000.0;
            bci->signal_processing.real_time_latency = 1.0;
            break;
        default:
            break;
        }

        // Customize based on application type
        switch (bci->application_type)
        {
        case BciApplicationType::MOTOR_CONTROL:
            bci->signal_processing.feature_extraction_method = "csp+band_power";
            bci->signal_processing.classification_algorithm = "cnn_lstm";
            break;
        case BciApplicationType::COMMUNICATION:
            bci->signal_processing.feature_extraction_method = "p300+ssvep";
            bci->signal_processing.classification_algorithm = "svm_ensemble";
            break;
        case BciApplicationType::COGNITIVE_ENHANCEMENT:
            bci->signal_processing.feature_extraction_method = "connectivity+complexity";
            bci->signal_processing.classification_algorithm = "deep_reinforcement";
            break;
        default:
            break;
        }
    }

    double
    Oran6gBrainComputerInterface::CalculateProcessingLatency(BrainSignalType signalType, NeuralInterfaceType interfaceType)
    {
        double baseLatency = m_processingLatency;

        // Adjust latency based on signal type
        switch (signalType)
        {
        case BrainSignalType::EEG:
            baseLatency *= 1.0;
            break;
        case BrainSignalType::FMRI:
            baseLatency *= 50.0; // Much higher latency
            break;
        case BrainSignalType::ECOG:
            baseLatency *= 0.5; // Lower latency
            break;
        case BrainSignalType::SPIKE_TRAINS:
            baseLatency *= 0.1; // Very low latency
            break;
        default:
            break;
        }

        // Adjust based on interface type
        switch (interfaceType)
        {
        case NeuralInterfaceType::INVASIVE:
            baseLatency *= 0.8;
            break;
        case NeuralInterfaceType::NON_INVASIVE:
            baseLatency *= 1.2;
            break;
        case NeuralInterfaceType::WIRELESS:
            baseLatency *= 1.5;
            break;
        default:
            break;
        }

        return baseLatency;
    }

    double
    Oran6gBrainComputerInterface::CalculateDecodingAccuracy(BrainSignalType signalType, BciApplicationType applicationType)
    {
        double baseAccuracy = m_avgDecodingAccuracy;

        // Adjust accuracy based on signal type
        switch (signalType)
        {
        case BrainSignalType::ECOG:
            baseAccuracy *= 1.1;
            break;
        case BrainSignalType::EEG:
            baseAccuracy *= 1.0;
            break;
        case BrainSignalType::FMRI:
            baseAccuracy *= 0.9;
            break;
        default:
            break;
        }

        // Adjust based on application complexity
        switch (applicationType)
        {
        case BciApplicationType::MOTOR_CONTROL:
            baseAccuracy *= 1.0;
            break;
        case BciApplicationType::COMMUNICATION:
            baseAccuracy *= 0.95;
            break;
        case BciApplicationType::COGNITIVE_ENHANCEMENT:
            baseAccuracy *= 0.8;
            break;
        case BciApplicationType::BRAIN_NETWORKING:
            baseAccuracy *= 0.7;
            break;
        default:
            break;
        }

        return std::min(baseAccuracy, 0.98); // Cap at 98%
    }

    void
    Oran6gBrainComputerInterface::StartBCI(uint32_t bciId)
    {
        NS_LOG_FUNCTION(this << bciId);

        if (bciId >= m_bcis.size())
        {
            NS_LOG_ERROR("Invalid BCI ID: " << bciId);
            return;
        }

        auto bci = m_bcis[bciId];
        if (!bci->is_calibrated)
        {
            NS_LOG_WARN("BCI " << bciId << " not calibrated, starting calibration");
            CalibrateBCI(bciId);
        }

        bci->is_active = true;
        bci->start_time = Simulator::Now();
        m_isActive = true;

        // Schedule periodic signal processing
        ScheduleSignalProcessing(bci);

        NS_LOG_INFO("Started BCI " << bciId);
    }

    void
    Oran6gBrainComputerInterface::StopBCI(uint32_t bciId)
    {
        NS_LOG_FUNCTION(this << bciId);

        if (bciId >= m_bcis.size())
        {
            NS_LOG_ERROR("Invalid BCI ID: " << bciId);
            return;
        }

        auto bci = m_bcis[bciId];
        bci->is_active = false;
        bci->stop_time = Simulator::Now();

        // Check if any BCIs are still active
        bool anyActive = false;
        for (const auto &b : m_bcis)
        {
            if (b->is_active)
            {
                anyActive = true;
                break;
            }
        }
        m_isActive = anyActive;

        NS_LOG_INFO("Stopped BCI " << bciId);
    }

    void
    Oran6gBrainComputerInterface::CalibrateBCI(uint32_t bciId)
    {
        NS_LOG_FUNCTION(this << bciId);

        if (bciId >= m_bcis.size())
        {
            NS_LOG_ERROR("Invalid BCI ID: " << bciId);
            return;
        }

        auto bci = m_bcis[bciId];

        // Simulate calibration process
        bci->calibration_data.baseline_signals.resize(m_numChannels, 0.0);
        bci->calibration_data.noise_characteristics.resize(m_numChannels, 0.1);
        bci->calibration_data.channel_weights.resize(m_numChannels, 1.0);
        bci->calibration_data.feature_scaling_factors.resize(10, 1.0);
        bci->calibration_data.calibration_accuracy = bci->decoding_accuracy;
        bci->calibration_data.calibration_timestamp = Simulator::Now();

        bci->is_calibrated = true;
        m_isCalibrated = true;

        NS_LOG_INFO("Calibrated BCI " << bciId << " with accuracy: " << bci->calibration_data.calibration_accuracy);
    }

    NeuralSignal
    Oran6gBrainComputerInterface::AcquireNeuralSignal(uint32_t bciId)
    {
        NS_LOG_FUNCTION(this << bciId);

        NeuralSignal signal;
        signal.bci_id = bciId;
        signal.timestamp = Simulator::Now();
        signal.channel_count = m_numChannels;

        // Generate synthetic neural signal data
        signal.raw_data.resize(m_numChannels);
        signal.processed_data.resize(m_numChannels);
        signal.features.resize(20); // Feature vector

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> noise(0.0, 0.1);

        for (uint32_t i = 0; i < m_numChannels; ++i)
        {
            // Generate signal with noise
            double baseSignal = std::sin(2 * M_PI * 10 * Simulator::Now().GetSeconds() + i * 0.1);
            signal.raw_data[i] = baseSignal + noise(gen);
            signal.processed_data[i] = ApplySignalProcessing(signal.raw_data[i], i);
        }

        // Extract features
        ExtractFeatures(signal);

        signal.signal_quality = CalculateSignalQuality(signal);

        return signal;
    }

    double
    Oran6gBrainComputerInterface::ApplySignalProcessing(double rawSignal, uint32_t channel)
    {
        // Simple signal processing simulation
        double processed = rawSignal;

        // Apply common average reference
        processed -= 0.1 * rawSignal; // Simplified CAR

        // Apply bandpass filtering (simplified)
        processed *= 0.9; // Simulated filtering

        // Apply adaptive filtering if enabled
        if (m_adaptiveFiltering)
        {
            processed *= 1.1; // Simulated adaptive enhancement
        }

        return processed;
    }

    void
    Oran6gBrainComputerInterface::ExtractFeatures(NeuralSignal &signal)
    {
        // Extract various features from the neural signal
        for (size_t i = 0; i < signal.features.size(); ++i)
        {
            if (i < 5) // Band power features
            {
                signal.features[i] = CalculateBandPower(signal.processed_data, i);
            }
            else if (i < 10) // Connectivity features
            {
                signal.features[i] = CalculateConnectivity(signal.processed_data, i - 5);
            }
            else if (i < 15) // Complexity features
            {
                signal.features[i] = CalculateComplexity(signal.processed_data, i - 10);
            }
            else // Other features
            {
                signal.features[i] = CalculateStatisticalFeature(signal.processed_data, i - 15);
            }
        }
    }

    double
    Oran6gBrainComputerInterface::CalculateBandPower(const std::vector<double> &data, int band)
    {
        // Simplified band power calculation
        double power = 0.0;
        for (double val : data)
        {
            power += val * val;
        }
        return power / data.size() * (1.0 + 0.1 * band); // Different for each band
    }

    double
    Oran6gBrainComputerInterface::CalculateConnectivity(const std::vector<double> &data, int pair)
    {
        // Simplified connectivity measure
        if (data.size() < 2)
            return 0.0;

        double correlation = 0.0;
        for (size_t i = 0; i < data.size() - 1; ++i)
        {
            correlation += data[i] * data[i + 1];
        }
        return correlation / (data.size() - 1) * (1.0 + 0.05 * pair);
    }

    double
    Oran6gBrainComputerInterface::CalculateComplexity(const std::vector<double> &data, int measure)
    {
        // Simplified complexity measure
        double complexity = 0.0;
        for (size_t i = 1; i < data.size(); ++i)
        {
            complexity += std::abs(data[i] - data[i - 1]);
        }
        return complexity / (data.size() - 1) * (1.0 + 0.02 * measure);
    }

    double
    Oran6gBrainComputerInterface::CalculateStatisticalFeature(const std::vector<double> &data, int feature)
    {
        // Calculate various statistical features
        switch (feature)
        {
        case 0: // Mean
            return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        case 1: // Variance
        {
            double mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
            double variance = 0.0;
            for (double val : data)
            {
                variance += (val - mean) * (val - mean);
            }
            return variance / data.size();
        }
        case 2: // Skewness (simplified)
            return 0.1 * std::sin(data.size());
        case 3: // Kurtosis (simplified)
            return 3.0 + 0.1 * std::cos(data.size());
        default:
            return 0.0;
        }
    }

    double
    Oran6gBrainComputerInterface::CalculateSignalQuality(const NeuralSignal &signal)
    {
        // Calculate signal quality based on various metrics
        double quality = m_snr / 25.0; // Base quality from SNR

        // Adjust based on artifact levels
        double artifactLevel = 0.0;
        for (double val : signal.raw_data)
        {
            if (std::abs(val) > 2.0) // Threshold for artifacts
            {
                artifactLevel += 0.1;
            }
        }
        quality *= (1.0 - std::min(artifactLevel, 0.5));

        return std::max(0.1, std::min(quality, 1.0));
    }

    BrainState
    Oran6gBrainComputerInterface::DecodeBrainState(const NeuralSignal &signal)
    {
        NS_LOG_FUNCTION(this << signal.bci_id);

        BrainState state = m_currentBrainState;
        state.timestamp = signal.timestamp;

        // Use appropriate ML model for decoding
        std::string modelKey = GetModelKeyForBCI(signal.bci_id);
        if (m_mlModels.find(modelKey) != m_mlModels.end())
        {
            auto &model = m_mlModels[modelKey];

            // Simulate ML inference
            double confidence = model.accuracy * signal.signal_quality;
            state.confidence_score = static_cast<uint32_t>(confidence * 100);

            // Update state based on features
            UpdateCognitiveLoad(state, signal.features);
            UpdateEmotionalState(state, signal.features);
            UpdateAttentionLevels(state, signal.features);
            UpdateMotorIntentions(state, signal.features);
            UpdateConsciousnessLevel(state, signal.features);
        }

        m_currentBrainState = state;
        m_brainStates.push_back(state);

        // Keep only recent states (memory management)
        if (m_brainStates.size() > 1000)
        {
            m_brainStates.erase(m_brainStates.begin());
        }

        return state;
    }

    std::string
    Oran6gBrainComputerInterface::GetModelKeyForBCI(uint32_t bciId)
    {
        if (bciId >= m_bcis.size())
            return "default";

        auto bci = m_bcis[bciId];
        switch (bci->application_type)
        {
        case BciApplicationType::MOTOR_CONTROL:
            return "motor_imagery";
        case BciApplicationType::COMMUNICATION:
            return "p300_speller";
        case BciApplicationType::COGNITIVE_ENHANCEMENT:
            return "cognitive_load";
        default:
            return "default";
        }
    }

    void
    Oran6gBrainComputerInterface::UpdateCognitiveLoad(BrainState &state, const std::vector<double> &features)
    {
        // Update cognitive load based on neural features
        if (features.size() >= 3)
        {
            state.cognitive_load["attention"] = std::max(0.0, std::min(1.0, features[0] * 0.5 + 0.5));
            state.cognitive_load["working_memory"] = std::max(0.0, std::min(1.0, features[1] * 0.4 + 0.4));
            state.cognitive_load["executive_control"] = std::max(0.0, std::min(1.0, features[2] * 0.3 + 0.3));
        }
    }

    void
    Oran6gBrainComputerInterface::UpdateEmotionalState(BrainState &state, const std::vector<double> &features)
    {
        // Update emotional state based on neural features
        if (features.size() >= 6)
        {
            state.emotional_state["valence"] = std::max(-1.0, std::min(1.0, features[3] * 0.8));
            state.emotional_state["arousal"] = std::max(0.0, std::min(1.0, features[4] * 0.5 + 0.5));
            state.emotional_state["stress"] = std::max(0.0, std::min(1.0, features[5] * 0.3 + 0.2));
        }
    }

    void
    Oran6gBrainComputerInterface::UpdateAttentionLevels(BrainState &state, const std::vector<double> &features)
    {
        // Update attention levels based on neural features
        if (features.size() >= 9)
        {
            state.attention_levels["focused"] = std::max(0.0, std::min(1.0, features[6] * 0.4 + 0.6));
            state.attention_levels["sustained"] = std::max(0.0, std::min(1.0, features[7] * 0.3 + 0.5));
            state.attention_levels["divided"] = std::max(0.0, std::min(1.0, features[8] * 0.2 + 0.3));
        }
    }

    void
    Oran6gBrainComputerInterface::UpdateMotorIntentions(BrainState &state, const std::vector<double> &features)
    {
        // Update motor intentions based on neural features
        if (features.size() >= 13)
        {
            state.motor_intentions["left_hand"] = std::max(0.0, std::min(1.0, features[9] * 0.5 + 0.1));
            state.motor_intentions["right_hand"] = std::max(0.0, std::min(1.0, features[10] * 0.5 + 0.1));
            state.motor_intentions["feet"] = std::max(0.0, std::min(1.0, features[11] * 0.3 + 0.05));
            state.motor_intentions["tongue"] = std::max(0.0, std::min(1.0, features[12] * 0.2 + 0.02));
        }
    }

    void
    Oran6gBrainComputerInterface::UpdateConsciousnessLevel(BrainState &state, const std::vector<double> &features)
    {
        // Update consciousness level based on overall neural activity
        double totalActivity = 0.0;
        for (double feature : features)
        {
            totalActivity += std::abs(feature);
        }

        state.consciousness_level = std::max(0.1, std::min(1.0, totalActivity / features.size() * 0.5 + 0.5));
    }

    void
    Oran6gBrainComputerInterface::ScheduleSignalProcessing(Ptr<BrainComputerInterface> bci)
    {
        NS_LOG_FUNCTION(this << bci->bci_id);

        if (!bci->is_active)
            return;

        // Schedule next signal acquisition and processing
        Time interval = MilliSeconds(1000.0 / bci->signal_processing.sampling_rate);
        Simulator::Schedule(interval, &Oran6gBrainComputerInterface::ProcessSignal, this, bci->bci_id);
    }

    void
    Oran6gBrainComputerInterface::ProcessSignal(uint32_t bciId)
    {
        NS_LOG_FUNCTION(this << bciId);

        if (bciId >= m_bcis.size() || !m_bcis[bciId]->is_active)
        {
            return;
        }

        // Acquire and process neural signal
        NeuralSignal signal = AcquireNeuralSignal(bciId);
        BrainState decodedState = DecodeBrainState(signal);

        // Store signal and state
        m_neuralSignals.push_back(signal);

        // Manage memory
        if (m_neuralSignals.size() > 10000)
        {
            m_neuralSignals.erase(m_neuralSignals.begin());
        }

        m_totalProcessedSignals++;

        // Trigger callbacks for applications
        TriggerApplicationCallbacks(bciId, decodedState);

        // Schedule next processing
        ScheduleSignalProcessing(m_bcis[bciId]);
    }

    void
    Oran6gBrainComputerInterface::TriggerApplicationCallbacks(uint32_t bciId, const BrainState &state)
    {
        // Trigger registered application callbacks
        for (const auto &connection : m_applicationConnections)
        {
            if (connection.bci_id == bciId && connection.callback)
            {
                connection.callback(state);
            }
        }
    }

    void
    Oran6gBrainComputerInterface::RegisterApplicationCallback(uint32_t bciId,
                                                              const std::string &appName,
                                                              BrainStateCallback callback)
    {
        NS_LOG_FUNCTION(this << bciId << appName);

        ApplicationConnection conn;
        conn.bci_id = bciId;
        conn.application_name = appName;
        conn.callback = callback;
        conn.connection_time = Simulator::Now();

        m_applicationConnections.push_back(conn);

        NS_LOG_INFO("Registered application callback for " << appName << " on BCI " << bciId);
    }

    BrainComputerInterfaceStats
    Oran6gBrainComputerInterface::GetStats() const
    {
        BrainComputerInterfaceStats stats;
        stats.total_bcis = m_bcis.size();
        stats.active_bcis = 0;
        stats.total_processed_signals = m_totalProcessedSignals;
        stats.avg_processing_latency = m_processingLatency;
        stats.avg_decoding_accuracy = m_avgDecodingAccuracy;
        stats.total_brain_states = m_brainStates.size();

        for (const auto &bci : m_bcis)
        {
            if (bci->is_active)
            {
                stats.active_bcis++;
            }
            stats.avg_signal_quality += bci->signal_quality;
        }

        if (!m_bcis.empty())
        {
            stats.avg_signal_quality /= m_bcis.size();
        }

        return stats;
    }

    std::vector<BrainState>
    Oran6gBrainComputerInterface::GetRecentBrainStates(uint32_t count) const
    {
        std::vector<BrainState> recent;

        if (m_brainStates.empty())
            return recent;

        size_t start = 0;
        if (m_brainStates.size() > count)
        {
            start = m_brainStates.size() - count;
        }

        for (size_t i = start; i < m_brainStates.size(); ++i)
        {
            recent.push_back(m_brainStates[i]);
        }

        return recent;
    }

    void
    Oran6gBrainComputerInterface::SetAdaptationRate(uint32_t bciId, double rate)
    {
        NS_LOG_FUNCTION(this << bciId << rate);

        if (bciId >= m_bcis.size())
        {
            NS_LOG_ERROR("Invalid BCI ID: " << bciId);
            return;
        }

        m_bcis[bciId]->adaptation_rate = std::max(0.0, std::min(1.0, rate));
        NS_LOG_INFO("Set adaptation rate for BCI " << bciId << " to " << rate);
    }

    void
    Oran6gBrainComputerInterface::UpdateMLModel(const std::string &modelName, const MLModel &model)
    {
        NS_LOG_FUNCTION(this << modelName);

        m_mlModels[modelName] = model;
        NS_LOG_INFO("Updated ML model: " << modelName << " with accuracy: " << model.accuracy);
    }

    bool
    Oran6gBrainComputerInterface::IsRealTimeCapable() const
    {
        return m_processingLatency <= 50.0; // Consider real-time if latency <= 50ms
    }

    std::string
    Oran6gBrainComputerInterface::GetBCIInfo(uint32_t bciId) const
    {
        if (bciId >= m_bcis.size())
        {
            return "Invalid BCI ID";
        }

        auto bci = m_bcis[bciId];
        std::stringstream info;
        info << "BCI " << bciId << ":\n";
        info << "  Signal Type: " << static_cast<int>(bci->signal_type) << "\n";
        info << "  Interface Type: " << static_cast<int>(bci->interface_type) << "\n";
        info << "  Application: " << static_cast<int>(bci->application_type) << "\n";
        info << "  Active: " << (bci->is_active ? "Yes" : "No") << "\n";
        info << "  Calibrated: " << (bci->is_calibrated ? "Yes" : "No") << "\n";
        info << "  Accuracy: " << bci->decoding_accuracy << "\n";
        info << "  Latency: " << bci->processing_latency << " ms\n";
        info << "  Signal Quality: " << bci->signal_quality << "\n";

        return info.str();
    }

} // namespace ns3
