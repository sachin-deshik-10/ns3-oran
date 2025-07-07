/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * ORAN-AI-TRANSFORMER: Transformer-based AI implementation
 */

#include "oran-ai-transformer.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("OranAiTransformer");
NS_OBJECT_ENSURE_REGISTERED(OranAiTransformer);

TypeId
OranAiTransformer::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::OranAiTransformer")
                            .SetParent<Object>()
                            .SetGroupName("Oran")
                            .AddConstructor<OranAiTransformer>()
                            .AddAttribute("ModelDimension",
                                        "Transformer model dimension",
                                        UintegerValue(512),
                                        MakeUintegerAccessor(&OranAiTransformer::m_modelDimension),
                                        MakeUintegerChecker<uint32_t>(128, 2048))
                            .AddAttribute("NumHeads",
                                        "Number of attention heads",
                                        UintegerValue(8),
                                        MakeUintegerAccessor(&OranAiTransformer::m_numHeads),
                                        MakeUintegerChecker<uint32_t>(1, 32))
                            .AddAttribute("NumLayers",
                                        "Number of transformer layers",
                                        UintegerValue(6),
                                        MakeUintegerAccessor(&OranAiTransformer::m_numLayers),
                                        MakeUintegerChecker<uint32_t>(1, 24))
                            .AddAttribute("LearningRate",
                                        "Learning rate for model updates",
                                        DoubleValue(0.001),
                                        MakeDoubleAccessor(&OranAiTransformer::m_learningRate),
                                        MakeDoubleChecker<double>(0.00001, 0.1))
                            .AddTraceSource("PredictionAccuracy",
                                          "Prediction accuracy trace",
                                          MakeTraceSourceAccessor(&OranAiTransformer::m_predictionAccuracy),
                                          "ns3::TracedValueCallback::Double")
                            .AddTraceSource("InferenceLatency",
                                          "Inference latency trace",
                                          MakeTraceSourceAccessor(&OranAiTransformer::m_inferenceLatency),
                                          "ns3::TracedValueCallback::Double");
    return tid;
}

OranAiTransformer::OranAiTransformer()
    : m_modelType(NETWORK_TRANSFORMER),
      m_attentionType(SELF_ATTENTION),
      m_modelDimension(512),
      m_numHeads(8),
      m_numLayers(6),
      m_contextWindow(128),
      m_isInitialized(false),
      m_predictionAccuracy(0.0),
      m_inferenceLatency(0.0),
      m_modelUncertainty(0.0),
      m_totalPredictions(0),
      m_correctPredictions(0),
      m_federatedLearningEnabled(false),
      m_nodeId(0),
      m_aggregationPeriod(Seconds(30.0)),
      m_zeroShotEnabled(false),
      m_learningRate(0.001),
      m_dropoutRate(0.1),
      m_attentionDropout(0.1),
      m_batchSize(32)
{
    NS_LOG_FUNCTION(this);
}

OranAiTransformer::~OranAiTransformer()
{
    NS_LOG_FUNCTION(this);
}

void
OranAiTransformer::InitializeModel(ModelType modelType, uint32_t modelDimension,
                                  uint32_t numHeads, uint32_t numLayers)
{
    NS_LOG_FUNCTION(this << modelType << modelDimension << numHeads << numLayers);
    
    m_modelType = modelType;
    m_modelDimension = modelDimension;
    m_numHeads = numHeads;
    m_numLayers = numLayers;
    
    // Initialize transformer architecture
    InitializeTransformerArchitecture();
    
    // Initialize performance metrics
    m_performanceMetrics["prediction_accuracy"] = 0.0;
    m_performanceMetrics["inference_latency"] = 0.0;
    m_performanceMetrics["model_uncertainty"] = 0.0;
    m_performanceMetrics["training_loss"] = 1.0;
    
    m_isInitialized = true;
    
    NS_LOG_INFO("AI Transformer initialized: " << modelType << ", dim=" << modelDimension
                << ", heads=" << numHeads << ", layers=" << numLayers);
}

