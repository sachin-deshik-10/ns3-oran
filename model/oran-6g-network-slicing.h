/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 */

#ifndef ORAN_6G_NETWORK_SLICING_H
#define ORAN_6G_NETWORK_SLICING_H

#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "ns3/nstime.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/vector.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

namespace ns3
{

    /**
     * \brief Network slice information structure
     */
    struct NetworkSlice
    {
        uint32_t sliceId;                         ///< Unique slice identifier
        std::string sliceType;                    ///< Type: eMBB, URLLC, mMTC, etc.
        double bandwidthMbps;                     ///< Allocated bandwidth in Mbps
        double latencyMs;                         ///< Required latency in milliseconds
        double reliabilityPercent;                ///< Required reliability percentage
        uint32_t maxUsers;                        ///< Maximum concurrent users
        bool isActive;                            ///< Whether slice is currently active
        Time creationTime;                        ///< When slice was created
        std::map<std::string, double> qosMetrics; ///< QoS performance metrics
    };

    /**
     * \brief QoS requirement specification
     */
    struct QoSRequirement
    {
        double minBandwidth;   ///< Minimum required bandwidth (Mbps)
        double maxLatency;     ///< Maximum allowed latency (ms)
        double minReliability; ///< Minimum reliability requirement (%)
        double priority;       ///< Service priority level (0-1)
    };

    /**
     * \brief Resource allocation information
     */
    struct ResourceAllocation
    {
        double cpuPercent;        ///< CPU allocation percentage
        double memoryMB;          ///< Memory allocation in MB
        double bandwidthMbps;     ///< Bandwidth allocation in Mbps
        uint32_t antennaElements; ///< Allocated antenna elements
    };

    /**
     * \ingroup oran
     * \brief 6G Network Slicing Management Module
     *
     * This class implements advanced network slicing capabilities for 6G O-RAN systems,
     * providing dynamic slice creation, resource allocation, and QoS management.
     *
     * Key Features:
     * - Dynamic slice lifecycle management
     * - Intelligent resource allocation
     * - QoS-aware slice optimization
     * - Multi-tenant isolation
     * - Real-time performance monitoring
     * - AI-driven slice orchestration
     */
    class Oran6gNetworkSlicing : public Object
    {
    public:
        /**
         * \brief Slice service types enumeration
         */
        enum SliceType
        {
            EMBB = 0,  ///< Enhanced Mobile Broadband
            URLLC = 1, ///< Ultra-Reliable Low-Latency Communications
            MMTC = 2,  ///< Massive Machine-Type Communications
            CUSTOM = 3 ///< Custom slice type
        };

        /**
         * \brief Resource allocation strategies
         */
        enum AllocationStrategy
        {
            STATIC = 0,      ///< Static resource allocation
            DYNAMIC = 1,     ///< Dynamic resource allocation
            AI_OPTIMIZED = 2 ///< AI-optimized allocation
        };

        /**
         * \brief Get the type ID
         * \return the object TypeId
         */
        static TypeId GetTypeId(void);

        /**
         * \brief Constructor
         */
        Oran6gNetworkSlicing();

        /**
         * \brief Destructor
         */
        virtual ~Oran6gNetworkSlicing();

        // Slice Management Methods

        /**
         * \brief Create a new network slice
         * \param sliceType The type of slice to create
         * \param qosReq QoS requirements for the slice
         * \return The ID of the created slice, or 0 if creation failed
         */
        uint32_t CreateSlice(SliceType sliceType, const QoSRequirement &qosReq);

        /**
         * \brief Delete an existing network slice
         * \param sliceId The ID of the slice to delete
         * \return true if deletion was successful
         */
        bool DeleteSlice(uint32_t sliceId);

        /**
         * \brief Modify an existing slice's parameters
         * \param sliceId The ID of the slice to modify
         * \param newQosReq New QoS requirements
         * \return true if modification was successful
         */
        bool ModifySlice(uint32_t sliceId, const QoSRequirement &newQosReq);

        /**
         * \brief Activate a slice
         * \param sliceId The ID of the slice to activate
         * \return true if activation was successful
         */
        bool ActivateSlice(uint32_t sliceId);

        /**
         * \brief Deactivate a slice
         * \param sliceId The ID of the slice to deactivate
         * \return true if deactivation was successful
         */
        bool DeactivateSlice(uint32_t sliceId);

        // Resource Management Methods

