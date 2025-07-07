/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 */

#include "oran-6g-terahertz.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include <cmath>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("Oran6gTerahertz");

NS_OBJECT_ENSURE_REGISTERED(Oran6gTerahertz);

TypeId
Oran6gTerahertz::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::Oran6gTerahertz")
                            .SetParent<Object>()
                            .SetGroupName("Oran")
                            .AddConstructor<Oran6gTerahertz>()
                            .AddAttribute("CenterFrequency",
                                        "Center frequency in THz",
                                        DoubleValue(0.1), // 100 GHz
                                        MakeDoubleAccessor(&Oran6gTerahertz::m_centerFrequency),
                                        MakeDoubleChecker<double>(0.1, 3.0))
                            .AddAttribute("Bandwidth",
                                        "Channel bandwidth in GHz",
                                        DoubleValue(10.0),
                                        MakeDoubleAccessor(&Oran6gTerahertz::m_bandwidth),
                                        MakeDoubleChecker<double>(1.0, 100.0))
                            .AddAttribute("AntennaElements",
                                        "Number of antenna elements for ultra-massive MIMO",
                                        UintegerValue(1024),
                                        MakeUintegerAccessor(&Oran6gTerahertz::m_antennaElements),
                                        MakeUintegerChecker<uint32_t>(100, 10000))
                            .AddAttribute("EnableIRS",
                                        "Enable Intelligent Reflecting Surfaces",
                                        BooleanValue(true),
                                        MakeBooleanAccessor(&Oran6gTerahertz::m_enableIRS),
                                        MakeBooleanChecker())
                            .AddTraceSource("ThroughputTrace",
                                          "THz channel throughput trace",
                                          MakeTraceSourceAccessor(&Oran6gTerahertz::m_throughputTrace),
                                          "ns3::TracedValueCallback::Double");
    return tid;
}

Oran6gTerahertz::Oran6gTerahertz()
    : m_centerFrequency(0.1), // 100 GHz
      m_bandwidth(10.0),      // 10 GHz
      m_antennaElements(1024),
      m_enableIRS(true),
      m_humidity(0.6),        // 60% relative humidity
      m_temperature(293.15),  // 20°C in Kelvin
      m_isInitialized(false),
      m_currentThroughput(0.0)
{
    NS_LOG_FUNCTION(this);
    InitializeTerahertzChannel();
}

Oran6gTerahertz::~Oran6gTerahertz()
{
    NS_LOG_FUNCTION(this);
}

void
Oran6gTerahertz::InitializeTerahertzChannel()
{
    NS_LOG_FUNCTION(this);
    
    // Initialize THz band characteristics
    m_thzBands.clear();
    
    // Define standard THz bands for 6G
    ThzBandInfo band1 = {0.1, 0.3, "Sub-THz (100-300 GHz)"};  // Good for outdoor
    ThzBandInfo band2 = {0.3, 1.0, "Mid-THz (300 GHz-1 THz)"};  // Balanced range/capacity
    ThzBandInfo band3 = {1.0, 3.0, "High-THz (1-3 THz)"};       // Extreme capacity, short range
    
    m_thzBands.push_back(band1);
    m_thzBands.push_back(band2);
    m_thzBands.push_back(band3);
    
    // Configure default atmospheric absorption coefficients
    ConfigureAtmosphericModel();
    
    m_isInitialized = true;
    
    NS_LOG_INFO("THz channel initialized with " << m_thzBands.size() << " bands");
}

void
Oran6gTerahertz::ConfigureTerahertzBand(double centerFreqTHz, double bandwidthGHz)
{
    NS_LOG_FUNCTION(this << centerFreqTHz << bandwidthGHz);
    
    NS_ASSERT_MSG(centerFreqTHz >= 0.1 && centerFreqTHz <= 3.0, 
                  "Center frequency must be between 0.1 and 3.0 THz");
    NS_ASSERT_MSG(bandwidthGHz >= 1.0 && bandwidthGHz <= 100.0,
                  "Bandwidth must be between 1.0 and 100.0 GHz");
    
    m_centerFrequency = centerFreqTHz;
    m_bandwidth = bandwidthGHz;
    
    // Calculate theoretical maximum throughput using Shannon-Hartley theorem
    // Enhanced for THz with advanced modulation schemes
    double snrDb = CalculateSnr(100.0); // Assume 100m reference distance
    double snrLinear = std::pow(10.0, snrDb / 10.0);
    
    // THz can support extremely high-order modulation (up to 4096-QAM)
    double spectralEfficiency = std::log2(1.0 + snrLinear);
    spectralEfficiency = std::min(spectralEfficiency, 12.0); // Practical limit for THz
    
    m_maxThroughput = (m_bandwidth * 1e9) * spectralEfficiency; // bits/second
    
    NS_LOG_INFO("THz band configured: " << centerFreqTHz << " THz, " 
                << bandwidthGHz << " GHz BW, Max throughput: " 
                << m_maxThroughput / 1e9 << " Gbps");
}

