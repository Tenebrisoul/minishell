# Testing Your Minishell

This directory contains a comprehensive test suite for minishell that covers all major functionality.

## Quick Start

```bash
# Run all tests (full report)
python3 test_minishell.py

# Run specific category
python3 test_minishell.py --category basic
python3 test_minishell.py --category semicolon

# Different output modes
python3 test_minishell.py --summary        # Only key statistics
python3 test_minishell.py --brief          # Only pass/fail counts
python3 test_minishell.py --percentage     # Only success rates
python3 test_minishell.py --failed-only    # Only failed test details
python3 test_minishell.py --quiet          # Only final result
python3 test_minishell.py --json-only      # JSON format output

# Combine category with output mode
python3 test_minishell.py --category pipes --failed-only
python3 test_minishell.py --category basic --percentage

# Specify minishell path
python3 test_minishell.py --minishell ./my_minishell --brief
```

## Output Modes

### `--summary` / `-s`
Shows only key statistics:
```
TEST SUMMARY
========================================
Total Tests: 191
Passed: 149
Failed: 42
Success Rate: 78.0%
```

### `--brief` / `-b`
Shows only pass/fail counts per category:
```
CATEGORY RESULTS
========================================
BASIC          : 12/14
BUILTINS       : 17/22
PIPES          :  7/14
...
```

### `--percentage` / `-p`
Shows only success percentages:
```
SUCCESS RATES
========================================
OVERALL        :  78.0%
-------------------------
BASIC          :  85.7%
PIPES          :  50.0%
...
```

### `--failed-only` / `-f`
Shows only failed test details:
```
FAILED TESTS (42/191)
============================================================

PIPES
----------------------------------------
  complex_pipe_chain             [FAIL]
    Expected: 'test', Got: 'testerror'
...
```

### `--quiet` / `-q`
Shows only final result (useful for scripts):
```
FAIL: 149/191 (78.0%)
```

### `--json-only` / `-j`
Outputs structured JSON data:
```json
{
  "timestamp": "2025-09-11 12:13:29",
  "total_tests": 191,
  "passed": 149,
  "failed": 42,
  "success_rate": 78.0,
  "categories": {
    "basic": {
      "total": 14,
      "passed": 12,
      "failed": 2,
      "success_rate": 85.7
    }
  }
}
```

## Test Categories

- **basic**: Basic command execution (echo, pwd, ls, etc.) - 14 tests
- **builtins**: Built-in commands (cd, env, export, unset, exit) - 22 tests
- **redirections**: Input/output redirections (<, >, >>) - 10 tests
- **pipes**: Pipe functionality (|) - 14 tests
- **env_vars**: Environment variable expansion ($VAR, $?) - 20 tests
- **quotes**: Quote handling (single, double, mixed) - 21 tests
- **semicolon**: Semicolon command sequences (;) - 17 tests
- **errors**: Error handling and edge cases - 26 tests
- **edge_cases**: Corner cases and stress tests - 28 tests
- **signals**: Signal handling (Ctrl+C, etc.) - 1 test
- **combinations**: Complex feature combinations - 18 tests

**Total: 191 comprehensive test cases**

## Test Results

The suite provides:
- ✅ **Pass/Fail status** for each test
- 📊 **Success percentage** by category
- 📝 **Detailed logs** with expected vs actual output
- 💾 **JSON log file** for programmatic analysis

## Example Output

```
============================================================
        MINISHELL COMPREHENSIVE TEST SUITE
============================================================

Testing Basic Commands...
Testing Built-in Commands...
...

TEST RESULTS SUMMARY
============================================================
Overall Results: 149 passed, 42 failed, 78.0% success rate

BASIC               : 12/14 ( 85.7%)
BUILTINS            : 17/22 ( 77.3%)
REDIRECTIONS        :  9/10 ( 90.0%)
PIPES               :  7/14 ( 50.0%)
ENV_VARS            : 16/20 ( 80.0%)
QUOTES              : 16/21 ( 76.2%)
SEMICOLON           : 12/17 ( 70.6%)
ERRORS              : 19/26 ( 73.1%)
EDGE_CASES          : 24/28 ( 85.7%)
SIGNALS             :  0/ 1 (  0.0%)
COMBINATIONS        : 17/18 ( 94.4%)
```

## Log Files

- `minishell_test_log.json`: Detailed test results in JSON format
- Contains timestamp, test details, and failure reasons

## Current Test Coverage

The test suite covers **191 different test cases** including:

### ✅ Working Features (149/191 tests passing - 78.0%)
- Basic command execution (%85.7)
- Built-in commands (%77.3)
- Input/output redirections (%90.0)
- Environment variable expansion (%80.0)
- Quote handling (%76.2)
- **Semicolon sequences** (%70.6) - newly implemented!
- Error handling (%73.1)
- Edge cases (%85.7)
- **Feature combinations** (%94.4) - complex scenarios!

### 🔧 Areas for Improvement (42 failing tests)
- Pipe functionality (%50.0) - needs improvement
- Some builtin edge cases
- Complex quote scenarios
- Signal handling (not implemented)
- Advanced error conditions

## Usage Tips

1. **Run tests after any changes** to ensure nothing breaks
2. **Use category testing** during development: `python3 test_minishell.py --category basic`
3. **Check the JSON log** for detailed failure analysis
4. **Monitor success rate** - aim for >90% for production readiness
5. **Use different output modes** for different purposes:
   - `--brief` for quick overview
   - `--failed-only` for debugging
   - `--quiet` for CI/CD scripts
   - `--json-only` for automation

## Practical Examples

```bash
# Quick check during development
python3 test_minishell.py --category pipes --percentage

# Debug specific failures
python3 test_minishell.py --category quotes --failed-only

# CI/CD pipeline check
python3 test_minishell.py --quiet

# Generate report for documentation
python3 test_minishell.py --json-only > test_results.json

# Monitor progress during bug fixing
python3 test_minishell.py --category errors --brief

# Full development cycle check
python3 test_minishell.py --summary
```

## Adding New Tests

To add new tests, modify `test_minishell.py`:

1. Add test method to appropriate category
2. Use `self.assert_equal()` for comparisons
3. Test both normal and error cases
4. Include edge cases and boundary conditions

Happy testing! 🚀
