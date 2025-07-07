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

#ifndef ORAN_COMMAND_LTE_2_LTE_HANDOVER_H
#define ORAN_COMMAND_LTE_2_LTE_HANDOVER_H

#include "oran-command.h"

namespace ns3
{

/**
 * \ingroup oran
 * A Command instructing an LTE eNB to handover a UE to another LTE eNB.
 * In this command the Target E2 Node ID is the serving LTE eNB, and the command
 * contains the cell ID of the eNB to handover to, and the RNTI of the UE to be
 * handoverd.
 */
class OranCommandLte2LteHandover : public OranCommand
{
  public:
    /**
     * Gets the TypeId of the OranCommandLte2LteHandover class.
     *
     * \return The TypeId.
     */
    static TypeId GetTypeId();
    /**
     * Creates an instance of the OranCommandLte2LteHandover class.
     */
    OranCommandLte2LteHandover();
    /**
     * The destructor of the OranCommandLte2LteHandover class.
     */
    ~OranCommandLte2LteHandover() override;

    std::string ToString() const override;

  private:
    /**
     * The ID of the cell to handover to.
     */
    uint16_t m_targetCellId;
    /**
     * The RNTI of the UE to handover.
     */
    uint16_t m_targetRnti;

  public:
    /**
     * Gets the ID of the cell to handover to.
     *
     * \returns The cell ID.
     */
    uint16_t GetTargetCellId() const;
    /**
     * Gets the RNTI of the UE to handover.
     *
     * \returns The RNTI.
     */
    uint16_t GetTargetRnti() const;
}; // class OranCommandLte2LteHandover

} // namespace ns3

#endif /* ORAN_COMMAND_LTE_2_LTE_HANDOVER_H */