        /**
         * \brief Allocate resources to a slice
         * \param sliceId The slice ID to allocate resources to
         * \param allocation The resource allocation specification
         * \return true if allocation was successful
         */
        bool AllocateResources(uint32_t sliceId, const ResourceAllocation &allocation);

        /**
         * \brief Optimize resource allocation across all slices
         * \return true if optimization was successful
         */
        bool OptimizeResourceAllocation();

        /**
         * \brief Get current resource utilization
         * \return Map of resource type to utilization percentage
         */
        std::map<std::string, double> GetResourceUtilization() const;

        /**
         * \brief Set allocation strategy
         * \param strategy The allocation strategy to use
         */
        void SetAllocationStrategy(AllocationStrategy strategy);

        // Monitoring and Analytics Methods

        /**
         * \brief Get slice performance metrics
         * \param sliceId The slice ID to get metrics for
         * \return Map of metric name to value
         */
        std::map<std::string, double> GetSliceMetrics(uint32_t sliceId) const;

        /**
         * \brief Get all active slices
         * \return Vector of active slice information
         */
        std::vector<NetworkSlice> GetActiveSlices() const;

        /**
         * \brief Update slice performance metrics
         * \param sliceId The slice ID to update
         * \param metrics New performance metrics
         */
        void UpdateSliceMetrics(uint32_t sliceId, const std::map<std::string, double> &metrics);

        /**
         * \brief Check if slice meets QoS requirements
         * \param sliceId The slice ID to check
         * \return true if QoS requirements are met
         */
        bool CheckQoSCompliance(uint32_t sliceId) const;

        // AI Integration Methods

        /**
         * \brief Enable AI-driven slice optimization
         * \param enable Whether to enable AI optimization
         */
        void EnableAIOptimization(bool enable);

        /**
         * \brief Train AI model with current slice data
         * \return true if training was successful
         */
        bool TrainAIModel();

        /**
         * \brief Get AI recommendations for slice optimization
         * \return Map of slice ID to recommended actions
         */
        std::map<uint32_t, std::string> GetAIRecommendations() const;

        // Configuration Methods

        /**
         * \brief Set maximum number of slices
         * \param maxSlices Maximum number of concurrent slices
         */
        void SetMaxSlices(uint32_t maxSlices);

        /**
         * \brief Set slice isolation level
         * \param level Isolation level (0=shared, 1=partial, 2=full)
         */
        void SetIsolationLevel(uint32_t level);

        /**
         * \brief Enable/disable dynamic resource allocation
         * \param enable Whether to enable dynamic allocation
         */
        void EnableDynamicAllocation(bool enable);

    private:
        // Private helper methods
        uint32_t GenerateSliceId();
        bool ValidateQoSRequirements(const QoSRequirement &qosReq) const;
        bool HasSufficientResources(const QoSRequirement &qosReq) const;
        void UpdateResourceMaps();
        void PerformAIOptimization();
        double CalculateSliceScore(uint32_t sliceId) const;

        // Slice management
        std::map<uint32_t, NetworkSlice> m_slices;            ///< Active slices map
        std::map<uint32_t, QoSRequirement> m_qosRequirements; ///< QoS requirements per slice
        std::map<uint32_t, ResourceAllocation> m_allocations; ///< Resource allocations per slice

        // Configuration parameters
        uint32_t m_maxSlices;                    ///< Maximum number of slices
        uint32_t m_isolationLevel;               ///< Slice isolation level
        bool m_enableDynamicAllocation;          ///< Enable dynamic allocation
        AllocationStrategy m_allocationStrategy; ///< Current allocation strategy
        double m_defaultBandwidth;               ///< Default bandwidth per slice

        // Resource tracking
        double m_totalBandwidth;         ///< Total available bandwidth
        double m_availableBandwidth;     ///< Currently available bandwidth
        uint32_t m_totalCpuCores;        ///< Total CPU cores
        uint32_t m_totalMemoryMB;        ///< Total memory in MB
        uint32_t m_totalAntennaElements; ///< Total antenna elements

        // AI and optimization
        bool m_enableAI;                                   ///< Whether AI optimization is enabled
        std::vector<std::vector<double>> m_aiTrainingData; ///< AI training dataset
        Time m_lastOptimization;                           ///< Last optimization timestamp

        // Statistics and monitoring
        uint32_t m_nextSliceId;                        ///< Next available slice ID
        Time m_lastMetricsUpdate;                      ///< Last metrics update time
        std::map<std::string, double> m_globalMetrics; ///< Global performance metrics
    };

} // namespace ns3

#endif /* ORAN_6G_NETWORK_SLICING_H */
