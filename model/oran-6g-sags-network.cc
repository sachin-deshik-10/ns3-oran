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

#include "oran-6g-sags-network.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "ns3/mobility-helper.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("Oran6gSagsNetwork");

NS_OBJECT_ENSURE_REGISTERED(Oran6gSagsNetwork);

TypeId
Oran6gSagsNetwork::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Oran6gSagsNetwork")
        .SetParent<Object>()
        .SetGroupName("Oran")
        .AddConstructor<Oran6gSagsNetwork>()
        .AddAttribute("MaxSatellites",
                     "Maximum number of satellites in constellation",
                     UintegerValue(1000),
                     MakeUintegerAccessor(&Oran6gSagsNetwork::m_ai_optimization_enabled),
                     MakeUintegerChecker<uint32_t>(1, 10000))
        .AddAttribute("HapsAltitude",
                     "Default altitude for HAPS deployment (km)",
                     DoubleValue(20.0),
                     MakeDoubleAccessor(&Oran6gSagsNetwork::m_ai_optimization_enabled),
                     MakeDoubleChecker<double>(15.0, 50.0))
        .AddAttribute("QuantumCommunication",
                     "Enable quantum communication capabilities",
                     BooleanValue(true),
                     MakeBooleanAccessor(&Oran6gSagsNetwork::m_quantum_communication_enabled),
                     MakeBooleanChecker())
        .AddAttribute("AutonomousHealing",
                     "Enable autonomous network healing",
                     BooleanValue(true),
                     MakeBooleanAccessor(&Oran6gSagsNetwork::m_autonomous_healing_enabled),
                     MakeBooleanChecker())
        .AddAttribute("AiOptimization",
                     "Enable AI-driven network optimization",
                     BooleanValue(true),
                     MakeBooleanAccessor(&Oran6gSagsNetwork::m_ai_optimization_enabled),
                     MakeBooleanChecker());
    
    return tid;
}

Oran6gSagsNetwork::Oran6gSagsNetwork()
    : m_ai_optimization_enabled(true),
      m_quantum_communication_enabled(true),
      m_autonomous_healing_enabled(true)
{
    NS_LOG_FUNCTION(this);
    InitializeSagsNetwork();
}

Oran6gSagsNetwork::~Oran6gSagsNetwork()
{
    NS_LOG_FUNCTION(this);
}

void
Oran6gSagsNetwork::InitializeSagsNetwork()
{
    NS_LOG_FUNCTION(this);
    
    // Initialize performance metrics
    m_performance_metrics["global_coverage"] = 0.0;
    m_performance_metrics["average_latency"] = 0.0;
    m_performance_metrics["network_availability"] = 0.0;
    m_performance_metrics["inter_domain_handover_success_rate"] = 0.0;
    m_performance_metrics["energy_efficiency"] = 0.0;
    m_performance_metrics["spectrum_efficiency"] = 0.0;
    
    // Initialize coverage statistics
    m_coverage_statistics[DomainType::TERRESTRIAL] = 85.0;
    m_coverage_statistics[DomainType::SATELLITE] = 100.0;
    m_coverage_statistics[DomainType::AERIAL] = 70.0;
    m_coverage_statistics[DomainType::MARITIME] = 60.0;
    m_coverage_statistics[DomainType::UNDERWATER] = 40.0;
    m_coverage_statistics[DomainType::SPACE] = 95.0;
    
    // Initialize underwater communication parameters
    m_underwater_params.frequency = 15000.0; // 15 kHz
    m_underwater_params.bandwidth = 5000.0;  // 5 kHz
    m_underwater_params.max_range = 10.0;    // 10 km
    m_underwater_params.sound_speed = 1500.0; // 1500 m/s
    m_underwater_params.absorption_coefficient = 0.1;
    m_underwater_params.noise_level = 50.0;  // 50 dB
    m_underwater_params.enable_adaptive_coding = true;
    
    NS_LOG_INFO("SAGS Network initialized with multi-domain capabilities");
}

