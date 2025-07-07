# O-RAN Architecture Documentation

## Table of Contents

- [Overview](#overview)
- [System Architecture](#system-architecture)
- [Component Design](#component-design)
- [Data Flow](#data-flow)
- [Class Hierarchy](#class-hierarchy)
- [Interface Specifications](#interface-specifications)
- [Deployment Patterns](#deployment-patterns)
- [Performance Considerations](#performance-considerations)

## Overview

The O-RAN module implements a comprehensive simulation framework for the Open Radio Access Network architecture within ns-3. The design prioritizes modularity, extensibility, and adherence to O-RAN specifications while providing efficient simulation capabilities.

## System Architecture

### High-Level Architecture

The O-RAN module follows a distributed architecture pattern with clear separation between control plane (Near-RT RIC) and data plane (E2 Nodes) components:

```mermaid
graph TB
    subgraph "Management Layer"
        OH[O-RAN Helper]
        CM[Configuration Manager]
    end
    
    subgraph "Near-RT RIC"
        subgraph "Control Components"
            LMC[Logic Module Controller]
            CMM[Conflict Mitigation Module]
            QTC[Query Trigger Controller]
        end
        
        subgraph "Data Components"
            DR[Data Repository]
            DM[Database Manager]
            Cache[Result Cache]
        end
        
        subgraph "Communication"
            E2T[E2 Terminator]
            CI[Command Interface]
            RI[Report Interface]
        end
    end
    
    subgraph "E2 Node Layer"
        subgraph "Node 1"
            ENT1[E2 Node Terminator]
            R1[Reporters]
            CP1[Command Processor]
        end
        
        subgraph "Node 2"
            ENT2[E2 Node Terminator]
            R2[Reporters]
            CP2[Command Processor]
        end
        
        subgraph "Node N"
            ENTN[E2 Node Terminator]
            RN[Reporters]
            CPN[Command Processor]
        end
    end
    
    OH --> LMC
    OH --> DR
    OH --> E2T
    
    LMC --> DR
    CMM --> DR
    E2T --> DR
    
    E2T <-->|E2 Interface| ENT1
    E2T <-->|E2 Interface| ENT2
    E2T <-->|E2 Interface| ENTN
    
    R1 --> ENT1
    R2 --> ENT2
    RN --> ENTN
    
    CI --> CP1
    CI --> CP2
    CI --> CPN
```

### Component Layering

The architecture is organized into distinct layers:

1. **Management Layer**: Configuration and lifecycle management
2. **RIC Layer**: Intelligent control and decision-making
3. **Communication Layer**: E2 interface implementation
4. **Node Layer**: Data collection and command execution

## Component Design

### Near-RT RIC Components

#### Data Repository

The Data Repository serves as the central data store for all RIC operations:

```mermaid
classDiagram
    class OranDataRepository {
        <<abstract>>
        +RegisterE2Node(terminator) uint64
        +DeregisterE2Node(nodeId) void
        +StoreReport(report) void
        +GetReports(filters) vector~Report~
        +StoreCommand(command) void
        +GetCommands(filters) vector~Command~
        +ExecuteQuery(sql) ResultSet
    }
    
    class OranDataRepositorySqlite {
        -m_databaseFile string
        -m_connection sqlite3*
        +InitializeDatabase() void
        +CreateTables() void
        +ExecuteQuery(sql) ResultSet
        +PrepareStatement(sql) Statement
    }
    
    OranDataRepository <|-- OranDataRepositorySqlite
```

**Key Features:**

- Backend-agnostic API design
- SQLite implementation with optimized queries
- Transaction support for data consistency
- Efficient indexing for time-series data
- Connection pooling for performance

#### Logic Module Framework

Logic Modules implement the decision-making algorithms:

```mermaid
classDiagram
    class OranLm {
        <<abstract>>
        #m_ric Ptr~OranNearRtRic~
        #m_processingDelay Time
        +Logic(reports) vector~Command~
        +SetRic(ric) void
        +SetProcessingDelay(delay) void
        #GetDataRepository() Ptr~DataRepository~
        #LogMessage(message) void
    }
    
    class OranLmNoop {
        +Logic(reports) vector~Command~
    }
    
    class OranLmLte2LteDistanceHandover {
        -m_handoverThreshold double
        +Logic(reports) vector~Command~
        -CalculateDistance(pos1, pos2) double
        -ShouldTriggerHandover(ueReport) bool
    }
    
    class OranLmLte2LteTorchHandover {
        -m_modelPath string
        -m_torchModule torch::jit::script::Module
        +Logic(reports) vector~Command~
        -LoadModel() void
        -PreprocessData(reports) torch::Tensor
        -InferHandover(input) torch::Tensor
    }
    
    OranLm <|-- OranLmNoop
    OranLm <|-- OranLmLte2LteDistanceHandover
    OranLm <|-- OranLmLte2LteTorchHandover
```

**Design Principles:**

- Template method pattern for common operations
- Pluggable architecture for custom algorithms
- Built-in support for ML frameworks
- Processing delay simulation
- Comprehensive logging

#### Conflict Mitigation

The Conflict Mitigation Module resolves conflicts between competing commands:

```mermaid
stateDiagram-v2
    [*] --> CollectCommands
    CollectCommands --> AnalyzeConflicts
    AnalyzeConflicts --> NoConflicts: No conflicts detected
    AnalyzeConflicts --> ResolveConflicts: Conflicts found
    
    ResolveConflicts --> ApplyPolicy
    ApplyPolicy --> DefaultLMPriority: Default LM priority
    ApplyPolicy --> FirstCommandPriority: First command priority
    ApplyPolicy --> CustomResolution: Custom logic
    
    DefaultLMPriority --> FinalizeCommands
    FirstCommandPriority --> FinalizeCommands
    CustomResolution --> FinalizeCommands
    NoConflicts --> FinalizeCommands
    
    FinalizeCommands --> [*]
```

### E2 Node Components

#### E2 Node Terminator

The E2 Node Terminator manages communication between nodes and the RIC:

```mermaid
sequenceDiagram
    participant Node as E2 Node
    participant ENT as E2 Node Terminator
    participant RIC as Near-RT RIC
    participant DR as Data Repository
    
    Note over Node,DR: Registration Phase
    Node->>ENT: Activate
    ENT->>RIC: Register
    RIC->>DR: Store Registration
    
    Note over Node,DR: Reporting Phase
    loop Periodic Reporting
        Node->>ENT: Generate Report
        ENT->>RIC: Send Report
        RIC->>DR: Store Report
    end
    
    Note over Node,DR: Command Phase
    RIC->>ENT: Send Command
    ENT->>Node: Execute Command
    Node->>ENT: Command Result
    ENT->>RIC: Report Result
    RIC->>DR: Store Result
    
    Note over Node,DR: Keep-Alive Phase
    loop Keep-Alive
        ENT->>RIC: Heartbeat
        RIC->>DR: Update Status
    end
```

#### Reporter Framework

Reporters collect and format node-specific metrics:

```mermaid
classDiagram
    class OranReporter {
        <<abstract>>
        #m_terminator Ptr~E2NodeTerminator~
        #m_trigger Ptr~ReportTrigger~
        #m_active bool
        +Activate() void
        +Deactivate() void
        +SetTrigger(trigger) void
        #GenerateReport() Ptr~Report~
    }
    
    class OranReporterLocation {
        +GenerateReport() Ptr~LocationReport~
        -GetNodePosition() Vector
    }
    
    class OranReporterLteUeCellInfo {
        +GenerateReport() Ptr~CellInfoReport~
        -GetAttachedCell() uint16
        -GetRsrp() double
        -GetRsrq() double
    }
    
    class OranReporterAppLoss {
        +GenerateReport() Ptr~AppLossReport~
        -CalculatePacketLoss() double
        -GetThroughput() double
    }
    
    OranReporter <|-- OranReporterLocation
    OranReporter <|-- OranReporterLteUeCellInfo
    OranReporter <|-- OranReporterAppLoss
```

## Data Flow

### Report Processing Flow

```mermaid
flowchart LR
    subgraph "E2 Node"
        NS[Node State] --> R[Reporter]
        T[Trigger] --> R
        R --> ENT[E2 Node Terminator]
    end
    
    subgraph "Near-RT RIC"
        ENT --> E2T[E2 Terminator]
        E2T --> DR[Data Repository]
        DR --> LM[Logic Module]
        LM --> CMM[Conflict Mitigation]
        CMM --> E2T
    end
    
    subgraph "Command Execution"
        E2T --> CMD[Command]
        CMD --> CP[Command Processor]
        CP --> NA[Node Action]
    end
```

### Query Processing Flow

```mermaid
flowchart TB
    Start([Start Query Cycle]) --> QT{Query Trigger}
    QT -->|Periodic| Timer[Timer Event]
    QT -->|Event-Based| Event[Report Event]
    QT -->|Custom| Custom[Custom Logic]
    
    Timer --> FD[Fetch Data]
    Event --> FD
    Custom --> FD
    
    FD --> DLM[Default Logic Module]
    FD --> ALM[Additional Logic Modules]
    
    DLM --> CMD1[Commands Set 1]
    ALM --> CMD2[Commands Set 2]
    
    CMD1 --> CMM[Conflict Mitigation]
    CMD2 --> CMM
    
    CMM --> FC[Final Commands]
    FC --> Exec[Execute Commands]
    Exec --> End([End Cycle])
```

## Class Hierarchy

### Core Framework Classes

```mermaid
classDiagram
    class Object {
        <<ns3::Object>>
    }
    
    class OranHelper {
        -m_verbose bool
        -m_ricLmQueryInterval Time
        +SetDataRepository(type, attributes) void
        +SetDefaultLogicModule(type, attributes) void
        +CreateNearRtRic() Ptr~OranNearRtRic~
        +DeployTerminators(ric, nodes) Container
    }
    
    class OranNearRtRic {
        -m_active bool
        -m_dataRepository Ptr~OranDataRepository~
        -m_defaultLm Ptr~OranLm~
        -m_additionalLms map~string,Ptr~OranLm~~
        +Activate() void
        +Start() void
        +QueryLogicModules() void
    }
    
    Object <|-- OranHelper
    Object <|-- OranNearRtRic
```

### Data Model Classes

```mermaid
classDiagram
    class OranReport {
        <<abstract>>
        #m_nodeId uint64
        #m_timestamp Time
        +GetNodeId() uint64
        +GetTimestamp() Time
    }
    
    class OranReportLocation {
        -m_position Vector
        +GetPosition() Vector
    }
    
    class OranReportLteUeCellInfo {
        -m_cellId uint16
        -m_rsrp double
        -m_rsrq double
        +GetCellId() uint16
        +GetRsrp() double
    }
    
    class OranCommand {
        <<abstract>>
        #m_targetNodeId uint64
        #m_timestamp Time
        +GetTargetNodeId() uint64
        +Execute() bool
    }
    
    class OranCommandLte2LteHandover {
        -m_targetCellId uint16
        +Execute() bool
        -TriggerHandover() void
    }
    
    OranReport <|-- OranReportLocation
    OranReport <|-- OranReportLteUeCellInfo
    OranCommand <|-- OranCommandLte2LteHandover
```

## Interface Specifications

### E2 Interface

The E2 interface defines the communication protocol between RIC and E2 nodes:

```mermaid
sequenceDiagram
    participant RIC as Near-RT RIC
    participant E2Node as E2 Node
    
    Note over RIC,E2Node: Connection Establishment
    E2Node->>RIC: E2 Setup Request
    RIC->>E2Node: E2 Setup Response
    
    Note over RIC,E2Node: Subscription Management
    RIC->>E2Node: RIC Subscription Request
    E2Node->>RIC: RIC Subscription Response
    
    Note over RIC,E2Node: Indication Reporting
    loop Periodic Reports
        E2Node->>RIC: RIC Indication
    end
    
    Note over RIC,E2Node: Control Messages
    RIC->>E2Node: RIC Control Request
    E2Node->>RIC: RIC Control Acknowledge
    
    Note over RIC,E2Node: Service Updates
    RIC->>E2Node: RIC Service Update
    E2Node->>RIC: RIC Service Update Acknowledge
```

### Data Repository Interface

```cpp
// Core data operations
class OranDataRepository {
public:
    // Node lifecycle
    virtual uint64_t RegisterE2Node(
        Ptr<OranE2NodeTerminator> terminator,
        std::string nodeType,
        Vector position) = 0;
    
    virtual void DeregisterE2Node(uint64_t nodeId) = 0;
    
    virtual void UpdateE2NodeRegistration(
        uint64_t nodeId,
        Time timestamp) = 0;
    
    // Report management
    virtual void StoreReport(Ptr<OranReport> report) = 0;
    
    virtual std::vector<Ptr<OranReport>> GetReports(
        uint64_t nodeId = 0,
        Time startTime = Time::Min(),
        Time endTime = Time::Max(),
        std::string reportType = "") = 0;
    
    // Command management
    virtual void StoreCommand(Ptr<OranCommand> command) = 0;
    
    virtual std::vector<Ptr<OranCommand>> GetCommands(
        uint64_t nodeId = 0,
        Time startTime = Time::Min(),
        Time endTime = Time::Max()) = 0;
    
    // Query interface
    virtual std::vector<std::map<std::string, std::string>> 
        ExecuteQuery(std::string sql) = 0;
};
```

## Deployment Patterns

### Single RIC Deployment

```mermaid
graph TB
    subgraph "Single RIC Architecture"
        RIC[Near-RT RIC]
        
        subgraph "E2 Nodes"
            UE1[UE Node 1]
            UE2[UE Node 2]
            ENB1[eNB Node 1]
            ENB2[eNB Node 2]
        end
    end
    
    RIC --> UE1
    RIC --> UE2
    RIC --> ENB1
    RIC --> ENB2
```

**Use Cases:**

- Small to medium scale simulations
- Single administrative domain
- Centralized control scenarios

### Hierarchical RIC Deployment

```mermaid
graph TB
    subgraph "Hierarchical Architecture"
        NonRTRIC[Non-RT RIC]
        
        subgraph "Near-RT RICs"
            RTRIC1[Near-RT RIC 1]
            RTRIC2[Near-RT RIC 2]
        end
        
        subgraph "E2 Nodes Domain 1"
            UE1[UE Nodes]
            ENB1[eNB Nodes]
        end
        
        subgraph "E2 Nodes Domain 2"
            UE2[UE Nodes]
            ENB2[eNB Nodes]
        end
    end
    
    NonRTRIC --> RTRIC1
    NonRTRIC --> RTRIC2
    RTRIC1 --> UE1
    RTRIC1 --> ENB1
    RTRIC2 --> UE2
    RTRIC2 --> ENB2
```

**Use Cases:**

- Large scale simulations
- Multi-domain scenarios
- Federated control

### Distributed RIC Deployment

```mermaid
graph TB
    subgraph "Distributed Architecture"
        subgraph "RIC Cluster"
            RIC1[RIC Instance 1]
            RIC2[RIC Instance 2]
            RIC3[RIC Instance 3]
            LB[Load Balancer]
        end
        
        subgraph "E2 Node Pool"
            UE[UE Nodes]
            ENB[eNB Nodes]
            GNB[gNB Nodes]
        end
    end
    
    LB --> RIC1
    LB --> RIC2
    LB --> RIC3
    
    RIC1 --> UE
    RIC2 --> ENB
    RIC3 --> GNB
```

**Use Cases:**

- High availability requirements
- Load distribution
- Fault tolerance scenarios

## Performance Considerations

### Memory Management

The module implements several memory optimization strategies:

1. **Object Pooling**: Reuse of report and command objects
2. **Smart Pointers**: Automatic memory management with ns-3 Ptr
3. **Data Caching**: Strategic caching of frequently accessed data
4. **Lazy Loading**: Deferred loading of large datasets

### Database Optimization

SQLite performance optimizations:

```sql
-- Indexing strategy
CREATE INDEX idx_reports_node_time ON reports(node_id, timestamp);
CREATE INDEX idx_commands_node_time ON commands(target_node_id, timestamp);
CREATE INDEX idx_registrations_node ON e2_node_registrations(node_id);

-- Query optimization
PRAGMA journal_mode = WAL;
PRAGMA synchronous = NORMAL;
PRAGMA cache_size = -64000;  -- 64MB cache
PRAGMA temp_store = MEMORY;
```

### Simulation Performance

Performance benchmarks for different simulation scales:

| Nodes | Duration | Memory Usage | CPU Usage | Database Size |
|-------|----------|--------------|-----------|---------------|
| 10    | 60s      | 45MB        | 15%       | 2MB          |
| 50    | 60s      | 120MB       | 35%       | 8MB          |
| 100   | 60s      | 220MB       | 55%       | 15MB         |
| 500   | 60s      | 980MB       | 85%       | 65MB         |

### Scalability Guidelines

1. **Node Count**: Up to 1000 nodes per RIC instance
2. **Report Frequency**: Maximum 1Hz per node for optimal performance
3. **Logic Module Complexity**: Processing time should be < 10ms
4. **Database Size**: Regular cleanup recommended for long simulations

### Monitoring and Profiling

Built-in performance monitoring:

```cpp
// Enable performance tracking
oranHelper->SetAttribute("EnablePerfMonitoring", BooleanValue(true));

// Access performance metrics
Ptr<OranPerfMonitor> monitor = ric->GetPerformanceMonitor();
std::map<std::string, double> metrics = monitor->GetMetrics();

// Key metrics
double avgProcessingTime = metrics["avg_processing_time"];
double memoryUsage = metrics["memory_usage_mb"];
double dbQueryTime = metrics["avg_db_query_time"];
```

This architecture document provides a comprehensive view of the O-RAN module design, enabling developers to understand the system structure and extend it effectively.