void
OranAiTransformer::ConfigureAttention(AttentionType attentionType, uint32_t contextWindow)
{
    NS_LOG_FUNCTION(this << attentionType << contextWindow);
    
    m_attentionType = attentionType;
    m_contextWindow = contextWindow;
    
    // Resize attention weight matrices
    m_attentionWeights.clear();
    m_attentionWeights.resize(m_numLayers);
    for (auto& layer : m_attentionWeights) {
        layer.resize(m_contextWindow, std::vector<double>(m_contextWindow, 0.0));
    }
    
    NS_LOG_INFO("Attention configured: type=" << attentionType << ", window=" << contextWindow);
}

void
OranAiTransformer::AddNetworkObservation(const NetworkState& state)
{
    NS_LOG_FUNCTION(this);
    
    // Add to history with context window limit
    m_networkHistory.push_back(state);
    if (m_networkHistory.size() > m_contextWindow) {
        m_networkHistory.erase(m_networkHistory.begin());
    }
    
    NS_LOG_DEBUG("Network observation added, history size: " << m_networkHistory.size());
}

OranAiTransformer::PredictionResult
OranAiTransformer::GeneratePrediction(Time predictionHorizon)
{
    NS_LOG_FUNCTION(this << predictionHorizon);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    PredictionResult result;
    result.predictionHorizon = predictionHorizon;
    
    if (!m_isInitialized || m_networkHistory.empty()) {
        NS_LOG_WARN("Model not initialized or no network history");
        result.uncertainty = 1.0;
        return result;
    }
    
    // Process input sequence through transformer
    std::vector<std::vector<double>> processedSequence = ProcessInputSequence(m_networkHistory);
    
    // Generate predictions based on model type
    switch (m_modelType) {
        case HANDOVER_PREDICTOR:
            result.prediction = GenerateHandoverPrediction(processedSequence);
            break;
        case RESOURCE_OPTIMIZER:
            result.prediction = GenerateResourceOptimization(processedSequence);
            break;
        case ANOMALY_DETECTOR:
            result.prediction = GenerateAnomalyDetection(processedSequence);
            break;
        case TRAFFIC_FORECASTER:
            result.prediction = GenerateTrafficForecast(processedSequence);
            break;
        case ENERGY_OPTIMIZER:
            result.prediction = GenerateEnergyOptimization(processedSequence);
            break;
        default:
            result.prediction = GenerateNetworkPrediction(processedSequence);
            break;
    }
    
    // Calculate uncertainty using Monte Carlo dropout
    result.uncertainty = CalculateUncertainty(result.prediction, 50);
    
    // Generate confidence scores
    result.confidence.resize(result.prediction.size());
    for (size_t i = 0; i < result.prediction.size(); ++i) {
        result.confidence[i] = 1.0 - result.uncertainty;
    }
    
    // Extract attention weights for explainability
    if (!m_attentionWeights.empty()) {
        result.attention = m_attentionWeights.back().back(); // Last layer, last position
    }
    
    // Generate explanation
    result.explanation = ExplainPrediction(result);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    m_inferenceLatency = latency / 1000.0; // Convert to milliseconds
    m_totalPredictions++;
    
    // Update performance metrics
    m_performanceMetrics["inference_latency"] = m_inferenceLatency;
    m_performanceMetrics["model_uncertainty"] = result.uncertainty;
    
    // Trigger callbacks
    if (!m_predictionCallback.IsNull()) {
        m_predictionCallback(result);
    }
    
    NS_LOG_DEBUG("Prediction generated in " << m_inferenceLatency << "ms, uncertainty=" 
                 << result.uncertainty);
    
    return result;
}