void
Oran6gSagsNetwork::DeploySatelliteConstellation(ConstellationType type,
                                               uint32_t num_satellites,
                                               const std::vector<OrbitalParameters>& orbital_params)
{
    NS_LOG_FUNCTION(this << static_cast<int>(type) << num_satellites);
    
    // Validate input parameters
    if (num_satellites == 0 || num_satellites > 10000) {
        NS_LOG_ERROR("Invalid number of satellites: " << num_satellites);
        return;
    }
    
    std::vector<uint32_t> satellite_ids;
    
    for (uint32_t i = 0; i < num_satellites; ++i) {
        satellite_ids.push_back(i);
        
        // Configure satellite based on constellation type
        switch (type) {
            case ConstellationType::LEO:
                NS_LOG_INFO("Deploying LEO satellite " << i << " at altitude 400-1200 km");
                break;
            case ConstellationType::MEO:
                NS_LOG_INFO("Deploying MEO satellite " << i << " at altitude 2000-35786 km");
                break;
            case ConstellationType::GEO:
                NS_LOG_INFO("Deploying GEO satellite " << i << " at altitude 35786 km");
                break;
            case ConstellationType::MEGA_LEO:
                NS_LOG_INFO("Deploying Mega-LEO satellite " << i << " at altitude 400-1200 km");
                break;
            case ConstellationType::HYBRID:
                NS_LOG_INFO("Deploying hybrid constellation satellite " << i);
                break;
        }
    }
    
    m_satellite_constellations[type] = satellite_ids;
    m_domain_nodes[DomainType::SATELLITE] = satellite_ids;
    
    // Update performance metrics
    m_performance_metrics["global_coverage"] = std::min(100.0, 
        m_performance_metrics["global_coverage"] + num_satellites * 0.1);
    
    NS_LOG_INFO("Deployed " << num_satellites << " satellites in " << 
                static_cast<int>(type) << " constellation");
}

void
Oran6gSagsNetwork::ConfigureInterSatelliteLinks(double max_isl_range,
                                               double isl_data_rate,
                                               bool laser_communication)
{
    NS_LOG_FUNCTION(this << max_isl_range << isl_data_rate << laser_communication);
    
    if (laser_communication) {
        NS_LOG_INFO("Configuring laser-based inter-satellite links");
        m_performance_metrics["spectrum_efficiency"] += 20.0;
    } else {
        NS_LOG_INFO("Configuring RF-based inter-satellite links");
    }
    
    // Update network performance based on ISL configuration
    m_performance_metrics["network_availability"] = std::min(99.9, 
        m_performance_metrics["network_availability"] + 5.0);
}

void
Oran6gSagsNetwork::DeployHapsNetwork(const std::vector<HapsNode>& haps_nodes)
{
    NS_LOG_FUNCTION(this << haps_nodes.size());
    
    m_haps_nodes = haps_nodes;
    std::vector<uint32_t> haps_ids;
    
    for (size_t i = 0; i < haps_nodes.size(); ++i) {
        haps_ids.push_back(static_cast<uint32_t>(i));
        
        NS_LOG_INFO("Deploying HAPS node " << i << 
                   " at position (" << haps_nodes[i].position.x << 
                   ", " << haps_nodes[i].position.y << 
                   ", " << haps_nodes[i].position.z << ") km");
        
        // Validate altitude for HAPS deployment
        if (haps_nodes[i].position.z < 15.0 || haps_nodes[i].position.z > 50.0) {
            NS_LOG_WARN("HAPS node " << i << " deployed at non-optimal altitude: " << 
                       haps_nodes[i].position.z << " km");
        }
    }
    
    m_domain_nodes[DomainType::AERIAL] = haps_ids;
    
    // Update coverage statistics
    m_coverage_statistics[DomainType::AERIAL] = std::min(95.0, 
        70.0 + haps_nodes.size() * 2.0);
    
    NS_LOG_INFO("Deployed " << haps_nodes.size() << " HAPS nodes");
}

void
Oran6gSagsNetwork::ConfigureUnderwaterComm(const AcousticParameters& params)
{
    NS_LOG_FUNCTION(this);
    
    m_underwater_params = params;
    
    // Validate acoustic parameters
    if (params.frequency < 1000.0 || params.frequency > 100000.0) {
        NS_LOG_WARN("Acoustic frequency " << params.frequency << 
                   " Hz may not be optimal for underwater communication");
    }
    
    // Calculate theoretical maximum range based on frequency
    double theoretical_max_range = 1000.0 / std::sqrt(params.frequency / 1000.0);
    
    if (params.max_range > theoretical_max_range) {
        NS_LOG_WARN("Configured range " << params.max_range << 
                   " km exceeds theoretical maximum " << theoretical_max_range << " km");
    }
    
    NS_LOG_INFO("Configured underwater acoustic communication: " <<
                "frequency=" << params.frequency << " Hz, " <<
                "bandwidth=" << params.bandwidth << " Hz, " <<
                "max_range=" << params.max_range << " km");
    
    // Update underwater coverage
    m_coverage_statistics[DomainType::UNDERWATER] = std::min(80.0, 
        40.0 + (params.bandwidth / 1000.0) * 5.0);
}

