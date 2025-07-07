# Project Structure

This document provides an overview of the O-RAN Module for ns-3 project structure and organization.

## Directory Structure

```
ns3-oran-master/
├── .github/                    # GitHub-specific files
│   ├── ISSUE_TEMPLATE/         # Issue templates
│   │   ├── bug_report.md       # Bug report template
│   │   └── feature_request.md  # Feature request template
│   └── workflows/              # GitHub Actions workflows
│       └── ci.yml              # Continuous integration pipeline
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md         # Architecture documentation
│   ├── API_REFERENCE.md        # API reference documentation
│   ├── USER_GUIDE.md           # User guide and tutorials
│   └── WORKFLOW.md             # Development workflow guide
├── examples/                   # Example simulations
│   ├── CMakeLists.txt          # Build configuration for examples
│   ├── oran-data-repository-example.cc
│   ├── oran-keep-alive-example.cc
│   ├── oran-lte-2-lte-distance-handover-example.cc
│   ├── oran-lte-2-lte-distance-handover-helper-example.cc
│   ├── oran-lte-2-lte-distance-handover-lm-processing-delay-example.cc
│   ├── oran-lte-2-lte-distance-handover-lm-query-trigger-example.cc
│   ├── oran-lte-2-lte-ml-handover-example.cc
│   ├── oran-lte-2-lte-ml-handover-example-classifier.py
│   ├── oran-lte-2-lte-ml-handover-example-generate-training-data.sh
│   ├── oran-lte-2-lte-rsrp-handover-lm-example.cc
│   ├── oran-multiple-net-devices-example.cc
│   ├── oran-random-walk-example.cc
│   ├── saved_trained_classification_pytorch.onnx
│   └── saved_trained_classification_pytorch.pt
├── helper/                     # Helper classes
│   ├── oran-helper.cc          # Main helper implementation
│   └── oran-helper.h           # Main helper header
├── model/                      # Core model implementation
│   ├── oran-cmm-*.{cc,h}       # Conflict Mitigation Modules
│   ├── oran-command-*.{cc,h}   # Command classes
│   ├── oran-data-repository*.{cc,h} # Data repository classes
│   ├── oran-e2-node-terminator*.{cc,h} # E2 node terminators
│   ├── oran-lm-*.{cc,h}        # Logic modules
│   ├── oran-near-rt-ric*.{cc,h} # Near-RT RIC implementation
│   ├── oran-query-trigger*.{cc,h} # Query triggers
│   ├── oran-report*.{cc,h}     # Report classes
│   └── oran-reporter*.{cc,h}   # Reporter classes
├── test/                       # Test suite
│   └── oran-test-suite.cc      # Unit tests
├── .clang-tidy                 # Clang-tidy configuration
├── .gitignore                  # Git ignore rules
├── CHANGELOG.md                # Version history and changes
├── CMakeLists.txt              # Build configuration
├── CONTRIBUTING.md             # Contribution guidelines
├── LICENSE.md                  # License information
├── README.md                   # Project overview and quick start
├── SECURITY.md                 # Security policy
└── THIRD_PARTY_LICENSES.md     # Third-party licenses
```

## Component Organization

### Core Components

#### Near-RT RIC (`model/oran-near-rt-ric*`)

- `oran-near-rt-ric.{cc,h}`: Main RIC implementation
- `oran-near-rt-ric-e2terminator.{cc,h}`: E2 terminator interface

#### Data Repository (`model/oran-data-repository*`)

- `oran-data-repository.{cc,h}`: Abstract data repository interface
- `oran-data-repository-sqlite.{cc,h}`: SQLite implementation

#### Logic Modules (`model/oran-lm-*`)

- `oran-lm.{cc,h}`: Base logic module class
- `oran-lm-noop.{cc,h}`: No-operation logic module
- `oran-lm-lte-2-lte-distance-handover.{cc,h}`: Distance-based handover
- `oran-lm-lte-2-lte-rsrp-handover.{cc,h}`: RSRP-based handover
- `oran-lm-lte-2-lte-torch-handover.{cc,h}`: PyTorch ML handover
- `oran-lm-lte-2-lte-onnx-handover.{cc,h}`: ONNX ML handover

#### Conflict Mitigation (`model/oran-cmm-*`)

- `oran-cmm.{cc,h}`: Base conflict mitigation module
- `oran-cmm-noop.{cc,h}`: No-operation mitigation
- `oran-cmm-handover.{cc,h}`: Handover-specific mitigation
- `oran-cmm-single-command-per-node.{cc,h}`: Single command enforcement

#### E2 Node Terminators (`model/oran-e2-node-terminator*`)

