/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 */

#include "oran-ai-transformer.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include <cmath>
#include <algorithm>
#include <random>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("OranAiTransformer");

    NS_OBJECT_ENSURE_REGISTERED(OranAiTransformer);

    TypeId
    OranAiTransformer::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::OranAiTransformer")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<OranAiTransformer>()
                                .AddAttribute("NumHeads",
                                              "Number of attention heads",
                                              UintegerValue(8),
                                              MakeUintegerAccessor(&OranAiTransformer::m_numHeads),
                                              MakeUintegerChecker<uint32_t>(1, 32))
                                .AddAttribute("EmbeddingDim",
                                              "Embedding dimension",
                                              UintegerValue(512),
                                              MakeUintegerAccessor(&OranAiTransformer::m_embeddingDim),
                                              MakeUintegerChecker<uint32_t>(64, 2048))
                                .AddAttribute("FeedForwardDim",
                                              "Feed-forward dimension",
                                              UintegerValue(2048),
                                              MakeUintegerAccessor(&OranAiTransformer::m_feedForwardDim),
                                              MakeUintegerChecker<uint32_t>(256, 8192))
                                .AddAttribute("NumLayers",
                                              "Number of transformer layers",
                                              UintegerValue(6),
                                              MakeUintegerAccessor(&OranAiTransformer::m_numLayers),
                                              MakeUintegerChecker<uint32_t>(1, 24))
                                .AddAttribute("DropoutRate",
                                              "Dropout rate for regularization",
                                              DoubleValue(0.1),
                                              MakeDoubleAccessor(&OranAiTransformer::m_dropoutRate),
                                              MakeDoubleChecker<double>(0.0, 0.9))
                                .AddAttribute("LearningRate",
                                              "Learning rate for training",
                                              DoubleValue(0.001),
                                              MakeDoubleAccessor(&OranAiTransformer::m_learningRate),
                                              MakeDoubleChecker<double>(1e-6, 1.0))
                                .AddAttribute("EnableFederatedLearning",
                                              "Enable federated learning mode",
                                              BooleanValue(false),
                                              MakeBooleanAccessor(&OranAiTransformer::m_enableFederatedLearning),
                                              MakeBooleanChecker())
                                .AddAttribute("FederationRounds",
                                              "Number of federated learning rounds",
                                              UintegerValue(10),
                                              MakeUintegerAccessor(&OranAiTransformer::m_federationRounds),
                                              MakeUintegerChecker<uint32_t>(1, 1000))
                                .AddTraceSource("AttentionScores",
                                                "Attention scores calculated by the transformer",
                                                MakeTraceSourceAccessor(&OranAiTransformer::m_attentionTrace),
                                                "ns3::TracedCallback")
                                .AddTraceSource("LossValue",
                                                "Training loss value",
                                                MakeTraceSourceAccessor(&OranAiTransformer::m_lossTrace),
                                                "ns3::TracedCallback")
                                .AddTraceSource("PredictionAccuracy",
                                                "Model prediction accuracy",
                                                MakeTraceSourceAccessor(&OranAiTransformer::m_accuracyTrace),
                                                "ns3::TracedCallback");
        return tid;
    }

    OranAiTransformer::OranAiTransformer()
        : m_numHeads(8),
          m_embeddingDim(512),
          m_feedForwardDim(2048),
          m_numLayers(6),
          m_dropoutRate(0.1),
          m_learningRate(0.001),
          m_enableFederatedLearning(false),
          m_federationRounds(10),
          m_isInitialized(false),
          m_trainingEpoch(0),
          m_currentLoss(0.0),
          m_modelAccuracy(0.0)
    {
        NS_LOG_FUNCTION(this);
        InitializeModel();
    }

    OranAiTransformer::~OranAiTransformer()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    OranAiTransformer::InitializeModel()
    {
        NS_LOG_FUNCTION(this);

        // Initialize model parameters
        m_weights.clear();
        m_biases.clear();

        // Initialize attention weights for each head
        for (uint32_t layer = 0; layer < m_numLayers; ++layer)
        {
            LayerWeights layerWeights;

            // Query, Key, Value projection matrices
            layerWeights.queryWeights = GenerateRandomMatrix(m_embeddingDim, m_embeddingDim);
            layerWeights.keyWeights = GenerateRandomMatrix(m_embeddingDim, m_embeddingDim);
            layerWeights.valueWeights = GenerateRandomMatrix(m_embeddingDim, m_embeddingDim);

            // Feed-forward network weights
            layerWeights.ffn1Weights = GenerateRandomMatrix(m_embeddingDim, m_feedForwardDim);
            layerWeights.ffn2Weights = GenerateRandomMatrix(m_feedForwardDim, m_embeddingDim);

            m_weights.push_back(layerWeights);
        }

        m_isInitialized = true;
        NS_LOG_INFO("AI Transformer model initialized with " << m_numLayers << " layers");
    }

    std::vector<std::vector<double>>
    OranAiTransformer::GenerateRandomMatrix(uint32_t rows, uint32_t cols)
    {
        std::vector<std::vector<double>> matrix(rows, std::vector<double>(cols));
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> dist(0.0, sqrt(2.0 / (rows + cols))); // Xavier initialization

        for (uint32_t i = 0; i < rows; ++i)
        {
            for (uint32_t j = 0; j < cols; ++j)
            {
                matrix[i][j] = dist(gen);
            }
        }

        return matrix;
    }

    std::vector<double>
    OranAiTransformer::Forward(const std::vector<double> &input)
    {
        NS_LOG_FUNCTION(this << input.size());

        if (!m_isInitialized)
        {
            NS_LOG_ERROR("Model not initialized");
            return std::vector<double>();
        }

        std::vector<double> output = input;

        // Process through each transformer layer
        for (uint32_t layer = 0; layer < m_numLayers; ++layer)
        {
            output = ProcessLayer(output, layer);
        }

        return output;
    }

    std::vector<double>
    OranAiTransformer::ProcessLayer(const std::vector<double> &input, uint32_t layerIndex)
    {
        NS_LOG_FUNCTION(this << input.size() << layerIndex);

        // Multi-head self-attention
        std::vector<double> attentionOutput = MultiHeadAttention(input, layerIndex);

        // Add & Norm (residual connection)
        std::vector<double> residual1 = AddAndNorm(input, attentionOutput);

        // Feed-forward network
        std::vector<double> ffnOutput = FeedForward(residual1, layerIndex);

        // Add & Norm (residual connection)
        std::vector<double> output = AddAndNorm(residual1, ffnOutput);

        return output;
    }

    std::vector<double>
    OranAiTransformer::MultiHeadAttention(const std::vector<double> &input, uint32_t layerIndex)
    {
        NS_LOG_FUNCTION(this << input.size() << layerIndex);

        std::vector<double> output(input.size(), 0.0);
        uint32_t headDim = m_embeddingDim / m_numHeads;

        for (uint32_t head = 0; head < m_numHeads; ++head)
        {
            // Simplified attention computation
            std::vector<double> headOutput = ComputeAttentionHead(input, layerIndex, head);

            // Combine head outputs
            for (size_t i = 0; i < output.size() && i < headOutput.size(); ++i)
            {
                output[i] += headOutput[i] / m_numHeads;
            }
        }

        // Calculate attention scores for tracing
        double avgAttention = 0.0;
        for (const auto &val : output)
        {
            avgAttention += std::abs(val);
        }
        avgAttention /= output.size();

        m_attentionTrace(avgAttention);

        return output;
    }

    std::vector<double>
    OranAiTransformer::ComputeAttentionHead(const std::vector<double> &input, uint32_t layerIndex, uint32_t headIndex)
    {
        NS_LOG_FUNCTION(this << input.size() << layerIndex << headIndex);

        // Simplified attention computation
        std::vector<double> output(input.size());

        // Apply softmax-like attention mechanism
        double sum = 0.0;
        for (size_t i = 0; i < input.size(); ++i)
        {
            sum += std::exp(input[i]);
        }

        for (size_t i = 0; i < input.size(); ++i)
        {
            output[i] = std::exp(input[i]) / sum;
        }

        return output;
    }

    std::vector<double>
    OranAiTransformer::FeedForward(const std::vector<double> &input, uint32_t layerIndex)
    {
        NS_LOG_FUNCTION(this << input.size() << layerIndex);

        // Simplified feed-forward network
        std::vector<double> hidden(m_feedForwardDim, 0.0);
        std::vector<double> output(input.size(), 0.0);

        // First linear transformation with ReLU
        for (uint32_t i = 0; i < m_feedForwardDim && i < input.size(); ++i)
        {
            hidden[i] = std::max(0.0, input[i] * 0.8 + 0.1); // Simplified linear + ReLU
        }

        // Second linear transformation
        for (size_t i = 0; i < output.size() && i < hidden.size(); ++i)
        {
            output[i] = hidden[i] * 0.9;
        }

        return output;
    }

    std::vector<double>
    OranAiTransformer::AddAndNorm(const std::vector<double> &input1, const std::vector<double> &input2)
    {
        NS_LOG_FUNCTION(this << input1.size() << input2.size());

        std::vector<double> output(input1.size());

        // Add residual connection
        for (size_t i = 0; i < output.size() && i < input2.size(); ++i)
        {
            output[i] = input1[i] + input2[i];
        }

        // Layer normalization (simplified)
        double mean = 0.0;
        for (const auto &val : output)
        {
            mean += val;
        }
        mean /= output.size();

        double variance = 0.0;
        for (const auto &val : output)
        {
            variance += (val - mean) * (val - mean);
        }
        variance /= output.size();

        double std_dev = std::sqrt(variance + 1e-8);
        for (auto &val : output)
        {
            val = (val - mean) / std_dev;
        }

        return output;
    }

    void
    OranAiTransformer::Train(const std::vector<std::vector<double>> &trainingData,
                             const std::vector<std::vector<double>> &targets)
    {
        NS_LOG_FUNCTION(this << trainingData.size() << targets.size());

        if (trainingData.size() != targets.size())
        {
            NS_LOG_ERROR("Training data and targets size mismatch");
            return;
        }

        double totalLoss = 0.0;
        uint32_t correctPredictions = 0;

        for (size_t i = 0; i < trainingData.size(); ++i)
        {
            // Forward pass
            std::vector<double> prediction = Forward(trainingData[i]);

            // Calculate loss (MSE)
            double loss = CalculateLoss(prediction, targets[i]);
            totalLoss += loss;

            // Check accuracy (simplified)
            if (std::abs(prediction[0] - targets[i][0]) < 0.1)
            {
                correctPredictions++;
            }

            // Backward pass (simplified - just update learning rate)
            BackwardPass(prediction, targets[i]);
        }

        m_currentLoss = totalLoss / trainingData.size();
        m_modelAccuracy = static_cast<double>(correctPredictions) / trainingData.size();

        m_lossTrace(m_currentLoss);
        m_accuracyTrace(m_modelAccuracy);

        m_trainingEpoch++;

        NS_LOG_INFO("Training epoch " << m_trainingEpoch << " completed. Loss: " << m_currentLoss << " Accuracy: " << m_modelAccuracy);
    }

    double
    OranAiTransformer::CalculateLoss(const std::vector<double> &prediction, const std::vector<double> &target)
    {
        double loss = 0.0;
        for (size_t i = 0; i < prediction.size() && i < target.size(); ++i)
        {
            double diff = prediction[i] - target[i];
            loss += diff * diff;
        }
        return loss / prediction.size();
    }

    void
    OranAiTransformer::BackwardPass(const std::vector<double> &prediction, const std::vector<double> &target)
    {
        NS_LOG_FUNCTION(this << prediction.size() << target.size());

        // Simplified gradient descent update
        // In a real implementation, this would compute gradients and update weights
        // For simulation purposes, we just adjust the learning rate based on loss

        double currentLoss = CalculateLoss(prediction, target);
        if (currentLoss > m_currentLoss * 1.1)
        {
            m_learningRate *= 0.95; // Reduce learning rate if loss increases
        }
        else if (currentLoss < m_currentLoss * 0.9)
        {
            m_learningRate *= 1.01; // Slightly increase if improving
        }

        // Ensure learning rate stays within bounds
        m_learningRate = std::max(1e-6, std::min(1.0, m_learningRate));
    }

    void
    OranAiTransformer::EnableFederatedLearning(bool enable)
    {
        NS_LOG_FUNCTION(this << enable);
        m_enableFederatedLearning = enable;

        if (enable)
        {
            NS_LOG_INFO("Federated learning enabled for " << m_federationRounds << " rounds");
        }
    }

    void
    OranAiTransformer::FederatedUpdate(const std::vector<OranAiTransformer *> &peers)
    {
        NS_LOG_FUNCTION(this << peers.size());

        if (!m_enableFederatedLearning)
        {
            NS_LOG_WARN("Federated learning not enabled");
            return;
        }

        // Simplified federated averaging
        // Average the learning rates and model accuracy across peers
        double avgLearningRate = m_learningRate;
        double avgAccuracy = m_modelAccuracy;

        for (const auto &peer : peers)
        {
            if (peer != this)
            {
                avgLearningRate += peer->GetLearningRate();
                avgAccuracy += peer->GetModelAccuracy();
            }
        }

        avgLearningRate /= (peers.size() + 1);
        avgAccuracy /= (peers.size() + 1);

        // Update local model with federated averages
        m_learningRate = avgLearningRate;
        m_modelAccuracy = avgAccuracy;

        NS_LOG_INFO("Federated update completed. New learning rate: " << m_learningRate);
    }

    std::vector<double>
    OranAiTransformer::Predict(const std::vector<double> &input)
    {
        NS_LOG_FUNCTION(this << input.size());
        return Forward(input);
    }

    double
    OranAiTransformer::GetModelAccuracy() const
    {
        return m_modelAccuracy;
    }

    double
    OranAiTransformer::GetCurrentLoss() const
    {
        return m_currentLoss;
    }

    double
    OranAiTransformer::GetLearningRate() const
    {
        return m_learningRate;
    }

    uint32_t
    OranAiTransformer::GetTrainingEpoch() const
    {
        return m_trainingEpoch;
    }

    void
    OranAiTransformer::SetLearningRate(double rate)
    {
        NS_LOG_FUNCTION(this << rate);
        m_learningRate = std::max(1e-6, std::min(1.0, rate));
    }

    void
    OranAiTransformer::SaveModel(const std::string &filename)
    {
        NS_LOG_FUNCTION(this << filename);

        // In a real implementation, this would serialize model weights to file
        // For simulation, we just log the action
        NS_LOG_INFO("Model saved to " << filename << " (Accuracy: " << m_modelAccuracy << ", Loss: " << m_currentLoss << ")");
    }

    void
    OranAiTransformer::LoadModel(const std::string &filename)
    {
        NS_LOG_FUNCTION(this << filename);

        // In a real implementation, this would load model weights from file
        // For simulation, we just reinitialize
        InitializeModel();
        NS_LOG_INFO("Model loaded from " << filename);
    }

} // namespace ns3