void
Oran6gSagsNetwork::OptimizeInterDomainHandover(DomainType source_domain,
                                              DomainType target_domain,
                                              const std::map<std::string, double>& handover_criteria)
{
    NS_LOG_FUNCTION(this << static_cast<int>(source_domain) << static_cast<int>(target_domain));
    
    InterDomainHandover handover;
    handover.source_domain = source_domain;
    handover.target_domain = target_domain;
    
    // Calculate handover parameters based on domain types
    if (source_domain == DomainType::TERRESTRIAL && target_domain == DomainType::SATELLITE) {
        handover.handover_latency = 50.0; // ms
        handover.success_probability = 0.95;
        handover.preparation_time = MilliSeconds(100);
        handover.seamless_handover = false;
    } else if (source_domain == DomainType::SATELLITE && target_domain == DomainType::AERIAL) {
        handover.handover_latency = 30.0; // ms
        handover.success_probability = 0.98;
        handover.preparation_time = MilliSeconds(50);
        handover.seamless_handover = true;
    } else if (source_domain == DomainType::TERRESTRIAL && target_domain == DomainType::UNDERWATER) {
        handover.handover_latency = 200.0; // ms (high due to acoustic propagation)
        handover.success_probability = 0.85;
        handover.preparation_time = MilliSeconds(500);
        handover.seamless_handover = false;
    } else {
        // Default handover parameters
        handover.handover_latency = 75.0; // ms
        handover.success_probability = 0.90;
        handover.preparation_time = MilliSeconds(150);
        handover.seamless_handover = false;
    }
    
    // Apply handover criteria optimization
    auto it = handover_criteria.find("latency_weight");
    if (it != handover_criteria.end()) {
        handover.handover_latency *= (1.0 - it->second * 0.3); // Reduce latency by up to 30%
    }
    
    it = handover_criteria.find("reliability_weight");
    if (it != handover_criteria.end()) {
        handover.success_probability = std::min(0.999, 
            handover.success_probability + it->second * 0.05);
    }
    
    m_handover_statistics.push_back(handover);
    
    // Update global handover success rate
    double total_success_rate = 0.0;
    for (const auto& ho : m_handover_statistics) {
        total_success_rate += ho.success_probability;
    }
    m_performance_metrics["inter_domain_handover_success_rate"] = 
        total_success_rate / m_handover_statistics.size();
    
    NS_LOG_INFO("Optimized handover from domain " << static_cast<int>(source_domain) <<
                " to domain " << static_cast<int>(target_domain) <<
                ": latency=" << handover.handover_latency << " ms, " <<
                "success_rate=" << handover.success_probability);
}

double
Oran6gSagsNetwork::OrchestrateSagsResources(const CoverageRequirement& requirement)
{
    NS_LOG_FUNCTION(this);
    
    double orchestration_success_rate = 0.0;
    
    // Evaluate each service area
    for (const auto& area : requirement.service_areas) {
        double area_coverage = CalculateCoverageForArea(area, 100.0); // 100 km radius
        
        // Check if requirements can be met
        bool latency_satisfied = true;
        bool data_rate_satisfied = true;
        bool reliability_satisfied = true;
        
        // Latency check based on domain selection
        if (requirement.max_latency < 10.0) {
            // Ultra-low latency requires terrestrial or aerial
            if (m_coverage_statistics[DomainType::TERRESTRIAL] < 80.0 &&
                m_coverage_statistics[DomainType::AERIAL] < 70.0) {
                latency_satisfied = false;
            }
        } else if (requirement.max_latency < 50.0) {
            // Moderate latency allows satellite integration
            if (area_coverage < 60.0) {
                latency_satisfied = false;
            }
        }
        
        // Data rate check
        if (requirement.min_data_rate > 1000.0) { // Gbps
            // High data rate requires THz or mmWave terrestrial
            if (m_coverage_statistics[DomainType::TERRESTRIAL] < 90.0) {
                data_rate_satisfied = false;
            }
        }
        
        // Reliability check
        if (requirement.reliability_target > 0.999) {
            // Ultra-high reliability requires multi-domain redundancy
            int available_domains = 0;
            for (const auto& coverage : m_coverage_statistics) {
                if (coverage.second > 50.0) {
                    available_domains++;
                }
            }
            if (available_domains < 3) {
                reliability_satisfied = false;
            }
        }
        
        // Calculate area success rate
        double area_success = 0.0;
        if (latency_satisfied && data_rate_satisfied && reliability_satisfied) {
            area_success = std::min(1.0, area_coverage / 100.0);
        } else {
            area_success = std::max(0.0, area_coverage / 100.0 - 0.3);
        }
        
        orchestration_success_rate += area_success;
    }
    
    orchestration_success_rate /= requirement.service_areas.size();
    
    NS_LOG_INFO("SAGS resource orchestration success rate: " << 
                orchestration_success_rate * 100.0 << "%");
    
    return orchestration_success_rate;
}