void
OranAiTransformer::UpdateModel(const NetworkState& actualOutcome,
                              const PredictionResult& previousPrediction)
{
    NS_LOG_FUNCTION(this);
    
    if (!m_isInitialized) {
        NS_LOG_WARN("Model not initialized");
        return;
    }
    
    // Calculate prediction error
    double error = CalculatePredictionError(actualOutcome, previousPrediction);
    
    // Update accuracy tracking
    if (error < 0.1) { // Consider prediction correct if error < 10%
        m_correctPredictions++;
    }
    
    m_predictionAccuracy = static_cast<double>(m_correctPredictions) / m_totalPredictions;
    
    // Perform gradient-based update (simplified)
    if (error > 0.05) { // Only update if significant error
        UpdateModelParameters(error);
    }
    
    // Update performance metrics
    m_performanceMetrics["prediction_accuracy"] = m_predictionAccuracy;
    m_performanceMetrics["training_loss"] = error;
    
    NS_LOG_DEBUG("Model updated, accuracy=" << m_predictionAccuracy << ", error=" << error);
}

void
OranAiTransformer::EnableFederatedLearning(uint32_t nodeId, Time aggregationPeriod)
{
    NS_LOG_FUNCTION(this << nodeId << aggregationPeriod);
    
    m_federatedLearningEnabled = true;
    m_nodeId = nodeId;
    m_aggregationPeriod = aggregationPeriod;
    
    // Schedule first federated learning round
    ScheduleFederatedLearning();
    
    NS_LOG_INFO("Federated learning enabled for node " << nodeId);
}

std::vector<double>
OranAiTransformer::GetModelParameters()
{
    NS_LOG_FUNCTION(this);
    
    std::vector<double> parameters;
    
    // Serialize model weights (simplified representation)
    for (const auto& layerWeights : m_modelWeights) {
        for (const auto& row : layerWeights) {
            for (double weight : row) {
                parameters.push_back(weight);
            }
        }
    }
    
    NS_LOG_DEBUG("Exported " << parameters.size() << " model parameters");
    return parameters;
}

void
OranAiTransformer::IntegrateFederatedUpdate(const std::vector<std::vector<double>>& modelUpdates,
                                          const std::vector<double>& nodeWeights)
{
    NS_LOG_FUNCTION(this);
    
    if (modelUpdates.empty() || nodeWeights.empty()) {
        NS_LOG_WARN("Empty federated updates");
        return;
    }
    
    // Store updates for aggregation
    m_federatedUpdates = modelUpdates;
    
    // Perform weighted averaging of model parameters
    PerformFederatedAggregation();
    
    NS_LOG_INFO("Federated update integrated from " << modelUpdates.size() << " nodes");
}

std::string
OranAiTransformer::ExplainPrediction(const PredictionResult& prediction)
{
    NS_LOG_FUNCTION(this);
    
    std::stringstream explanation;
    
    explanation << "AI Prediction Analysis:\n";
    explanation << "- Model Type: " << GetModelTypeName(m_modelType) << "\n";
    explanation << "- Confidence: " << (prediction.confidence.empty() ? 0.0 : prediction.confidence[0]) * 100 << "%\n";
    explanation << "- Uncertainty: " << prediction.uncertainty * 100 << "%\n";
    explanation << "- Prediction Horizon: " << prediction.predictionHorizon.GetSeconds() << "s\n";
    
    // Add attention-based explanation
    if (!prediction.attention.empty()) {
        explanation << "- Key Factors: ";
        // Find top attention weights
        std::vector<std::pair<double, size_t>> attentionPairs;
        for (size_t i = 0; i < prediction.attention.size(); ++i) {
            attentionPairs.push_back({prediction.attention[i], i});
        }
        std::sort(attentionPairs.rbegin(), attentionPairs.rend());
        
        for (size_t i = 0; i < std::min(size_t(3), attentionPairs.size()); ++i) {
            explanation << "Feature" << attentionPairs[i].second << " (" 
                       << attentionPairs[i].first * 100 << "%) ";
        }
        explanation << "\n";
    }
    
    return explanation.str();
}

std::map<std::string, double>
OranAiTransformer::GetPerformanceMetrics()
{
    NS_LOG_FUNCTION(this);
    return m_performanceMetrics;
}

void
OranAiTransformer::SetDataRepository(Ptr<OranDataRepository> repository)
{
    NS_LOG_FUNCTION(this);
    m_dataRepository = repository;
}

void
OranAiTransformer::SetPredictionCallback(PredictionCallback callback)
{
    NS_LOG_FUNCTION(this);
    m_predictionCallback = callback;
}