- `oran-e2-node-terminator.{cc,h}`: Base terminator class
- `oran-e2-node-terminator-container.{cc,h}`: Container for terminators
- `oran-e2-node-terminator-wired.{cc,h}`: Wired network terminator
- `oran-e2-node-terminator-lte-ue.{cc,h}`: LTE UE terminator
- `oran-e2-node-terminator-lte-enb.{cc,h}`: LTE eNB terminator

#### Reporters (`model/oran-reporter*`)

- `oran-reporter.{cc,h}`: Base reporter class
- `oran-reporter-location.{cc,h}`: Location reporting
- `oran-reporter-lte-ue-cell-info.{cc,h}`: LTE cell information
- `oran-reporter-lte-ue-rsrp-rsrq.{cc,h}`: LTE signal strength
- `oran-reporter-apploss.{cc,h}`: Application loss reporting

#### Reports (`model/oran-report*`)

- `oran-report.{cc,h}`: Base report class
- `oran-report-location.{cc,h}`: Location reports
- `oran-report-lte-ue-cell-info.{cc,h}`: LTE cell info reports
- `oran-report-lte-ue-rsrp-rsrq.{cc,h}`: LTE signal reports
- `oran-report-apploss.{cc,h}`: Application loss reports

#### Report Triggers (`model/oran-report-trigger*`)

- `oran-report-trigger.{cc,h}`: Base trigger class
- `oran-report-trigger-periodic.{cc,h}`: Periodic triggers
- `oran-report-trigger-location-change.{cc,h}`: Location change triggers
- `oran-report-trigger-lte-ue-handover.{cc,h}`: Handover triggers

#### Query Triggers (`model/oran-query-trigger*`)

- `oran-query-trigger.{cc,h}`: Base query trigger
- `oran-query-trigger-noop.{cc,h}`: No-operation trigger
- `oran-query-trigger-custom.{cc,h}`: Custom trigger implementation

#### Commands (`model/oran-command*`)

- `oran-command.{cc,h}`: Base command class
- `oran-command-lte-2-lte-handover.{cc,h}`: LTE handover commands

### Helper Classes

#### O-RAN Helper (`helper/oran-helper.{cc,h}`)

Central configuration and deployment helper providing:

- Component configuration and instantiation
- Deployment automation
- Lifecycle management
- Attribute configuration

### Examples and Use Cases

#### Basic Examples

- **Data Repository**: Demonstrates data storage and retrieval
- **Keep Alive**: Shows E2 node registration and monitoring
- **Random Walk**: Basic location reporting with mobile nodes

#### LTE Handover Examples

- **Distance-based**: Handover decisions based on distance metrics
- **RSRP-based**: Handover logic using signal strength
- **ML-based**: Machine learning-driven handover decisions

#### Advanced Examples

- **Multiple Devices**: Multi-interface node handling
- **Processing Delay**: Impact of processing delays on decisions
- **Custom Triggers**: Implementation of custom query triggers

### Documentation Structure

#### User Documentation

- **README.md**: Project overview and quick start guide
- **docs/USER_GUIDE.md**: Comprehensive user guide with tutorials
- **docs/API_REFERENCE.md**: Detailed API documentation

#### Developer Documentation

- **docs/ARCHITECTURE.md**: System architecture and design
- **docs/WORKFLOW.md**: Development workflow and processes
- **CONTRIBUTING.md**: Contribution guidelines and standards

#### Project Management

- **CHANGELOG.md**: Version history and release notes
- **SECURITY.md**: Security policy and vulnerability reporting
- **LICENSE.md**: License terms and conditions

## Build System

### CMake Configuration

- **CMakeLists.txt**: Main build configuration
- **examples/CMakeLists.txt**: Example build configuration

### Code Quality

- **.clang-tidy**: Static analysis configuration
- **.gitignore**: Version control ignore rules

## Testing Framework

### Unit Tests

- **test/oran-test-suite.cc**: Comprehensive test suite covering:
  - Data repository functionality
  - Near-RT RIC lifecycle management
  - Reporter and trigger mechanisms
  - Logic module execution
  - Command processing

### Integration Tests

Examples serve as integration tests:

- Verify end-to-end functionality
- Test component interactions
- Validate configuration options

## Naming Conventions

### Files

- **Headers**: `.h` extension
- **Implementation**: `.cc` extension
- **Prefix**: All files start with `oran-`

### Classes

- **CamelCase**: `OranNearRtRic`, `OranDataRepository`
- **Descriptive**: Names reflect functionality and purpose

### Methods

- **CamelCase**: Starting with lowercase letter
- **Action-oriented**: `RegisterE2Node()`, `GenerateReport()`

### Attributes

- **CamelCase**: Starting with lowercase letter
- **Descriptive**: `processingDelayRv`, `databaseFile`

This structure provides a clear separation of concerns while maintaining modularity and extensibility for future enhancements.
