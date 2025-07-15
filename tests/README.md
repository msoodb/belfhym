# Belfhym Testing Framework

This directory contains a comprehensive testing framework for the Belfhym lunar rover system. The framework provides multiple testing approaches to validate the system before deploying to hardware.

## Overview

The testing framework includes:

- **Unit Tests**: Individual module testing with mock hardware
- **Integration Tests**: Inter-module communication testing
- **Hardware Simulation**: Mock hardware abstraction layer
- **Coverage Analysis**: Code coverage reporting
- **Memory Leak Detection**: Valgrind integration
- **Static Analysis**: Code quality checks
- **QEMU Simulation**: Basic STM32F103 simulation (limited)

## Quick Start

### Prerequisites

Install dependencies on Fedora:
```bash
sudo dnf install gcc valgrind cppcheck lcov qemu-system-arm
```

Or use the automated installer:
```bash
make install-deps
```

### Running Tests

#### Basic test run:
```bash
# Run all unit tests
make test

# Or use the comprehensive test runner
./run_all_tests.sh
```

#### Advanced testing:
```bash
# Run with coverage analysis
./run_all_tests.sh --coverage

# Run with memory leak detection
./run_all_tests.sh --valgrind

# Run with static analysis
./run_all_tests.sh --static-analysis

# Run all analysis tools
./run_all_tests.sh --all
```

#### Individual test modules:
```bash
make test-i2c                    # I2C module tests
make test-controller             # Controller logic tests
make test-sensor-integration     # Sensor integration tests
```

## Test Structure

### 1. Unit Tests

#### `test_i2c.c`
Tests the I2C communication module:
- Initialization testing
- Read/write operations
- Error handling
- Device response simulation

#### `test_controller.c`
Tests the controller logic:
- Sensor data processing
- Motor control decisions
- Mode switching
- Emergency stop functionality

#### `test_sensor_integration.c`
Tests sensor integration:
- ADC sensor readings
- I2C sensor communication
- Data aggregation
- Timing requirements

### 2. Mock Hardware Layer

#### `mock_hardware.h/c`
Provides software simulation of hardware:
- GPIO pin states
- ADC channel values
- I2C transactions
- UART communication
- PWM channel settings
- System timing

### 3. Unity Test Framework

#### `unity.h/c`
Lightweight test framework providing:
- Test assertions
- Test organization
- Result reporting
- Error tracking

## Testing Approaches

### 1. Host-Based Unit Testing

**Advantages:**
- Fast execution
- Easy debugging
- No hardware required
- Automated CI/CD integration

**Limitations:**
- Cannot test real hardware interactions
- Limited real-time behavior testing

**Use Cases:**
- Algorithm validation
- Logic testing
- Error handling verification
- Data structure validation

### 2. Hardware Simulation

**Advantages:**
- Tests hardware interactions
- Validates timing requirements
- Simulates error conditions
- Reproducible test conditions

**Limitations:**
- Not 100% accurate to real hardware
- Cannot test electrical characteristics

**Use Cases:**
- Protocol testing
- State machine validation
- Integration testing
- Performance testing

### 3. QEMU Simulation

**Advantages:**
- Actual ARM Cortex-M3 simulation
- Tests FreeRTOS integration
- Validates memory layout
- Tests boot sequence

**Limitations:**
- Limited peripheral support
- No GPIO/interrupt simulation
- Basic functionality only

**Use Cases:**
- Boot sequence validation
- Memory layout verification
- FreeRTOS task testing
- Stack usage analysis

## Mock Hardware Usage

### GPIO Testing
```c
// Configure mock GPIO
mock_gpio_config_output(1, 5);  // Port B, Pin 5 as output
mock_gpio_set_pin(1, 5);        // Set pin high
TEST_ASSERT_EQUAL(1, mock_gpio_read_pin(1, 5));
```

### I2C Testing
```c
// Setup I2C mock response
uint8_t response[] = {0x01, 0x02, 0x03};
mock_i2c_set_device_response(0, 0x68, response, sizeof(response));

// Test I2C read
uint8_t buffer[3];
int result = mock_i2c_read(0, 0x68, buffer, sizeof(buffer));
TEST_ASSERT_EQUAL(0, result);
TEST_ASSERT_EQUAL_MEMORY(response, buffer, sizeof(response));
```

### ADC Testing
```c
// Set mock ADC value
mock_adc_set_channel_value(5, 2048);  // 50% of 4096

// Test ADC read
uint16_t value;
int result = mock_adc_read(5, &value);
TEST_ASSERT_EQUAL(0, result);
TEST_ASSERT_EQUAL(2048, value);
```

