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

#include "oran-lm-lte-2-lte-torch-handover.h"

#include "oran-command-lte-2-lte-handover.h"

#include <ns3/abort.h>
#include <ns3/log.h>
#include <ns3/simulator.h>
#include <ns3/string.h>
#include <ns3/uinteger.h>

#include <fstream>

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("OranLmLte2LteTorchHandover");
NS_OBJECT_ENSURE_REGISTERED(OranLmLte2LteTorchHandover);

TypeId
OranLmLte2LteTorchHandover::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::OranLmLte2LteTorchHandover")
            .SetParent<OranLm>()
            .AddConstructor<OranLmLte2LteTorchHandover>()
            .AddAttribute("TorchModelPath",
                          "The file path of the ML model.",
                          StringValue("saved_trained_classification_pytorch.pt"),
                          MakeStringAccessor(&OranLmLte2LteTorchHandover::SetTorchModelPath),
                          MakeStringChecker());

    return tid;
}

OranLmLte2LteTorchHandover::OranLmLte2LteTorchHandover()
{
    NS_LOG_FUNCTION(this);

    m_name = "OranLmLte2LteTorchHandover";
}

OranLmLte2LteTorchHandover::~OranLmLte2LteTorchHandover()
{
    NS_LOG_FUNCTION(this);
}

std::vector<Ptr<OranCommand>>
OranLmLte2LteTorchHandover::Run()
{
    NS_LOG_FUNCTION(this);

    std::vector<Ptr<OranCommand>> commands;

    if (m_active)
    {
        NS_ABORT_MSG_IF(m_nearRtRic == nullptr,
                        "Attempting to run LM (" + m_name + ") with NULL Near-RT RIC");

        Ptr<OranDataRepository> data = m_nearRtRic->Data();
        std::vector<UeInfo> ueInfos = GetUeInfos(data);
        std::vector<EnbInfo> enbInfos = GetEnbInfos(data);
        commands = GetHandoverCommands(data, ueInfos, enbInfos);
    }

    return commands;
}

void
OranLmLte2LteTorchHandover::SetTorchModelPath(const std::string& torchModelPath)
{
    std::ifstream f(torchModelPath.c_str());
    NS_ABORT_MSG_IF(!f.good(),
                    "Torch model file \""
                        << torchModelPath << "\" not found."
                        << " Sample model \"saved_trained_classification_pytorch.pt\""
                        << " can be copied from the example folder to the working directory.");
    f.close();

    try
    {
        m_model = torch::jit::load(torchModelPath);
    }
    catch (const c10::Error& e)
    {
        NS_ABORT_MSG("Could not load trained ML model.");
    }
}

std::vector<OranLmLte2LteTorchHandover::UeInfo>
OranLmLte2LteTorchHandover::GetUeInfos(Ptr<OranDataRepository> data) const
{
    NS_LOG_FUNCTION(this << data);

    std::vector<UeInfo> ueInfos;
    for (const auto ueId : data->GetLteUeE2NodeIds())
    {
        UeInfo ueInfo;
        ueInfo.nodeId = ueId;
        bool found;
        std::tie(found, ueInfo.cellId, ueInfo.rnti) = data->GetLteUeCellInfo(ueInfo.nodeId);
        if (found)
        {
            std::map<Time, Vector> nodePositions =
                data->GetNodePositions(ueInfo.nodeId, Seconds(0), Simulator::Now());
            if (!nodePositions.empty())
            {
                ueInfo.position = nodePositions.rbegin()->second;
                ueInfo.loss = data->GetAppLoss(ueInfo.nodeId);
                ueInfos.push_back(ueInfo);
            }
            else
            {
                NS_LOG_INFO("Could not find LTE UE location for E2 Node ID = " << ueInfo.nodeId);
            }
        }
        else
        {
            NS_LOG_INFO("Could not find LTE UE cell info for E2 Node ID = " << ueInfo.nodeId);
        }
    }
    return ueInfos;
}

std::vector<OranLmLte2LteTorchHandover::EnbInfo>
OranLmLte2LteTorchHandover::GetEnbInfos(Ptr<OranDataRepository> data) const
{
    NS_LOG_FUNCTION(this << data);

    std::vector<EnbInfo> enbInfos;
    for (const auto enbId : data->GetLteEnbE2NodeIds())
    {
        EnbInfo enbInfo;
        enbInfo.nodeId = enbId;
        bool found;
        std::tie(found, enbInfo.cellId) = data->GetLteEnbCellInfo(enbInfo.nodeId);
        if (found)
        {
            std::map<Time, Vector> nodePositions =
                data->GetNodePositions(enbInfo.nodeId, Seconds(0), Simulator::Now());

            if (!nodePositions.empty())
            {
                enbInfo.position = nodePositions.rbegin()->second;
                enbInfos.push_back(enbInfo);
            }
            else
            {
                NS_LOG_INFO("Could not find LTE eNB location for E2 Node ID = " << enbInfo.nodeId);
            }
        }
        else
        {
            NS_LOG_INFO("Could not find LTE eNB cell info for E2 Node ID = " << enbInfo.nodeId);
        }
    }
    return enbInfos;
}

