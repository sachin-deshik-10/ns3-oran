/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service. You may
 * use, copy and distribute copies of the software in any medium, provided that
 * you keep intact this entire notice. You may improve, modify and create
 * derivative works of the software or any portion of the software, and you may
 * copy and distribute such modifications or works. Modified works should carry
 * a notice stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the National
 * Institute of Standards and Technology as the source of the software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES NO
 * WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION OF
 * LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
 * NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
 * DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of using and
 * distributing the software and you assume all risks associated with its use,
 * including but not limited to the risks and costs of program errors,
 * compliance with applicable laws, damage to or loss of data, programs or
 * equipment, and the unavailability or interruption of operation. This
 * software is not intended to be used in any situation where a failure could
 * cause risk of injury or damage to property. The software developed by NIST
 * employees is not subject to copyright protection within the United States.
 */

#ifndef ORAN_E2_NODE_TERMINATOR_LTE_UE_H
#define ORAN_E2_NODE_TERMINATOR_LTE_UE_H

#include "oran-e2-node-terminator.h"

#include <ns3/lte-ue-net-device.h>

namespace ns3
{

/**
 * \ingroup oran
 *
 * E2 Node Terminator for LTE UEs. This Terminator does not process any Commands.
 */
class OranE2NodeTerminatorLteUe : public OranE2NodeTerminator
{
  public:
    /**
     * Get the TypeId of the OranE2NodeTerminatorLteUe class.
     *
     * \return The TypeId.
     */
    static TypeId GetTypeId();
    /**
     * Constructor of the OranE2NodeTerminatorLteUe class.
     */
    OranE2NodeTerminatorLteUe();
    /**
     * Destructor of the OranE2NodeTerminatorLteUe class.
     */
    ~OranE2NodeTerminatorLteUe() override;
    /**
     * Get the E2 Node Type. For this Terminator, this method always returns
     * the LTE UE node type
     *
     * \return the E2 Node Type.
     */
    OranNearRtRic::NodeType GetNodeType() const override;
    /**
     * Receive a Command. All Commands are silently ignored.
     *
     * \param command The received command.
     */
    void ReceiveCommand(Ptr<OranCommand> command) override;
    /**
     * Get the NetDevice of the LTE UE.
     *
     * \return The net device.
     */
    virtual Ptr<LteUeNetDevice> GetNetDevice() const;
}; // class OranE2NodeTerminatorLteUe

} // namespace ns3

#endif /* ORAN_E2_NODE_TERMINATOR_LTE_UE_H */