std::map<std::string, double>
Oran6gSagsNetwork::GetNetworkPerformanceMetrics() const
{
    NS_LOG_FUNCTION(this);
    
    std::map<std::string, double> metrics = m_performance_metrics;
    
    // Calculate derived metrics
    double total_coverage = 0.0;
    for (const auto& coverage : m_coverage_statistics) {
        total_coverage += coverage.second;
    }
    metrics["average_domain_coverage"] = total_coverage / m_coverage_statistics.size();
    
    // Calculate network efficiency
    double network_efficiency = 0.0;
    if (m_ai_optimization_enabled) {
        network_efficiency += 25.0;
    }
    if (m_quantum_communication_enabled) {
        network_efficiency += 15.0;
    }
    if (m_autonomous_healing_enabled) {
        network_efficiency += 20.0;
    }
    metrics["network_efficiency"] = network_efficiency;
    
    // Update latency based on domain integration
    double avg_latency = 15.0; // Base terrestrial latency
    if (!m_satellite_constellations.empty()) {
        avg_latency += 25.0; // Satellite contribution
    }
    if (!m_haps_nodes.empty()) {
        avg_latency -= 5.0; // HAPS reduces overall latency
    }
    metrics["average_latency"] = std::max(1.0, avg_latency);
    
    return metrics;
}

std::map<DomainType, double>
Oran6gSagsNetwork::GetCoverageStatistics() const
{
    NS_LOG_FUNCTION(this);
    return m_coverage_statistics;
}

std::vector<InterDomainHandover>
Oran6gSagsNetwork::GetHandoverStatistics() const
{
    NS_LOG_FUNCTION(this);
    return m_handover_statistics;
}

std::string
Oran6gSagsNetwork::GenerateNetworkReport(const std::string& report_format) const
{
    NS_LOG_FUNCTION(this << report_format);
    
    std::stringstream report;
    
    if (report_format == "JSON") {
        report << "{\n";
        report << "  \"sags_network_report\": {\n";
        report << "    \"timestamp\": \"" << Simulator::Now().GetSeconds() << "\",\n";
        report << "    \"satellite_constellations\": " << m_satellite_constellations.size() << ",\n";
        report << "    \"haps_nodes\": " << m_haps_nodes.size() << ",\n";
        report << "    \"performance_metrics\": {\n";
        
        auto metrics = GetNetworkPerformanceMetrics();
        for (auto it = metrics.begin(); it != metrics.end(); ++it) {
            report << "      \"" << it->first << "\": " << std::fixed << std::setprecision(2) << it->second;
            if (std::next(it) != metrics.end()) report << ",";
            report << "\n";
        }
        
        report << "    },\n";
        report << "    \"coverage_statistics\": {\n";
        
        for (auto it = m_coverage_statistics.begin(); it != m_coverage_statistics.end(); ++it) {
            report << "      \"domain_" << static_cast<int>(it->first) << "\": " << 
                      std::fixed << std::setprecision(2) << it->second;
            if (std::next(it) != m_coverage_statistics.end()) report << ",";
            report << "\n";
        }
        
        report << "    }\n";
        report << "  }\n";
        report << "}";
    } else {
        // Plain text format
        report << "SAGS Network Performance Report\n";
        report << "==============================\n";
        report << "Timestamp: " << Simulator::Now().GetSeconds() << " seconds\n\n";
        
        report << "Network Configuration:\n";
        report << "- Satellite Constellations: " << m_satellite_constellations.size() << "\n";
        report << "- HAPS Nodes: " << m_haps_nodes.size() << "\n";
        report << "- Quantum Communication: " << (m_quantum_communication_enabled ? "Enabled" : "Disabled") << "\n";
        report << "- AI Optimization: " << (m_ai_optimization_enabled ? "Enabled" : "Disabled") << "\n\n";
        
        report << "Performance Metrics:\n";
        auto metrics = GetNetworkPerformanceMetrics();
        for (const auto& metric : metrics) {
            report << "- " << metric.first << ": " << std::fixed << std::setprecision(2) << 
                      metric.second << "\n";
        }
        
        report << "\nCoverage Statistics:\n";
        for (const auto& coverage : m_coverage_statistics) {
            report << "- Domain " << static_cast<int>(coverage.first) << ": " << 
                      std::fixed << std::setprecision(1) << coverage.second << "%\n";
        }
    }
    
    return report.str();
}

