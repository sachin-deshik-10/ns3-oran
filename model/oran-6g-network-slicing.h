/*
 * Copyright (c) 2025 O-RAN Enhanced Module
 *
 * 6G Network Slicing Module
 * 
 * This module implements dynamic network slicing for 6G networks
 * with AI-driven resource allocation and multi-tenancy support.
 */

#ifndef ORAN_6G_NETWORK_SLICING_H
#define ORAN_6G_NETWORK_SLICING_H

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/nstime.h"
#include "oran-6g-terahertz.h"
#include "oran-ai-transformer.h"
#include <vector>
#include <map>
#include <memory>

namespace ns3 {

/**
 * \ingroup oran
 * \brief 6G Network Slicing with AI Orchestration
 *
 * This class implements dynamic network slicing for 6G networks
 * with intelligent resource allocation and multi-tenant support.
 */
class Oran6gNetworkSlicing : public Object
{
public:
    /**
     * \brief Get the type ID.
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

    /**
     * \brief Network slice types
     */
    enum SliceType
    {
        EMBB_SLICE = 0,           ///< Enhanced Mobile Broadband
        URLLC_SLICE = 1,          ///< Ultra-Reliable Low Latency
        MMTC_SLICE = 2,           ///< Massive Machine Type Communications
        CUSTOM_SLICE = 3          ///< Custom application slice
    };

    /**
     * \brief QoS requirements for network slices
     */
    struct QosRequirements
    {
        double maxLatency;        ///< Maximum latency in ms
        double minThroughput;     ///< Minimum throughput in Mbps
        double reliability;       ///< Reliability requirement (0-1)
        double availablility;     ///< Availability requirement (0-1)
        uint32_t priority;        ///< Priority level (1-10)
        std::string serviceName;  ///< Service name
    };

    /**
     * \brief Network slice configuration
     */
    struct SliceConfiguration
    {
        uint32_t sliceId;         ///< Unique slice identifier
        SliceType sliceType;      ///< Type of slice
        QosRequirements qos;      ///< QoS requirements
        double allocatedBandwidth; ///< Allocated bandwidth in MHz
        uint32_t allocatedRBs;    ///< Allocated resource blocks
        std::vector<uint32_t> associatedUEs; ///< UEs in this slice
        Time creationTime;        ///< Slice creation time
        bool isActive;            ///< Slice active status
    };

    /**
     * \brief Slice performance metrics
     */
    struct SliceMetrics
    {
        uint32_t sliceId;         ///< Slice identifier
        double actualLatency;     ///< Measured latency
        double actualThroughput;  ///< Measured throughput
        double resourceUtilization; ///< Resource utilization (0-1)
        double slaCompliance;     ///< SLA compliance (0-1)
        uint32_t activeConnections; ///< Number of active connections
    };

    /**
     * \brief Create a new network slice
     * \param sliceType Type of slice to create
     * \param qos QoS requirements for the slice
     * \return Slice ID if successful, 0 if failed
     */
    uint32_t CreateSlice(SliceType sliceType, const QosRequirements& qos);

    /**
     * \brief Delete a network slice
     * \param sliceId ID of slice to delete
     * \return True if successful
     */
    bool DeleteSlice(uint32_t sliceId);

    /**
     * \brief Modify slice configuration
     * \param sliceId ID of slice to modify
     * \param newQos New QoS requirements
     * \return True if successful
     */
    bool ModifySlice(uint32_t sliceId, const QosRequirements& newQos);

    /**
     * \brief Associate UE with a slice
     * \param sliceId Target slice ID
     * \param ueId UE identifier
     * \return True if successful
     */
    bool AssociateUeWithSlice(uint32_t sliceId, uint32_t ueId);

    /**
     * \brief Optimize slice resources using AI
     * \param metrics Current slice performance metrics
     */
    void OptimizeSliceResources(const std::vector<SliceMetrics>& metrics);

    /**
     * \brief Connect to THz module for resource allocation
     * \param thzModule Pointer to THz module
     */
    void ConnectToThzModule(Ptr<Oran6gTerahertz> thzModule);

