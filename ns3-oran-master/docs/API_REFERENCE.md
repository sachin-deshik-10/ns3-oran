# O-RAN Module API Reference

## Table of Contents

- [Core Classes](#core-classes)
- [Helper Classes](#helper-classes)
- [Data Model Classes](#data-model-classes)
- [Logic Module Classes](#logic-module-classes)
- [Reporter Classes](#reporter-classes)
- [Command Classes](#command-classes)
- [Trigger Classes](#trigger-classes)
- [Utility Classes](#utility-classes)
- [Constants and Enums](#constants-and-enums)

## Core Classes

### OranNearRtRic

The Near-Real Time RAN Intelligent Controller implementation.

```cpp
class OranNearRtRic : public Object
```

#### Public Methods

##### Lifecycle Management

```cpp
void Activate();
```

Activates the Near-RT RIC and all its components.

**Preconditions:** All required components must be configured

**Postconditions:** RIC becomes active and ready for operation

```cpp
void Deactivate();
```

Deactivates the Near-RT RIC and stops all operations.

```cpp
void Start();
```

Starts periodic query operations for logic modules.

```cpp
void Stop();
```

Stops periodic query operations.

##### Component Access

```cpp
Ptr<OranDataRepository> Data() const;
```

**Returns:** Pointer to the data repository

**Usage:**

```cpp
Ptr<OranNearRtRic> ric = oranHelper->CreateNearRtRic();
Ptr<OranDataRepository> repo = ric->Data();
repo->StoreReport(report);
```

```cpp
Ptr<OranLm> GetDefaultLogicModule() const;
```

**Returns:** Pointer to the default logic module

```cpp
void SetDefaultLogicModule(Ptr<OranLm> newDefaultLm);
```

**Parameters:**

- `newDefaultLm`: New default logic module to set

**Preconditions:** `newDefaultLm` must not be null

##### Logic Module Management

```cpp
Ptr<OranLm> GetAdditionalLogicModule(std::string name) const;
```

**Parameters:**

- `name`: Name of the additional logic module

**Returns:** Pointer to the logic module or null if not found

```cpp
void AddAdditionalLogicModule(std::string name, Ptr<OranLm> lm);
```

**Parameters:**

- `name`: Unique name for the logic module
- `lm`: Logic module to add

**Throws:** Exception if name already exists

```cpp
void RemoveAdditionalLogicModule(std::string name);
```

**Parameters:**

- `name`: Name of logic module to remove

**Returns:** True if module was removed, false if not found

#### Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `LmQueryInterval` | Time | Seconds(5.0) | Interval between logic module queries |
| `LmQueryMaxWaitTime` | Time | Seconds(1.0) | Maximum wait time for logic module response |
| `E2NodeInactivityThreshold` | Time | Seconds(10.0) | Threshold for considering E2 node inactive |
| `LmQueryLateCommandPolicy` | String | "DROP" | Policy for late commands: "DROP" or "EXECUTE" |

#### Traces

| Trace | Signature | Description |
|-------|-----------|-------------|
| `QueryStart` | `void (Time)` | Fired when query cycle starts |
| `QueryComplete` | `void (Time, uint32_t)` | Fired when query cycle completes |
| `CommandGenerated` | `void (Ptr<OranCommand>)` | Fired when command is generated |

### OranDataRepository

Abstract base class for data storage operations.

```cpp
class OranDataRepository : public Object
```

#### Pure Virtual Methods

```cpp
virtual uint64_t RegisterE2Node(
    Ptr<OranE2NodeTerminator> terminator,
    std::string nodeType = "",
    Vector position = Vector(0, 0, 0)) = 0;
```

Registers an E2 node with the repository.

**Parameters:**

- `terminator`: E2 node terminator instance
- `nodeType`: Type of the node (optional)
- `position`: Initial position of the node

**Returns:** Unique node ID assigned to the registered node

```cpp
virtual void DeregisterE2Node(uint64_t nodeId) = 0;
```

Removes an E2 node from the repository.

**Parameters:**

- `nodeId`: ID of the node to remove

```cpp
virtual void StoreReport(Ptr<OranReport> report) = 0;
```

Stores a report in the repository.

**Parameters:**

- `report`: Report instance to store

**Preconditions:** Report must have valid node ID and timestamp

```cpp
virtual std::vector<Ptr<OranReport>> GetReports(
    uint64_t nodeId = 0,
    Time startTime = Time::Min(),
    Time endTime = Time::Max(),
    std::string reportType = "") = 0;
```

Retrieves reports based on filters.

**Parameters:**

- `nodeId`: Filter by node ID (0 for all nodes)
- `startTime`: Start time for report filtering
- `endTime`: End time for report filtering
- `reportType`: Filter by report type (empty for all types)

**Returns:** Vector of matching reports

```cpp
virtual void StoreCommand(Ptr<OranCommand> command) = 0;
```

Stores a command in the repository.

```cpp
virtual std::vector<Ptr<OranCommand>> GetCommands(
    uint64_t nodeId = 0,
    Time startTime = Time::Min(),
    Time endTime = Time::Max()) = 0;
```

Retrieves commands based on filters.

#### Utility Methods

```cpp
virtual void LogMessage(
    std::string component,
    std::string message,
    std::string level = "INFO") = 0;
```

Logs a message to the repository.

**Parameters:**

- `component`: Component name generating the log
- `message`: Log message content
- `level`: Log level (DEBUG, INFO, WARN, ERROR)

### OranDataRepositorySqlite

SQLite implementation of the data repository.

```cpp
class OranDataRepositorySqlite : public OranDataRepository
```

#### Additional Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `DatabaseFile` | String | "oran_db.sqlite" | Path to SQLite database file |
| `PragmaJournalMode` | String | "DELETE" | SQLite journal mode |
| `PragmaSynchronous` | String | "FULL" | SQLite synchronous mode |
| `PragmaCacheSize` | Integer | 2000 | SQLite cache size in pages |

#### Methods

```cpp
void InitializeDatabase();
```

Creates database tables if they don't exist.

```cpp
sqlite3* GetDatabaseConnection();
```

**Returns:** Raw SQLite connection handle

**Warning:** Use with caution, prefer high-level methods

```cpp
std::vector<std::map<std::string, std::string>> 
ExecuteQuery(std::string sql);
```

Executes arbitrary SQL query.

**Parameters:**

- `sql`: SQL query string

**Returns:** Result set as vector of row maps

**Usage:**

```cpp
auto results = repo->ExecuteQuery(
    "SELECT node_id, COUNT(*) as report_count FROM reports GROUP BY node_id");
for (const auto& row : results) {
    uint64_t nodeId = std::stoull(row.at("node_id"));
    uint32_t count = std::stoul(row.at("report_count"));
    std::cout << "Node " << nodeId << ": " << count << " reports" << std::endl;
}
```

## Helper Classes

### OranHelper

Central configuration and deployment helper.

```cpp
class OranHelper : public Object
```

#### Configuration Methods

```cpp
void SetDataRepository(std::string type, ...);
```

Configures the data repository type and attributes.

**Parameters:**

- `type`: Repository type (e.g., "ns3::OranDataRepositorySqlite")
- `...`: Variable attribute pairs (name, value)

**Usage:**

```cpp
oranHelper->SetDataRepository("ns3::OranDataRepositorySqlite",
                              "DatabaseFile", StringValue("sim.db"),
                              "PragmaJournalMode", StringValue("WAL"));
```

```cpp
void SetDefaultLogicModule(std::string type, ...);
```

Configures the default logic module.

```cpp
void SetConflictMitigationModule(std::string type, ...);
```

Configures the conflict mitigation module.

```cpp
void SetE2NodeTerminator(std::string type, ...);
```

Configures E2 node terminator for subsequent deployments.

```cpp
void AddReporter(std::string type, ...);
```

Adds a reporter configuration for subsequent deployments.

```cpp
void AddLogicModule(std::string name, std::string type, ...);
```

Adds an additional logic module configuration.

```cpp
void AddQueryTrigger(std::string name, std::string type, ...);
```

Adds a query trigger configuration.

#### Deployment Methods

```cpp
Ptr<OranNearRtRic> CreateNearRtRic();
```

Creates and configures a Near-RT RIC instance.

**Returns:** Configured Near-RT RIC instance

**Preconditions:** Data repository and default logic module must be configured

```cpp
OranE2NodeTerminatorContainer DeployTerminators(
    Ptr<OranNearRtRic> ric,
    NodeContainer nodes,
    uint32_t netDeviceIndex = 0);
```

Deploys E2 node terminators on specified nodes.

**Parameters:**

- `ric`: Target RIC for registration
- `nodes`: Nodes to deploy terminators on
- `netDeviceIndex`: Network device index for LTE nodes

**Returns:** Container of deployed terminators

#### Lifecycle Methods

```cpp
void ActivateAndStartNearRtRic(Ptr<OranNearRtRic> ric);
```

Activates and starts RIC operations.

```cpp
void ActivateE2NodeTerminators(OranE2NodeTerminatorContainer terminators);
```

Activates all terminators in the container.

#### Validation Methods

```cpp
bool ValidateConfiguration() const;
```

**Returns:** True if current configuration is valid

```cpp
std::vector<std::string> GetConfigurationErrors() const;
```

**Returns:** List of configuration error messages

## Data Model Classes

### OranReport

Abstract base class for all report types.

```cpp
class OranReport : public Object
```

#### Core Methods

```cpp
uint64_t GetNodeId() const;
void SetNodeId(uint64_t nodeId);
```

Node identifier for the reporting node.

```cpp
Time GetTimestamp() const;
void SetTimestamp(Time timestamp);
```

Timestamp when the report was generated.

```cpp
virtual std::string GetReportType() const = 0;
```

**Returns:** String identifier for the report type

```cpp
virtual std::string Serialize() const;
virtual void Deserialize(const std::string& data);
```

Serialization methods for data storage.

### OranReportLocation

Location report implementation.

```cpp
class OranReportLocation : public OranReport
```

#### Methods

```cpp
Vector GetPosition() const;
void SetPosition(Vector position);
```

3D position of the reporting node.

```cpp
Vector GetVelocity() const;
void SetVelocity(Vector velocity);
```

Current velocity vector of the node.

**Usage:**

```cpp
Ptr<OranReportLocation> report = CreateObject<OranReportLocation>();
report->SetNodeId(nodeId);
report->SetTimestamp(Simulator::Now());
report->SetPosition(Vector(100.0, 50.0, 0.0));
report->SetVelocity(Vector(10.0, 0.0, 0.0));
```

### OranReportLteUeCellInfo

LTE UE cell attachment information.

```cpp
class OranReportLteUeCellInfo : public OranReport
```

#### Methods

```cpp
uint16_t GetCellId() const;
void SetCellId(uint16_t cellId);
```

ID of the cell the UE is attached to.

```cpp
double GetRsrp() const;
void SetRsrp(double rsrp);
```

Reference Signal Received Power in dBm.

```cpp
double GetRsrq() const;
void SetRsrq(double rsrq);
```

Reference Signal Received Quality in dB.

```cpp
uint16_t GetRnti() const;
void SetRnti(uint16_t rnti);
```

Radio Network Temporary Identifier.

### OranCommand

Abstract base class for all command types.

```cpp
class OranCommand : public Object
```

#### Core Methods

```cpp
uint64_t GetTargetNodeId() const;
void SetTargetNodeId(uint64_t nodeId);
```

Target node for command execution.

```cpp
Time GetTimestamp() const;
void SetTimestamp(Time timestamp);
```

Timestamp when command was generated.

```cpp
virtual bool Execute() = 0;
```

Executes the command on the target node.

**Returns:** True if execution was successful

```cpp
virtual std::string GetCommandType() const = 0;
```

**Returns:** String identifier for the command type

### OranCommandLte2LteHandover

LTE-to-LTE handover command implementation.

```cpp
class OranCommandLte2LteHandover : public OranCommand
```

#### Methods

```cpp
uint16_t GetTargetCellId() const;
void SetTargetCellId(uint16_t cellId);
```

Target cell ID for handover.

```cpp
uint16_t GetSourceCellId() const;
void SetSourceCellId(uint16_t cellId);
```

Source cell ID (current attachment).

```cpp
virtual bool Execute() override;
```

Triggers the LTE handover procedure.

**Implementation Details:**

- Validates target cell availability
- Initiates RRC connection reconfiguration
- Updates UE context at target eNB
- Reports handover completion

## Logic Module Classes

### OranLm

Abstract base class for logic modules.

```cpp
class OranLm : public Object
```

#### Core Methods

```cpp
virtual std::vector<Ptr<OranCommand>> Logic(
    std::vector<Ptr<OranReport>> reports) = 0;
```

Main logic processing method.

**Parameters:**

- `reports`: Vector of reports to process

**Returns:** Vector of commands to execute

**Performance Notes:**

- Method should complete within processing delay limit
- Avoid blocking operations
- Use GetDataRepository() for additional data access

```cpp
void SetRic(Ptr<OranNearRtRic> ric);
Ptr<OranNearRtRic> GetRic() const;
```

RIC association for data access.

```cpp
void SetProcessingDelay(Time delay);
Time GetProcessingDelay() const;
```

Simulated processing delay for the logic module.

#### Utility Methods

```cpp
Ptr<OranDataRepository> GetDataRepository() const;
```

**Returns:** Data repository for additional queries

```cpp
void LogMessage(std::string message, std::string level = "INFO");
```

Logs a message via the data repository.

**Parameters:**

- `message`: Message content
- `level`: Log level (DEBUG, INFO, WARN, ERROR)

```cpp
std::vector<Ptr<OranReport>> GetRecentReports(
    Time timeWindow,
    std::string reportType = "") const;
```

Retrieves recent reports within time window.

### OranLmNoop

No-operation logic module implementation.

```cpp
class OranLmNoop : public OranLm
```

#### Methods

```cpp
virtual std::vector<Ptr<OranCommand>> Logic(
    std::vector<Ptr<OranReport>> reports) override;
```

**Returns:** Empty command vector (no operations performed)

**Usage:** Testing and baseline scenarios

### OranLmLte2LteDistanceHandover

Distance-based handover logic module.

```cpp
class OranLmLte2LteDistanceHandover : public OranLm
```

#### Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `HandoverThreshold` | Double | 100.0 | Distance threshold in meters |
| `HysteresisMargin` | Double | 10.0 | Hysteresis margin to prevent ping-pong |
| `MinHandoverInterval` | Time | Seconds(5.0) | Minimum time between handovers |

#### Methods

```cpp
virtual std::vector<Ptr<OranCommand>> Logic(
    std::vector<Ptr<OranReport>> reports) override;
```

Implements distance-based handover algorithm:

1. Analyzes UE location reports
2. Calculates distances to serving and neighbor cells
3. Triggers handover if distance threshold exceeded
4. Applies hysteresis to prevent oscillation

**Algorithm:**

```cpp
if (distanceToServing > threshold + hysteresis && 
    distanceToNeighbor < distanceToServing - hysteresis &&
    timeSinceLastHandover > minInterval) {
    // Trigger handover to neighbor cell
}
```

### OranLmLte2LteTorchHandover

PyTorch-based machine learning handover logic.

```cpp
class OranLmLte2LteTorchHandover : public OranLm
```

#### Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `ModelPath` | String | "" | Path to PyTorch model file |
| `InputSize` | Integer | 8 | Input feature vector size |
| `OutputSize` | Integer | 2 | Output prediction vector size |
| `PredictionThreshold` | Double | 0.5 | Threshold for handover decision |

#### Methods

```cpp
void LoadModel(const std::string& modelPath);
```

Loads PyTorch model from file.

**Throws:** Exception if model loading fails

```cpp
torch::Tensor PreprocessReports(
    const std::vector<Ptr<OranReport>>& reports);
```

Converts reports to input tensor format.

**Feature Extraction:**

- UE position (x, y, z)
- UE velocity (vx, vy, vz)
- RSRP measurement
- RSRQ measurement
- Time since last handover

```cpp
virtual std::vector<Ptr<OranCommand>> Logic(
    std::vector<Ptr<OranReport>> reports) override;
```

Implements ML-based handover decisions:

1. Preprocesses reports into feature vectors
2. Runs inference using loaded model
3. Applies threshold to prediction output
4. Generates handover commands for positive predictions

## Reporter Classes

### OranReporter

Abstract base class for all reporters.

```cpp
class OranReporter : public Object
```

#### Core Methods

```cpp
void Activate();
void Deactivate();
bool IsActive() const;
```

Lifecycle management for the reporter.

```cpp
void SetTrigger(Ptr<OranReportTrigger> trigger);
Ptr<OranReportTrigger> GetTrigger() const;
```

Associates a trigger with the reporter.

```cpp
void SetE2Terminator(Ptr<OranE2NodeTerminator> terminator);
```

Associates the reporter with an E2 node terminator.

#### Protected Methods

```cpp
virtual Ptr<OranReport> GenerateReport() = 0;
```

Generates a report based on current node state.

**Implementation Notes:**

- Must be implemented by derived classes
- Should capture current node metrics
- Return null if no report should be generated

```cpp
void SendReport(Ptr<OranReport> report);
```

Sends generated report via the E2 terminator.

### OranReporterLocation

Location reporter implementation.

```cpp
class OranReporterLocation : public OranReporter
```

#### Methods

```cpp
virtual Ptr<OranReport> GenerateReport() override;
```

Generates location report from node's mobility model.

**Implementation:**

- Retrieves position from MobilityModel
- Calculates velocity if ConstantVelocityMobilityModel
- Creates OranReportLocation instance

### OranReporterLteUeCellInfo

LTE UE cell information reporter.

```cpp
class OranReporterLteUeCellInfo : public OranReporter
```

#### Methods

```cpp
virtual Ptr<OranReport> GenerateReport() override;
```

Generates cell attachment information.

**Data Sources:**

- LteUeNetDevice for cell ID and RNTI
- LteUePhy for RSRP/RSRQ measurements
- LteUeRrc for connection state

**Preconditions:** Node must have LteUeNetDevice installed

### OranReporterAppLoss

Application-level packet loss reporter.

```cpp
class OranReporterAppLoss : public OranReporter
```

#### Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `MonitoringInterval` | Time | Seconds(1.0) | Measurement window for loss calculation |
| `ApplicationPort` | Integer | 0 | Port to monitor (0 for all) |

#### Methods

```cpp
virtual Ptr<OranReport> GenerateReport() override;
```

Generates application loss metrics.

**Metrics Collected:**

- Packet loss rate
- Throughput
- Latency statistics
- Jitter measurements

## Trigger Classes

### OranReportTrigger

Abstract base class for report triggers.

```cpp
class OranReportTrigger : public Object
```

#### Core Methods

```cpp
virtual void Initialize(Ptr<OranReporter> reporter) = 0;
```

Initializes the trigger with associated reporter.

```cpp
virtual void Start() = 0;
virtual void Stop() = 0;
```

Lifecycle management for trigger operations.

### OranReportTriggerPeriodic

Periodic report trigger implementation.

```cpp
class OranReportTriggerPeriodic : public OranReportTrigger
```

#### Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `ReportingInterval` | Time | Seconds(5.0) | Interval between reports |
| `InitialDelay` | Time | Seconds(0.0) | Delay before first report |

#### Methods

```cpp
virtual void Initialize(Ptr<OranReporter> reporter) override;
```

Sets up periodic event scheduling.

```cpp
virtual void Start() override;
```

Begins periodic report generation.

### OranReportTriggerLteUeHandover

LTE UE handover event trigger.

```cpp
class OranReportTriggerLteUeHandover : public OranReportTrigger
```

#### Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `InitialReport` | Boolean | false | Generate report on initialization |

#### Methods

```cpp
virtual void Initialize(Ptr<OranReporter> reporter) override;
```

Connects to LTE handover traces.

**Trace Connections:**

- LteUeRrc::HandoverStart
- LteUeRrc::HandoverEndOk
- LteUeRrc::HandoverEndError

## Query Trigger Classes

### OranQueryTrigger

Abstract base class for query triggers.

```cpp
class OranQueryTrigger : public Object
```

#### Core Methods

```cpp
virtual void Initialize(Ptr<OranNearRtRic> ric) = 0;
```

Initializes trigger with RIC reference.

```cpp
virtual void Start() = 0;
virtual void Stop() = 0;
```

Lifecycle management.

### OranQueryTriggerPeriodic

Periodic query trigger for regular LM execution.

```cpp
class OranQueryTriggerPeriodic : public OranQueryTrigger
```

#### Attributes

| Attribute | Type | Default | Description |
|-----------|------|---------|-------------|
| `QueryInterval` | Time | Seconds(5.0) | Interval between queries |

### OranQueryTriggerCustom

Custom callback-based query trigger.

```cpp
class OranQueryTriggerCustom : public OranQueryTrigger
```

#### Attributes

| Attribute | Type | Description |
|-----------|------|-------------|
| `CustomCallback` | Callback | User-defined trigger condition |

#### Callback Signature

```cpp
typedef Callback<bool, Ptr<OranNearRtRic>, Time> CustomQueryCallback;
```

**Parameters:**

- `ric`: RIC instance
- `currentTime`: Current simulation time

**Returns:** True if query should be triggered

## Utility Classes

### OranE2NodeTerminatorContainer

Container for managing E2 node terminators.

```cpp
class OranE2NodeTerminatorContainer
```

#### Methods

```cpp
void Add(Ptr<OranE2NodeTerminator> terminator);
void Add(OranE2NodeTerminatorContainer other);
```

Adds terminators to the container.

```cpp
Iterator Begin();
Iterator End();
```

STL-style iteration support.

```cpp
uint32_t GetN() const;
Ptr<OranE2NodeTerminator> Get(uint32_t i) const;
```

Access methods for individual terminators.

```cpp
void ActivateAll();
void DeactivateAll();
```

Bulk lifecycle operations.

### OranConflictMitigationModule

Abstract base class for conflict mitigation.

```cpp
class OranCmm : public Object
```

#### Core Methods

```cpp
virtual std::vector<Ptr<OranCommand>> MitigateConflicts(
    std::vector<Ptr<OranCommand>> commands) = 0;
```

Resolves conflicts between commands.

**Parameters:**

- `commands`: Commands from all logic modules

**Returns:** Resolved command set

#### Built-in Implementations

##### OranCmmNoop

No conflict mitigation (passes all commands through).

##### OranCmmSingleCommandPerNode

Ensures only one command per node:

- Default LM commands take precedence
- First command wins for additional LMs

## Constants and Enums

### Report Types

```cpp
namespace OranReportTypes {
    const std::string LOCATION = "LocationReport";
    const std::string LTE_UE_CELL_INFO = "LteUeCellInfoReport";
    const std::string APP_LOSS = "AppLossReport";
    const std::string LTE_UE_RSRP_RSRQ = "LteUeRsrpRsrqReport";
}
```

### Command Types

```cpp
namespace OranCommandTypes {
    const std::string LTE_2_LTE_HANDOVER = "Lte2LteHandoverCommand";
    const std::string POWER_CONTROL = "PowerControlCommand";
    const std::string SCHEDULING = "SchedulingCommand";
}
```

### Log Levels

```cpp
enum class OranLogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};
```

### Processing Policies

```cpp
enum class LateCommandPolicy {
    DROP,    // Discard late commands
    EXECUTE  // Execute commands even if late
};
```

## Error Handling

### Exception Types

```cpp
class OranException : public std::exception {
public:
    OranException(const std::string& message);
    virtual const char* what() const noexcept override;
};

class OranConfigurationException : public OranException {
    // Configuration-specific errors
};

class OranDataException : public OranException {
    // Data repository errors
};

class OranLogicModuleException : public OranException {
    // Logic module processing errors
};
```

### Error Codes

```cpp
enum class OranErrorCode {
    SUCCESS = 0,
    INVALID_CONFIGURATION = 1,
    DATABASE_ERROR = 2,
    LOGIC_MODULE_TIMEOUT = 3,
    COMMAND_EXECUTION_FAILED = 4,
    INSUFFICIENT_RESOURCES = 5
};
```

## Performance Monitoring

### Performance Metrics

```cpp
class OranPerfMonitor : public Object {
public:
    struct Metrics {
        double avgProcessingTime;     // Average LM processing time
        double maxProcessingTime;     // Maximum LM processing time
        uint64_t totalReports;        // Total reports processed
        uint64_t totalCommands;       // Total commands generated
        double memoryUsageMB;         // Current memory usage
        double dbQueryTimeMs;         // Average database query time
    };
    
    Metrics GetMetrics() const;
    void ResetMetrics();
    void EnableDetailedProfiling(bool enable);
};
```

This comprehensive API reference provides detailed documentation for all public interfaces in the O-RAN module, enabling developers to effectively use and extend the framework.
