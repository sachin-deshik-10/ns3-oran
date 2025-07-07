/*
 * Copyright (c) 2025 O-RAN Alliance
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Author: Advanced O-RAN Research Team
 */

#ifndef ORAN_6G_SAGS_NETWORK_H
#define ORAN_6G_SAGS_NETWORK_H

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/vector.h"
#include "ns3/mobility-model.h"
#include "ns3/ptr.h"
#include "ns3/simulation-time.h"
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace ns3 {

/**
 * @brief Constellation types for satellite deployment
 */
enum class ConstellationType {
    LEO,        ///< Low Earth Orbit (160-2000 km)
    MEO,        ///< Medium Earth Orbit (2000-35786 km)
    GEO,        ///< Geostationary Earth Orbit (35786 km)
    MEGA_LEO,   ///< Mega constellation LEO (400-1200 km)
    HYBRID      ///< Multi-orbit hybrid constellation
};

/**
 * @brief High Altitude Platform Station (HAPS) node configuration
 */
struct HapsNode {
    Vector position;                ///< 3D position (lat, lon, altitude)
    double coverage_radius;         ///< Coverage radius in km
    double transmit_power;          ///< Transmit power in dBm
    std::string platform_type;      ///< Platform type (balloon, aircraft, airship)
    bool is_mobile;                 ///< Whether the HAPS is mobile
    double energy_capacity;         ///< Energy capacity in Wh
    double solar_generation;        ///< Solar power generation in W
};

/**
 * @brief Underwater acoustic communication parameters
 */
struct AcousticParameters {
    double frequency;               ///< Carrier frequency in Hz
    double bandwidth;               ///< Available bandwidth in Hz
    double max_range;               ///< Maximum communication range in km
    double sound_speed;             ///< Speed of sound in water (m/s)
    double absorption_coefficient;   ///< Acoustic absorption coefficient
    double noise_level;             ///< Background noise level in dB
    bool enable_adaptive_coding;    ///< Enable adaptive channel coding
};

/**
 * @brief Domain types for inter-domain communication
 */
enum class DomainType {
    TERRESTRIAL,    ///< Terrestrial cellular networks
    SATELLITE,      ///< Satellite communication
    AERIAL,         ///< HAPS and UAV networks
    MARITIME,       ///< Maritime communication
    UNDERWATER,     ///< Underwater acoustic networks
    SPACE           ///< Deep space communication
};

/**
 * @brief Coverage requirements for global orchestration
 */
struct CoverageRequirement {
    std::vector<Vector> service_areas;  ///< Geographic areas requiring coverage
    double min_data_rate;               ///< Minimum data rate requirement
    double max_latency;                 ///< Maximum latency requirement
    double reliability_target;          ///< Target reliability (0-1)
    Time service_duration;              ///< Duration of service requirement
    std::string priority_level;         ///< Priority level (critical, high, normal, low)
};

/**
 * @brief Handover information between domains
 */
struct InterDomainHandover {
    DomainType source_domain;
    DomainType target_domain;
    double handover_latency;
    double success_probability;
    std::vector<double> qos_parameters;
    Time preparation_time;
    bool seamless_handover;
};

/**
 * @brief Orbital mechanics parameters for satellite networks
 */
struct OrbitalParameters {
    double semi_major_axis;         ///< Semi-major axis in km
    double eccentricity;            ///< Orbital eccentricity
    double inclination;             ///< Orbital inclination in degrees
    double raan;                    ///< Right Ascension of Ascending Node
    double arg_of_perigee;         ///< Argument of perigee
    double true_anomaly;           ///< True anomaly at epoch
    Time epoch;                    ///< Epoch time
};

/**
 * @brief Space-Air-Ground-Sea (SAGS) Integrated 6G Network
 * 
 * This class implements a comprehensive multi-domain 6G network architecture
 * that seamlessly integrates terrestrial, aerial, satellite, maritime, and 
 * underwater communication systems for ubiquitous global connectivity.
 * 
 * Key Features:
 * - Multi-orbit satellite constellation management
 * - High Altitude Platform Station (HAPS) deployment
 * - Underwater acoustic communication
 * - Intelligent inter-domain handover
 * - Global coverage orchestration with AI optimization
 * - Space-based edge computing and caching
 * - Autonomous network self-healing and optimization
 */
class Oran6gSagsNetwork : public Object
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
    Oran6gSagsNetwork();

    /**
     * @brief Destructor
     */
    virtual ~Oran6gSagsNetwork();

    // Satellite Constellation Management

    /**
     * @brief Deploy a satellite constellation
     * @param type Constellation type (LEO, MEO, GEO, etc.)
     * @param num_satellites Number of satellites in constellation
     * @param orbital_params Orbital parameters for each satellite
     */
    void DeploySatelliteConstellation(ConstellationType type, 
                                     uint32_t num_satellites,
                                     const std::vector<OrbitalParameters>& orbital_params);

    /**
     * @brief Configure satellite inter-satellite links (ISL)
     * @param max_isl_range Maximum ISL range in km
     * @param isl_data_rate Data rate for ISL in Gbps
     * @param laser_communication Enable laser communication for ISL
     */
    void ConfigureInterSatelliteLinks(double max_isl_range,
                                     double isl_data_rate,
                                     bool laser_communication = true);

    /**
     * @brief Optimize satellite beamforming for ground coverage
     * @param coverage_areas Areas requiring satellite coverage
     * @param beam_optimization_algorithm Algorithm for beam optimization
     */
    void OptimizeSatelliteBeamforming(const std::vector<Vector>& coverage_areas,
                                     const std::string& beam_optimization_algorithm);

    // High Altitude Platform Stations (HAPS)

    /**
     * @brief Deploy HAPS network
     * @param haps_nodes Vector of HAPS node configurations
     */
    void DeployHapsNetwork(const std::vector<HapsNode>& haps_nodes);

    /**
     * @brief Configure HAPS mobility patterns
     * @param node_id HAPS node ID
     * @param mobility_pattern Mobility pattern (stationary, circular, linear)
     * @param mobility_parameters Parameters for mobility pattern
     */
    void ConfigureHapsMobility(uint32_t node_id,
                              const std::string& mobility_pattern,
                              const std::map<std::string, double>& mobility_parameters);

    /**
     * @brief Optimize HAPS energy management
     * @param solar_prediction_model Solar irradiance prediction model
     * @param energy_optimization_strategy Energy optimization strategy
     */
    void OptimizeHapsEnergyManagement(const std::string& solar_prediction_model,
                                     const std::string& energy_optimization_strategy);

    // Underwater Acoustic Communication

    /**
     * @brief Configure underwater acoustic communication
     * @param params Acoustic communication parameters
     */
    void ConfigureUnderwaterComm(const AcousticParameters& params);

    /**
     * @brief Deploy underwater sensor network
     * @param sensor_positions Positions of underwater sensors
     * @param sensor_capabilities Capabilities of each sensor
     */
    void DeployUnderwaterSensorNetwork(const std::vector<Vector>& sensor_positions,
                                      const std::vector<std::string>& sensor_capabilities);

    /**
     * @brief Optimize underwater routing protocols
     * @param routing_algorithm Routing algorithm (AODV-UW, DBR, VBF)
     * @param adaptation_parameters Parameters for protocol adaptation
     */
    void OptimizeUnderwaterRouting(const std::string& routing_algorithm,
                                  const std::map<std::string, double>& adaptation_parameters);

    // Inter-Domain Handover and Integration

    /**
     * @brief Optimize inter-domain handover
     * @param source_domain Source communication domain
     * @param target_domain Target communication domain
     * @param handover_criteria Criteria for handover decision
     */
    void OptimizeInterDomainHandover(DomainType source_domain,
                                    DomainType target_domain,
                                    const std::map<std::string, double>& handover_criteria);

    /**
     * @brief Configure seamless mobility across domains
     * @param mobility_prediction_horizon Time horizon for mobility prediction
     * @param handover_preparation_time Time to prepare for handover
     */
    void ConfigureSeamlessMobility(Time mobility_prediction_horizon,
                                  Time handover_preparation_time);

    /**
     * @brief Enable AI-driven network selection
     * @param ai_model AI model for network selection
     * @param context_awareness_level Level of context awareness (0-1)
     */
    void EnableAiNetworkSelection(const std::string& ai_model,
                                 double context_awareness_level);

    // Global Coverage Orchestration

    /**
     * @brief Orchestrate SAGS resources for coverage requirements
     * @param requirement Coverage requirement specification
     * @return Success rate of coverage orchestration
     */
    double OrchestrateSagsResources(const CoverageRequirement& requirement);

    /**
     * @brief Optimize global resource allocation
     * @param traffic_prediction Traffic prediction for different regions
     * @param optimization_objective Optimization objective (latency, energy, cost)
     */
    void OptimizeGlobalResourceAllocation(const std::map<Vector, double>& traffic_prediction,
                                         const std::string& optimization_objective);

    /**
     * @brief Enable autonomous network healing
     * @param healing_algorithm Self-healing algorithm
     * @param failure_detection_threshold Threshold for failure detection
     */
    void EnableAutonomousNetworkHealing(const std::string& healing_algorithm,
                                       double failure_detection_threshold);

    // Advanced 6G Features

    /**
     * @brief Configure space-based edge computing
     * @param satellite_edge_nodes Satellites with edge computing capabilities
     * @param edge_caching_strategy Strategy for edge content caching
     */
    void ConfigureSpaceBasedEdgeComputing(const std::vector<uint32_t>& satellite_edge_nodes,
                                         const std::string& edge_caching_strategy);

    /**
     * @brief Enable quantum communication across domains
     * @param quantum_key_distribution Enable QKD for secure communication
     * @param quantum_error_correction Enable quantum error correction
     */
    void EnableQuantumCommunication(bool quantum_key_distribution,
                                   bool quantum_error_correction);

    /**
     * @brief Configure holographic communication support
     * @param hologram_compression_ratio Compression ratio for holographic data
     * @param multi_domain_synchronization Enable synchronization across domains
     */
    void ConfigureHolographicCommunication(double hologram_compression_ratio,
                                          bool multi_domain_synchronization);

    // Performance Monitoring and Analytics

    /**
     * @brief Get current network performance metrics
     * @return Map of performance metrics
     */
    std::map<std::string, double> GetNetworkPerformanceMetrics() const;

    /**
     * @brief Get coverage statistics
     * @return Coverage statistics for each domain
     */
    std::map<DomainType, double> GetCoverageStatistics() const;

    /**
     * @brief Get inter-domain handover statistics
     * @return Handover statistics between domains
     */
    std::vector<InterDomainHandover> GetHandoverStatistics() const;

    /**
     * @brief Generate SAGS network report
     * @param report_format Report format (JSON, XML, CSV)
     * @return Generated report as string
     */
    std::string GenerateNetworkReport(const std::string& report_format) const;

    // Machine Learning and AI Integration

    /**
     * @brief Train AI models for network optimization
     * @param training_data Historical network data for training
     * @param model_type Type of AI model (DNN, transformer, GNN)
     */
    void TrainAiOptimizationModels(const std::vector<std::map<std::string, double>>& training_data,
                                  const std::string& model_type);

    /**
     * @brief Enable predictive network analytics
     * @param prediction_horizon Time horizon for predictions
     * @param prediction_accuracy_target Target accuracy for predictions
     */
    void EnablePredictiveAnalytics(Time prediction_horizon,
                                  double prediction_accuracy_target);

    /**
     * @brief Configure federated learning across domains
     * @param fl_aggregation_method Federated learning aggregation method
     * @param privacy_preserving_techniques Privacy preservation techniques
     */
    void ConfigureFederatedLearning(const std::string& fl_aggregation_method,
                                   const std::vector<std::string>& privacy_preserving_techniques);

private:
    // Private member variables
    std::map<ConstellationType, std::vector<uint32_t>> m_satellite_constellations;
    std::vector<HapsNode> m_haps_nodes;
    AcousticParameters m_underwater_params;
    std::map<DomainType, std::vector<uint32_t>> m_domain_nodes;
    std::vector<InterDomainHandover> m_handover_statistics;
    
    // Network state
    std::map<std::string, double> m_performance_metrics;
    std::map<DomainType, double> m_coverage_statistics;
    bool m_ai_optimization_enabled;
    bool m_quantum_communication_enabled;
    bool m_autonomous_healing_enabled;
    
    // Private helper methods
    void InitializeSagsNetwork();
    void UpdateOrbitalPositions(Time current_time);
    void UpdateHapsPositions(Time current_time);
    void CalculateInterDomainConnectivity();
    void OptimizeNetworkTopology();
    void UpdatePerformanceMetrics();
    double CalculateCoverageForArea(const Vector& area_center, double area_radius);
    bool CheckHandoverFeasibility(DomainType source, DomainType target);
};

} // namespace ns3

#endif /* ORAN_6G_SAGS_NETWORK_H */