    /**
     * \brief Connect to AI transformer for intelligent orchestration
     * \param aiModule Pointer to AI transformer
     */
    void ConnectToAiTransformer(Ptr<OranAiTransformer> aiModule);

    /**
     * \brief Get all active slices
     * \return Vector of active slice configurations
     */
    std::vector<SliceConfiguration> GetActiveSlices() const;

    /**
     * \brief Get slice performance metrics
     * \param sliceId Slice ID to get metrics for (0 for all slices)
     * \return Vector of slice metrics
     */
    std::vector<SliceMetrics> GetSliceMetrics(uint32_t sliceId = 0) const;

    /**
     * \brief Enable dynamic resource allocation
     * \param enabled Enable/disable dynamic allocation
     * \param updateInterval Update interval for resource reallocation
     */
    void EnableDynamicAllocation(bool enabled, Time updateInterval = Seconds(1.0));

    /**
     * \brief Set slice admission control policy
     * \param maxSlices Maximum number of concurrent slices
     * \param reservedResources Percentage of resources to reserve (0-1)
     */
    void SetAdmissionControl(uint32_t maxSlices, double reservedResources = 0.1);

    /**
     * \brief Callback for slice creation events
     */
    typedef Callback<void, uint32_t, SliceType> SliceCreatedCallback;

    /**
     * \brief Callback for slice deletion events
     */
    typedef Callback<void, uint32_t> SliceDeletedCallback;

    /**
     * \brief Set slice event callbacks
     * \param createdCallback Callback for slice creation
     * \param deletedCallback Callback for slice deletion
     */
    void SetSliceCallbacks(SliceCreatedCallback createdCallback,
                          SliceDeletedCallback deletedCallback);

protected:
    /**
     * \brief Dispose method
     */
    virtual void DoDispose() override;

private:
    /**
     * \brief Calculate resource requirements for slice
     * \param sliceType Type of slice
     * \param qos QoS requirements
     * \return Required bandwidth in MHz
     */
    double CalculateResourceRequirements(SliceType sliceType, const QosRequirements& qos);

    /**
     * \brief Perform resource allocation for all slices
     */
    void PerformResourceAllocation();

    /**
     * \brief Check SLA compliance for slices
     */
    void CheckSlaCompliance();

    /**
     * \brief Update slice metrics
     */
    void UpdateSliceMetrics();

    /**
     * \brief Schedule next resource optimization
     */
    void ScheduleResourceOptimization();

    // Slice management
    std::map<uint32_t, SliceConfiguration> m_activeSlices;  ///< Active slices
    std::map<uint32_t, SliceMetrics> m_sliceMetrics;       ///< Slice metrics
    uint32_t m_nextSliceId;                                ///< Next available slice ID

    // Resource management
    double m_totalBandwidth;                               ///< Total available bandwidth
    uint32_t m_totalResourceBlocks;                        ///< Total resource blocks
    double m_availableBandwidth;                           ///< Currently available bandwidth
    uint32_t m_availableResourceBlocks;                    ///< Available resource blocks

    // AI integration
    Ptr<Oran6gTerahertz> m_thzModule;                     ///< THz module for resources
    Ptr<OranAiTransformer> m_aiTransformer;               ///< AI for optimization

    // Configuration
    uint32_t m_maxSlices;                                  ///< Maximum concurrent slices
    double m_reservedResources;                            ///< Reserved resource percentage
    bool m_dynamicAllocationEnabled;                       ///< Dynamic allocation status
    Time m_updateInterval;                                 ///< Resource update interval

    // Events and callbacks
    EventId m_optimizationEvent;                          ///< Optimization event
    SliceCreatedCallback m_sliceCreatedCallback;          ///< Slice creation callback
    SliceDeletedCallback m_sliceDeletedCallback;          ///< Slice deletion callback

    // Performance tracking
    uint32_t m_totalSlicesCreated;                        ///< Total slices created
    uint32_t m_totalSlicesDeleted;                        ///< Total slices deleted
    double m_averageSlaCompliance;                        ///< Average SLA compliance
};

} // namespace ns3

#endif /* ORAN_6G_NETWORK_SLICING_H */