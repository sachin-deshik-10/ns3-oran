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

#ifndef ORAN_6G_HOLOGRAPHIC_H
#define ORAN_6G_HOLOGRAPHIC_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/vector.h"
#include "ns3/traced-callback.h"
#include "ns3/random-variable-stream.h"
#include <vector>
#include <map>
#include <complex>

namespace ns3
{

    /**
     * \ingroup oran
     * \brief Holographic 6G Communication Module
     *
     * This class implements holographic communication capabilities including:
     * - 3D Point Cloud Transmission
     * - Holographic Data Compression
     * - Spatial Light Field Communication
     * - Real-time Hologram Rendering
     * - Multi-dimensional Data Encoding
     */
    class Oran6gHolographic : public Object
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
        Oran6gHolographic();

        /**
         * Destructor
         */
        virtual ~Oran6gHolographic();

        // Holographic Data Structures
        struct HolographicPoint
        {
            Vector3D position;
            Vector3D color; // RGB values
            double intensity;
            double phase;
            Vector3D normal; // Surface normal
        };

        struct LightField
        {
            uint32_t fieldId;
            std::vector<std::complex<double>> amplitudes;
            std::vector<double> phases;
            Vector3D dimensions;
            double wavelength;
            double coherenceLength;
        };

        struct HologramFrame
        {
            uint32_t frameId;
            std::vector<HolographicPoint> points;
            LightField lightField;
            Time timestamp;
            double quality;
            uint32_t compressionRatio;
        };

        struct SpatialLayer
        {
            uint32_t layerId;
            Vector3D position;
            Vector3D size;
            std::vector<HolographicPoint> layerData;
            double resolution;
            double detailLevel;
        };

        // Holographic Transmission
        /**
         * \brief Initialize holographic communication system
         * \param resolution Holographic resolution (points per cubic meter)
         * \param wavelength Operating wavelength in nanometers
         * \param compressionLevel Compression level (1-10)
         * \return Initialization success
         */
        bool InitializeHolographicSystem(double resolution, double wavelength, uint32_t compressionLevel);

        /**
         * \brief Create 3D point cloud hologram
         * \param points Vector of 3D points with properties
         * \param boundingBox Bounding box for the hologram
         * \return Generated hologram frame
         */
        HologramFrame CreatePointCloudHologram(const std::vector<HolographicPoint> &points,
                                               const std::pair<Vector3D, Vector3D> &boundingBox);

        /**
         * \brief Encode holographic data for transmission
         * \param hologram Hologram frame to encode
         * \param targetBitrate Target transmission bitrate
         * \return Encoded data size in bytes
         */
        uint32_t EncodeHolographicData(const HologramFrame &hologram, double targetBitrate);

        /**
         * \brief Decode holographic data after transmission
         * \param encodedData Encoded holographic data
         * \param frameSize Expected frame size
         * \return Decoded hologram frame
         */
        HologramFrame DecodeHolographicData(const std::vector<uint8_t> &encodedData, uint32_t frameSize);

        // Light Field Processing
        /**
         * \brief Generate light field from 3D scene
         * \param scenePoints Points representing the 3D scene
         * \param viewingAngles Supported viewing angles
         * \return Generated light field
         */
        LightField GenerateLightField(const std::vector<HolographicPoint> &scenePoints,
                                      const std::vector<Vector3D> &viewingAngles);

        /**
         * \brief Compress light field data
         * \param lightField Original light field
         * \param compressionRatio Target compression ratio
         * \return Compressed light field
         */
        LightField CompressLightField(const LightField &lightField, uint32_t compressionRatio);

        /**
         * \brief Interpolate light field for new viewing angle
         * \param lightField Source light field
         * \param newAngle Desired viewing angle
         * \return Interpolated light field view
         */
        LightField InterpolateLightField(const LightField &lightField, const Vector3D &newAngle);