std::vector<Ptr<OranCommand>>
OranLmLte2LteTorchHandover::GetHandoverCommands(
    Ptr<OranDataRepository> data,
    std::vector<OranLmLte2LteTorchHandover::UeInfo> ueInfos,
    std::vector<OranLmLte2LteTorchHandover::EnbInfo> enbInfos)
{
    NS_LOG_FUNCTION(this << data);

    std::vector<Ptr<OranCommand>> commands;

    std::map<uint16_t, float> distanceEnb1;
    std::map<uint16_t, float> distanceEnb2;
    std::map<uint16_t, float> loss;

    for (const auto ueInfo : ueInfos)
    {
        for (const auto enbInfo : enbInfos)
        {
            float d = std::sqrt(std::pow(ueInfo.position.x - enbInfo.position.x, 2) +
                                std::pow(ueInfo.position.y - enbInfo.position.y, 2));
            if (enbInfo.cellId == 1)
            {
                distanceEnb1[ueInfo.nodeId] = d;
            }
            else
            {
                distanceEnb2[ueInfo.nodeId] = d;
            }
        }
        loss[ueInfo.nodeId] = ueInfo.loss;
    }

    std::vector<float> inputv = {distanceEnb1[1],
                                 distanceEnb2[1],
                                 loss[1],
                                 distanceEnb1[2],
                                 distanceEnb2[2],
                                 loss[2],
                                 distanceEnb1[3],
                                 distanceEnb2[3],
                                 loss[3],
                                 distanceEnb1[4],
                                 distanceEnb2[4],
                                 loss[4]};

    LogLogicToRepository("ML input tensor: (" + std::to_string(inputv.at(0)) + ", " +
                         std::to_string(inputv.at(1)) + ", " + std::to_string(inputv.at(2)) + ", " +
                         std::to_string(inputv.at(3)) + ", " + std::to_string(inputv.at(4)) + ", " +
                         std::to_string(inputv.at(5)) + ", " + std::to_string(inputv.at(6)) + ", " +
                         std::to_string(inputv.at(7)) + ", " + std::to_string(inputv.at(8)) + ", " +
                         std::to_string(inputv.at(9)) + ", " + std::to_string(inputv.at(10)) +
                         ", " + std::to_string(inputv.at(11)) + ", " + ")");

    std::vector<torch::jit::IValue> inputs;
    inputs.push_back(torch::from_blob(inputv.data(), {1, 12}).to(torch::kFloat32));
    at::Tensor output = torch::softmax(m_model.forward(inputs).toTensor(), 1);

    int configuration = output.argmax(1).item().toInt();
    LogLogicToRepository("ML Chooses configuration " + std::to_string(configuration));

    for (const auto ueInfo : ueInfos)
    {
        if (ueInfo.nodeId == 2)
        {
            if (ueInfo.cellId == 1 && (configuration == 2 || configuration == 3))
            {
                // Handover to cellId 2
                Ptr<OranCommandLte2LteHandover> handoverCommand =
                    CreateObject<OranCommandLte2LteHandover>();
                handoverCommand->SetAttribute("TargetE2NodeId", UintegerValue(5));
                handoverCommand->SetAttribute("TargetRnti", UintegerValue(ueInfo.rnti));
                handoverCommand->SetAttribute("TargetCellId", UintegerValue(2));
                data->LogCommandLm(m_name, handoverCommand);
                commands.push_back(handoverCommand);

                LogLogicToRepository("Moving UE 2 to Cell ID 2");
            }
            else
            {
                if (ueInfo.cellId == 2 && (configuration == 0 || configuration == 1))
                {
                    // Handover to cellId 1
                    Ptr<OranCommandLte2LteHandover> handoverCommand =
                        CreateObject<OranCommandLte2LteHandover>();
                    handoverCommand->SetAttribute("TargetE2NodeId", UintegerValue(6));
                    handoverCommand->SetAttribute("TargetRnti", UintegerValue(ueInfo.rnti));
                    handoverCommand->SetAttribute("TargetCellId", UintegerValue(1));
                    data->LogCommandLm(m_name, handoverCommand);
                    commands.push_back(handoverCommand);

                    LogLogicToRepository("Moving UE 2 to Cell ID 1");
                }
            }
        }
        else
        {
            if (ueInfo.nodeId == 3)
            {
                if (ueInfo.cellId == 1 && (configuration == 1 || configuration == 3))
                {
                    // Handover to cellId 2
                    Ptr<OranCommandLte2LteHandover> handoverCommand =
                        CreateObject<OranCommandLte2LteHandover>();
                    handoverCommand->SetAttribute("TargetE2NodeId", UintegerValue(5));
                    handoverCommand->SetAttribute("TargetRnti", UintegerValue(ueInfo.rnti));
                    handoverCommand->SetAttribute("TargetCellId", UintegerValue(2));
                    data->LogCommandLm(m_name, handoverCommand);
                    commands.push_back(handoverCommand);

                    LogLogicToRepository("Moving UE 3 to Cell ID 2");
                }
                else
                {
                    if (ueInfo.cellId == 2 && (configuration == 0 || configuration == 2))
                    {
                        // Handover to cellId 1
                        Ptr<OranCommandLte2LteHandover> handoverCommand =
                            CreateObject<OranCommandLte2LteHandover>();
                        handoverCommand->SetAttribute("TargetE2NodeId", UintegerValue(6));
                        handoverCommand->SetAttribute("TargetRnti", UintegerValue(ueInfo.rnti));
                        handoverCommand->SetAttribute("TargetCellId", UintegerValue(1));
                        data->LogCommandLm(m_name, handoverCommand);
                        commands.push_back(handoverCommand);

                        LogLogicToRepository("Moving UE 3 to Cell ID 1");
                    }
                }
            }
        }
    }

    return commands;
}

} // namespace ns3
