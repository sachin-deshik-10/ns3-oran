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

#include "oran-6g-semantic-communications.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include <algorithm>
#include <random>
#include <cmath>
#include <sstream>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("Oran6gSemanticCommunications");

    NS_OBJECT_ENSURE_REGISTERED(Oran6gSemanticCommunications);

    TypeId
    Oran6gSemanticCommunications::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::Oran6gSemanticCommunications")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<Oran6gSemanticCommunications>()
                                .AddAttribute("SemanticCompressionRatio",
                                              "Semantic compression efficiency ratio",
                                              DoubleValue(0.85),
                                              MakeDoubleAccessor(&Oran6gSemanticCommunications::m_compressionRatio),
                                              MakeDoubleChecker<double>(0.1, 1.0))
                                .AddAttribute("KnowledgeGraphSize",
                                              "Size of the knowledge graph in nodes",
                                              UintegerValue(10000),
                                              MakeUintegerAccessor(&Oran6gSemanticCommunications::m_knowledgeGraphSize),
                                              MakeUintegerChecker<uint32_t>())
                                .AddAttribute("SemanticAccuracy",
                                              "Semantic understanding accuracy",
                                              DoubleValue(0.92),
                                              MakeDoubleAccessor(&Oran6gSemanticCommunications::m_semanticAccuracy),
                                              MakeDoubleChecker<double>(0.5, 1.0))
                                .AddAttribute("ContextAwarenessEnabled",
                                              "Enable context-aware communication",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&Oran6gSemanticCommunications::m_contextAwareness),
                                              MakeBooleanChecker())
                                .AddAttribute("MultimodalProcessingEnabled",
                                              "Enable multimodal semantic processing",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&Oran6gSemanticCommunications::m_multimodalProcessing),
                                              MakeBooleanChecker());
        return tid;
    }

    Oran6gSemanticCommunications::Oran6gSemanticCommunications()
        : m_compressionRatio(0.85),
          m_knowledgeGraphSize(10000),
          m_semanticAccuracy(0.92),
          m_contextAwareness(true),
          m_multimodalProcessing(true),
          m_isInitialized(false),
          m_totalMessagesProcessed(0),
          m_totalDataCompressed(0),
          m_averageLatency(5.0),
          m_knowledgeBaseVersion(1)
    {
        NS_LOG_FUNCTION(this);
        InitializeSemanticFramework();
        InitializeKnowledgeGraph();
        InitializeNeuralNetworks();
        InitializeContextEngine();
    }

    Oran6gSemanticCommunications::~Oran6gSemanticCommunications()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    Oran6gSemanticCommunications::DoDispose()
    {
        NS_LOG_FUNCTION(this);
        m_knowledgeGraph.clear();
        m_semanticCache.clear();
        m_contextHistory.clear();
        m_neuralNetworks.clear();
        Object::DoDispose();
    }

    void
    Oran6gSemanticCommunications::InitializeSemanticFramework()
    {
        NS_LOG_FUNCTION(this);

        // Initialize semantic processing pipeline
        m_semanticConfig.encoding_algorithm = "Transformer-Based";
        m_semanticConfig.compression_method = "Semantic-Lossy";
        m_semanticConfig.context_window_size = 512;
        m_semanticConfig.attention_heads = 16;
        m_semanticConfig.hidden_dimensions = 768;
        m_semanticConfig.vocabulary_size = 50000;
        m_semanticConfig.max_sequence_length = 1024;

        // Initialize semantic metrics
        m_semanticMetrics.compression_efficiency = m_compressionRatio;
        m_semanticMetrics.semantic_fidelity = m_semanticAccuracy;
        m_semanticMetrics.processing_latency = m_averageLatency;
        m_semanticMetrics.bandwidth_savings = 0.0;
        m_semanticMetrics.error_rate = 0.05;

        m_isInitialized = true;

        NS_LOG_INFO("Semantic communication framework initialized");
    }

    void
    Oran6gSemanticCommunications::InitializeKnowledgeGraph()
    {
        NS_LOG_FUNCTION(this);

        // Create sample knowledge graph nodes
        for (uint32_t i = 0; i < m_knowledgeGraphSize; ++i)
        {
            KnowledgeNode node;
            node.node_id = i;
            node.concept_name = "Concept_" + std::to_string(i);
            node.semantic_embedding.resize(256); // 256-dimensional embeddings

            // Generate random embeddings
            std::random_device rd;
            std::mt19937 gen(rd());
            std::normal_distribution<> dis(0.0, 1.0);

            for (auto &val : node.semantic_embedding)
            {
                val = dis(gen);
            }

            node.confidence_score = 0.8 + (i % 20) * 0.01; // Vary confidence
            node.usage_frequency = i % 1000;
            node.last_updated = Simulator::Now();

            // Add relationships (simplified)
            if (i > 0)
            {
                node.related_concepts.push_back(i - 1);
            }
            if (i < m_knowledgeGraphSize - 1)
            {
                node.related_concepts.push_back(i + 1);
            }

            m_knowledgeGraph[i] = node;
        }

        NS_LOG_INFO("Knowledge graph initialized with " << m_knowledgeGraphSize << " nodes");
    }

    void
    Oran6gSemanticCommunications::InitializeNeuralNetworks()
    {
        NS_LOG_FUNCTION(this);

        // Initialize semantic encoder network
        SemanticNeuralNetwork encoder;
        encoder.network_name = "Semantic Encoder";
        encoder.network_type = NetworkType::TRANSFORMER;
        encoder.input_dimensions = 512;
        encoder.output_dimensions = 256;
        encoder.hidden_layers = 12;
        encoder.attention_heads = 16;
        encoder.accuracy = 0.94;
        encoder.inference_time = 2.5; // milliseconds
        encoder.is_trained = true;
        encoder.training_samples = 1000000;
        m_neuralNetworks["encoder"] = encoder;

        // Initialize semantic decoder network
        SemanticNeuralNetwork decoder;
        decoder.network_name = "Semantic Decoder";
        decoder.network_type = NetworkType::TRANSFORMER;
        decoder.input_dimensions = 256;
        decoder.output_dimensions = 512;
        decoder.hidden_layers = 12;
        decoder.attention_heads = 16;
        decoder.accuracy = 0.93;
        decoder.inference_time = 3.0;
        decoder.is_trained = true;
        decoder.training_samples = 1000000;
        m_neuralNetworks["decoder"] = decoder;

        // Initialize context understanding network
        SemanticNeuralNetwork contextNet;
        contextNet.network_name = "Context Understanding";
        contextNet.network_type = NetworkType::LSTM;
        contextNet.input_dimensions = 768;
        contextNet.output_dimensions = 128;
        contextNet.hidden_layers = 6;
        contextNet.attention_heads = 8;
        contextNet.accuracy = 0.89;
        contextNet.inference_time = 1.5;
        contextNet.is_trained = true;
        contextNet.training_samples = 500000;
        m_neuralNetworks["context"] = contextNet;

        NS_LOG_INFO("Semantic neural networks initialized");
    }

    void
    Oran6gSemanticCommunications::InitializeContextEngine()
    {
        NS_LOG_FUNCTION(this);

        if (!m_contextAwareness)
            return;

        // Initialize context processing engine
        m_contextEngine.temporal_window = Seconds(60); // 1-minute context window
        m_contextEngine.spatial_radius = 100.0;        // 100m spatial context
        m_contextEngine.semantic_similarity_threshold = 0.8;
        m_contextEngine.context_update_frequency = Seconds(5);
        m_contextEngine.max_context_history = 1000;

        // Initialize context categories
        m_contextEngine.context_categories = {
            "Location", "Time", "User_Activity", "Network_State",
            "Application_Type", "Device_Capability", "Environmental"};

        // Start context monitoring
        Simulator::Schedule(m_contextEngine.context_update_frequency,
                            &Oran6gSemanticCommunications::UpdateContextInformation, this);

        NS_LOG_INFO("Context-aware engine initialized");
    }

    SemanticMessage
    Oran6gSemanticCommunications::EncodeMessage(const std::string &originalMessage,
                                                const CommunicationContext &context)
    {
        NS_LOG_FUNCTION(this << originalMessage.length());

        SemanticMessage semanticMsg;
        semanticMsg.message_id = m_totalMessagesProcessed++;
        semanticMsg.original_size = originalMessage.length();
        semanticMsg.encoding_time = Simulator::Now();
        semanticMsg.context = context;

        // Extract semantic features
        std::vector<double> semanticFeatures = ExtractSemanticFeatures(originalMessage);

        // Apply knowledge graph enhancement
        EnhanceWithKnowledgeGraph(semanticFeatures, context);

        // Compress semantic representation
        semanticMsg.compressed_representation = CompressSemanticFeatures(semanticFeatures);
        semanticMsg.compressed_size = semanticMsg.compressed_representation.size();

        // Calculate compression ratio
        semanticMsg.compression_ratio = static_cast<double>(semanticMsg.compressed_size) /
                                        semanticMsg.original_size;

        // Add semantic metadata
        semanticMsg.semantic_metadata = GenerateSemanticMetadata(originalMessage, context);

        // Calculate semantic similarity score
        semanticMsg.semantic_fidelity = CalculateSemanticFidelity(originalMessage, semanticFeatures);

        // Update statistics
        m_totalDataCompressed += semanticMsg.original_size - semanticMsg.compressed_size;

        // Cache for potential reuse
        CacheSemanticMessage(semanticMsg);

        NS_LOG_INFO("Encoded message " << semanticMsg.message_id
                                       << " with compression ratio: " << semanticMsg.compression_ratio);

        return semanticMsg;
    }

    std::string
    Oran6gSemanticCommunications::DecodeMessage(const SemanticMessage &semanticMessage,
                                                const CommunicationContext &receiverContext)
    {
        NS_LOG_FUNCTION(this << semanticMessage.message_id);

        // Check semantic cache first
        std::string cachedResult = CheckSemanticCache(semanticMessage);
        if (!cachedResult.empty())
        {
            NS_LOG_INFO("Retrieved message from semantic cache");
            return cachedResult;
        }

        // Decompress semantic representation
        std::vector<double> semanticFeatures = DecompressSemanticFeatures(
            semanticMessage.compressed_representation);

        // Apply context-aware enhancement
        if (m_contextAwareness)
        {
            EnhanceWithReceiverContext(semanticFeatures, receiverContext);
        }

        // Use knowledge graph for semantic reconstruction
        EnhanceWithKnowledgeGraphDecoding(semanticFeatures, semanticMessage.context);

        // Generate reconstructed message
        std::string reconstructedMessage = ReconstructMessage(semanticFeatures,
                                                              semanticMessage.semantic_metadata);

        // Validate semantic accuracy
        double accuracy = ValidateSemanticAccuracy(semanticMessage, reconstructedMessage);

        NS_LOG_INFO("Decoded message " << semanticMessage.message_id
                                       << " with accuracy: " << accuracy);

        return reconstructedMessage;
    }

    std::vector<double>
    Oran6gSemanticCommunications::ExtractSemanticFeatures(const std::string &message)
    {
        std::vector<double> features;

        // Tokenize message (simplified)
        std::vector<std::string> tokens = TokenizeMessage(message);

        // Convert to embeddings using neural network
        if (m_neuralNetworks.find("encoder") != m_neuralNetworks.end())
        {
            auto &encoder = m_neuralNetworks["encoder"];
            features = SimulateNeuralNetworkInference(encoder, tokens);
        }
        else
        {
            // Fallback: simple feature extraction
            features.resize(256);
            for (size_t i = 0; i < features.size(); ++i)
            {
                features[i] = static_cast<double>(message.length() + i) / 1000.0;
            }
        }

        return features;
    }

    std::vector<std::string>
    Oran6gSemanticCommunications::TokenizeMessage(const std::string &message)
    {
        std::vector<std::string> tokens;
        std::stringstream ss(message);
        std::string token;

        while (ss >> token)
        {
            tokens.push_back(token);
        }

        return tokens;
    }

    std::vector<double>
    Oran6gSemanticCommunications::SimulateNeuralNetworkInference(
        const SemanticNeuralNetwork &network,
        const std::vector<std::string> &tokens)
    {
        std::vector<double> output(network.output_dimensions);

        // Simulate neural network processing
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> dis(0.0, 1.0);

        for (auto &val : output)
        {
            val = dis(gen) * 0.5; // Normalized output
        }

        // Add some token-based influence
        for (size_t i = 0; i < tokens.size() && i < output.size(); ++i)
        {
            output[i] += static_cast<double>(tokens[i].length()) / 100.0;
        }

        return output;
    }

    void
    Oran6gSemanticCommunications::EnhanceWithKnowledgeGraph(std::vector<double> &features,
                                                            const CommunicationContext &context)
    {
        // Find relevant knowledge graph nodes
        std::vector<uint32_t> relevantNodes = FindRelevantKnowledgeNodes(features, context);

        // Enhance features with knowledge graph information
        for (uint32_t nodeId : relevantNodes)
        {
            auto it = m_knowledgeGraph.find(nodeId);
            if (it != m_knowledgeGraph.end())
            {
                const auto &node = it->second;

                // Blend knowledge embeddings with features
                for (size_t i = 0; i < std::min(features.size(), node.semantic_embedding.size()); ++i)
                {
                    features[i] = 0.8 * features[i] + 0.2 * node.semantic_embedding[i];
                }
            }
        }
    }

    std::vector<uint32_t>
    Oran6gSemanticCommunications::FindRelevantKnowledgeNodes(
        const std::vector<double> &features,
        const CommunicationContext &context)
    {
        std::vector<uint32_t> relevantNodes;

        // Simple relevance calculation based on similarity
        for (const auto &nodePair : m_knowledgeGraph)
        {
            const auto &node = nodePair.second;
            double similarity = CalculateCosineSimilarity(features, node.semantic_embedding);

            if (similarity > 0.7) // Threshold for relevance
            {
                relevantNodes.push_back(node.node_id);
            }

            if (relevantNodes.size() >= 10) // Limit to top 10 nodes
            {
                break;
            }
        }

        return relevantNodes;
    }

    double
    Oran6gSemanticCommunications::CalculateCosineSimilarity(const std::vector<double> &vec1,
                                                            const std::vector<double> &vec2)
    {
        if (vec1.size() != vec2.size())
            return 0.0;

        double dotProduct = 0.0;
        double norm1 = 0.0;
        double norm2 = 0.0;

        for (size_t i = 0; i < vec1.size(); ++i)
        {
            dotProduct += vec1[i] * vec2[i];
            norm1 += vec1[i] * vec1[i];
            norm2 += vec2[i] * vec2[i];
        }

        if (norm1 == 0.0 || norm2 == 0.0)
            return 0.0;

        return dotProduct / (sqrt(norm1) * sqrt(norm2));
    }

    std::vector<uint8_t>
    Oran6gSemanticCommunications::CompressSemanticFeatures(const std::vector<double> &features)
    {
        std::vector<uint8_t> compressed;

        // Quantize and compress features
        for (double feature : features)
        {
            // Quantize to 8-bit representation
            int quantized = static_cast<int>((feature + 1.0) * 127.5); // Assuming features in [-1, 1]
            quantized = std::max(0, std::min(255, quantized));
            compressed.push_back(static_cast<uint8_t>(quantized));
        }

        // Apply additional compression (simplified)
        // In reality, would use more sophisticated compression algorithms

        return compressed;
    }

    SemanticMetadata
    Oran6gSemanticCommunications::GenerateSemanticMetadata(const std::string &message,
                                                           const CommunicationContext &context)
    {
        SemanticMetadata metadata;

        metadata.message_type = ClassifyMessageType(message);
        metadata.semantic_concepts = ExtractSemanticConcepts(message);
        metadata.context_tags = ExtractContextTags(context);
        metadata.importance_score = CalculateImportanceScore(message, context);
        metadata.temporal_relevance = CalculateTemporalRelevance(context);
        metadata.spatial_relevance = CalculateSpatialRelevance(context);
        metadata.user_preferences = ExtractUserPreferences(context);

        return metadata;
    }

    MessageType
    Oran6gSemanticCommunications::ClassifyMessageType(const std::string &message)
    {
        // Simple message type classification
        if (message.find("emergency") != std::string::npos ||
            message.find("urgent") != std::string::npos)
        {
            return MessageType::EMERGENCY;
        }
        else if (message.find("data") != std::string::npos ||
                 message.find("sensor") != std::string::npos)
        {
            return MessageType::SENSOR_DATA;
        }
        else if (message.find("control") != std::string::npos ||
                 message.find("command") != std::string::npos)
        {
            return MessageType::CONTROL_COMMAND;
        }
        else if (message.find("video") != std::string::npos ||
                 message.find("stream") != std::string::npos)
        {
            return MessageType::MULTIMEDIA;
        }
        else
        {
            return MessageType::GENERAL_COMMUNICATION;
        }
    }

    std::vector<std::string>
    Oran6gSemanticCommunications::ExtractSemanticConcepts(const std::string &message)
    {
        std::vector<std::string> concepts;

        // Simple concept extraction (in reality, would use NLP)
        std::vector<std::string> keywords = {
            "network", "data", "user", "device", "communication",
            "security", "performance", "latency", "bandwidth", "quality"};

        for (const std::string &keyword : keywords)
        {
            if (message.find(keyword) != std::string::npos)
            {
                concepts.push_back(keyword);
            }
        }

        return concepts;
    }

    std::vector<std::string>
    Oran6gSemanticCommunications::ExtractContextTags(const CommunicationContext &context)
    {
        std::vector<std::string> tags;

        tags.push_back("location:" + context.location);
        tags.push_back("application:" + context.application_type);
        tags.push_back("device:" + context.device_type);
        tags.push_back("priority:" + std::to_string(static_cast<int>(context.priority_level)));

        return tags;
    }

    double
    Oran6gSemanticCommunications::CalculateImportanceScore(const std::string &message,
                                                           const CommunicationContext &context)
    {
        double score = 0.5; // Base importance

        // Adjust based on priority level
        switch (context.priority_level)
        {
        case PriorityLevel::CRITICAL:
            score += 0.4;
            break;
        case PriorityLevel::HIGH:
            score += 0.3;
            break;
        case PriorityLevel::MEDIUM:
            score += 0.1;
            break;
        case PriorityLevel::LOW:
            score += 0.0;
            break;
        }

        // Adjust based on message content
        if (message.find("emergency") != std::string::npos)
        {
            score += 0.3;
        }
        if (message.find("critical") != std::string::npos)
        {
            score += 0.2;
        }

        return std::min(1.0, score);
    }

    double
    Oran6gSemanticCommunications::CalculateTemporalRelevance(const CommunicationContext &context)
    {
        // Calculate temporal relevance based on time constraints
        if (context.time_constraints.max_delay.IsZero())
        {
            return 1.0; // No time constraints
        }

        double relevance = 1.0 - (Simulator::Now().GetSeconds() /
                                  context.time_constraints.max_delay.GetSeconds());

        return std::max(0.0, std::min(1.0, relevance));
    }

    double
    Oran6gSemanticCommunications::CalculateSpatialRelevance(const CommunicationContext &context)
    {
        // Simple spatial relevance calculation
        // In reality, would consider actual geographical distances
        return 0.8; // Default spatial relevance
    }

    std::vector<std::string>
    Oran6gSemanticCommunications::ExtractUserPreferences(const CommunicationContext &context)
    {
        std::vector<std::string> preferences;

        if (context.qos_requirements.min_throughput > 100.0)
        {
            preferences.push_back("high_throughput");
        }
        if (context.qos_requirements.max_latency < MilliSeconds(10))
        {
            preferences.push_back("low_latency");
        }
        if (context.qos_requirements.min_reliability > 0.99)
        {
            preferences.push_back("high_reliability");
        }

        return preferences;
    }

    double
    Oran6gSemanticCommunications::CalculateSemanticFidelity(const std::string &original,
                                                            const std::vector<double> &features)
    {
        // Simplified fidelity calculation
        // In reality, would use more sophisticated semantic similarity measures

        double fidelity = m_semanticAccuracy;

        // Adjust based on message complexity
        double complexity = static_cast<double>(original.length()) / 1000.0;
        fidelity *= (1.0 - complexity * 0.1); // Reduce fidelity for complex messages

        // Adjust based on feature quality
        double featureQuality = 0.0;
        for (double f : features)
        {
            featureQuality += std::abs(f);
        }
        featureQuality /= features.size();

        fidelity *= (0.5 + 0.5 * std::min(1.0, featureQuality));

        return std::max(0.5, std::min(1.0, fidelity));
    }

    void
    Oran6gSemanticCommunications::CacheSemanticMessage(const SemanticMessage &message)
    {
        // Add to cache with LRU eviction
        m_semanticCache[message.message_id] = message;

        // Implement simple LRU eviction
        if (m_semanticCache.size() > 1000) // Cache size limit
        {
            auto oldest = m_semanticCache.begin();
            m_semanticCache.erase(oldest);
        }
    }

    std::string
    Oran6gSemanticCommunications::CheckSemanticCache(const SemanticMessage &message)
    {
        auto it = m_semanticCache.find(message.message_id);
        if (it != m_semanticCache.end())
        {
            // Would normally return the cached decoded message
            // For simplicity, return empty string indicating cache miss
        }

        return ""; // Cache miss
    }

    std::vector<double>
    Oran6gSemanticCommunications::DecompressSemanticFeatures(const std::vector<uint8_t> &compressed)
    {
        std::vector<double> features;

        // Decompress and dequantize
        for (uint8_t byte : compressed)
        {
            double feature = (static_cast<double>(byte) / 127.5) - 1.0; // Back to [-1, 1]
            features.push_back(feature);
        }

        return features;
    }

    void
    Oran6gSemanticCommunications::EnhanceWithReceiverContext(std::vector<double> &features,
                                                             const CommunicationContext &receiverContext)
    {
        // Adjust features based on receiver context
        if (receiverContext.device_capabilities.processing_power < 0.5)
        {
            // Simplify features for low-power devices
            for (auto &feature : features)
            {
                feature *= 0.8; // Reduce complexity
            }
        }

        if (receiverContext.network_conditions.bandwidth < 10.0)
        {
            // Further compress for low bandwidth
            for (size_t i = 1; i < features.size(); i += 2)
            {
                features[i] *= 0.5; // Reduce alternate features
            }
        }
    }

    void
    Oran6gSemanticCommunications::EnhanceWithKnowledgeGraphDecoding(
        std::vector<double> &features,
        const CommunicationContext &context)
    {
        // Similar to encoding enhancement but for decoding
        std::vector<uint32_t> relevantNodes = FindRelevantKnowledgeNodes(features, context);

        for (uint32_t nodeId : relevantNodes)
        {
            auto it = m_knowledgeGraph.find(nodeId);
            if (it != m_knowledgeGraph.end())
            {
                const auto &node = it->second;

                // Enhance features with knowledge for better reconstruction
                for (size_t i = 0; i < std::min(features.size(), node.semantic_embedding.size()); ++i)
                {
                    features[i] = 0.7 * features[i] + 0.3 * node.semantic_embedding[i];
                }
            }
        }
    }

    std::string
    Oran6gSemanticCommunications::ReconstructMessage(const std::vector<double> &features,
                                                     const SemanticMetadata &metadata)
    {
        std::string reconstructed;

        // Use decoder neural network if available
        if (m_neuralNetworks.find("decoder") != m_neuralNetworks.end())
        {
            auto &decoder = m_neuralNetworks["decoder"];
            std::vector<std::string> tokens = SimulateDecoderInference(decoder, features);

            // Combine tokens into message
            for (const auto &token : tokens)
            {
                if (!reconstructed.empty())
                    reconstructed += " ";
                reconstructed += token;
            }
        }
        else
        {
            // Fallback reconstruction
            reconstructed = "Reconstructed message with " + std::to_string(features.size()) + " semantic features";
        }

        // Enhance with metadata information
        EnhanceReconstructionWithMetadata(reconstructed, metadata);

        return reconstructed;
    }

    std::vector<std::string>
    Oran6gSemanticCommunications::SimulateDecoderInference(const SemanticNeuralNetwork &decoder,
                                                           const std::vector<double> &features)
    {
        std::vector<std::string> tokens;

        // Simulate decoder output based on features
        std::vector<std::string> vocabulary = {
            "network", "data", "communication", "user", "device",
            "performance", "quality", "service", "application", "system",
            "connection", "bandwidth", "latency", "throughput", "reliability"};

        // Select tokens based on feature values
        for (size_t i = 0; i < features.size() && tokens.size() < 20; ++i)
        {
            if (std::abs(features[i]) > 0.3) // Threshold for token selection
            {
                size_t vocabIndex = static_cast<size_t>(std::abs(features[i]) * vocabulary.size()) % vocabulary.size();
                tokens.push_back(vocabulary[vocabIndex]);
            }
        }

        return tokens;
    }

    void
    Oran6gSemanticCommunications::EnhanceReconstructionWithMetadata(std::string &reconstructed,
                                                                    const SemanticMetadata &metadata)
    {
        // Add important concepts from metadata
        for (const auto &concept : metadata.semantic_concepts)
        {
            if (reconstructed.find(concept) == std::string::npos)
            {
                reconstructed += " " + concept;
            }
        }

        // Adjust based on message type
        switch (metadata.message_type)
        {
        case MessageType::EMERGENCY:
            if (reconstructed.find("emergency") == std::string::npos)
            {
                reconstructed = "EMERGENCY: " + reconstructed;
            }
            break;
        case MessageType::CONTROL_COMMAND:
            if (reconstructed.find("command") == std::string::npos)
            {
                reconstructed += " [COMMAND]";
            }
            break;
        default:
            break;
        }
    }

    double
    Oran6gSemanticCommunications::ValidateSemanticAccuracy(const SemanticMessage &original,
                                                           const std::string &reconstructed)
    {
        // Simple accuracy validation
        // In reality, would use sophisticated semantic similarity measures

        double accuracy = m_semanticAccuracy;

        // Adjust based on compression ratio
        accuracy *= (0.5 + 0.5 * original.compression_ratio);

        // Add some randomness to simulate real-world variations
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> noise(0.0, 0.05);

        accuracy += noise(gen);
        accuracy = std::max(0.6, std::min(0.99, accuracy));

        return accuracy;
    }

    void
    Oran6gSemanticCommunications::UpdateContextInformation()
    {
        NS_LOG_FUNCTION(this);

        if (!m_contextAwareness)
            return;

        // Simulate context updates
        ContextInformation newContext;
        newContext.timestamp = Simulator::Now();
        newContext.location = "Dynamic_Location_" + std::to_string(Simulator::Now().GetSeconds());
        newContext.user_activity = "Active";
        newContext.network_state = "Normal";
        newContext.environmental_factors = {"Indoor", "Low_Noise", "Good_Lighting"};

        // Add to context history
        m_contextHistory.push_back(newContext);

        // Maintain context history size
        if (m_contextHistory.size() > m_contextEngine.max_context_history)
        {
            m_contextHistory.erase(m_contextHistory.begin());
        }

        // Schedule next update
        Simulator::Schedule(m_contextEngine.context_update_frequency,
                            &Oran6gSemanticCommunications::UpdateContextInformation, this);
    }

    void
    Oran6gSemanticCommunications::UpdateKnowledgeGraph(const std::string &newConcept,
                                                       const std::vector<double> &embedding)
    {
        NS_LOG_FUNCTION(this << newConcept);

        KnowledgeNode newNode;
        newNode.node_id = m_knowledgeGraph.size();
        newNode.concept_name = newConcept;
        newNode.semantic_embedding = embedding;
        newNode.confidence_score = 0.7; // Initial confidence
        newNode.usage_frequency = 1;
        newNode.last_updated = Simulator::Now();

        // Find related concepts
        for (const auto &nodePair : m_knowledgeGraph)
        {
            double similarity = CalculateCosineSimilarity(embedding, nodePair.second.semantic_embedding);
            if (similarity > 0.8)
            {
                newNode.related_concepts.push_back(nodePair.first);
            }
        }

        m_knowledgeGraph[newNode.node_id] = newNode;
        m_knowledgeBaseVersion++;

        NS_LOG_INFO("Added new concept to knowledge graph: " << newConcept);
    }

    SemanticCommunicationStats
    Oran6gSemanticCommunications::GetStats() const
    {
        SemanticCommunicationStats stats;

        stats.total_messages_processed = m_totalMessagesProcessed;
        stats.total_data_compressed = m_totalDataCompressed;
        stats.average_compression_ratio = m_compressionRatio;
        stats.average_semantic_accuracy = m_semanticAccuracy;
        stats.average_processing_latency = m_averageLatency;
        stats.knowledge_graph_size = m_knowledgeGraph.size();
        stats.semantic_cache_size = m_semanticCache.size();
        stats.context_history_size = m_contextHistory.size();
        stats.knowledge_base_version = m_knowledgeBaseVersion;

        // Calculate bandwidth savings
        if (m_totalMessagesProcessed > 0)
        {
            stats.bandwidth_savings = static_cast<double>(m_totalDataCompressed) /
                                      (m_totalMessagesProcessed * 1000); // Assume 1KB average message
        }
        else
        {
            stats.bandwidth_savings = 0.0;
        }

        return stats;
    }

    std::vector<ContextInformation>
    Oran6gSemanticCommunications::GetRecentContext(uint32_t count) const
    {
        std::vector<ContextInformation> recent;

        if (m_contextHistory.empty())
            return recent;

        size_t start = 0;
        if (m_contextHistory.size() > count)
        {
            start = m_contextHistory.size() - count;
        }

        for (size_t i = start; i < m_contextHistory.size(); ++i)
        {
            recent.push_back(m_contextHistory[i]);
        }

        return recent;
    }

    std::vector<std::string>
    Oran6gSemanticCommunications::GetTopSemanticConcepts(uint32_t count) const
    {
        std::vector<std::pair<std::string, uint32_t>> conceptFrequency;

        // Collect concept frequencies
        for (const auto &nodePair : m_knowledgeGraph)
        {
            conceptFrequency.push_back({nodePair.second.concept_name,
                                        nodePair.second.usage_frequency});
        }

        // Sort by frequency
        std::sort(conceptFrequency.begin(), conceptFrequency.end(),
                  [](const auto &a, const auto &b)
                  { return a.second > b.second; });

        std::vector<std::string> topConcepts;
        for (size_t i = 0; i < std::min(static_cast<size_t>(count), conceptFrequency.size()); ++i)
        {
            topConcepts.push_back(conceptFrequency[i].first);
        }

        return topConcepts;
    }

    double
    Oran6gSemanticCommunications::GetSemanticSimilarity(const std::string &message1,
                                                        const std::string &message2) const
    {
        // Extract features for both messages
        std::vector<double> features1 = const_cast<Oran6gSemanticCommunications *>(this)->ExtractSemanticFeatures(message1);
        std::vector<double> features2 = const_cast<Oran6gSemanticCommunications *>(this)->ExtractSemanticFeatures(message2);

        return CalculateCosineSimilarity(features1, features2);
    }

    bool
    Oran6gSemanticCommunications::IsSemanticCompressionEffective(double originalSize,
                                                                 double compressedSize) const
    {
        double compressionRatio = compressedSize / originalSize;
        return compressionRatio < 0.9; // Effective if compressed to less than 90% of original
    }

    std::string
    Oran6gSemanticCommunications::GetSemanticReport() const
    {
        std::stringstream report;

        report << "=== Semantic Communications Report ===\n";
        report << "Total Messages Processed: " << m_totalMessagesProcessed << "\n";
        report << "Average Compression Ratio: " << (m_compressionRatio * 100) << "%\n";
        report << "Semantic Accuracy: " << (m_semanticAccuracy * 100) << "%\n";
        report << "Knowledge Graph Size: " << m_knowledgeGraph.size() << " nodes\n";
        report << "Average Processing Latency: " << m_averageLatency << " ms\n";
        report << "Context Awareness: " << (m_contextAwareness ? "Enabled" : "Disabled") << "\n";
        report << "Multimodal Processing: " << (m_multimodalProcessing ? "Enabled" : "Disabled") << "\n";
        report << "Total Data Saved: " << (m_totalDataCompressed / 1024.0) << " KB\n";

        return report.str();
    }

} // namespace ns3