        // Spatial Layered Encoding
        /**
         * \brief Create spatial layers for multi-resolution transmission
         * \param hologram Source hologram
         * \param numLayers Number of spatial layers
         * \return Vector of spatial layers
         */
        std::vector<SpatialLayer> CreateSpatialLayers(const HologramFrame &hologram, uint32_t numLayers);

        /**
         * \brief Adaptive spatial layer selection
         * \param layers Available spatial layers
         * \param viewerPosition Current viewer position
         * \param availableBandwidth Available transmission bandwidth
         * \return Selected layers for transmission
         */
        std::vector<uint32_t> SelectSpatialLayers(const std::vector<SpatialLayer> &layers,
                                                  const Vector3D &viewerPosition,
                                                  double availableBandwidth);

        /**
         * \brief Progressive holographic transmission
         * \param hologram Source hologram
         * \param transmissionStages Number of progressive stages
         * \return Staged transmission data
         */
        std::vector<std::vector<uint8_t>> ProgressiveTransmission(const HologramFrame &hologram,
                                                                  uint32_t transmissionStages);

        // Real-time Processing
        /**
         * \brief Real-time hologram rendering
         * \param hologram Hologram to render
         * \param viewerPosition Viewer's position
         * \param viewingDirection Viewing direction
         * \return Rendered view data
         */
        std::vector<uint8_t> RenderHologramView(const HologramFrame &hologram,
                                                const Vector3D &viewerPosition,
                                                const Vector3D &viewingDirection);

        /**
         * \brief Adaptive quality control
         * \param currentQuality Current rendering quality
         * \param networkConditions Network performance metrics
         * \param userPreferences User quality preferences
         * \return Adjusted quality parameters
         */
        std::map<std::string, double> AdaptiveQualityControl(double currentQuality,
                                                             const std::map<std::string, double> &networkConditions,
                                                             const std::map<std::string, double> &userPreferences);

        /**
         * \brief Predictive frame generation
         * \param previousFrames Vector of previous hologram frames
         * \param motionVectors Estimated motion vectors
         * \return Predicted next frame
         */
        HologramFrame PredictiveFrameGeneration(const std::vector<HologramFrame> &previousFrames,
                                                const std::vector<Vector3D> &motionVectors);

        // Advanced Compression
        /**
         * \brief Wavelet-based holographic compression
         * \param hologram Source hologram
         * \param waveletType Wavelet type for compression
         * \return Compression ratio achieved
         */
        double WaveletHolographicCompression(const HologramFrame &hologram, const std::string &waveletType);

        /**
         * \brief Neural network compression
         * \param hologram Source hologram
         * \param networkType Neural network architecture
         * \return Compressed representation
         */
        std::vector<double> NeuralHolographicCompression(const HologramFrame &hologram,
                                                         const std::string &networkType);

        /**
         * \brief Fractal compression for self-similar patterns
         * \param hologram Source hologram
         * \param fractalDepth Maximum fractal depth
         * \return Fractal compression data
         */
        std::vector<double> FractalHolographicCompression(const HologramFrame &hologram, uint32_t fractalDepth);

        // Transmission Optimization
        /**
         * \brief Calculate optimal transmission parameters
         * \param hologram Hologram to transmit
         * \param channelConditions Channel characteristics
         * \return Optimal transmission parameters
         */
        std::map<std::string, double> CalculateTransmissionParameters(const HologramFrame &hologram,
                                                                      const std::map<std::string, double> &channelConditions);

        /**
         * \brief Error resilient encoding
         * \param hologram Source hologram
         * \param errorRate Expected channel error rate
         * \return Error-resilient encoded data
         */
        std::vector<uint8_t> ErrorResilientEncoding(const HologramFrame &hologram, double errorRate);

        /**
         * \brief Multiple description coding
         * \param hologram Source hologram
         * \param numDescriptions Number of descriptions
         * \return Multiple coded descriptions
         */
        std::vector<std::vector<uint8_t>> MultipleDescriptionCoding(const HologramFrame &hologram,
                                                                    uint32_t numDescriptions);