std::vector<std::vector<double>>
OranAiTransformer::GetAttentionVisualization()
{
    NS_LOG_FUNCTION(this);
    
    if (m_attentionWeights.empty()) {
        return std::vector<std::vector<double>>();
    }
    
    // Return the last layer's attention weights for visualization
    return m_attentionWeights.back();
}

// Private helper methods

void
OranAiTransformer::InitializeTransformerArchitecture()
{
    NS_LOG_FUNCTION(this);
    
    // Initialize model weights with Xavier initialization
    std::random_device rd;
    std::mt19937 gen(rd());
    double scale = std::sqrt(2.0 / m_modelDimension);
    std::normal_distribution<double> distribution(0.0, scale);
    
    m_modelWeights.clear();
    m_modelWeights.resize(m_numLayers);
    
    for (auto& layer : m_modelWeights) {
        layer.resize(m_modelDimension);
        for (auto& row : layer) {
            row.resize(m_modelDimension);
            for (auto& weight : row) {
                weight = distribution(gen);
            }
        }
    }
    
    NS_LOG_DEBUG("Transformer architecture initialized with " << m_numLayers << " layers");
}

std::vector<std::vector<double>>
OranAiTransformer::ProcessInputSequence(const std::vector<NetworkState>& inputSequence)
{
    NS_LOG_FUNCTION(this);
    
    std::vector<std::vector<double>> processedSequence;
    
    // Convert network states to feature vectors
    for (size_t i = 0; i < inputSequence.size(); ++i) {
        const auto& state = inputSequence[i];
        
        // Combine all metrics into a single feature vector
        std::vector<double> features;
        features.insert(features.end(), state.cellMetrics.begin(), state.cellMetrics.end());
        features.insert(features.end(), state.ueMetrics.begin(), state.ueMetrics.end());
        features.insert(features.end(), state.networkTopology.begin(), state.networkTopology.end());
        features.insert(features.end(), state.trafficPattern.begin(), state.trafficPattern.end());
        features.insert(features.end(), state.interferenceMap.begin(), state.interferenceMap.end());
        
        // Pad or truncate to model dimension
        features.resize(m_modelDimension, 0.0);
        
        // Add positional encoding
        std::vector<double> posEncoding = CalculatePositionalEncoding(i, m_modelDimension);
        for (size_t j = 0; j < features.size() && j < posEncoding.size(); ++j) {
            features[j] += posEncoding[j];
        }
        
        processedSequence.push_back(features);
    }
    
    // Apply transformer layers
    for (uint32_t layer = 0; layer < m_numLayers; ++layer) {
        processedSequence = ApplyTransformerLayer(processedSequence, layer);
    }
    
    return processedSequence;
}

std::vector<std::vector<double>>
OranAiTransformer::ApplyTransformerLayer(const std::vector<std::vector<double>>& input, uint32_t layer)
{
    NS_LOG_FUNCTION(this << layer);
    
    // Apply multi-head attention
    auto attentionOutput = ApplyMultiHeadAttention(input, input, input);
    
    // Store attention weights for explainability
    if (layer < m_attentionWeights.size()) {
        m_attentionWeights[layer] = attentionOutput.second;
    }
    
    // Apply residual connection and layer normalization
    auto residualOutput = attentionOutput.first;
    for (size_t i = 0; i < residualOutput.size(); ++i) {
        for (size_t j = 0; j < residualOutput[i].size(); ++j) {
            residualOutput[i][j] += input[i][j]; // Residual connection
        }
        residualOutput[i] = ApplyLayerNorm(residualOutput[i]);
    }
    
    // Apply feed-forward network
    auto ffOutput = ApplyFeedForward(residualOutput);
    
    // Apply second residual connection and layer normalization
    for (size_t i = 0; i < ffOutput.size(); ++i) {
        for (size_t j = 0; j < ffOutput[i].size(); ++j) {
            ffOutput[i][j] += residualOutput[i][j]; // Residual connection
        }
        ffOutput[i] = ApplyLayerNorm(ffOutput[i]);
    }
    
    return ffOutput;
}

