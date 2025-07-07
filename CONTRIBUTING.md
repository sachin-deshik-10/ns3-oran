# Contributing to O-RAN Module for ns-3

We welcome contributions to the O-RAN module! This document provides guidelines for contributing to ensure a smooth collaboration process.

## Table of Contents

- [Getting Started](#getting-started)
- [Development Environment](#development-environment)
- [Contribution Workflow](#contribution-workflow)
- [Coding Standards](#coding-standards)
- [Testing Requirements](#testing-requirements)
- [Documentation Guidelines](#documentation-guidelines)
- [Pull Request Process](#pull-request-process)
- [Community Guidelines](#community-guidelines)

## Getting Started

### Prerequisites

Before contributing, ensure you have:

- Experience with C++ and ns-3 network simulator
- Understanding of O-RAN architecture and concepts
- Familiarity with Git version control
- Development environment set up as described in README.md

### Types of Contributions

We welcome various types of contributions:

- **Bug fixes**: Corrections to existing functionality
- **Feature enhancements**: New capabilities and improvements
- **Documentation**: Improvements to guides, API docs, and examples
- **Examples**: New simulation scenarios and use cases
- **Testing**: Additional test cases and performance benchmarks
- **Performance optimizations**: Improvements to efficiency and scalability

### Before You Start

1. **Check existing issues**: Look for related issues or feature requests
2. **Discuss large changes**: For significant modifications, create an issue first
3. **Read the codebase**: Familiarize yourself with the existing architecture
4. **Understand the roadmap**: Check planned features to avoid duplication

## Development Environment

### Setup

```bash
# Clone the repository
git clone https://github.com/sachin-deshik-10/ns3-oran-master.git
cd ns3-oran-master

# Install dependencies (Ubuntu example)
sudo apt-get install build-essential cmake sqlite3 libsqlite3-dev

# Install optional ML dependencies
# For PyTorch support
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-latest.zip
unzip libtorch-cxx11-abi-shared-with-deps-latest.zip
export LIBTORCHPATH=$(pwd)/libtorch

# For ONNX Runtime support
wget https://github.com/microsoft/onnxruntime/releases/download/v1.15.1/onnxruntime-linux-x64-1.15.1.tgz
tar -xzf onnxruntime-linux-x64-1.15.1.tgz
export LIBONNXPATH=$(pwd)/onnxruntime-linux-x64-1.15.1
```

### Build System

The O-RAN module uses CMake for building:

```bash
# Configure build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with tests
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Development Tools

Recommended tools for development:

- **IDE**: Visual Studio Code, CLion, or Qt Creator
- **Debugger**: GDB with pretty printers for ns-3 objects
- **Profiler**: Valgrind for memory analysis, gprof for performance
- **Static Analysis**: Clang Static Analyzer, cppcheck
- **Code Formatting**: clang-format with provided configuration

## Contribution Workflow

### 1. Fork and Clone

```bash
# Fork the repository on GitHub
# Clone your fork
git clone https://github.com/sachin-deshik-10/ns3-oran-master.git
cd ns3-oran-master



### 2. Create Feature Branch

```bash
# Update your main branch
git checkout main
git pull upstream main

# Create feature branch
git checkout -b feature/your-feature-name
```

### 3. Make Changes

Follow these guidelines when making changes:

- **Small commits**: Make logical, focused commits
- **Descriptive messages**: Write clear commit messages
- **Test early**: Run tests frequently during development
- **Document changes**: Update relevant documentation

### 4. Commit Guidelines

Use conventional commit format:

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

Types:

- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `perf`: Performance improvements

Examples:

```bash
git commit -m "feat(lm): add machine learning handover logic module"
git commit -m "fix(reporter): resolve memory leak in location reporter"
git commit -m "docs(api): update OranHelper class documentation"
```

### 5. Push and Create PR

```bash
# Push feature branch
git push origin feature/your-feature-name

# Create pull request on GitHub
```

## Coding Standards

### C++ Style Guidelines

We follow ns-3 coding conventions with some additions:

#### Naming Conventions

```cpp
// Classes: PascalCase with "Oran" prefix
class OranMyNewComponent : public Object
{
};

// Methods: PascalCase
void ProcessReports();
bool IsActive() const;

// Variables: camelCase with prefix
private:
    uint32_t m_nodeCount;           // Member variables with m_ prefix
    static uint32_t s_instanceCount; // Static members with s_ prefix

// Constants: ALL_CAPS
const double MAX_PROCESSING_DELAY = 1.0;

// Enums: PascalCase
enum class ReportType {
    Location,
    CellInfo,
    AppLoss
};
```

#### Code Organization

```cpp
// Header file structure
#ifndef ORAN_MY_COMPONENT_H
#define ORAN_MY_COMPONENT_H

#include "ns3/object.h"
#include "ns3/ptr.h"
// ... other includes

namespace ns3 {

/**
 * \ingroup oran
 * \brief Brief description of the class
 *
 * Detailed description of the class functionality,
 * usage examples, and important notes.
 */
class OranMyComponent : public Object
{
public:
    /**
     * \brief Get the type ID
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * Constructor
     */
    OranMyComponent();

    /**
     * Destructor
     */
    virtual ~OranMyComponent();

    // Public methods with documentation

private:
    // Private methods and member variables
};

} // namespace ns3

#endif /* ORAN_MY_COMPONENT_H */
```

#### Implementation File Structure

```cpp
#include "oran-my-component.h"

#include "ns3/log.h"
#include "ns3/simulator.h"
// ... other includes

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("OranMyComponent");
NS_OBJECT_ENSURE_REGISTERED(OranMyComponent);

TypeId
OranMyComponent::GetTypeId()
{
    static TypeId tid = TypeId("ns3::OranMyComponent")
        .SetParent<Object>()
        .SetGroupName("Oran")
        .AddConstructor<OranMyComponent>()
        .AddAttribute("AttributeName",
                      "Attribute description",
                      TypeValue(DefaultValue()),
                      MakeTypeAccessor(&OranMyComponent::m_attribute),
                      MakeTypeChecker());
    return tid;
}

OranMyComponent::OranMyComponent()
    : m_attribute(DefaultValue())
{
    NS_LOG_FUNCTION(this);
}

OranMyComponent::~OranMyComponent()
{
    NS_LOG_FUNCTION(this);
}

} // namespace ns3
```

#### Error Handling

```cpp
// Use NS_ASSERT for debugging assertions
NS_ASSERT_MSG(ptr != nullptr, "Pointer cannot be null");

// Use NS_FATAL_ERROR for unrecoverable errors
if (!InitializeDatabase()) {
    NS_FATAL_ERROR("Failed to initialize database");
}

// Use NS_LOG for different severity levels
NS_LOG_FUNCTION(this);          // Function entry/exit
NS_LOG_LOGIC("Processing " << reports.size() << " reports");
NS_LOG_WARN("Large processing delay detected: " << delay);
NS_LOG_ERROR("Command execution failed for node " << nodeId);
```

#### Memory Management

```cpp
// Use ns-3 smart pointers
Ptr<OranReport> report = CreateObject<OranReportLocation>();

// Avoid raw pointers for ns-3 objects
// WRONG:
// OranReport* report = new OranReportLocation();

// Use RAII for resource management
class DatabaseConnection {
public:
    DatabaseConnection(const std::string& filename) {
        m_db = sqlite3_open(filename.c_str(), &m_handle);
    }
    
    ~DatabaseConnection() {
        if (m_handle) {
            sqlite3_close(m_handle);
        }
    }
    
private:
    sqlite3* m_handle;
};
```

### Code Quality

#### Performance Considerations

```cpp
// Prefer const references for complex types
void ProcessReports(const std::vector<Ptr<OranReport>>& reports);

// Use move semantics when appropriate
std::vector<Ptr<OranCommand>> GenerateCommands() {
    std::vector<Ptr<OranCommand>> commands;
    // ... populate commands
    return commands; // RVO/move will optimize this
}

// Cache expensive computations
class OranLmDistanceHandover : public OranLm {
private:
    mutable std::map<uint64_t, Vector> m_cachedPositions;
    mutable Time m_cacheTime;
    
    Vector GetCachedPosition(uint64_t nodeId) const {
        if (Simulator::Now() > m_cacheTime + Seconds(1.0)) {
            m_cachedPositions.clear();
            m_cacheTime = Simulator::Now();
        }
        // ... implementation
    }
};
```

#### Thread Safety

```cpp
// Document thread safety requirements
/**
 * \brief Thread-safe method for concurrent access
 * 
 * This method can be called from multiple threads safely.
 * Uses internal locking mechanism.
 */
void ThreadSafeMethod();

// Use appropriate synchronization
class ThreadSafeDataRepository {
private:
    mutable std::mutex m_mutex;
    std::vector<Ptr<OranReport>> m_reports;
    
public:
    void StoreReport(Ptr<OranReport> report) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_reports.push_back(report);
    }
};
```

## Testing Requirements

### Unit Tests

Every new class should have corresponding unit tests:

```cpp
// File: test/oran-my-component-test-suite.cc
#include "ns3/test.h"
#include "ns3/oran-my-component.h"

using namespace ns3;

class OranMyComponentTestCase : public TestCase
{
public:
    OranMyComponentTestCase();

private:
    virtual void DoRun() override;
};

OranMyComponentTestCase::OranMyComponentTestCase()
    : TestCase("OranMyComponent basic functionality")
{
}

void
OranMyComponentTestCase::DoRun()
{
    // Create test instance
    Ptr<OranMyComponent> component = CreateObject<OranMyComponent>();
    
    // Test basic functionality
    NS_TEST_ASSERT_MSG_NE(component, nullptr, "Component creation failed");
    
    // Test specific methods
    component->SetAttribute("TestValue", UintegerValue(42));
    UintegerValue value;
    component->GetAttribute("TestValue", value);
    NS_TEST_ASSERT_MSG_EQ(value.Get(), 42, "Attribute setting failed");
}

// Test suite registration
class OranMyComponentTestSuite : public TestSuite
{
public:
    OranMyComponentTestSuite();
};

OranMyComponentTestSuite::OranMyComponentTestSuite()
    : TestSuite("oran-my-component", UNIT)
{
    AddTestCase(new OranMyComponentTestCase, TestCase::QUICK);
}

static OranMyComponentTestSuite g_oranMyComponentTestSuite;
```

### Integration Tests

Test interactions between components:

```cpp
class OranIntegrationTestCase : public TestCase
{
public:
    OranIntegrationTestCase();

private:
    virtual void DoRun() override;
    void ValidateReportFlow();
    void ValidateCommandExecution();
};

void
OranIntegrationTestCase::DoRun()
{
    // Set up complete simulation environment
    NodeContainer nodes;
    nodes.Create(3);
    
    Ptr<OranHelper> helper = CreateObject<OranHelper>();
    helper->SetDataRepository("ns3::OranDataRepositorySqlite",
                              "DatabaseFile", StringValue(":memory:"));
    
    Ptr<OranNearRtRic> ric = helper->CreateNearRtRic();
    
    // Test report flow
    ValidateReportFlow();
    
    // Test command execution
    ValidateCommandExecution();
}
```

### Performance Tests

Include performance benchmarks for critical paths:

```cpp
class OranPerformanceTestCase : public TestCase
{
private:
    virtual void DoRun() override;
    void BenchmarkReportProcessing();
    void BenchmarkDatabaseOperations();
};

void
OranPerformanceTestCase::BenchmarkReportProcessing()
{
    const uint32_t numReports = 10000;
    std::vector<Ptr<OranReport>> reports;
    
    // Generate test reports
    for (uint32_t i = 0; i < numReports; ++i) {
        Ptr<OranReportLocation> report = CreateObject<OranReportLocation>();
        report->SetNodeId(i);
        report->SetPosition(Vector(i * 10.0, 0, 0));
        reports.push_back(report);
    }
    
    // Measure processing time
    auto start = std::chrono::high_resolution_clock::now();
    
    Ptr<OranLmLte2LteDistanceHandover> lm = 
        CreateObject<OranLmLte2LteDistanceHandover>();
    std::vector<Ptr<OranCommand>> commands = lm->Logic(reports);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Verify performance requirements
    NS_TEST_ASSERT_MSG_LT(duration.count(), 100, 
                         "Processing time too high: " << duration.count() << "ms");
}
```

### Test Coverage

Aim for high test coverage:

```bash
# Generate coverage report
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
make
ctest
gcovr -r . --html --html-details -o coverage.html
```

## Documentation Guidelines

### Code Documentation

Use Doxygen-style comments:

```cpp
/**
 * \brief Brief description of the method
 *
 * Detailed description explaining the method's purpose,
 * behavior, and any important implementation details.
 *
 * \param reports Vector of reports to process
 * \param threshold Processing threshold value
 * \return Vector of generated commands
 *
 * \throws OranException if processing fails
 *
 * \note Important notes about usage or behavior
 * \warning Warnings about potential issues
 * \see Related methods or classes
 *
 * Example usage:
 * \code
 * std::vector<Ptr<OranReport>> reports = GetReports();
 * auto commands = ProcessReports(reports, 0.5);
 * \endcode
 */
std::vector<Ptr<OranCommand>> ProcessReports(
    const std::vector<Ptr<OranReport>>& reports,
    double threshold);
```

### Example Documentation

New examples should include comprehensive documentation:

```cpp
/**
 * \file oran-my-example.cc
 * \brief Example demonstrating new O-RAN functionality
 *
 * This example shows how to:
 * - Configure the new component
 * - Set up the simulation environment
 * - Analyze the results
 *
 * The simulation creates a scenario with:
 * - 5 LTE eNBs in a grid topology
 * - 20 mobile UEs with random walk mobility
 * - Custom logic module for interference mitigation
 *
 * Expected output:
 * - Database file with reports and commands
 * - Performance statistics
 * - Handover decision logs
 *
 * Usage:
 * \code
 * ./waf --run "oran-my-example --numUes=20 --simTime=60"
 * \endcode
 */
```

### README Updates

Update README.md for significant changes:

- Add new features to feature list
- Update installation instructions if needed
- Add new examples to examples section
- Update API documentation links

## Pull Request Process

### Before Submitting

1. **Rebase on latest main**:

   ```bash
   git fetch upstream
   git rebase upstream/main
   ```

2. **Run all tests**:

   ```bash
   ./waf configure --enable-tests
   ./waf build
   ./waf --run "test-runner --suite=oran"
   ```

3. **Check code formatting**:

   ```bash
   clang-format -i **/*.{cc,h}
   ```

4. **Update documentation**:
   - API documentation for new classes
   - Example documentation for new features
   - README.md for user-facing changes

### PR Template

Use this template for pull requests:

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests added/updated
- [ ] Performance tests added/updated
- [ ] All tests pass

## Documentation
- [ ] Code comments added/updated
- [ ] API documentation updated
- [ ] Example documentation updated
- [ ] README.md updated if needed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Changes generate no new warnings
- [ ] Backward compatibility maintained
```

### Review Process

1. **Automated checks**: CI/CD pipeline validates:
   - Code compilation
   - Test execution
   - Code formatting
   - Documentation generation

2. **Peer review**: Core maintainers review:
   - Code quality and design
   - Test coverage
   - Documentation completeness
   - Backward compatibility

3. **Integration testing**: Extended testing on multiple platforms

### Addressing Review Comments

- Respond to all review comments
- Make requested changes in new commits
- Update the PR description if scope changes
- Request re-review when ready

## Community Guidelines

### Communication

- **Be respectful**: Treat all contributors with respect
- **Be constructive**: Provide helpful feedback and suggestions
- **Be patient**: Remember that contributors may have different experience levels
- **Be collaborative**: Work together to find the best solutions

### Issue Reporting

When reporting issues:

1. **Search existing issues** first
2. **Use issue templates** provided
3. **Provide minimal reproduction cases**
4. **Include system information**
5. **Be responsive** to follow-up questions

### Feature Requests

For new features:

1. **Discuss the need** and use cases
2. **Consider alternatives** and trade-offs
3. **Provide implementation sketches** if possible
4. **Collaborate on design** before implementation

### Code of Conduct

We follow the ns-3 project's code of conduct:

- Use welcoming and inclusive language
- Be respectful of differing viewpoints and experiences
- Gracefully accept constructive criticism
- Focus on what is best for the community
- Show empathy towards other community members

### Recognition

Contributors are recognized through:

- Git commit history and authorship
- Contributor lists in documentation
- Release notes acknowledgments
- Conference paper acknowledgments where appropriate

## Getting Help

If you need help:

1. **Documentation**: Check existing documentation first
2. **Issues**: Search existing issues for similar problems
3. **Discussions**: Use GitHub Discussions for questions
4. **Contact**: Reach out to maintainers for complex issues

Thank you for contributing to the O-RAN module! Your contributions help advance research and development in intelligent wireless networks.
