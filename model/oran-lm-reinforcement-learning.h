/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * NIST-developed software is provided by NIST as a public service.
 * Advanced Reinforcement Learning Logic Module for O-RAN
 *
 * This module implements state-of-the-art Deep Q-Network (DQN) and
 * Proximal Policy Optimization (PPO) algorithms for intelligent
 * handover decisions in O-RAN architecture.
 */

#ifndef ORAN_LM_REINFORCEMENT_LEARNING_H
#define ORAN_LM_REINFORCEMENT_LEARNING_H

#include "oran-lm.h"
#include "ns3/traced-value.h"
#include "ns3/random-variable-stream.h"
#include <torch/torch.h>
#include <memory>
#include <deque>

namespace ns3
{

    /**
     * State representation for RL agent
     */
    struct RLState
    {
        double rsrp_serving;     // Current serving cell RSRP
        double rsrp_target;      // Target cell RSRP
        double distance_serving; // Distance to serving cell
        double distance_target;  // Distance to target cell
        double velocity;         // UE velocity
        double load_serving;     // Serving cell load
        double load_target;      // Target cell load
        double handover_history; // Recent handover frequency
        double interference;     // Interference level
        double battery_level;    // UE battery level (for energy optimization)
    };

    /**
     * Action space for RL agent
     */
    enum class RLAction
    {
        NO_HANDOVER = 0,
        HANDOVER_TO_BEST_RSRP = 1,
        HANDOVER_TO_LEAST_LOADED = 2,
        HANDOVER_TO_PREDICTED_BEST = 3
    };

    /**
     * Experience tuple for experience replay
     */
    struct Experience
    {
        RLState state;
        RLAction action;
        double reward;
        RLState next_state;
        bool done;
    };

    /**
     * Deep Q-Network implementation
     */
    class DQNNetwork : public torch::nn::Module
    {
    public:
        DQNNetwork(int64_t state_size, int64_t action_size, int64_t hidden_size = 256);
        torch::Tensor forward(torch::Tensor x);

    private:
        torch::nn::Linear fc1{nullptr};
        torch::nn::Linear fc2{nullptr};
        torch::nn::Linear fc3{nullptr};
        torch::nn::Linear fc4{nullptr};
        torch::nn::Dropout dropout{nullptr};
    };

    /**
     * Proximal Policy Optimization (PPO) Actor-Critic Network
     */
    class PPONetwork : public torch::nn::Module
    {
    public:
        PPONetwork(int64_t state_size, int64_t action_size, int64_t hidden_size = 256);
        std::pair<torch::Tensor, torch::Tensor> forward(torch::Tensor x);
        torch::Tensor get_action_probs(torch::Tensor x);
        torch::Tensor get_value(torch::Tensor x);

    private:
        // Shared layers
        torch::nn::Linear shared_fc1{nullptr};
        torch::nn::Linear shared_fc2{nullptr};

        // Actor head
        torch::nn::Linear actor_fc{nullptr};

        // Critic head
        torch::nn::Linear critic_fc{nullptr};

        torch::nn::Dropout dropout{nullptr};
    };

    /**
     * Advanced Reinforcement Learning Logic Module
     *
     * This class implements cutting-edge RL algorithms for intelligent
     * handover decisions, including:
     * - Deep Q-Networks (DQN) with experience replay
     * - Proximal Policy Optimization (PPO)
     * - Multi-agent coordination
     * - Transfer learning capabilities
     * - Real-time adaptation
     */
    class OranLmReinforcementLearning : public OranLm
    {
    public:
        /**
         * Get the TypeId of the OranLmReinforcementLearning class.
         *
         * \\return The TypeId.
         */
        static TypeId GetTypeId();

        /**
         * Constructor of the OranLmReinforcementLearning class.
         */
        OranLmReinforcementLearning();

        /**
         * Destructor of the OranLmReinforcementLearning class.
         */
        ~OranLmReinforcementLearning() override;

        /**
         * Logic Module function override.
         * Implements RL-based decision making for handover optimization.
         *
         * \\param reports The input reports to run the logic on.
         * \\return The output commands.
         */
        std::vector<Ptr<OranCommand>> Logic(std::vector<Ptr<OranReport>> reports) override;

