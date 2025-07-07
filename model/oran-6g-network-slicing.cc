/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 */

#include "oran-6g-network-slicing.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include <algorithm>
#include <cmath>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("Oran6gNetworkSlicing");

    NS_OBJECT_ENSURE_REGISTERED(Oran6gNetworkSlicing);

    TypeId
    Oran6gNetworkSlicing::GetTypeId(void)
    {
        static TypeId tid = TypeId("ns3::Oran6gNetworkSlicing")
                                .SetParent<Object>()
                                .SetGroupName("Oran")
                                .AddConstructor<Oran6gNetworkSlicing>()
                                .AddAttribute("MaxSlices",
                                              "Maximum number of network slices",
                                              UintegerValue(16),
                                              MakeUintegerAccessor(&Oran6gNetworkSlicing::m_maxSlices),
                                              MakeUintegerChecker<uint32_t>(1, 64))
                                .AddAttribute("DefaultSliceBandwidth",
                                              "Default bandwidth allocation per slice (Mbps)",
                                              DoubleValue(100.0),
                                              MakeDoubleAccessor(&Oran6gNetworkSlicing::m_defaultBandwidth),
                                              MakeDoubleChecker<double>(1.0, 10000.0))
                                .AddAttribute("EnableDynamicAllocation",
                                              "Enable dynamic resource allocation",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&Oran6gNetworkSlicing::m_enableDynamicAllocation),
                                              MakeBooleanChecker())
                                .AddAttribute("SliceIsolationLevel",
                                              "Level of slice isolation (0=shared, 1=partial, 2=full)",
                                              UintegerValue(1),
                                              MakeUintegerAccessor(&Oran6gNetworkSlicing::m_isolationLevel),
                                              MakeUintegerChecker<uint32_t>(0, 2))
                                .AddAttribute("QosThreshold",
                                              "QoS threshold for slice performance",
                                              DoubleValue(0.95),
                                              MakeDoubleAccessor(&Oran6gNetworkSlicing::m_qosThreshold),
                                              MakeDoubleChecker<double>(0.1, 1.0))
                                .AddTraceSource("SliceCreated",
                                                "A new network slice has been created",
                                                MakeTraceSourceAccessor(&Oran6gNetworkSlicing::m_sliceCreatedTrace),
                                                "ns3::TracedCallback")
                                .AddTraceSource("SliceModified",
                                                "A network slice has been modified",
                                                MakeTraceSourceAccessor(&Oran6gNetworkSlicing::m_sliceModifiedTrace),
                                                "ns3::TracedCallback")
                                .AddTraceSource("ResourceAllocation",
                                                "Resource allocation events",
                                                MakeTraceSourceAccessor(&Oran6gNetworkSlicing::m_resourceAllocationTrace),
                                                "ns3::TracedCallback");
        return tid;
    }

    Oran6gNetworkSlicing::Oran6gNetworkSlicing()
        : m_maxSlices(16),
          m_defaultBandwidth(100.0),
          m_enableDynamicAllocation(true),
          m_isolationLevel(1),
          m_qosThreshold(0.95),
          m_totalBandwidth(1000.0),
          m_availableBandwidth(1000.0),
          m_nextSliceId(1)
    {
        NS_LOG_FUNCTION(this);
        InitializeSlicingEngine();
    }

    Oran6gNetworkSlicing::~Oran6gNetworkSlicing()
    {
        NS_LOG_FUNCTION(this);
        m_networkSlices.clear();
    }

    void
    Oran6gNetworkSlicing::InitializeSlicingEngine()
    {
        NS_LOG_FUNCTION(this);

        // Initialize default network slice configurations
        m_sliceTemplates.clear();

        // Enhanced Mobile Broadband (eMBB)
        SliceTemplate eMBB;
        eMBB.type = SLICE_EMBB;
        eMBB.minBandwidth = 50.0;
        eMBB.maxLatency = 10.0; // ms
        eMBB.minReliability = 0.99;
        eMBB.priority = 2;
        m_sliceTemplates[SLICE_EMBB] = eMBB;

        // Ultra-Reliable Low Latency Communications (URLLC)
        SliceTemplate uRLLC;
        uRLLC.type = SLICE_URLLC;
        uRLLC.minBandwidth = 10.0;
        uRLLC.maxLatency = 1.0; // ms
        uRLLC.minReliability = 0.99999;
        uRLLC.priority = 1;
        m_sliceTemplates[SLICE_URLLC] = uRLLC;

        // Massive Machine Type Communications (mMTC)
        SliceTemplate mMTC;
        mMTC.type = SLICE_MMTC;
        mMTC.minBandwidth = 1.0;
        mMTC.maxLatency = 100.0; // ms
        mMTC.minReliability = 0.95;
        mMTC.priority = 3;
        m_sliceTemplates[SLICE_MMTC] = mMTC;

        // Extended Reality (XR)
        SliceTemplate xR;
        xR.type = SLICE_XR;
        xR.minBandwidth = 200.0;
        xR.maxLatency = 5.0; // ms
        xR.minReliability = 0.999;
        xR.priority = 1;
        m_sliceTemplates[SLICE_XR] = xR;

        // Automotive and V2X
        SliceTemplate automotive;
        automotive.type = SLICE_AUTOMOTIVE;
        automotive.minBandwidth = 20.0;
        automotive.maxLatency = 2.0; // ms
        automotive.minReliability = 0.9999;
        automotive.priority = 1;
        m_sliceTemplates[SLICE_AUTOMOTIVE] = automotive;

        NS_LOG_INFO("Network slicing engine initialized with " << m_sliceTemplates.size() << " slice templates");
    }

    uint32_t
    Oran6gNetworkSlicing::CreateSlice(SliceType type, const SliceRequirements &requirements)
    {
        NS_LOG_FUNCTION(this << static_cast<uint32_t>(type));

        if (m_networkSlices.size() >= m_maxSlices)
        {
            NS_LOG_ERROR("Maximum number of slices reached");
            return 0;
        }

        // Get template for the slice type
        auto templateIt = m_sliceTemplates.find(type);
        if (templateIt == m_sliceTemplates.end())
        {
            NS_LOG_ERROR("Unknown slice type: " << static_cast<uint32_t>(type));
            return 0;
        }

        SliceTemplate sliceTemplate = templateIt->second;

        // Create new network slice
        NetworkSlice newSlice;
        newSlice.id = m_nextSliceId++;
        newSlice.type = type;
        newSlice.state = SLICE_ACTIVE;
        newSlice.creationTime = Simulator::Now();

        // Set requirements (use template defaults if not specified)
        newSlice.requirements.bandwidth = requirements.bandwidth > 0 ? requirements.bandwidth : sliceTemplate.minBandwidth;
        newSlice.requirements.latency = requirements.latency > 0 ? requirements.latency : sliceTemplate.maxLatency;
        newSlice.requirements.reliability = requirements.reliability > 0 ? requirements.reliability : sliceTemplate.minReliability;
        newSlice.requirements.priority = requirements.priority > 0 ? requirements.priority : sliceTemplate.priority;

        // Allocate resources
        if (!AllocateResources(newSlice))
        {
            NS_LOG_ERROR("Failed to allocate resources for slice " << newSlice.id);
            return 0;
        }

        // Add to active slices
        m_networkSlices[newSlice.id] = newSlice;

        // Initialize performance metrics
        m_sliceMetrics[newSlice.id] = SliceMetrics();
        m_sliceMetrics[newSlice.id].throughput = 0.0;
        m_sliceMetrics[newSlice.id].latency = 0.0;
        m_sliceMetrics[newSlice.id].packetLoss = 0.0;
        m_sliceMetrics[newSlice.id].reliability = 1.0;

        m_sliceCreatedTrace(newSlice.id);

        NS_LOG_INFO("Created network slice " << newSlice.id << " of type " << static_cast<uint32_t>(type) << " with bandwidth " << newSlice.allocatedResources.bandwidth << " Mbps");

        return newSlice.id;
    }

    bool
    Oran6gNetworkSlicing::AllocateResources(NetworkSlice &slice)
    {
        NS_LOG_FUNCTION(this << slice.id);

        // Check if enough bandwidth is available
        if (slice.requirements.bandwidth > m_availableBandwidth)
        {
            if (m_enableDynamicAllocation)
            {
                // Try to reallocate resources from lower priority slices
                if (!ReallocateResources(slice.requirements.bandwidth, slice.requirements.priority))
                {
                    NS_LOG_WARN("Cannot allocate " << slice.requirements.bandwidth << " Mbps, only " << m_availableBandwidth << " Mbps available");
                    return false;
                }
            }
            else
            {
                NS_LOG_ERROR("Insufficient bandwidth for slice allocation");
                return false;
            }
        }

        // Allocate resources
        slice.allocatedResources.bandwidth = slice.requirements.bandwidth;
        slice.allocatedResources.computeUnits = CalculateComputeUnits(slice);
        slice.allocatedResources.memoryMB = CalculateMemoryRequirement(slice);
        slice.allocatedResources.storageGB = CalculateStorageRequirement(slice);

        // Update available resources
        m_availableBandwidth -= slice.requirements.bandwidth;

        m_resourceAllocationTrace(slice.id, slice.allocatedResources.bandwidth);

        return true;
    }

    bool
    Oran6gNetworkSlicing::ReallocateResources(double requiredBandwidth, uint32_t priority)
    {
        NS_LOG_FUNCTION(this << requiredBandwidth << priority);

        double reclaimedBandwidth = 0.0;
        std::vector<uint32_t> slicesToModify;

        // Find lower priority slices that can give up bandwidth
        for (auto &slicePair : m_networkSlices)
        {
            NetworkSlice &slice = slicePair.second;
            if (slice.requirements.priority > priority && slice.state == SLICE_ACTIVE)
            {
                double excessBandwidth = slice.allocatedResources.bandwidth -
                                         m_sliceTemplates[slice.type].minBandwidth;
                if (excessBandwidth > 0)
                {
                    double toReclaim = std::min(excessBandwidth, requiredBandwidth - reclaimedBandwidth);
                    slice.allocatedResources.bandwidth -= toReclaim;
                    reclaimedBandwidth += toReclaim;
                    slicesToModify.push_back(slice.id);

                    if (reclaimedBandwidth >= requiredBandwidth)
                        break;
                }
            }
        }

        if (reclaimedBandwidth >= requiredBandwidth)
        {
            m_availableBandwidth += reclaimedBandwidth;

            // Trace slice modifications
            for (uint32_t sliceId : slicesToModify)
            {
                m_sliceModifiedTrace(sliceId);
            }

            NS_LOG_INFO("Reclaimed " << reclaimedBandwidth << " Mbps from " << slicesToModify.size() << " slices");
            return true;
        }

        NS_LOG_WARN("Could only reclaim " << reclaimedBandwidth << " Mbps, need " << requiredBandwidth);
        return false;
    }

    uint32_t
    Oran6gNetworkSlicing::CalculateComputeUnits(const NetworkSlice &slice)
    {
        // Base compute units based on slice type
        uint32_t baseUnits = 10;

        switch (slice.type)
        {
        case SLICE_EMBB:
            baseUnits = 20;
            break;
        case SLICE_URLLC:
            baseUnits = 50; // Higher processing for low latency
            break;
        case SLICE_MMTC:
            baseUnits = 5; // Lower processing requirements
            break;
        case SLICE_XR:
            baseUnits = 100; // High processing for XR applications
            break;
        case SLICE_AUTOMOTIVE:
            baseUnits = 75; // High processing for safety-critical apps
            break;
        }

        // Scale based on bandwidth requirements
        return static_cast<uint32_t>(baseUnits * (slice.requirements.bandwidth / m_defaultBandwidth));
    }

    uint32_t
    Oran6gNetworkSlicing::CalculateMemoryRequirement(const NetworkSlice &slice)
    {
        // Base memory in MB
        uint32_t baseMem = 100;

        switch (slice.type)
        {
        case SLICE_EMBB:
            baseMem = 200;
            break;
        case SLICE_URLLC:
            baseMem = 150;
            break;
        case SLICE_MMTC:
            baseMem = 50;
            break;
        case SLICE_XR:
            baseMem = 500;
            break;
        case SLICE_AUTOMOTIVE:
            baseMem = 300;
            break;
        }

        return static_cast<uint32_t>(baseMem * (slice.requirements.bandwidth / m_defaultBandwidth));
    }

    uint32_t
    Oran6gNetworkSlicing::CalculateStorageRequirement(const NetworkSlice &slice)
    {
        // Base storage in GB
        uint32_t baseStorage = 10;

        switch (slice.type)
        {
        case SLICE_EMBB:
            baseStorage = 50;
            break;
        case SLICE_URLLC:
            baseStorage = 20;
            break;
        case SLICE_MMTC:
            baseStorage = 5;
            break;
        case SLICE_XR:
            baseStorage = 100;
            break;
        case SLICE_AUTOMOTIVE:
            baseStorage = 30;
            break;
        }

        return baseStorage;
    }

    bool
    Oran6gNetworkSlicing::ModifySlice(uint32_t sliceId, const SliceRequirements &newRequirements)
    {
        NS_LOG_FUNCTION(this << sliceId);

        auto sliceIt = m_networkSlices.find(sliceId);
        if (sliceIt == m_networkSlices.end())
        {
            NS_LOG_ERROR("Slice " << sliceId << " not found");
            return false;
        }

        NetworkSlice &slice = sliceIt->second;

        // Calculate bandwidth difference
        double bandwidthDiff = newRequirements.bandwidth - slice.allocatedResources.bandwidth;

        if (bandwidthDiff > 0)
        {
            // Need more bandwidth
            if (bandwidthDiff > m_availableBandwidth)
            {
                if (!ReallocateResources(bandwidthDiff, slice.requirements.priority))
                {
                    NS_LOG_ERROR("Cannot modify slice " << sliceId << " - insufficient resources");
                    return false;
                }
            }
            m_availableBandwidth -= bandwidthDiff;
        }
        else if (bandwidthDiff < 0)
        {
            // Freeing bandwidth
            m_availableBandwidth += (-bandwidthDiff);
        }

        // Update slice requirements and allocated resources
        slice.requirements = newRequirements;
        slice.allocatedResources.bandwidth = newRequirements.bandwidth;
        slice.allocatedResources.computeUnits = CalculateComputeUnits(slice);
        slice.allocatedResources.memoryMB = CalculateMemoryRequirement(slice);

        m_sliceModifiedTrace(sliceId);

        NS_LOG_INFO("Modified slice " << sliceId << " - new bandwidth: " << newRequirements.bandwidth << " Mbps");

        return true;
    }

    bool
    Oran6gNetworkSlicing::DeleteSlice(uint32_t sliceId)
    {
        NS_LOG_FUNCTION(this << sliceId);

        auto sliceIt = m_networkSlices.find(sliceId);
        if (sliceIt == m_networkSlices.end())
        {
            NS_LOG_ERROR("Slice " << sliceId << " not found");
            return false;
        }

        NetworkSlice &slice = sliceIt->second;

        // Free allocated resources
        m_availableBandwidth += slice.allocatedResources.bandwidth;

        // Remove from active slices
        m_networkSlices.erase(sliceIt);
        m_sliceMetrics.erase(sliceId);

        NS_LOG_INFO("Deleted slice " << sliceId << " - freed " << slice.allocatedResources.bandwidth << " Mbps");

        return true;
    }

    void
    Oran6gNetworkSlicing::UpdateSliceMetrics(uint32_t sliceId, const SliceMetrics &metrics)
    {
        NS_LOG_FUNCTION(this << sliceId);

        auto metricsIt = m_sliceMetrics.find(sliceId);
        if (metricsIt == m_sliceMetrics.end())
        {
            NS_LOG_WARN("Metrics for slice " << sliceId << " not found");
            return;
        }

        m_sliceMetrics[sliceId] = metrics;

        // Check if slice is meeting QoS requirements
        auto sliceIt = m_networkSlices.find(sliceId);
        if (sliceIt != m_networkSlices.end())
        {
            NetworkSlice &slice = sliceIt->second;

            // Check latency requirement
            if (metrics.latency > slice.requirements.latency)
            {
                NS_LOG_WARN("Slice " << sliceId << " latency (" << metrics.latency << " ms) exceeds requirement (" << slice.requirements.latency << " ms)");
                HandleQosViolation(sliceId, QOS_LATENCY_VIOLATION);
            }

            // Check reliability requirement
            if (metrics.reliability < slice.requirements.reliability)
            {
                NS_LOG_WARN("Slice " << sliceId << " reliability (" << metrics.reliability << ") below requirement (" << slice.requirements.reliability << ")");
                HandleQosViolation(sliceId, QOS_RELIABILITY_VIOLATION);
            }
        }
    }

    void
    Oran6gNetworkSlicing::HandleQosViolation(uint32_t sliceId, QosViolationType violation)
    {
        NS_LOG_FUNCTION(this << sliceId << static_cast<uint32_t>(violation));

        auto sliceIt = m_networkSlices.find(sliceId);
        if (sliceIt == m_networkSlices.end())
        {
            return;
        }

        NetworkSlice &slice = sliceIt->second;

        switch (violation)
        {
        case QOS_LATENCY_VIOLATION:
            // Try to allocate more resources to reduce latency
            if (m_enableDynamicAllocation)
            {
                double additionalBandwidth = slice.allocatedResources.bandwidth * 0.2; // 20% increase
                if (additionalBandwidth <= m_availableBandwidth)
                {
                    slice.allocatedResources.bandwidth += additionalBandwidth;
                    m_availableBandwidth -= additionalBandwidth;
                    NS_LOG_INFO("Increased bandwidth for slice " << sliceId << " to address latency violation");
                }
            }
            break;

        case QOS_RELIABILITY_VIOLATION:
            // Increase redundancy and error correction
            slice.allocatedResources.computeUnits =
                static_cast<uint32_t>(slice.allocatedResources.computeUnits * 1.5);
            NS_LOG_INFO("Increased compute units for slice " << sliceId << " to address reliability violation");
            break;

        case QOS_BANDWIDTH_VIOLATION:
            // This should be handled by dynamic allocation
            break;
        }
    }

    std::vector<uint32_t>
    Oran6gNetworkSlicing::GetActiveSlices() const
    {
        std::vector<uint32_t> activeSlices;
        for (const auto &slicePair : m_networkSlices)
        {
            if (slicePair.second.state == SLICE_ACTIVE)
            {
                activeSlices.push_back(slicePair.first);
            }
        }
        return activeSlices;
    }

    SliceMetrics
    Oran6gNetworkSlicing::GetSliceMetrics(uint32_t sliceId) const
    {
        auto metricsIt = m_sliceMetrics.find(sliceId);
        if (metricsIt != m_sliceMetrics.end())
        {
            return metricsIt->second;
        }
        return SliceMetrics(); // Return default metrics if not found
    }

    NetworkSlice
    Oran6gNetworkSlicing::GetSliceInfo(uint32_t sliceId) const
    {
        auto sliceIt = m_networkSlices.find(sliceId);
        if (sliceIt != m_networkSlices.end())
        {
            return sliceIt->second;
        }
        return NetworkSlice(); // Return default slice if not found
    }

    double
    Oran6gNetworkSlicing::GetTotalAllocatedBandwidth() const
    {
        return m_totalBandwidth - m_availableBandwidth;
    }

    double
    Oran6gNetworkSlicing::GetAvailableBandwidth() const
    {
        return m_availableBandwidth;
    }

    uint32_t
    Oran6gNetworkSlicing::GetSliceCount() const
    {
        return m_networkSlices.size();
    }

    void
    Oran6gNetworkSlicing::SetTotalBandwidth(double bandwidth)
    {
        NS_LOG_FUNCTION(this << bandwidth);

        double allocatedBandwidth = GetTotalAllocatedBandwidth();
        m_totalBandwidth = bandwidth;
        m_availableBandwidth = bandwidth - allocatedBandwidth;

        if (m_availableBandwidth < 0)
        {
            NS_LOG_WARN("Total bandwidth reduced below allocated bandwidth");
            m_availableBandwidth = 0;
        }
    }

    void
    Oran6gNetworkSlicing::EnableDynamicAllocation(bool enable)
    {
        NS_LOG_FUNCTION(this << enable);
        m_enableDynamicAllocation = enable;
    }

    void
    Oran6gNetworkSlicing::PrintSliceStatus() const
    {
        NS_LOG_INFO("=== Network Slicing Status ===");
        NS_LOG_INFO("Total Bandwidth: " << m_totalBandwidth << " Mbps");
        NS_LOG_INFO("Available Bandwidth: " << m_availableBandwidth << " Mbps");
        NS_LOG_INFO("Active Slices: " << m_networkSlices.size());

        for (const auto &slicePair : m_networkSlices)
        {
            const NetworkSlice &slice = slicePair.second;
            NS_LOG_INFO("Slice " << slice.id << " (Type: " << static_cast<uint32_t>(slice.type) << ", BW: " << slice.allocatedResources.bandwidth << " Mbps, " << "Priority: " << slice.requirements.priority << ")");
        }
    }

} // namespace ns3
