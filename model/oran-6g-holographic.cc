/*
 * Copyright (c) 2025 O-RAN 6G Research Consortium
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * O-RAN 6G Holographic Communication Module
 * Advanced holographic data transmission and 3D content delivery
 */

#include "oran-6g-holographic.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/random-variable-stream.h"
#include "ns3/uniform-random-variable.h"
#include "ns3/normal-random-variable.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <complex>
#include <fftw3.h> // For FFT operations (if available)

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("Oran6gHolographic");

    NS_OBJECT_ENSURE_REGISTERED(Oran6gHolographic);

    TypeId
    Oran6gHolographic::GetTypeId(void)
    {
        static TypeId tid =
            TypeId("ns3::Oran6gHolographic")
                .SetParent<Object>()
                .SetGroupName("Oran")
                .AddConstructor<Oran6gHolographic>()
                .AddAttribute("HolographicResolution",
                              "Holographic resolution in points per cubic meter",
                              DoubleValue(1000000.0),
                              MakeDoubleAccessor(&Oran6gHolographic::m_holographicResolution),
                              MakeDoubleChecker<double>(1000.0))
                .AddAttribute("CompressionRatio",
                              "Holographic data compression ratio",
                              DoubleValue(10.0),
                              MakeDoubleAccessor(&Oran6gHolographic::m_compressionRatio),
                              MakeDoubleChecker<double>(1.0, 100.0))
                .AddAttribute("WavelengthRange",
                              "Operating wavelength range in nanometers",
                              StringValue("380-780"),
                              MakeStringAccessor(&Oran6gHolographic::m_wavelengthRange),
                              MakeStringChecker())
                .AddAttribute("CoherenceLength",
                              "Coherence length in micrometers",
                              DoubleValue(100.0),
                              MakeDoubleAccessor(&Oran6gHolographic::m_coherenceLength),
                              MakeDoubleChecker<double>(1.0))
                .AddAttribute("EnableRealTimeRendering",
                              "Enable real-time hologram rendering",
                              BooleanValue(true),
                              MakeBooleanAccessor(&Oran6gHolographic::m_enableRealTimeRendering),
                              MakeBooleanChecker())
                .AddAttribute("SpatialLayers",
                              "Number of spatial layers for holographic content",
                              UintegerValue(8),
                              MakeUintegerAccessor(&Oran6gHolographic::m_spatialLayers),
                              MakeUintegerChecker<uint32_t>(1, 64))
                .AddAttribute("QuantumCoherence",
                              "Enable quantum coherence effects",
                              BooleanValue(false),
                              MakeBooleanAccessor(&Oran6gHolographic::m_quantumCoherence),
                              MakeBooleanChecker())
                .AddTraceSource("HologramTransmitted",
                                "Trace fired when a hologram is transmitted",
                                MakeTraceSourceAccessor(&Oran6gHolographic::m_hologramTransmittedTrace),
                                "ns3::Oran6gHolographic::HologramTransmittedCallback")
                .AddTraceSource("LightFieldProcessed",
                                "Trace fired when a light field is processed",
                                MakeTraceSourceAccessor(&Oran6gHolographic::m_lightFieldProcessedTrace),
                                "ns3::Oran6gHolographic::LightFieldProcessedCallback")
                .AddTraceSource("SpatialCompressionApplied",
                                "Trace fired when spatial compression is applied",
                                MakeTraceSourceAccessor(&Oran6gHolographic::m_spatialCompressionTrace),
                                "ns3::Oran6gHolographic::SpatialCompressionCallback");
        return tid;
    }

    Oran6gHolographic::Oran6gHolographic()
        : m_holographicResolution(1000000.0),
          m_compressionRatio(10.0),
          m_wavelengthRange("380-780"),
          m_coherenceLength(100.0),
          m_enableRealTimeRendering(true),
          m_spatialLayers(8),
          m_quantumCoherence(false),
          m_nextFrameId(1),
          m_nextFieldId(1),
          m_nextLayerId(1),
          m_isInitialized(false),
          m_totalHologramsProcessed(0),
          m_totalDataTransmitted(0),
          m_renderingLatency(0.0),
          m_compressionEfficiency(0.0)
    {
        NS_LOG_FUNCTION(this);

        // Initialize random number generators
        m_uniformRng = CreateObject<UniformRandomVariable>();
        m_normalRng = CreateObject<NormalRandomVariable>();

        // Initialize holographic parameters
        ParseWavelengthRange();
        InitializeDefaultParameters();

        // Initialize performance metrics
        m_performanceMetrics.transmission_latency = 0.0;
        m_performanceMetrics.rendering_quality = 1.0;
        m_performanceMetrics.compression_efficiency = 0.0;
        m_performanceMetrics.spatial_resolution = m_holographicResolution;
        m_performanceMetrics.temporal_coherence = 0.0;
        m_performanceMetrics.processing_throughput = 0.0;

        NS_LOG_INFO("O-RAN 6G Holographic Communication system created");
    }

    Oran6gHolographic::~Oran6gHolographic()
    {
        NS_LOG_FUNCTION(this);

        // Clean up holographic data structures
        m_hologramFrames.clear();
        m_lightFields.clear();
        m_spatialLayers.clear();
        m_pointClouds.clear();

        NS_LOG_INFO("O-RAN 6G Holographic Communication system destroyed");
    }

    void
    Oran6gHolographic::InitializeHolographicSystem(double resolution,
                                                   const Vector3D &volume_dimensions,
                                                   const std::string &rendering_mode)
    {
        NS_LOG_FUNCTION(this << resolution << rendering_mode);

        m_holographicResolution = resolution;
        m_volumeDimensions = volume_dimensions;
        m_renderingMode = rendering_mode;

        // Initialize holographic volume
        InitializeHolographicVolume();

        // Setup rendering pipeline
        SetupRenderingPipeline(rendering_mode);

        // Initialize spatial indexing
        InitializeSpatialIndexing();

        // Setup compression algorithms
        InitializeCompressionAlgorithms();

        // Initialize light field processing
        InitializeLightFieldProcessing();

        m_isInitialized = true;

        NS_LOG_INFO("Holographic system initialized with resolution: " << resolution
                                                                       << " and rendering mode: " << rendering_mode);
    }

    uint32_t
    Oran6gHolographic::CreateHologram(const std::vector<HolographicPoint> &point_cloud,
                                      const std::string &encoding_method)
    {
        NS_LOG_FUNCTION(this << point_cloud.size() << encoding_method);

        if (!m_isInitialized)
        {
            NS_LOG_WARN("Holographic system not initialized");
            return 0;
        }

        uint32_t frame_id = m_nextFrameId++;

        // Create hologram frame
        HologramFrame frame;
        frame.frameId = frame_id;
        frame.points = point_cloud;
        frame.timestamp = Simulator::Now();
        frame.quality = CalculateHologramQuality(point_cloud);

        // Generate light field from point cloud
        frame.lightField = GenerateLightField(point_cloud, encoding_method);

        // Apply compression
        frame.compressionRatio = ApplyHolographicCompression(frame, encoding_method);

        // Store hologram
        m_hologramFrames[frame_id] = frame;

        // Update performance metrics
        m_totalHologramsProcessed++;
        UpdatePerformanceMetrics();

        // Fire trace
        m_hologramTransmittedTrace(frame_id, point_cloud.size(), frame.quality);

        NS_LOG_INFO("Created hologram with ID: " << frame_id
                                                 << " containing " << point_cloud.size() << " points");

        return frame_id;
    }

    double
    Oran6gHolographic::TransmitHologram(uint32_t hologram_id,
                                        const NodeContainer &destination_nodes,
                                        const std::string &transmission_protocol)
    {
        NS_LOG_FUNCTION(this << hologram_id << destination_nodes.GetN() << transmission_protocol);

        if (m_hologramFrames.find(hologram_id) == m_hologramFrames.end())
        {
            NS_LOG_ERROR("Hologram not found: " << hologram_id);
            return 0.0;
        }

        const auto &hologram = m_hologramFrames[hologram_id];
        double transmission_success = 0.0;

        // Calculate transmission parameters
        uint32_t data_size = CalculateHologramDataSize(hologram);
        double transmission_latency = CalculateTransmissionLatency(data_size, transmission_protocol);

        // Transmit to each destination node
        for (uint32_t i = 0; i < destination_nodes.GetN(); ++i)
        {
            Ptr<Node> node = destination_nodes.Get(i);

            if (transmission_protocol == "light-field-streaming")
            {
                transmission_success += TransmitLightFieldStream(hologram, node);
            }
            else if (transmission_protocol == "compressed-holographic")
            {
                transmission_success += TransmitCompressedHologram(hologram, node);
            }
            else if (transmission_protocol == "adaptive-quality")
            {
                transmission_success += TransmitAdaptiveQuality(hologram, node);
            }
            else if (transmission_protocol == "quantum-coherent")
            {
                transmission_success += TransmitQuantumCoherent(hologram, node);
            }
            else
            {
                // Default point cloud transmission
                transmission_success += TransmitPointCloud(hologram, node);
            }
        }

        // Average success rate
        transmission_success /= destination_nodes.GetN();

        // Update statistics
        m_totalDataTransmitted += data_size;
        m_performanceMetrics.transmission_latency = transmission_latency;

        NS_LOG_INFO("Transmitted hologram " << hologram_id << " to " << destination_nodes.GetN()
                                            << " nodes with success rate: " << transmission_success);

        return transmission_success;
    }

    std::vector<HolographicPoint>
    Oran6gHolographic::ProcessLightField(const LightField &light_field,
                                         const std::string &processing_algorithm)
    {
        NS_LOG_FUNCTION(this << light_field.fieldId << processing_algorithm);

        std::vector<HolographicPoint> processed_points;

        if (processing_algorithm == "fourier-transform")
        {
            processed_points = ProcessFourierTransform(light_field);
        }
        else if (processing_algorithm == "wavelet-decomposition")
        {
            processed_points = ProcessWaveletDecomposition(light_field);
        }
        else if (processing_algorithm == "holographic-reconstruction")
        {
            processed_points = ProcessHolographicReconstruction(light_field);
        }
        else if (processing_algorithm == "coherent-detection")
        {
            processed_points = ProcessCoherentDetection(light_field);
        }
        else if (processing_algorithm == "interference-pattern")
        {
            processed_points = ProcessInterferencePattern(light_field);
        }
        else if (processing_algorithm == "phase-retrieval")
        {
            processed_points = ProcessPhaseRetrieval(light_field);
        }
        else
        {
            // Default direct conversion
            processed_points = ProcessDirectConversion(light_field);
        }

        // Fire trace
        m_lightFieldProcessedTrace(light_field.fieldId, processed_points.size());

        NS_LOG_INFO("Processed light field " << light_field.fieldId
                                             << " using " << processing_algorithm
                                             << " resulting in " << processed_points.size() << " points");

        return processed_points;
    }

    double
    Oran6gHolographic::ApplyAdvancedCompression(uint32_t hologram_id,
                                                const std::string &compression_algorithm,
                                                double quality_threshold)
    {
        NS_LOG_FUNCTION(this << hologram_id << compression_algorithm << quality_threshold);

        if (m_hologramFrames.find(hologram_id) == m_hologramFrames.end())
        {
            NS_LOG_ERROR("Hologram not found for compression: " << hologram_id);
            return 0.0;
        }

        auto &hologram = m_hologramFrames[hologram_id];
        double compression_ratio = 1.0;

        if (compression_algorithm == "spatial-wavelet")
        {
            compression_ratio = ApplySpatialWaveletCompression(hologram, quality_threshold);
        }
        else if (compression_algorithm == "temporal-prediction")
        {
            compression_ratio = ApplyTemporalPredictionCompression(hologram, quality_threshold);
        }
        else if (compression_algorithm == "holographic-entropy")
        {
            compression_ratio = ApplyHolographicEntropyCompression(hologram, quality_threshold);
        }
        else if (compression_algorithm == "quantum-compression")
        {
            compression_ratio = ApplyQuantumCompression(hologram, quality_threshold);
        }
        else if (compression_algorithm == "neural-compression")
        {
            compression_ratio = ApplyNeuralCompression(hologram, quality_threshold);
        }
        else if (compression_algorithm == "adaptive-mesh")
        {
            compression_ratio = ApplyAdaptiveMeshCompression(hologram, quality_threshold);
        }
        else
        {
            // Default lossless compression
            compression_ratio = ApplyLosslessCompression(hologram);
        }

        // Update hologram compression ratio
        hologram.compressionRatio = compression_ratio;

        // Update performance metrics
        m_compressionEfficiency = compression_ratio;
        m_performanceMetrics.compression_efficiency = compression_ratio;

        // Fire trace
        m_spatialCompressionTrace(hologram_id, compression_ratio, quality_threshold);

        NS_LOG_INFO("Applied " << compression_algorithm << " compression to hologram "
                               << hologram_id << " achieving ratio: " << compression_ratio);

        return compression_ratio;
    }

    std::vector<SpatialLayer>
    Oran6gHolographic::CreateSpatialLayers(const std::vector<HolographicPoint> &point_cloud,
                                           uint32_t num_layers,
                                           const std::string &layering_strategy)
    {
        NS_LOG_FUNCTION(this << point_cloud.size() << num_layers << layering_strategy);

        std::vector<SpatialLayer> layers;

        if (layering_strategy == "depth-based")
        {
            layers = CreateDepthBasedLayers(point_cloud, num_layers);
        }
        else if (layering_strategy == "importance-based")
        {
            layers = CreateImportanceBasedLayers(point_cloud, num_layers);
        }
        else if (layering_strategy == "frequency-based")
        {
            layers = CreateFrequencyBasedLayers(point_cloud, num_layers);
        }
        else if (layering_strategy == "adaptive-octree")
        {
            layers = CreateAdaptiveOctreeLayers(point_cloud, num_layers);
        }
        else if (layering_strategy == "perceptual-quality")
        {
            layers = CreatePerceptualQualityLayers(point_cloud, num_layers);
        }
        else
        {
            // Default uniform layering
            layers = CreateUniformLayers(point_cloud, num_layers);
        }

        // Store layers
        for (auto &layer : layers)
        {
            layer.layerId = m_nextLayerId++;
            m_spatialLayers[layer.layerId] = layer;
        }

        NS_LOG_INFO("Created " << layers.size() << " spatial layers using "
                               << layering_strategy << " strategy");

        return layers;
    }

    double
    Oran6gHolographic::RenderRealTimeHologram(uint32_t hologram_id,
                                              const Vector3D &viewing_position,
                                              const Vector3D &viewing_direction)
    {
        NS_LOG_FUNCTION(this << hologram_id << viewing_position << viewing_direction);

        if (m_hologramFrames.find(hologram_id) == m_hologramFrames.end())
        {
            NS_LOG_ERROR("Hologram not found for rendering: " << hologram_id);
            return 0.0;
        }

        const auto &hologram = m_hologramFrames[hologram_id];

        auto start_time = std::chrono::high_resolution_clock::now();

        // Calculate viewing parameters
        ViewingParameters params = CalculateViewingParameters(viewing_position, viewing_direction);

        // Perform frustum culling
        std::vector<HolographicPoint> visible_points = PerformFrustumCulling(hologram.points, params);

        // Apply level-of-detail
        std::vector<HolographicPoint> lod_points = ApplyLevelOfDetail(visible_points, params);

        // Generate interference patterns
        InterferencePattern pattern = GenerateInterferencePattern(lod_points, params);

        // Render holographic display
        double rendering_quality = RenderHolographicDisplay(pattern, params);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        // Update rendering latency
        m_renderingLatency = duration.count() / 1000.0; // Convert to milliseconds
        m_performanceMetrics.rendering_quality = rendering_quality;

        NS_LOG_INFO("Rendered real-time hologram " << hologram_id
                                                   << " with quality: " << rendering_quality
                                                   << " in " << m_renderingLatency << " ms");

        return rendering_quality;
    }

    // Private helper methods implementation

    void
    Oran6gHolographic::ParseWavelengthRange()
    {
        // Parse wavelength range string (e.g., "380-780")
        size_t dash_pos = m_wavelengthRange.find('-');
        if (dash_pos != std::string::npos)
        {
            m_minWavelength = std::stod(m_wavelengthRange.substr(0, dash_pos));
            m_maxWavelength = std::stod(m_wavelengthRange.substr(dash_pos + 1));
        }
        else
        {
            // Default visible spectrum
            m_minWavelength = 380.0;
            m_maxWavelength = 780.0;
        }
    }

    void
    Oran6gHolographic::InitializeDefaultParameters()
    {
        // Initialize default holographic parameters
        m_defaultWavelength = (m_minWavelength + m_maxWavelength) / 2.0; // 580 nm (green)
        m_refractiveIndex = 1.0;                                         // Vacuum/air
        m_pixelPitch = 8.0;                                              // 8 μm pixel pitch
        m_propagationDistance = 1000.0;                                  // 1 mm
        m_numericalAperture = 0.1;

        NS_LOG_DEBUG("Initialized default holographic parameters");
    }

    void
    Oran6gHolographic::InitializeHolographicVolume()
    {
        // Calculate volume parameters
        m_volumeVoxels.x = static_cast<uint32_t>(m_volumeDimensions.x * std::cbrt(m_holographicResolution));
        m_volumeVoxels.y = static_cast<uint32_t>(m_volumeDimensions.y * std::cbrt(m_holographicResolution));
        m_volumeVoxels.z = static_cast<uint32_t>(m_volumeDimensions.z * std::cbrt(m_holographicResolution));

        // Initialize voxel grid
        m_voxelGrid.resize(m_volumeVoxels.x * m_volumeVoxels.y * m_volumeVoxels.z);

        NS_LOG_INFO("Initialized holographic volume: " << m_volumeVoxels.x
                                                       << "x" << m_volumeVoxels.y << "x" << m_volumeVoxels.z << " voxels");
    }

    LightField
    Oran6gHolographic::GenerateLightField(const std::vector<HolographicPoint> &point_cloud,
                                          const std::string &encoding_method)
    {
        LightField light_field;
        light_field.fieldId = m_nextFieldId++;
        light_field.wavelength = m_defaultWavelength;
        light_field.coherenceLength = m_coherenceLength;
        light_field.dimensions = m_volumeDimensions;

        // Generate complex amplitudes and phases
        uint32_t field_size = m_volumeVoxels.x * m_volumeVoxels.y;
        light_field.amplitudes.resize(field_size);
        light_field.phases.resize(field_size);

        if (encoding_method == "fresnel-hologram")
        {
            GenerateFresnelHologram(point_cloud, light_field);
        }
        else if (encoding_method == "fourier-hologram")
        {
            GenerateFourierHologram(point_cloud, light_field);
        }
        else if (encoding_method == "computer-generated")
        {
            GenerateComputerGeneratedHologram(point_cloud, light_field);
        }
        else if (encoding_method == "digital-holography")
        {
            GenerateDigitalHologram(point_cloud, light_field);
        }
        else
        {
            // Default amplitude-phase encoding
            GenerateAmplitudePhaseEncoding(point_cloud, light_field);
        }

        return light_field;
    }

    void
    Oran6gHolographic::UpdatePerformanceMetrics()
    {
        // Update temporal coherence
        m_performanceMetrics.temporal_coherence = CalculateTemporalCoherence();

        // Update processing throughput
        m_performanceMetrics.processing_throughput = CalculateProcessingThroughput();

        // Update spatial resolution
        m_performanceMetrics.spatial_resolution = m_holographicResolution;

        NS_LOG_DEBUG("Performance metrics updated");
    }

    double
    Oran6gHolographic::CalculateTemporalCoherence()
    {
        // Calculate temporal coherence based on wavelength and bandwidth
        double bandwidth = m_maxWavelength - m_minWavelength;
        double coherence_time = m_defaultWavelength * m_defaultWavelength / (3e8 * bandwidth * 1e-9);

        return std::min(1.0, coherence_time / 1e-12); // Normalize to picoseconds
    }

    double
    Oran6gHolographic::CalculateProcessingThroughput()
    {
        if (m_renderingLatency > 0.0)
        {
            return 1000.0 / m_renderingLatency; // Frames per second
        }
        return 0.0;
    }

    void
    Oran6gHolographic::SetupRenderingPipeline(const std::string &rendering_mode)
    {
        NS_LOG_FUNCTION(this << rendering_mode);

        if (rendering_mode == "real-time")
        {
            m_renderingPipeline.enable_real_time = true;
            m_renderingPipeline.quality_level = 0.8;
            m_renderingPipeline.frame_rate_target = 60.0;
        }
        else if (rendering_mode == "high-quality")
        {
            m_renderingPipeline.enable_real_time = false;
            m_renderingPipeline.quality_level = 1.0;
            m_renderingPipeline.frame_rate_target = 30.0;
        }
        else if (rendering_mode == "adaptive")
        {
            m_renderingPipeline.enable_real_time = true;
            m_renderingPipeline.quality_level = 0.9;
            m_renderingPipeline.frame_rate_target = 45.0;
        }

        NS_LOG_INFO("Rendering pipeline configured for mode: " << rendering_mode);
    }

    void
    Oran6gHolographic::InitializeSpatialIndexing()
    {
        NS_LOG_FUNCTION(this);

        // Initialize octree for spatial indexing
        m_spatialIndex.max_depth = 8;
        m_spatialIndex.max_points_per_node = 1000;
        m_spatialIndex.bounds = m_volumeDimensions;

        // Create root octree node
        m_spatialIndex.root_node = std::make_shared<OctreeNode>();
        m_spatialIndex.root_node->bounds = m_volumeDimensions;
        m_spatialIndex.root_node->depth = 0;

        NS_LOG_INFO("Spatial indexing initialized with octree structure");
    }

    void
    Oran6gHolographic::InitializeCompressionAlgorithms()
    {
        NS_LOG_FUNCTION(this);

        // Initialize compression algorithms
        m_compressionAlgorithms["wavelet"] = {{"quality", 0.85}, {"ratio", 8.0}};
        m_compressionAlgorithms["fourier"] = {{"quality", 0.90}, {"ratio", 6.0}};
        m_compressionAlgorithms["neural"] = {{"quality", 0.95}, {"ratio", 12.0}};
        m_compressionAlgorithms["quantum"] = {{"quality", 0.98}, {"ratio", 15.0}};

        NS_LOG_INFO("Compression algorithms initialized");
    }

    void
    Oran6gHolographic::InitializeLightFieldProcessing()
    {
        NS_LOG_FUNCTION(this);

        // Initialize light field processing parameters
        m_lightFieldParams.angular_resolution = 256;
        m_lightFieldParams.spatial_resolution = 1024;
        m_lightFieldParams.depth_resolution = 128;
        m_lightFieldParams.temporal_resolution = 60; // FPS

        NS_LOG_INFO("Light field processing initialized");
    }

    double
    Oran6gHolographic::CalculateHologramQuality(const std::vector<HolographicPoint> &point_cloud)
    {
        if (point_cloud.empty())
            return 0.0;

        double quality = 0.0;
        double total_intensity = 0.0;
        double spatial_coherence = 0.0;

        // Calculate average intensity
        for (const auto &point : point_cloud)
        {
            total_intensity += point.intensity;
        }
        double avg_intensity = total_intensity / point_cloud.size();

        // Calculate spatial coherence
        for (size_t i = 0; i < point_cloud.size() && i < 1000; ++i) // Sample for performance
        {
            for (size_t j = i + 1; j < point_cloud.size() && j < 1000; ++j)
            {
                Vector3D diff = point_cloud[i].position - point_cloud[j].position;
                double distance = std::sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

                if (distance < m_coherenceLength)
                {
                    double phase_diff = std::abs(point_cloud[i].phase - point_cloud[j].phase);
                    spatial_coherence += std::cos(phase_diff) * std::exp(-distance / m_coherenceLength);
                }
            }
        }

        // Normalize coherence
        spatial_coherence /= (point_cloud.size() * point_cloud.size());

        // Combine factors for overall quality
        quality = 0.6 * (avg_intensity / 1.0) + 0.4 * spatial_coherence;

        return std::min(1.0, std::max(0.0, quality));
    }

    uint32_t
    Oran6gHolographic::CalculateHologramDataSize(const HologramFrame &hologram)
    {
        // Calculate data size in bytes
        uint32_t point_data_size = hologram.points.size() *
                                   (3 * sizeof(double) + // position
                                    3 * sizeof(double) + // color
                                    2 * sizeof(double) + // intensity, phase
                                    3 * sizeof(double)); // normal

        uint32_t light_field_size = hologram.lightField.amplitudes.size() *
                                    (sizeof(std::complex<double>) + sizeof(double));

        uint32_t metadata_size = 64; // Fixed metadata size

        uint32_t total_size = point_data_size + light_field_size + metadata_size;

        // Apply compression
        total_size = static_cast<uint32_t>(total_size / hologram.compressionRatio);

        return total_size;
    }

    double
    Oran6gHolographic::CalculateTransmissionLatency(uint32_t data_size, const std::string &protocol)
    {
        double bandwidth = 1e9; // 1 Gbps default
        double protocol_overhead = 1.0;

        if (protocol == "light-field-streaming")
        {
            bandwidth = 10e9; // 10 Gbps
            protocol_overhead = 1.2;
        }
        else if (protocol == "compressed-holographic")
        {
            bandwidth = 5e9; // 5 Gbps
            protocol_overhead = 1.1;
        }
        else if (protocol == "quantum-coherent")
        {
            bandwidth = 100e9; // 100 Gbps
            protocol_overhead = 1.5;
        }

        // Calculate latency in milliseconds
        double transmission_time = (data_size * 8.0 * protocol_overhead) / bandwidth * 1000.0;
        double processing_latency = m_uniformRng->GetValue(0.1, 0.5); // Processing overhead

        return transmission_time + processing_latency;
    }

    double
    Oran6gHolographic::TransmitLightFieldStream(const HologramFrame &hologram, Ptr<Node> node)
    {
        // Simulate light field streaming transmission
        double success_probability = 0.95;
        double quality_factor = hologram.quality;
        double compression_efficiency = 1.0 / hologram.compressionRatio;

        // Factor in network conditions
        double network_quality = m_uniformRng->GetValue(0.8, 1.0);

        double transmission_success = success_probability * quality_factor *
                                      compression_efficiency * network_quality;

        return std::min(1.0, transmission_success);
    }

    double
    Oran6gHolographic::TransmitCompressedHologram(const HologramFrame &hologram, Ptr<Node> node)
    {
        // Simulate compressed hologram transmission
        double success_probability = 0.92;
        double compression_benefit = std::min(2.0, hologram.compressionRatio / 5.0);

        return std::min(1.0, success_probability * compression_benefit);
    }

    double
    Oran6gHolographic::TransmitAdaptiveQuality(const HologramFrame &hologram, Ptr<Node> node)
    {
        // Adaptive quality transmission based on conditions
        double base_success = 0.88;
        double adaptive_factor = 1.0;

        // Adapt based on hologram complexity
        if (hologram.points.size() > 100000)
        {
            adaptive_factor = 0.9; // Reduce quality for complex holograms
        }
        else if (hologram.points.size() < 10000)
        {
            adaptive_factor = 1.1; // Boost quality for simple holograms
        }

        return std::min(1.0, base_success * adaptive_factor);
    }

    double
    Oran6gHolographic::TransmitQuantumCoherent(const HologramFrame &hologram, Ptr<Node> node)
    {
        // Quantum coherent transmission
        double quantum_fidelity = 0.98;
        double coherence_factor = m_quantumCoherence ? 1.2 : 1.0;

        return std::min(1.0, quantum_fidelity * coherence_factor);
    }

    double
    Oran6gHolographic::TransmitPointCloud(const HologramFrame &hologram, Ptr<Node> node)
    {
        // Basic point cloud transmission
        double base_success = 0.85;
        double size_factor = 1.0 - (hologram.points.size() / 1000000.0) * 0.1; // Penalty for large clouds

        return std::max(0.5, base_success * size_factor);
    }

    std::vector<HolographicPoint>
    Oran6gHolographic::ProcessFourierTransform(const LightField &light_field)
    {
        std::vector<HolographicPoint> points;

        // Simulate Fourier transform processing
        uint32_t num_points = light_field.amplitudes.size() / 4; // Reduction factor

        for (uint32_t i = 0; i < num_points; ++i)
        {
            HolographicPoint point;

            // Generate position from frequency domain
            point.position.x = m_uniformRng->GetValue(-light_field.dimensions.x / 2, light_field.dimensions.x / 2);
            point.position.y = m_uniformRng->GetValue(-light_field.dimensions.y / 2, light_field.dimensions.y / 2);
            point.position.z = m_uniformRng->GetValue(-light_field.dimensions.z / 2, light_field.dimensions.z / 2);

            // Extract amplitude and phase
            if (i < light_field.amplitudes.size())
            {
                point.intensity = std::abs(light_field.amplitudes[i]);
                point.phase = std::arg(light_field.amplitudes[i]);
            }
            else
            {
                point.intensity = 0.5;
                point.phase = 0.0;
            }

            // Set color based on wavelength
            point.color.x = (light_field.wavelength - m_minWavelength) / (m_maxWavelength - m_minWavelength);
            point.color.y = point.intensity;
            point.color.z = 0.5;

            // Set normal (simplified)
            point.normal.x = 0.0;
            point.normal.y = 0.0;
            point.normal.z = 1.0;

            points.push_back(point);
        }

        return points;
    }

    std::vector<HolographicPoint>
    Oran6gHolographic::ProcessDirectConversion(const LightField &light_field)
    {
        std::vector<HolographicPoint> points;

        // Direct conversion from light field to points
        for (size_t i = 0; i < light_field.amplitudes.size(); ++i)
        {
            HolographicPoint point;

            // Calculate 3D position from index
            uint32_t x = i % m_volumeVoxels.x;
            uint32_t y = (i / m_volumeVoxels.x) % m_volumeVoxels.y;
            uint32_t z = i / (m_volumeVoxels.x * m_volumeVoxels.y);

            point.position.x = (x / double(m_volumeVoxels.x)) * light_field.dimensions.x;
            point.position.y = (y / double(m_volumeVoxels.y)) * light_field.dimensions.y;
            point.position.z = (z / double(m_volumeVoxels.z)) * light_field.dimensions.z;

            point.intensity = std::abs(light_field.amplitudes[i]);
            point.phase = std::arg(light_field.amplitudes[i]);

            // Only add points with significant intensity
            if (point.intensity > 0.1)
            {
                points.push_back(point);
            }
        }

        return points;
    }

    uint32_t
    Oran6gHolographic::ApplyHolographicCompression(HologramFrame &frame, const std::string &method)
    {
        double compression_ratio = m_compressionRatio;

        if (method == "spatial-wavelet")
        {
            compression_ratio *= 1.2;
        }
        else if (method == "neural-compression")
        {
            compression_ratio *= 1.5;
        }
        else if (method == "quantum-compression")
        {
            compression_ratio *= 2.0;
        }

        // Apply compression (simplified)
        uint32_t original_size = frame.points.size();
        uint32_t compressed_size = static_cast<uint32_t>(original_size / compression_ratio);

        // Keep only the most significant points
        if (compressed_size < frame.points.size())
        {
            std::sort(frame.points.begin(), frame.points.end(),
                      [](const HolographicPoint &a, const HolographicPoint &b)
                      {
                          return a.intensity > b.intensity;
                      });

            frame.points.resize(compressed_size);
        }

        return static_cast<uint32_t>(compression_ratio);
    }

    void
    Oran6gHolographic::GenerateFresnelHologram(const std::vector<HolographicPoint> &point_cloud, LightField &light_field)
    {
        // Generate Fresnel hologram from point cloud
        for (size_t i = 0; i < light_field.amplitudes.size(); ++i)
        {
            std::complex<double> amplitude(0.0, 0.0);

            // Calculate hologram coordinates
            uint32_t x = i % m_volumeVoxels.x;
            uint32_t y = (i / m_volumeVoxels.x) % m_volumeVoxels.y;

            double holo_x = (x / double(m_volumeVoxels.x) - 0.5) * light_field.dimensions.x;
            double holo_y = (y / double(m_volumeVoxels.y) - 0.5) * light_field.dimensions.y;

            // Sum contributions from all points
            for (const auto &point : point_cloud)
            {
                double dx = holo_x - point.position.x;
                double dy = holo_y - point.position.y;
                double dz = point.position.z; // Distance to hologram plane

                double distance = std::sqrt(dx * dx + dy * dy + dz * dz);

                // Fresnel diffraction calculation
                double k = 2.0 * M_PI / (light_field.wavelength * 1e-9); // Wave number
                double phase = k * distance + point.phase;

                std::complex<double> contribution =
                    std::complex<double>(point.intensity * std::cos(phase),
                                         point.intensity * std::sin(phase)) /
                    distance;

                amplitude += contribution;
            }

            light_field.amplitudes[i] = amplitude;
            light_field.phases[i] = std::arg(amplitude);
        }
    }

    void
    Oran6gHolographic::GenerateAmplitudePhaseEncoding(const std::vector<HolographicPoint> &point_cloud, LightField &light_field)
    {
        // Simple amplitude-phase encoding
        for (size_t i = 0; i < light_field.amplitudes.size() && i < point_cloud.size(); ++i)
        {
            const auto &point = point_cloud[i];

            light_field.amplitudes[i] = std::complex<double>(
                point.intensity * std::cos(point.phase),
                point.intensity * std::sin(point.phase));

            light_field.phases[i] = point.phase;
        }

        // Fill remaining with zeros
        for (size_t i = point_cloud.size(); i < light_field.amplitudes.size(); ++i)
        {
            light_field.amplitudes[i] = std::complex<double>(0.0, 0.0);
            light_field.phases[i] = 0.0;
        }
    }

} // namespace ns3