double
Oran6gTerahertz::CalculatePathLoss(double distance, double frequency)
{
    NS_LOG_FUNCTION(this << distance << frequency);
    
    // Free space path loss in dB
    double fspl = 20.0 * std::log10(distance) + 20.0 * std::log10(frequency * 1e12) 
                  + 20.0 * std::log10(4.0 * M_PI / 3e8);
    
    // Add atmospheric absorption
    double atmosphericLoss = CalculateAtmosphericAbsorption(distance, frequency);
    
    // Add molecular absorption (significant at THz frequencies)
    double molecularLoss = CalculateMolecularAbsorption(distance, frequency);
    
    // Total path loss
    double totalLoss = fspl + atmosphericLoss + molecularLoss;
    
    NS_LOG_DEBUG("Path loss calculation: FSPL=" << fspl << "dB, Atmospheric=" 
                 << atmosphericLoss << "dB, Molecular=" << molecularLoss 
                 << "dB, Total=" << totalLoss << "dB");
    
    return totalLoss;
}

double
Oran6gTerahertz::CalculateAtmosphericAbsorption(double distance, double frequency)
{
    NS_LOG_FUNCTION(this << distance << frequency);
    
    // ITU-R P.676 model for atmospheric absorption
    // Simplified model for water vapor and oxygen absorption
    
    double waterVaporDensity = CalculateWaterVaporDensity();
    double oxygenAbsorption = CalculateOxygenAbsorption(frequency);
    double waterVaporAbsorption = CalculateWaterVaporAbsorption(frequency, waterVaporDensity);
    
    // Total atmospheric absorption coefficient (dB/km)
    double absorptionCoeff = oxygenAbsorption + waterVaporAbsorption;
    
    // Total absorption over distance
    double totalAbsorption = absorptionCoeff * (distance / 1000.0); // Convert m to km
    
    return totalAbsorption;
}

double
Oran6gTerahertz::CalculateMolecularAbsorption(double distance, double frequency)
{
    NS_LOG_FUNCTION(this << distance << frequency);
    
    // Molecular absorption becomes significant at THz frequencies
    // This includes rotational and vibrational molecular transitions
    
    double baseAbsorption = 0.1; // dB/km at reference frequency
    double frequencyFactor = std::pow(frequency, 1.5); // Frequency dependence
    
    // Molecular absorption peaks at specific frequencies
    std::vector<double> absorptionPeaks = {0.557, 0.752, 1.097}; // THz
    double peakEffect = 1.0;
    
    for (double peak : absorptionPeaks) {
        if (std::abs(frequency - peak) < 0.05) {
            peakEffect += 2.0 * std::exp(-std::pow((frequency - peak) / 0.02, 2));
        }
    }
    
    double molecularAbsorption = baseAbsorption * frequencyFactor * peakEffect * (distance / 1000.0);
    
    return molecularAbsorption;
}

void
Oran6gTerahertz::ConfigureUltraMassiveMimo(uint32_t antennaElements, 
                                          Oran6gTerahertz::BeamformingStrategy strategy)
{
    NS_LOG_FUNCTION(this << antennaElements << strategy);
    
    m_antennaElements = antennaElements;
    m_beamformingStrategy = strategy;
    
    // Calculate beamforming gain
    double beamformingGain = 10.0 * std::log10(antennaElements);
    
    // Add advanced beamforming techniques for THz
    switch (strategy) {
        case ANALOG_BEAMFORMING:
            m_beamformingGain = beamformingGain * 0.7; // Lower efficiency
            break;
        case DIGITAL_BEAMFORMING:
            m_beamformingGain = beamformingGain * 0.9; // High efficiency, high power
            break;
        case HYBRID_BEAMFORMING:
            m_beamformingGain = beamformingGain * 0.8; // Balanced
            break;
        case AI_ENHANCED_BEAMFORMING:
            m_beamformingGain = beamformingGain * 0.95; // Optimal with ML
            break;
    }
    
    NS_LOG_INFO("Ultra-massive MIMO configured: " << antennaElements 
                << " elements, Gain: " << m_beamformingGain << " dB");
}

void
Oran6gTerahertz::EnableIntelligentReflectingSurfaces(uint32_t irsElements)
{
    NS_LOG_FUNCTION(this << irsElements);
    
    m_irsElements = irsElements;
    m_enableIRS = true;
    
    // IRS gain calculation (simplified model)
    m_irsGain = 10.0 * std::log10(irsElements) + 5.0; // Additional 5dB for smart reflection
    
    NS_LOG_INFO("IRS enabled with " << irsElements << " elements, Additional gain: " 
                << m_irsGain << " dB");
}