        /**
         * Set the RL algorithm type
         * \\param algorithm "DQN" or "PPO"
         */
        void SetAlgorithm(const std::string &algorithm);

        /**
         * Enable/disable online training
         * \\param enable True to enable online training
         */
        void SetOnlineTraining(bool enable);

        /**
         * Load pre-trained model
         * \\param modelPath Path to the pre-trained model
         */
        void LoadPretrainedModel(const std::string &modelPath);

        /**
         * Save current model
         * \\param modelPath Path to save the model
         */
        void SaveModel(const std::string &modelPath);

        /**
         * Set multi-agent coordination mode
         * \\param enable True to enable multi-agent coordination
         */
        void SetMultiAgentMode(bool enable);

    protected:
        void DoDispose() override;

    private:
        /**
         * Initialize the RL agent
         */
        void InitializeAgent();

        /**
         * Convert reports to RL state representation
         */
        RLState ExtractState(const std::vector<Ptr<OranReport>> &reports, uint64_t nodeId);

        /**
         * Calculate reward based on current state and action
         */
        double CalculateReward(const RLState &state, RLAction action, const RLState &nextState);

        /**
         * Select action using current policy (epsilon-greedy for DQN, sampling for PPO)
         */
        RLAction SelectAction(const RLState &state);

        /**
         * Train the neural network
         */
        void TrainNetwork();

        /**
         * Update target network (for DQN)
         */
        void UpdateTargetNetwork();

        /**
         * Store experience for replay
         */
        void StoreExperience(const Experience &exp);

        /**
         * Perform PPO training step
         */
        void PPOTrainingStep();

        /**
         * Perform DQN training step
         */
        void DQNTrainingStep();

        /**
         * Multi-agent coordination logic
         */
        std::vector<RLAction> CoordinateMultiAgent(const std::vector<RLState> &states);

        // Network parameters
        std::string m_algorithm; ///< RL algorithm type
        bool m_onlineTraining;   ///< Enable online training
        bool m_multiAgentMode;   ///< Multi-agent coordination

        // Network architecture
        std::shared_ptr<DQNNetwork> m_dqnNetwork;    ///< DQN main network
        std::shared_ptr<DQNNetwork> m_targetNetwork; ///< DQN target network
        std::shared_ptr<PPONetwork> m_ppoNetwork;    ///< PPO network

        // Optimizers
        std::unique_ptr<torch::optim::Adam> m_optimizer;

        // Training parameters
        double m_learningRate; ///< Learning rate
        double m_epsilon;      ///< Exploration rate
        double m_epsilonDecay; ///< Epsilon decay rate
        double m_epsilonMin;   ///< Minimum epsilon
        double m_gamma;        ///< Discount factor
        double m_tau;          ///< Soft update parameter

        // PPO specific parameters
        double m_clipRatio; ///< PPO clip ratio
        int m_ppoEpochs;    ///< PPO training epochs

        // Experience replay
        std::deque<Experience> m_replayBuffer;
        uint32_t m_bufferSize; ///< Replay buffer size
        uint32_t m_batchSize;  ///< Training batch size

        // Training state
        uint32_t m_updateFrequency;       ///< Network update frequency
        uint32_t m_targetUpdateFrequency; ///< Target network update frequency
        uint32_t m_stepCount;             ///< Step counter

        // Performance tracking
        TracedValue<double> m_averageReward;   ///< Average reward tracker
        TracedValue<double> m_epsilon_trace;   ///< Epsilon value tracker
        TracedValue<uint32_t> m_handoverCount; ///< Handover count tracker

        // Multi-agent coordination
        std::map<uint64_t, RLState> m_agentStates; ///< Agent states
        std::map<uint64_t, double> m_agentRewards; ///< Agent rewards

        // Device and computation
        torch::Device m_device; ///< Computation device (CPU/CUDA)

        // Random number generation
        Ptr<UniformRandomVariable> m_uniformRv; ///< Uniform random variable
    };

} // namespace ns3

#endif /* ORAN_LM_REINFORCEMENT_LEARNING_H */