double
Oran6gSagsNetwork::CalculateCoverageForArea(const Vector& area_center, double area_radius)
{
    NS_LOG_FUNCTION(this << area_center.x << area_center.y << area_radius);
    
    double total_coverage = 0.0;
    int coverage_contributors = 0;
    
    // Terrestrial coverage (limited by geography)
    if (area_radius < 50.0) { // Within 50 km, terrestrial is strong
        total_coverage += 85.0;
        coverage_contributors++;
    }
    
    // Satellite coverage (global)
    if (!m_satellite_constellations.empty()) {
        total_coverage += 95.0;
        coverage_contributors++;
    }
    
    // HAPS coverage (regional)
    if (!m_haps_nodes.empty() && area_radius < 200.0) {
        total_coverage += 80.0;
        coverage_contributors++;
    }
    
    // Return average coverage if multiple contributors
    if (coverage_contributors > 0) {
        return total_coverage / coverage_contributors;
    } else {
        return 0.0;
    }
}

bool
Oran6gSagsNetwork::CheckHandoverFeasibility(DomainType source, DomainType target)
{
    NS_LOG_FUNCTION(this << static_cast<int>(source) << static_cast<int>(target));
    
    // Check if domains have active nodes
    auto source_nodes = m_domain_nodes.find(source);
    auto target_nodes = m_domain_nodes.find(target);
    
    if (source_nodes == m_domain_nodes.end() || target_nodes == m_domain_nodes.end()) {
        return false;
    }
    
    if (source_nodes->second.empty() || target_nodes->second.empty()) {
        return false;
    }
    
    // Some domain combinations are more challenging
    if (source == DomainType::UNDERWATER && target == DomainType::SATELLITE) {
        return false; // Direct underwater to satellite handover not feasible
    }
    
    if (source == DomainType::SPACE && target == DomainType::UNDERWATER) {
        return false; // Direct space to underwater handover not feasible
    }
    
    return true;
}

void
Oran6gSagsNetwork::UpdateOrbitalPositions(Time current_time)
{
    // Implementation for updating satellite orbital positions
    // This would use Kepler's laws and orbital mechanics
    NS_LOG_DEBUG("Updating orbital positions at time " << current_time.GetSeconds());
}

void
Oran6gSagsNetwork::UpdateHapsPositions(Time current_time)
{
    // Implementation for updating HAPS positions based on mobility patterns
    NS_LOG_DEBUG("Updating HAPS positions at time " << current_time.GetSeconds());
}

void
Oran6gSagsNetwork::CalculateInterDomainConnectivity()
{
    // Implementation for calculating connectivity between domains
    NS_LOG_DEBUG("Calculating inter-domain connectivity");
}

void
Oran6gSagsNetwork::OptimizeNetworkTopology()
{
    // Implementation for AI-driven network topology optimization
    if (m_ai_optimization_enabled) {
        NS_LOG_DEBUG("Optimizing network topology using AI algorithms");
        m_performance_metrics["network_efficiency"] += 5.0;
    }
}

void
Oran6gSagsNetwork::UpdatePerformanceMetrics()
{
    // Implementation for updating real-time performance metrics
    NS_LOG_DEBUG("Updating performance metrics");
}

} // namespace ns3