double
Oran6gTerahertz::CalculateSnr(double distance)
{
    NS_LOG_FUNCTION(this << distance);
    
    // Transmit power (dBm)
    double txPower = 30.0; // 1W
    
    // Path loss
    double pathLoss = CalculatePathLoss(distance, m_centerFrequency);
    
    // Antenna gains
    double txGain = 20.0; // High-gain antenna
    double rxGain = 20.0;
    
    // Beamforming gain
    double bfGain = m_beamformingGain;
    
    // IRS gain (if enabled)
    double irsGain = m_enableIRS ? m_irsGain : 0.0;
    
    // Noise power calculation for THz
    double boltzmannConstant = 1.38e-23; // J/K
    double noiseFigure = 8.0; // dB (higher for THz receivers)
    double thermalNoise = 10.0 * std::log10(boltzmannConstant * m_temperature * m_bandwidth * 1e9 * 1000); // dBm
    double noisePower = thermalNoise + noiseFigure;
    
    // Received signal power
    double rxPower = txPower + txGain + rxGain + bfGain + irsGain - pathLoss;
    
    // SNR calculation
    double snr = rxPower - noisePower;
    
    NS_LOG_DEBUG("SNR calculation: RxPower=" << rxPower << "dBm, NoisePower=" 
                 << noisePower << "dBm, SNR=" << snr << "dB");
    
    return snr;
}

double
Oran6gTerahertz::CalculateThroughput(double distance)
{
    NS_LOG_FUNCTION(this << distance);
    
    double snr = CalculateSnr(distance);
    double snrLinear = std::pow(10.0, snr / 10.0);
    
    // Shannon capacity with THz enhancements
    double spectralEfficiency = std::log2(1.0 + snrLinear);
    
    // Apply practical limitations for THz
    spectralEfficiency = std::min(spectralEfficiency, 12.0); // Max for advanced modulation
    
    // Account for atmospheric effects reducing effective bandwidth
    double effectiveBandwidth = m_bandwidth * 1e9;
    if (m_centerFrequency > 1.0) {
        effectiveBandwidth *= 0.8; // Reduced effective BW at very high frequencies
    }
    
    double throughput = effectiveBandwidth * spectralEfficiency;
    
    // Update trace
    m_currentThroughput = throughput;
    m_throughputTrace(throughput);
    
    NS_LOG_INFO("THz throughput at " << distance << "m: " 
                << throughput / 1e9 << " Gbps (SE: " << spectralEfficiency << ")");
    
    return throughput;
}

void
Oran6gTerahertz::ConfigureAtmosphericModel()
{
    NS_LOG_FUNCTION(this);
    
    // Initialize atmospheric parameters
    m_humidity = 0.6;        // 60% relative humidity
    m_temperature = 293.15;  // 20°C in Kelvin
    m_pressure = 101325.0;   // 1 atm in Pa
}

double
Oran6gTerahertz::CalculateWaterVaporDensity()
{
    // Calculate water vapor density based on temperature and humidity
    double saturationPressure = 611.2 * std::exp(17.67 * (m_temperature - 273.15) / 
                                                 (m_temperature - 273.15 + 243.5));
    double vaporPressure = m_humidity * saturationPressure;
    double waterVaporDensity = (vaporPressure * 0.018) / (8.314 * m_temperature); // kg/m³
    
    return waterVaporDensity * 1000.0; // Convert to g/m³
}

double
Oran6gTerahertz::CalculateOxygenAbsorption(double frequency)
{
    // ITU-R P.676 oxygen absorption model (simplified)
    double baseAbsorption = 0.05; // dB/km
    double frequencyTerm = std::pow(frequency, 2) / (std::pow(frequency, 2) + 36.0);
    
    return baseAbsorption * frequencyTerm;
}

double
Oran6gTerahertz::CalculateWaterVaporAbsorption(double frequency, double waterVaporDensity)
{
    // ITU-R P.676 water vapor absorption model (simplified)
    double baseAbsorption = 0.1; // dB/km per g/m³
    double frequencyTerm = std::pow(frequency, 2) / (std::pow(frequency, 2) + 25.0);
    
    return baseAbsorption * waterVaporDensity * frequencyTerm;
}

Ptr<Oran6gTerahertz>
Oran6gTerahertz::CreateTzPhyLayer(double centerFreqTHz, double bandwidthGHz, uint32_t antennaElements)
{
    Ptr<Oran6gTerahertz> thzPhy = CreateObject<Oran6gTerahertz>();
    thzPhy->ConfigureTerahertzBand(centerFreqTHz, bandwidthGHz);
    thzPhy->ConfigureUltraMassiveMimo(antennaElements, AI_ENHANCED_BEAMFORMING);
    thzPhy->EnableIntelligentReflectingSurfaces(512);
    
    return thzPhy;
}

} // namespace ns3