## Coverage Analysis

Generate code coverage reports:
```bash
make coverage
```

Results are saved in `results/coverage_html/index.html`

## Memory Leak Detection

Run tests with Valgrind:
```bash
make valgrind
```

Results are saved in `results/*_valgrind.txt`

## Static Analysis

Run static code analysis:
```bash
make cppcheck
```

Results are saved in `results/cppcheck.xml`

## Test Results

Test results are saved in the `results/` directory:
- `test_*.log` - Individual test outputs
- `coverage_html/` - Coverage reports
- `*_valgrind.txt` - Memory leak reports
- `cppcheck.xml` - Static analysis results
- `qemu_test.log` - QEMU simulation output

## Adding New Tests

### 1. Create Test File

Create `test_new_module.c`:
```c
#include "unity.h"
#include "mock_hardware.h"

void setUp(void) {
    mock_hardware_reset();
}

void tearDown(void) {
    // Cleanup
}

void test_new_functionality(void) {
    // Test implementation
    TEST_ASSERT_EQUAL(expected, actual);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_new_functionality);
    return UNITY_END();
}
```

### 2. Update Makefile

Add your test to the `TEST_SRCS` variable in `Makefile`:
```makefile
TEST_SRCS = test_i2c.c test_controller.c test_sensor_integration.c test_new_module.c
```

### 3. Update Test Runner

Add your test to `run_all_tests.sh`:
```bash
# Run New Module tests
if ./build/test_new_module > results/test_new_module.log 2>&1; then
    print_status "PASS" "New Module tests"
    TEST_RESULTS+=("New Module:PASS")
else
    print_status "FAIL" "New Module tests - See results/test_new_module.log"
    TEST_RESULTS+=("New Module:FAIL")
fi
```

## Best Practices

### 1. Test Organization
- One test file per module
- Clear test names describing functionality
- Group related tests together
- Use setUp/tearDown for common initialization

### 2. Mock Hardware
- Reset mock hardware before each test
- Set up specific hardware states for each test
- Verify expected hardware interactions
- Test error conditions

### 3. Test Coverage
- Test normal operation paths
- Test error conditions
- Test boundary conditions
- Test edge cases

### 4. Assertions
- Use specific assertions (TEST_ASSERT_EQUAL vs TEST_ASSERT_TRUE)
- Include meaningful error messages
- Test both positive and negative cases
- Verify all function outputs

## Continuous Integration

The testing framework is designed for CI/CD integration:

```bash
# CI pipeline example
./run_all_tests.sh --all
if [ $? -eq 0 ]; then
    echo "All tests passed - deploying to hardware"
else
    echo "Tests failed - blocking deployment"
    exit 1
fi
```

## Troubleshooting

### Common Issues

1. **Tests fail to build**
   - Check GCC installation
   - Verify include paths in Makefile
   - Check for missing dependencies

2. **Mock hardware not responding**
   - Ensure mock_hardware_reset() is called in setUp()
   - Check mock hardware state setup
   - Verify mock hardware initialization

3. **Coverage reports empty**
   - Ensure lcov is installed
   - Check that tests are actually running
   - Verify coverage flags are set correctly

4. **QEMU simulation fails**
   - Check QEMU ARM installation
   - Verify main project builds successfully
   - Check for compatible board simulation

### Debug Tips

1. **Use verbose output**
   ```bash
   ./build/test_i2c  # Run individual test for detailed output
   ```

2. **Check mock hardware state**
   ```c
   mock_print_gpio_state();
   mock_print_i2c_transactions();
   ```

3. **Add debug prints**
   ```c
   printf("Debug: value = %d\n", value);
   ```

## Future Enhancements

1. **Hardware-in-the-Loop (HIL) Testing**
   - Real hardware with controlled inputs
   - Automated test equipment integration
   - Real-time performance validation

2. **Enhanced QEMU Integration**
   - Custom peripheral models
   - GPIO simulation
   - Interrupt testing

3. **Performance Testing**
   - Timing analysis
   - Memory usage profiling
   - Power consumption estimation

4. **Regression Testing**
   - Automated test execution
   - Performance benchmarks
   - Historical result tracking

## Conclusion

This testing framework provides comprehensive validation of the Belfhym rover system before hardware deployment. It combines multiple testing approaches to ensure reliability, performance, and correctness of the embedded software.

The framework is designed to be:
- **Easy to use**: Simple commands for common operations
- **Comprehensive**: Multiple testing approaches
- **Maintainable**: Clear structure and documentation
- **Extensible**: Easy to add new tests and features

Use this framework regularly during development to catch issues early and ensure high-quality software delivery.