        // Performance Metrics
        /**
         * \brief Get holographic communication metrics
         * \return Performance metrics map
         */
        std::map<std::string, double> GetHolographicMetrics() const;

        /**
         * \brief Calculate holographic quality metrics
         * \param originalHologram Original hologram
         * \param processedHologram Processed hologram
         * \return Quality metrics (PSNR, SSIM, etc.)
         */
        std::map<std::string, double> CalculateQualityMetrics(const HologramFrame &originalHologram,
                                                              const HologramFrame &processedHologram);

        /**
         * \brief Measure transmission efficiency
         * \param originalSize Original data size
         * \param transmittedSize Transmitted data size
         * \param transmissionTime Transmission time
         * \return Efficiency metrics
         */
        std::map<std::string, double> MeasureTransmissionEfficiency(uint32_t originalSize,
                                                                    uint32_t transmittedSize,
                                                                    Time transmissionTime);

        // Configuration
        /**
         * \brief Set holographic parameters
         * \param parameters Configuration parameters
         */
        void SetHolographicParameters(const std::map<std::string, double> &parameters);

        /**
         * \brief Enable adaptive transmission
         * \param enable Enable/disable adaptive transmission
         * \param adaptationRate Adaptation update rate
         */
        void EnableAdaptiveTransmission(bool enable, double adaptationRate = 10.0);

        /**
         * \brief Set viewer tracking
         * \param enable Enable viewer position tracking
         * \param trackingAccuracy Tracking accuracy in meters
         */
        void SetViewerTracking(bool enable, double trackingAccuracy = 0.01);

    private:
        // Private member variables
        bool m_holographicSystemInitialized; //< System initialization status
        bool m_adaptiveTransmissionEnabled;  //< Adaptive transmission status
        bool m_viewerTrackingEnabled;        //< Viewer tracking status

        double m_holographicResolution; //< Points per cubic meter
        double m_operatingWavelength;   //< Wavelength in nm
        double m_adaptationRate;        //< Adaptation update rate
        double m_trackingAccuracy;      //< Tracking accuracy
        double m_currentQuality;        //< Current rendering quality

        uint32_t m_compressionLevel; //< Compression level
        uint32_t m_nextFrameId;      //< Next frame ID
        uint32_t m_nextFieldId;      //< Next light field ID
        uint32_t m_nextLayerId;      //< Next spatial layer ID

        Vector3D m_currentViewerPosition;   //< Current viewer position
        Vector3D m_currentViewingDirection; //< Current viewing direction

        std::vector<HologramFrame> m_frameBuffer;           //< Frame buffer
        std::vector<LightField> m_lightFieldBuffer;         //< Light field buffer
        std::vector<SpatialLayer> m_spatialLayers;          //< Spatial layers
        std::map<std::string, double> m_holographicMetrics; //< Performance metrics

        Ptr<UniformRandomVariable> m_uniformRv; //< Random variable
        Ptr<NormalRandomVariable> m_normalRv;   //< Normal random variable

        // Traced callbacks
        TracedCallback<double> m_qualityTrace;        //< Quality measurement trace
        TracedCallback<double> m_compressionTrace;    //< Compression ratio trace
        TracedCallback<uint32_t> m_transmissionTrace; //< Transmission size trace

        // Private helper methods
        void InitializeHolographicBuffers();
        void UpdateHolographicMetrics();
        double CalculateHolographicComplexity(const HologramFrame &hologram);
        Vector3D CalculateOptimalViewingAngle(const HologramFrame &hologram, const Vector3D &viewerPos);
        std::vector<HolographicPoint> FilterPointsByDistance(const std::vector<HolographicPoint> &points,
                                                             const Vector3D &viewerPos, double maxDistance);
        double CalculateCompressionEfficiency(uint32_t originalSize, uint32_t compressedSize);
        void ApplyTemporalFiltering(std::vector<HologramFrame> &frames);
        LightField InterpolateTemporalLightField(const LightField &previous, const LightField &current, double alpha);
    };

} // namespace ns3

#endif /* ORAN_6G_HOLOGRAPHIC_H */