std::pair<std::vector<std::vector<double>>, std::vector<std::vector<double>>>
OranAiTransformer::ApplyMultiHeadAttention(const std::vector<std::vector<double>>& queries,
                                         const std::vector<std::vector<double>>& keys,
                                         const std::vector<std::vector<double>>& values)
{
    NS_LOG_FUNCTION(this);
    
    size_t seqLen = queries.size();
    size_t headDim = m_modelDimension / m_numHeads;
    
    std::vector<std::vector<double>> output(seqLen, std::vector<double>(m_modelDimension, 0.0));
    std::vector<std::vector<double>> attentionWeights(seqLen, std::vector<double>(seqLen, 0.0));
    
    // Simplified multi-head attention (single head for demonstration)
    for (size_t i = 0; i < seqLen; ++i) {
        std::vector<double> attentionScores(seqLen, 0.0);
        
        // Calculate attention scores
        for (size_t j = 0; j < seqLen; ++j) {
            double score = 0.0;
            for (size_t k = 0; k < headDim && k < queries[i].size() && k < keys[j].size(); ++k) {
                score += queries[i][k] * keys[j][k];
            }
            attentionScores[j] = score / std::sqrt(headDim);
        }
        
        // Apply softmax
        double maxScore = *std::max_element(attentionScores.begin(), attentionScores.end());
        double sumExp = 0.0;
        for (auto& score : attentionScores) {
            score = std::exp(score - maxScore);
            sumExp += score;
        }
        for (auto& score : attentionScores) {
            score /= sumExp;
        }
        
        attentionWeights[i] = attentionScores;
        
        // Apply attention to values
        for (size_t j = 0; j < seqLen; ++j) {
            for (size_t k = 0; k < m_modelDimension && k < values[j].size(); ++k) {
                output[i][k] += attentionScores[j] * values[j][k];
            }
        }
    }
    
    return std::make_pair(output, attentionWeights);
}

std::vector<std::vector<double>>
OranAiTransformer::ApplyFeedForward(const std::vector<std::vector<double>>& input)
{
    NS_LOG_FUNCTION(this);
    
    std::vector<std::vector<double>> output = input;
    
    // Simplified feed-forward network: linear -> ReLU -> linear
    for (auto& sequence : output) {
        // First linear transformation with ReLU
        for (auto& value : sequence) {
            value = std::max(0.0, value * 2.0); // Simplified weights
        }
        
        // Second linear transformation
        for (auto& value : sequence) {
            value = value * 0.5; // Simplified weights
        }
    }
    
    return output;
}

std::vector<double>
OranAiTransformer::CalculatePositionalEncoding(uint32_t position, uint32_t dimension)
{
    NS_LOG_FUNCTION(this << position << dimension);
    
    std::vector<double> encoding(dimension, 0.0);
    
    for (uint32_t i = 0; i < dimension; i += 2) {
        double angle = position / std::pow(10000.0, static_cast<double>(i) / dimension);
        encoding[i] = std::sin(angle);
        if (i + 1 < dimension) {
            encoding[i + 1] = std::cos(angle);
        }
    }
    
    return encoding;
}

std::vector<double>
OranAiTransformer::ApplyLayerNorm(const std::vector<double>& input)
{
    NS_LOG_FUNCTION(this);
    
    if (input.empty()) {
        return input;
    }
    
    // Calculate mean
    double mean = 0.0;
    for (double value : input) {
        mean += value;
    }
    mean /= input.size();
    
    // Calculate variance
    double variance = 0.0;
    for (double value : input) {
        variance += (value - mean) * (value - mean);
    }
    variance /= input.size();
    
    // Apply normalization
    std::vector<double> normalized(input.size());
    double eps = 1e-6;
    for (size_t i = 0; i < input.size(); ++i) {
        normalized[i] = (input[i] - mean) / std::sqrt(variance + eps);
    }
    
    return normalized;
}

// Additional helper methods continue...
// (Due to length constraints, showing key methods only)

} // namespace ns3