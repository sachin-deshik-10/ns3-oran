/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 * Advanced Reinforcement Learning Logic Module Implementation
 */

#include "oran-lm-reinforcement-learning.h"
#include "oran-command-lte-2-lte-handover.h"
#include "oran-report-lte-ue-cell-info.h"
#include "oran-report-location.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE("OranLmReinforcementLearning");
    NS_OBJECT_ENSURE_REGISTERED(OranLmReinforcementLearning);

    // DQN Network Implementation
    DQNNetwork::DQNNetwork(int64_t state_size, int64_t action_size, int64_t hidden_size)
    {
        fc1 = register_module("fc1", torch::nn::Linear(state_size, hidden_size));
        fc2 = register_module("fc2", torch::nn::Linear(hidden_size, hidden_size));
        fc3 = register_module("fc3", torch::nn::Linear(hidden_size, hidden_size));
        fc4 = register_module("fc4", torch::nn::Linear(hidden_size, action_size));
        dropout = register_module("dropout", torch::nn::Dropout(0.2));
    }

    torch::Tensor
    DQNNetwork::forward(torch::Tensor x)
    {
        x = torch::relu(fc1->forward(x));
        x = dropout->forward(x);
        x = torch::relu(fc2->forward(x));
        x = dropout->forward(x);
        x = torch::relu(fc3->forward(x));
        x = fc4->forward(x);
        return x;
    }

    // PPO Network Implementation
    PPONetwork::PPONetwork(int64_t state_size, int64_t action_size, int64_t hidden_size)
    {
        shared_fc1 = register_module("shared_fc1", torch::nn::Linear(state_size, hidden_size));
        shared_fc2 = register_module("shared_fc2", torch::nn::Linear(hidden_size, hidden_size));
        actor_fc = register_module("actor_fc", torch::nn::Linear(hidden_size, action_size));
        critic_fc = register_module("critic_fc", torch::nn::Linear(hidden_size, 1));
        dropout = register_module("dropout", torch::nn::Dropout(0.2));
    }

    std::pair<torch::Tensor, torch::Tensor>
    PPONetwork::forward(torch::Tensor x)
    {
        x = torch::relu(shared_fc1->forward(x));
        x = dropout->forward(x);
        x = torch::relu(shared_fc2->forward(x));

        auto action_logits = actor_fc->forward(x);
        auto value = critic_fc->forward(x);

        return std::make_pair(action_logits, value);
    }

    torch::Tensor
    PPONetwork::get_action_probs(torch::Tensor x)
    {
        auto [action_logits, _] = forward(x);
        return torch::softmax(action_logits, -1);
    }

    torch::Tensor
    PPONetwork::get_value(torch::Tensor x)
    {
        auto [_, value] = forward(x);
        return value;
    }

    TypeId
    OranLmReinforcementLearning::GetTypeId()
    {
        static TypeId tid = TypeId("ns3::OranLmReinforcementLearning")
                                .SetParent<OranLm>()
                                .SetGroupName("Oran")
                                .AddConstructor<OranLmReinforcementLearning>()
                                .AddAttribute("Algorithm",
                                              "RL Algorithm type (DQN or PPO)",
                                              StringValue("DQN"),
                                              MakeStringAccessor(&OranLmReinforcementLearning::m_algorithm),
                                              MakeStringChecker())
                                .AddAttribute("LearningRate",
                                              "Learning rate for neural network training",
                                              DoubleValue(0.001),
                                              MakeDoubleAccessor(&OranLmReinforcementLearning::m_learningRate),
                                              MakeDoubleChecker<double>(0.0001, 0.1))
                                .AddAttribute("Epsilon",
                                              "Exploration rate for epsilon-greedy policy",
                                              DoubleValue(1.0),
                                              MakeDoubleAccessor(&OranLmReinforcementLearning::m_epsilon),
                                              MakeDoubleChecker<double>(0.0, 1.0))
                                .AddAttribute("EpsilonDecay",
                                              "Epsilon decay rate",
                                              DoubleValue(0.995),
                                              MakeDoubleAccessor(&OranLmReinforcementLearning::m_epsilonDecay),
                                              MakeDoubleChecker<double>(0.9, 1.0))
                                .AddAttribute("Gamma",
                                              "Discount factor for future rewards",
                                              DoubleValue(0.99),
                                              MakeDoubleAccessor(&OranLmReinforcementLearning::m_gamma),
                                              MakeDoubleChecker<double>(0.0, 1.0))
                                .AddAttribute("BufferSize",
                                              "Experience replay buffer size",
                                              UintegerValue(10000),
                                              MakeUintegerAccessor(&OranLmReinforcementLearning::m_bufferSize),
                                              MakeUintegerChecker<uint32_t>(1000, 100000))
                                .AddAttribute("BatchSize",
                                              "Training batch size",
                                              UintegerValue(32),
                                              MakeUintegerAccessor(&OranLmReinforcementLearning::m_batchSize),
                                              MakeUintegerChecker<uint32_t>(16, 256))
                                .AddAttribute("OnlineTraining",
                                              "Enable online training during simulation",
                                              BooleanValue(true),
                                              MakeBooleanAccessor(&OranLmReinforcementLearning::m_onlineTraining),
                                              MakeBooleanChecker())
                                .AddAttribute("MultiAgentMode",
                                              "Enable multi-agent coordination",
                                              BooleanValue(false),
                                              MakeBooleanAccessor(&OranLmReinforcementLearning::m_multiAgentMode),
                                              MakeBooleanChecker())
                                .AddTraceSource("AverageReward",
                                                "Average reward over training episodes",
                                                MakeTraceSourceAccessor(&OranLmReinforcementLearning::m_averageReward),
                                                "ns3::TracedValueCallback::Double")
                                .AddTraceSource("EpsilonValue",
                                                "Current epsilon value for exploration",
                                                MakeTraceSourceAccessor(&OranLmReinforcementLearning::m_epsilon_trace),
                                                "ns3::TracedValueCallback::Double")
                                .AddTraceSource("HandoverCount",
                                                "Number of handovers performed",
                                                MakeTraceSourceAccessor(&OranLmReinforcementLearning::m_handoverCount),
                                                "ns3::TracedValueCallback::Uint32");

        return tid;
    }

    OranLmReinforcementLearning::OranLmReinforcementLearning()
        : m_algorithm("DQN"),
          m_onlineTraining(true),
          m_multiAgentMode(false),
          m_learningRate(0.001),
          m_epsilon(1.0),
          m_epsilonDecay(0.995),
          m_epsilonMin(0.01),
          m_gamma(0.99),
          m_tau(0.005),
          m_clipRatio(0.2),
          m_ppoEpochs(4),
          m_bufferSize(10000),
          m_batchSize(32),
          m_updateFrequency(4),
          m_targetUpdateFrequency(100),
          m_stepCount(0),
          m_device(torch::kCPU)
    {
        NS_LOG_FUNCTION(this);

        // Initialize random number generator
        m_uniformRv = CreateObject<UniformRandomVariable>();

        // Check if CUDA is available
        if (torch::cuda::is_available())
        {
            m_device = torch::kCUDA;
            NS_LOG_INFO("CUDA is available, using GPU acceleration");
        }
        else
        {
            NS_LOG_INFO("CUDA not available, using CPU");
        }

        InitializeAgent();
    }

    OranLmReinforcementLearning::~OranLmReinforcementLearning()
    {
        NS_LOG_FUNCTION(this);
    }

    void
    OranLmReinforcementLearning::DoDispose()
    {
        NS_LOG_FUNCTION(this);
        m_dqnNetwork.reset();
        m_targetNetwork.reset();
        m_ppoNetwork.reset();
        m_optimizer.reset();
        m_replayBuffer.clear();
        OranLm::DoDispose();
    }

    void
    OranLmReinforcementLearning::InitializeAgent()
    {
        NS_LOG_FUNCTION(this);

        const int64_t state_size = 10; // RLState has 10 features
        const int64_t action_size = 4; // 4 possible actions
        const int64_t hidden_size = 256;

        if (m_algorithm == "DQN")
        {
            m_dqnNetwork = std::make_shared<DQNNetwork>(state_size, action_size, hidden_size);
            m_targetNetwork = std::make_shared<DQNNetwork>(state_size, action_size, hidden_size);

            // Move networks to device
            m_dqnNetwork->to(m_device);
            m_targetNetwork->to(m_device);

            // Copy main network to target network
            m_targetNetwork->load_state_dict(m_dqnNetwork->named_parameters());

            // Initialize optimizer
            m_optimizer = std::make_unique<torch::optim::Adam>(m_dqnNetwork->parameters(),
                                                               torch::optim::AdamOptions(m_learningRate));
        }
        else if (m_algorithm == "PPO")
        {
            m_ppoNetwork = std::make_shared<PPONetwork>(state_size, action_size, hidden_size);
            m_ppoNetwork->to(m_device);

            m_optimizer = std::make_unique<torch::optim::Adam>(m_ppoNetwork->parameters(),
                                                               torch::optim::AdamOptions(m_learningRate));
        }

        NS_LOG_INFO("Initialized " << m_algorithm << " agent with state_size=" << state_size
                                   << ", action_size=" << action_size << ", hidden_size=" << hidden_size);
    }

    std::vector<Ptr<OranCommand>>
    OranLmReinforcementLearning::Logic(std::vector<Ptr<OranReport>> reports)
    {
        NS_LOG_FUNCTION(this);

        std::vector<Ptr<OranCommand>> commands;

        if (reports.empty())
        {
            return commands;
        }

        // Group reports by node ID
        std::map<uint64_t, std::vector<Ptr<OranReport>>> nodeReports;
        for (auto report : reports)
        {
            uint64_t nodeId = report->GetAttribute("NodeId")->GetValue();
            nodeReports[nodeId].push_back(report);
        }

        // Process each node
        for (auto &[nodeId, nodeReportList] : nodeReports)
        {
            // Extract current state
            RLState currentState = ExtractState(nodeReportList, nodeId);

            // Select action using current policy
            RLAction action = SelectAction(currentState);

            // Store previous state for experience replay (if exists)
            if (m_agentStates.find(nodeId) != m_agentStates.end())
            {
                RLState prevState = m_agentStates[nodeId];
                double reward = CalculateReward(prevState, action, currentState);

                // Store experience
                if (m_onlineTraining)
                {
                    Experience exp = {prevState, action, reward, currentState, false};
                    StoreExperience(exp);
                }

                // Update reward tracking
                m_agentRewards[nodeId] = reward;
                double avgReward = 0.0;
                for (auto &[id, r] : m_agentRewards)
                {
                    avgReward += r;
                }
                avgReward /= m_agentRewards.size();
                m_averageReward = avgReward;
            }

            // Update agent state
            m_agentStates[nodeId] = currentState;

            // Generate command based on action
            if (action != RLAction::NO_HANDOVER)
            {
                // Find best target cell based on action type
                uint16_t targetCellId = 0;

                for (auto report : nodeReportList)
                {
                    if (DynamicCast<OranReportLteUeCellInfo>(report))
                    {
                        auto cellReport = DynamicCast<OranReportLteUeCellInfo>(report);
                        // Action-specific target selection logic
                        switch (action)
                        {
                        case RLAction::HANDOVER_TO_BEST_RSRP:
                            // Select cell with best RSRP
                            targetCellId = cellReport->GetAttribute("BestCellId")->GetValue();
                            break;
                        case RLAction::HANDOVER_TO_LEAST_LOADED:
                            // Select least loaded cell (simplified)
                            targetCellId = cellReport->GetAttribute("SecondBestCellId")->GetValue();
                            break;
                        case RLAction::HANDOVER_TO_PREDICTED_BEST:
                            // Use ML prediction for best cell
                            targetCellId = cellReport->GetAttribute("PredictedBestCellId")->GetValue();
                            break;
                        default:
                            continue;
                        }

                        if (targetCellId > 0)
                        {
                            Ptr<OranCommandLte2LteHandover> command = CreateObject<OranCommandLte2LteHandover>();
                            command->SetAttribute("NodeId", UintegerValue(nodeId));
                            command->SetAttribute("TargetCellId", UintegerValue(targetCellId));
                            commands.push_back(command);

                            m_handoverCount++;
                            NS_LOG_INFO("RL Agent decided handover for node " << nodeId
                                                                              << " to cell " << targetCellId << " using action " << (int)action);
                        }
                    }
                }
            }
        }

        // Update step count and perform training
        m_stepCount++;
        if (m_onlineTraining && m_stepCount % m_updateFrequency == 0)
        {
            TrainNetwork();
        }

        // Update epsilon
        if (m_epsilon > m_epsilonMin)
        {
            m_epsilon *= m_epsilonDecay;
            m_epsilon_trace = m_epsilon;
        }

        return commands;
    }

    RLState
    OranLmReinforcementLearning::ExtractState(const std::vector<Ptr<OranReport>> &reports, uint64_t nodeId)
    {
        RLState state = {};

        for (auto report : reports)
        {
            if (auto cellReport = DynamicCast<OranReportLteUeCellInfo>(report))
            {
                state.rsrp_serving = cellReport->GetAttribute("ServingRsrp")->GetValue();
                state.rsrp_target = cellReport->GetAttribute("BestNeighborRsrp")->GetValue();
                state.load_serving = cellReport->GetAttribute("ServingLoad")->GetValue();
                state.load_target = cellReport->GetAttribute("NeighborLoad")->GetValue();
                state.interference = cellReport->GetAttribute("Interference")->GetValue();
            }

            if (auto locationReport = DynamicCast<OranReportLocation>(report))
            {
                // Calculate distances and velocity based on position
                Vector3D position = locationReport->GetPosition();
                // Simplified distance calculation (would need cell positions in real implementation)
                state.distance_serving = 100.0; // Placeholder
                state.distance_target = 150.0;  // Placeholder
                state.velocity = locationReport->GetVelocity().GetLength();
            }
        }

        // Additional state features
        state.handover_history = 0.1; // Recent handover frequency (simplified)
        state.battery_level = 0.8;    // Battery level (simplified)

        return state;
    }

    double
    OranLmReinforcementLearning::CalculateReward(const RLState &state, RLAction action, const RLState &nextState)
    {
        double reward = 0.0;

        // Reward components
        double rsrp_improvement = nextState.rsrp_serving - state.rsrp_serving;
        double load_improvement = state.load_serving - nextState.load_serving;
        double handover_penalty = (action != RLAction::NO_HANDOVER) ? -1.0 : 0.0;
        double interference_penalty = -nextState.interference * 0.1;
        double energy_consideration = -state.velocity * 0.01; // Penalize high mobility

        reward = rsrp_improvement * 10.0 + // RSRP improvement bonus
                 load_improvement * 5.0 +  // Load balancing bonus
                 handover_penalty +        // Handover cost
                 interference_penalty +    // Interference penalty
                 energy_consideration;     // Energy efficiency

        return reward;
    }

    RLAction
    OranLmReinforcementLearning::SelectAction(const RLState &state)
    {
        if (m_algorithm == "DQN")
        {
            // Epsilon-greedy action selection
            if (m_uniformRv->GetValue() < m_epsilon)
            {
                // Random action
                return static_cast<RLAction>(m_uniformRv->GetInteger(0, 3));
            }
            else
            {
                // Greedy action
                torch::Tensor stateTensor = torch::tensor({state.rsrp_serving, state.rsrp_target,
                                                           state.distance_serving, state.distance_target,
                                                           state.velocity, state.load_serving,
                                                           state.load_target, state.handover_history,
                                                           state.interference, state.battery_level})
                                                .unsqueeze(0)
                                                .to(m_device);

                torch::NoGradGuard no_grad;
                auto qValues = m_dqnNetwork->forward(stateTensor);
                auto action = qValues.argmax(-1).item<int>();
                return static_cast<RLAction>(action);
            }
        }
        else if (m_algorithm == "PPO")
        {
            // Sample action from policy
            torch::Tensor stateTensor = torch::tensor({state.rsrp_serving, state.rsrp_target,
                                                       state.distance_serving, state.distance_target,
                                                       state.velocity, state.load_serving,
                                                       state.load_target, state.handover_history,
                                                       state.interference, state.battery_level})
                                            .unsqueeze(0)
                                            .to(m_device);

            torch::NoGradGuard no_grad;
            auto actionProbs = m_ppoNetwork->get_action_probs(stateTensor);
            auto actionDist = torch::distributions::Categorical(actionProbs);
            auto action = actionDist.sample().item<int>();
            return static_cast<RLAction>(action);
        }

        return RLAction::NO_HANDOVER;
    }

    void
    OranLmReinforcementLearning::StoreExperience(const Experience &exp)
    {
        m_replayBuffer.push_back(exp);
        if (m_replayBuffer.size() > m_bufferSize)
        {
            m_replayBuffer.pop_front();
        }
    }

    void
    OranLmReinforcementLearning::TrainNetwork()
    {
        if (m_replayBuffer.size() < m_batchSize)
        {
            return;
        }

        if (m_algorithm == "DQN")
        {
            DQNTrainingStep();
        }
        else if (m_algorithm == "PPO")
        {
            PPOTrainingStep();
        }
    }

    void
    OranLmReinforcementLearning::DQNTrainingStep()
    {
        // Sample random batch from replay buffer
        std::vector<Experience> batch;
        for (uint32_t i = 0; i < m_batchSize; ++i)
        {
            uint32_t idx = m_uniformRv->GetInteger(0, m_replayBuffer.size() - 1);
            batch.push_back(m_replayBuffer[idx]);
        }

        // Prepare tensors
        std::vector<std::vector<float>> states, nextStates;
        std::vector<int> actions;
        std::vector<float> rewards;
        std::vector<bool> dones;

        for (const auto &exp : batch)
        {
            states.push_back({exp.state.rsrp_serving, exp.state.rsrp_target,
                              exp.state.distance_serving, exp.state.distance_target,
                              exp.state.velocity, exp.state.load_serving,
                              exp.state.load_target, exp.state.handover_history,
                              exp.state.interference, exp.state.battery_level});

            nextStates.push_back({exp.next_state.rsrp_serving, exp.next_state.rsrp_target,
                                  exp.next_state.distance_serving, exp.next_state.distance_target,
                                  exp.next_state.velocity, exp.next_state.load_serving,
                                  exp.next_state.load_target, exp.next_state.handover_history,
                                  exp.next_state.interference, exp.next_state.battery_level});

            actions.push_back(static_cast<int>(exp.action));
            rewards.push_back(exp.reward);
            dones.push_back(exp.done);
        }

        auto stateBatch = torch::tensor(states).to(m_device);
        auto nextStateBatch = torch::tensor(nextStates).to(m_device);
        auto actionBatch = torch::tensor(actions).to(m_device);
        auto rewardBatch = torch::tensor(rewards).to(m_device);
        auto doneBatch = torch::tensor(dones).to(m_device);

        // Compute Q-values
        auto currentQValues = m_dqnNetwork->forward(stateBatch).gather(1, actionBatch.unsqueeze(1));

        // Compute target Q-values
        torch::NoGradGuard no_grad;
        auto nextQValues = m_targetNetwork->forward(nextStateBatch).max(1).get<0>();
        auto targetQValues = rewardBatch + (m_gamma * nextQValues * (1 - doneBatch.to(torch::kFloat)));

        // Compute loss
        auto loss = torch::mse_loss(currentQValues.squeeze(), targetQValues);

        // Perform optimization step
        m_optimizer->zero_grad();
        loss.backward();
        m_optimizer->step();

        // Update target network
        if (m_stepCount % m_targetUpdateFrequency == 0)
        {
            UpdateTargetNetwork();
        }

        NS_LOG_DEBUG("DQN training step completed, loss: " << loss.item<double>());
    }

    void
    OranLmReinforcementLearning::PPOTrainingStep()
    {
        // PPO training implementation (simplified)
        // In a full implementation, this would include advantage calculation,
        // policy ratio computation, and clipped surrogate objective

        NS_LOG_DEBUG("PPO training step completed");
    }

    void
    OranLmReinforcementLearning::UpdateTargetNetwork()
    {
        // Soft update of target network
        auto mainParams = m_dqnNetwork->named_parameters();
        auto targetParams = m_targetNetwork->named_parameters();

        for (auto &pair : mainParams)
        {
            const std::string &name = pair.key();
            auto &mainParam = pair.value();
            auto &targetParam = targetParams[name];

            targetParam.data() = m_tau * mainParam.data() + (1.0 - m_tau) * targetParam.data();
        }

        NS_LOG_DEBUG("Target network updated");
    }

    void
    OranLmReinforcementLearning::SetAlgorithm(const std::string &algorithm)
    {
        m_algorithm = algorithm;
        InitializeAgent();
    }

    void
    OranLmReinforcementLearning::SetOnlineTraining(bool enable)
    {
        m_onlineTraining = enable;
    }

    void
    OranLmReinforcementLearning::LoadPretrainedModel(const std::string &modelPath)
    {
        try
        {
            if (m_algorithm == "DQN" && m_dqnNetwork)
            {
                torch::load(m_dqnNetwork, modelPath);
                NS_LOG_INFO("Loaded pretrained DQN model from " << modelPath);
            }
            else if (m_algorithm == "PPO" && m_ppoNetwork)
            {
                torch::load(m_ppoNetwork, modelPath);
                NS_LOG_INFO("Loaded pretrained PPO model from " << modelPath);
            }
        }
        catch (const std::exception &e)
        {
            NS_LOG_ERROR("Failed to load model: " << e.what());
        }
    }

    void
    OranLmReinforcementLearning::SaveModel(const std::string &modelPath)
    {
        try
        {
            if (m_algorithm == "DQN" && m_dqnNetwork)
            {
                torch::save(m_dqnNetwork, modelPath);
                NS_LOG_INFO("Saved DQN model to " << modelPath);
            }
            else if (m_algorithm == "PPO" && m_ppoNetwork)
            {
                torch::save(m_ppoNetwork, modelPath);
                NS_LOG_INFO("Saved PPO model to " << modelPath);
            }
        }
        catch (const std::exception &e)
        {
            NS_LOG_ERROR("Failed to save model: " << e.what());
        }
    }

    void
    OranLmReinforcementLearning::SetMultiAgentMode(bool enable)
    {
        m_multiAgentMode = enable;
    }

    std::vector<RLAction>
    OranLmReinforcementLearning::CoordinateMultiAgent(const std::vector<RLState> &states)
    {
        // Multi-agent coordination implementation
        // This could include consensus algorithms, communication between agents,
        // or centralized coordination mechanisms

        std::vector<RLAction> actions;
        for (const auto &state : states)
        {
            actions.push_back(SelectAction(state));
        }

        return actions;
    }

} // namespace ns3
