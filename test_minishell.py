#!/usr/bin/env python3
"""
École 42 Minishell Test Suite
Comprehensive testing framework for minishell project

Usage:
    python3 test_minishell.py <shell_program> [options]
    
Examples:
    python3 test_minishell.py bash
    python3 test_minishell.py ./minishell
    python3 test_minishell.py /bin/bash --verbose
    python3 test_minishell.py ./minishell --category echo
    python3 test_minishell.py ./minishell --timeout 10
"""

import json
import subprocess
import sys
import os
import time
import argparse
import signal
from typing import Dict, List, Tuple, Optional
import tempfile
import shutil
from dataclasses import dataclass
from enum import Enum
import threading
from datetime import datetime
import re

class TestResult(Enum):
    PASSED = "\033[92m✅ PASSED\033[0m"
    FAILED = "\033[91m❌ FAILED\033[0m"
    ERROR = "\033[95m💥 ERROR\033[0m"
    TIMEOUT = "\033[93m⏰ TIMEOUT\033[0m"
    SKIPPED = "\033[96m⏭️ SKIPPED\033[0m"

@dataclass
class TestCase:
    name: str
    command: str
    category: str
    description: str
    expected_output: Optional[str] = None
    expected_exit_code: Optional[int] = None
    timeout: int = 5
    interactive: bool = False
    requires_files: List[str] = None
    creates_files: List[str] = None

class MinishellTester:
    def __init__(self, shell_program: str, verbose: bool = False, timeout: int = 5, detailed: bool = False):
        self.shell_program = shell_program
        self.verbose = verbose
        self.detailed = detailed
        self.default_timeout = timeout
        self.test_dir = tempfile.mkdtemp(prefix="minishell_test_")
        self.original_dir = os.getcwd()
        self.start_time = None
        self.failed_tests = []
        self.results = {
            TestResult.PASSED: 0,
            TestResult.FAILED: 0,
            TestResult.ERROR: 0,
            TestResult.TIMEOUT: 0,
            TestResult.SKIPPED: 0
        }
        
    def __enter__(self):
        os.chdir(self.test_dir)
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        os.chdir(self.original_dir)
        shutil.rmtree(self.test_dir, ignore_errors=True)

    def load_test_cases(self, json_file: str) -> List[TestCase]:
        """Load test cases from JSON file"""
        # Check if file exists with absolute path
        if not os.path.isabs(json_file):
            json_file = os.path.abspath(json_file)
            
        if not os.path.exists(json_file):
            print(f"❌ Test file {json_file} not found!")
            print(f"Current working directory: {os.getcwd()}")
            print(f"Looking for file at: {json_file}")
            sys.exit(1)
            
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                data = json.load(f)
            
            test_cases = []
            for category, tests in data.items():
                for test in tests:
                    test_case = TestCase(
                        name=test['name'],
                        command=test['command'],
                        category=category,
                        description=test.get('description', ''),
                        expected_output=test.get('expected_output'),
                        expected_exit_code=test.get('expected_exit_code'),
                        timeout=test.get('timeout', self.default_timeout),
                        interactive=test.get('interactive', False),
                        requires_files=test.get('requires_files', []),
                        creates_files=test.get('creates_files', [])
                    )
                    test_cases.append(test_case)
            return test_cases
            
        except json.JSONDecodeError as e:
            print(f"❌ Error parsing JSON file: {e}")
            sys.exit(1)

    def setup_test_files(self, test_case: TestCase) -> bool:
        """Setup required files for test"""
        try:
            for file_spec in test_case.requires_files or []:
                if isinstance(file_spec, str):
                    # Simple file creation
                    with open(file_spec, 'w') as f:
                        f.write("test content\n")
                elif isinstance(file_spec, dict):
                    # File with specific content
                    filename = file_spec['name']
                    content = file_spec.get('content', 'test content\n')
                    mode = file_spec.get('mode', 'w')
                    with open(filename, mode) as f:
                        f.write(content)
            return True
        except Exception as e:
            if self.verbose:
                print(f"Error setting up test files: {e}")
            return False

    def cleanup_test_files(self, test_case: TestCase):
        """Cleanup files created by test"""
        for filename in test_case.creates_files or []:
            try:
                if os.path.exists(filename):
                    os.remove(filename)
            except:
                pass

    def run_command(self, command: str, timeout: int) -> Tuple[str, str, int]:
        """Run a command and return stdout, stderr, exit_code"""
        try:
            # For minishell, we need to pipe the command with exit
            if "minishell" in self.shell_program:
                process = subprocess.Popen(
                    [self.shell_program],
                    stdin=subprocess.PIPE,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                    env=os.environ.copy()
                )
                
                # Send command + exit to minishell
                input_text = f"{command}\nexit\n"
                
                try:
                    stdout, stderr = process.communicate(input=input_text, timeout=timeout)
                    exit_code = process.returncode
                    
                    # Clean up minishell prompt from output
                    lines = stdout.split('\n')
                    cleaned_lines = []
                    for line in lines:
                        # Remove minishell prompt and empty lines
                        if not line.startswith('minishell>') and line.strip() != 'exit' and line.strip():
                            cleaned_lines.append(line)
                    
                    cleaned_output = '\n'.join(cleaned_lines)
                    return cleaned_output, stderr, exit_code
                    
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.communicate()
                    return "", "Process timed out", -1
            else:
                # For other shells (bash, etc.), use normal shell execution
                process = subprocess.Popen(
                    command,
                    shell=True,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                    executable=self.shell_program,
                    env=os.environ.copy()
                )
                
                try:
                    stdout, stderr = process.communicate(timeout=timeout)
                    exit_code = process.returncode
                    return stdout, stderr, exit_code
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.communicate()
                    return "", "Process timed out", -1
                
        except Exception as e:
            return "", f"Error running command: {e}", -1

    def compare_outputs(self, expected: str, actual: str) -> bool:
        """Compare expected and actual outputs"""
        if expected is None:
            return True
            
        # Normalize whitespace
        expected_lines = [line.rstrip() for line in expected.split('\n')]
        actual_lines = [line.rstrip() for line in actual.split('\n')]
        
        # Remove empty lines at the end
        while expected_lines and expected_lines[-1] == '':
            expected_lines.pop()
        while actual_lines and actual_lines[-1] == '':
            actual_lines.pop()
            
        return expected_lines == actual_lines

    def run_single_test(self, test_case: TestCase) -> Tuple[TestResult, Dict]:
        """Run a single test case and return result with details"""
        test_details = {
            'stdout': '',
            'stderr': '',
            'exit_code': None,
            'execution_time': 0,
            'error_message': ''
        }

        if test_case.interactive:
            return TestResult.SKIPPED, test_details

        # Setup test files
        if not self.setup_test_files(test_case):
            test_details['error_message'] = 'Failed to setup test files'
            return TestResult.ERROR, test_details

        try:
            start_time = time.time()

            # Run the command
            stdout, stderr, exit_code = self.run_command(
                test_case.command,
                test_case.timeout
            )

            test_details['stdout'] = stdout
            test_details['stderr'] = stderr
            test_details['exit_code'] = exit_code
            test_details['execution_time'] = time.time() - start_time

            # Check for timeout
            if exit_code == -1 and "timed out" in stderr:
                test_details['error_message'] = 'Command timed out'
                return TestResult.TIMEOUT, test_details

            # Check exit code
            if test_case.expected_exit_code is not None:
                if exit_code != test_case.expected_exit_code:
                    test_details['error_message'] = f"Exit code mismatch: expected {test_case.expected_exit_code}, got {exit_code}"
                    return TestResult.FAILED, test_details

            # Check output
            if not self.compare_outputs(test_case.expected_output, stdout):
                test_details['error_message'] = f"Output mismatch\nExpected: {repr(test_case.expected_output)}\nActual: {repr(stdout)}"
                return TestResult.FAILED, test_details

            return TestResult.PASSED, test_details

        except Exception as e:
            test_details['error_message'] = f"Exception during test execution: {str(e)}"
            return TestResult.ERROR, test_details

        finally:
            self.cleanup_test_files(test_case)

    def create_progress_bar(self, current: int, total: int, width: int = 50) -> str:
        """Create a progress bar string"""
        percentage = current / total
        filled = int(width * percentage)
        bar = '█' * filled + '░' * (width - filled)
        return f"[{bar}] {percentage:.1%} ({current}/{total})"

    def run_tests(self, test_cases: List[TestCase], category_filter: str = None) -> None:
        """Run all test cases with enhanced output"""
        filtered_tests = test_cases
        if category_filter:
            filtered_tests = [t for t in test_cases if t.category == category_filter]

        total_tests = len(filtered_tests)
        self.start_time = datetime.now()

        # Print header with styling
        print("\033[1;36m" + "=" * 80 + "\033[0m")
        print(f"\033[1;36m🚀 ÉCOLE 42 MINISHELL TEST SUITE\033[0m")
        print(f"\033[1;37mShell Program:\033[0m {self.shell_program}")
        print(f"\033[1;37mTotal Tests:\033[0m {total_tests}")
        if category_filter:
            print(f"\033[1;37mCategory Filter:\033[0m {category_filter}")
        print(f"\033[1;37mTest Directory:\033[0m {self.test_dir}")
        print(f"\033[1;37mStarted:\033[0m {self.start_time.strftime('%Y-%m-%d %H:%M:%S')}")
        print("\033[1;36m" + "=" * 80 + "\033[0m")

        # Group tests by category for better organization
        categories = {}
        for test in filtered_tests:
            if test.category not in categories:
                categories[test.category] = []
            categories[test.category].append(test)

        test_number = 0
        for category, tests in categories.items():
            print(f"\n\033[1;33m📂 Category: {category.upper()} ({len(tests)} tests)\033[0m")
            print("\033[33m" + "-" * 60 + "\033[0m")

            for test_case in tests:
                test_number += 1

                # Progress bar
                progress = self.create_progress_bar(test_number, total_tests)
                print(f"\033[2K\r{progress}", end="")

                print(f"\n[{test_number:3d}/{total_tests}] \033[1m{test_case.name}\033[0m")

                if self.verbose or self.detailed:
                    print(f"    \033[90mCommand:\033[0m {test_case.command}")
                    print(f"    \033[90mDescription:\033[0m {test_case.description}")

                start_time = time.time()
                result, details = self.run_single_test(test_case)
                end_time = time.time()

                self.results[result] += 1
                duration = end_time - start_time

                # Display result with details
                print(f"    \033[1mResult:\033[0m {result.value} ({duration:.3f}s)")

                # Show detailed information for failed tests or in verbose mode
                if result in [TestResult.FAILED, TestResult.ERROR, TestResult.TIMEOUT] or self.detailed:
                    if details['error_message']:
                        print(f"    \033[91mError:\033[0m {details['error_message']}")
                    if self.detailed and details['stdout']:
                        print(f"    \033[94mOutput:\033[0m {repr(details['stdout'])}")
                    if self.detailed and details['stderr']:
                        print(f"    \033[93mStderr:\033[0m {repr(details['stderr'])}")
                    if details['exit_code'] is not None:
                        print(f"    \033[90mExit Code:\033[0m {details['exit_code']}")

                # Store failed tests for summary
                if result in [TestResult.FAILED, TestResult.ERROR, TestResult.TIMEOUT]:
                    self.failed_tests.append({
                        'test': test_case,
                        'result': result,
                        'details': details
                    })

        print("\n")
        self.print_summary()

    def print_summary(self):
        """Print enhanced test results summary"""
        total = sum(self.results.values())
        end_time = datetime.now()
        duration = end_time - self.start_time if self.start_time else None

        print("\033[1;36m" + "=" * 80 + "\033[0m")
        print(f"\033[1;36m📊 TEST EXECUTION SUMMARY\033[0m")
        print("\033[1;36m" + "=" * 80 + "\033[0m")

        # Time information
        if duration:
            print(f"\033[1;37mExecution Time:\033[0m {duration.total_seconds():.2f} seconds")
            print(f"\033[1;37mCompleted:\033[0m {end_time.strftime('%Y-%m-%d %H:%M:%S')}")
            print()

        # Results breakdown
        for result, count in self.results.items():
            percentage = (count / total * 100) if total > 0 else 0
            print(f"{result.value}: {count:3d} ({percentage:5.1f}%)")

        print("\033[36m" + "-" * 80 + "\033[0m")

        # Overall status
        success_rate = (self.results[TestResult.PASSED] / total * 100) if total > 0 else 0
        print(f"\033[1;37mSuccess Rate:\033[0m {success_rate:.1f}%")

        if self.results[TestResult.PASSED] == total:
            print("\033[1;32m🎉 ALL TESTS PASSED! Excellent work!\033[0m")
            print("\033[32m✨ Your minishell implementation appears to be working correctly!\033[0m")
        else:
            failed_count = self.results[TestResult.FAILED] + self.results[TestResult.ERROR] + self.results[TestResult.TIMEOUT]
            print(f"\033[1;31m⚠️  {failed_count} TEST(S) FAILED\033[0m")

            if self.failed_tests:
                print("\n\033[1;31m📋 FAILED TESTS SUMMARY:\033[0m")
                print("\033[31m" + "-" * 60 + "\033[0m")

                for i, failure in enumerate(self.failed_tests[:10], 1):  # Show first 10 failures
                    test = failure['test']
                    result = failure['result']
                    details = failure['details']

                    print(f"\033[1;31m{i}. {test.category}: {test.name}\033[0m")
                    print(f"   \033[90mCommand:\033[0m {test.command}")
                    print(f"   \033[91mResult:\033[0m {result.value}")
                    if details['error_message']:
                        # Truncate long error messages
                        error_msg = details['error_message']
                        if len(error_msg) > 100:
                            error_msg = error_msg[:97] + "..."
                        print(f"   \033[91mError:\033[0m {error_msg}")
                    print()

                if len(self.failed_tests) > 10:
                    print(f"\033[90m... and {len(self.failed_tests) - 10} more failed tests\033[0m")

        print("\033[1;36m" + "=" * 80 + "\033[0m")

        # Recommendations
        if self.results[TestResult.FAILED] > 0:
            print("\033[1;33m💡 RECOMMENDATIONS:\033[0m")
            print("\033[33m• Run with --verbose flag for detailed error information\033[0m")
            print("\033[33m• Run with --detailed flag for complete test output\033[0m")
            print("\033[33m• Focus on failed test categories first\033[0m")
            print("\033[33m• Check your shell's error handling and edge cases\033[0m")

def create_test_json():
    """Create the test cases JSON file"""
    test_cases = {
        "echo": [
            {
                "name": "basic_echo",
                "command": "echo hello world",
                "description": "Basic echo test",
                "expected_output": "hello world\n",
                "expected_exit_code": 0
            },
            {
                "name": "echo_with_quotes",
                "command": "echo \"hello world\"",
                "description": "Echo with double quotes",
                "expected_output": "hello world\n",
                "expected_exit_code": 0
            },
            {
                "name": "echo_single_quotes",
                "command": "echo 'hello world'",
                "description": "Echo with single quotes",
                "expected_output": "hello world\n",
                "expected_exit_code": 0
            },
            {
                "name": "echo_empty",
                "command": "echo",
                "description": "Echo without arguments",
                "expected_output": "\n",
                "expected_exit_code": 0
            },
            {
                "name": "echo_n_flag",
                "command": "echo -n hello",
                "description": "Echo with -n flag",
                "expected_output": "hello",
                "expected_exit_code": 0
            },
            {
                "name": "echo_multiple_n",
                "command": "echo -n -n hello",
                "description": "Echo with multiple -n flags",
                "expected_output": "hello",
                "expected_exit_code": 0
            },
            {
                "name": "echo_n_combined",
                "command": "echo -nnnn hello",
                "description": "Echo with combined -n flags",
                "expected_output": "hello",
                "expected_exit_code": 0
            },
            {
                "name": "echo_n_quoted",
                "command": "echo \"-n\" hello",
                "description": "Echo with quoted -n",
                "expected_output": "-n hello\n",
                "expected_exit_code": 0
            },
            {
                "name": "echo_empty_quotes",
                "command": "echo \"\"",
                "description": "Echo with empty double quotes",
                "expected_output": "\n",
                "expected_exit_code": 0
            },
            {
                "name": "echo_empty_single_quotes",
                "command": "echo ''",
                "description": "Echo with empty single quotes",
                "expected_output": "\n",
                "expected_exit_code": 0
            }
        ],
        "cd": [
            {
                "name": "cd_home",
                "command": "cd && pwd",
                "description": "Change to home directory",
                "expected_exit_code": 0
            },
            {
                "name": "cd_dot",
                "command": "cd . && pwd",
                "description": "Change to current directory",
                "expected_exit_code": 0
            },
            {
                "name": "cd_parent",
                "command": "cd .. && pwd",
                "description": "Change to parent directory",
                "expected_exit_code": 0
            },
            {
                "name": "cd_root",
                "command": "cd / && pwd",
                "description": "Change to root directory",
                "expected_output": "/\n",
                "expected_exit_code": 0
            },
            {
                "name": "cd_nonexistent",
                "command": "cd /nonexistent_directory_test_123",
                "description": "Change to non-existent directory",
                "expected_exit_code": 1
            },
            {
                "name": "cd_too_many_args",
                "command": "cd /tmp /usr",
                "description": "CD with too many arguments",
                "expected_exit_code": 1
            }
        ],
        "pwd": [
            {
                "name": "basic_pwd",
                "command": "pwd",
                "description": "Basic pwd test",
                "expected_exit_code": 0
            },
            {
                "name": "pwd_with_args",
                "command": "pwd arg1",
                "description": "PWD with arguments (should error)",
                "expected_exit_code": 1
            },
            {
                "name": "pwd_after_cd",
                "command": "cd /tmp && pwd",
                "description": "PWD after changing directory",
                "expected_output": "/tmp\n",
                "expected_exit_code": 0
            }
        ],
        "env": [
            {
                "name": "basic_env",
                "command": "env | grep HOME",
                "description": "Basic env test - find HOME",
                "expected_exit_code": 0
            },
            {
                "name": "env_with_args",
                "command": "env arg1",
                "description": "ENV with arguments (should error)",
                "expected_exit_code": 125
            }
        ],
        "export": [
            {
                "name": "export_simple",
                "command": "export TEST=hello && echo $TEST",
                "description": "Simple export test",
                "expected_output": "hello\n",
                "expected_exit_code": 0
            },
            {
                "name": "export_empty",
                "command": "export TEST= && echo \"[$TEST]\"",
                "description": "Export empty variable",
                "expected_output": "[]\n",
                "expected_exit_code": 0
            },
            {
                "name": "export_multiple",
                "command": "export A=1 B=2 && echo $A$B",
                "description": "Export multiple variables",
                "expected_output": "12\n",
                "expected_exit_code": 0
            },
            {
                "name": "export_invalid_name",
                "command": "export 123=value",
                "description": "Export with invalid variable name",
                "expected_exit_code": 1
            },
            {
                "name": "export_no_equals",
                "command": "export TEST",
                "description": "Export existing variable",
                "expected_exit_code": 0
            }
        ],
        "unset": [
            {
                "name": "unset_variable",
                "command": "export TEST=hello && unset TEST && echo \"[$TEST]\"",
                "description": "Unset a variable",
                "expected_output": "[]\n",
                "expected_exit_code": 0
            },
            {
                "name": "unset_nonexistent",
                "command": "unset NONEXISTENT_VAR_12345",
                "description": "Unset non-existent variable",
                "expected_exit_code": 0
            },
            {
                "name": "unset_multiple",
                "command": "export A=1 B=2 && unset A B && echo \"[$A][$B]\"",
                "description": "Unset multiple variables",
                "expected_output": "[][]\n",
                "expected_exit_code": 0
            }
        ],
        "exit": [
            {
                "name": "exit_no_args",
                "command": "exit",
                "description": "Exit without arguments",
                "expected_exit_code": 0
            },
            {
                "name": "exit_zero",
                "command": "exit 0",
                "description": "Exit with code 0",
                "expected_exit_code": 0
            },
            {
                "name": "exit_one",
                "command": "exit 1",
                "description": "Exit with code 1",
                "expected_exit_code": 1
            },
            {
                "name": "exit_42",
                "command": "exit 42",
                "description": "Exit with code 42",
                "expected_exit_code": 42
            },
            {
                "name": "exit_negative",
                "command": "exit -1",
                "description": "Exit with negative code",
                "expected_exit_code": 255
            },
            {
                "name": "exit_too_many_args",
                "command": "exit 0 1",
                "description": "Exit with too many arguments",
                "expected_exit_code": 1
            },
            {
                "name": "exit_invalid_arg",
                "command": "exit abc",
                "description": "Exit with invalid argument",
                "expected_exit_code": 255
            }
        ],
        "pipes": [
            {
                "name": "simple_pipe",
                "command": "echo hello | cat",
                "description": "Simple pipe test",
                "expected_output": "hello\n",
                "expected_exit_code": 0
            },
            {
                "name": "multiple_pipes",
                "command": "echo hello | cat | cat",
                "description": "Multiple pipes",
                "expected_output": "hello\n",
                "expected_exit_code": 0
            },
            {
                "name": "pipe_with_grep",
                "command": "echo hello | grep hello",
                "description": "Pipe with grep",
                "expected_output": "hello\n",
                "expected_exit_code": 0
            },
            {
                "name": "pipe_grep_nomatch",
                "command": "echo hello | grep world",
                "description": "Pipe with grep (no match)",
                "expected_output": "",
                "expected_exit_code": 1
            }
        ],
        "redirections": [
            {
                "name": "output_redirect",
                "command": "echo hello > test_output.txt && cat test_output.txt",
                "description": "Output redirection",
                "expected_output": "hello\n",
                "expected_exit_code": 0,
                "creates_files": ["test_output.txt"]
            },
            {
                "name": "append_redirect",
                "command": "echo hello > test_append.txt && echo world >> test_append.txt && cat test_append.txt",
                "description": "Append redirection",
                "expected_output": "hello\nworld\n",
                "expected_exit_code": 0,
                "creates_files": ["test_append.txt"]
            },
            {
                "name": "input_redirect",
                "command": "cat < /etc/passwd | head -1",
                "description": "Input redirection",
                "expected_exit_code": 0
            }
        ],
        "variables": [
            {
                "name": "home_variable",
                "command": "echo $HOME",
                "description": "HOME variable expansion",
                "expected_exit_code": 0
            },
            {
                "name": "path_variable",
                "command": "echo $PATH | grep bin",
                "description": "PATH variable expansion",
                "expected_exit_code": 0
            },
            {
                "name": "exit_status",
                "command": "true && echo $?",
                "description": "Exit status variable",
                "expected_output": "0\n",
                "expected_exit_code": 0
            },
            {
                "name": "exit_status_false",
                "command": "false; echo $?",
                "description": "Exit status after false",
                "expected_output": "1\n",
                "expected_exit_code": 0
            },
            {
                "name": "undefined_variable",
                "command": "echo \"[$UNDEFINED_VARIABLE_123]\"",
                "description": "Undefined variable expansion",
                "expected_output": "[]\n",
                "expected_exit_code": 0
            }
        ],
        "quotes": [
            {
                "name": "double_quotes_with_var",
                "command": "export TEST=world && echo \"hello $TEST\"",
                "description": "Variable expansion in double quotes",
                "expected_output": "hello world\n",
                "expected_exit_code": 0
            },
            {
                "name": "single_quotes_no_var",
                "command": "export TEST=world && echo 'hello $TEST'",
                "description": "No variable expansion in single quotes",
                "expected_output": "hello $TEST\n",
                "expected_exit_code": 0
            },
            {
                "name": "mixed_quotes",
                "command": "echo \"hello 'world'\"",
                "description": "Mixed quotes - double containing single",
                "expected_output": "hello 'world'\n",
                "expected_exit_code": 0
            },
            {
                "name": "mixed_quotes_reverse",
                "command": "echo 'hello \"world\"'",
                "description": "Mixed quotes - single containing double",
                "expected_output": "hello \"world\"\n",
                "expected_exit_code": 0
            }
        ],
        "errors": [
            {
                "name": "command_not_found",
                "command": "nonexistent_command_12345",
                "description": "Command not found error",
                "expected_exit_code": 127
            },
            {
                "name": "syntax_error_pipe_start",
                "command": "| echo hello",
                "description": "Syntax error - pipe at start",
                "expected_exit_code": 258
            },
            {
                "name": "syntax_error_pipe_end",
                "command": "echo hello |",
                "description": "Syntax error - pipe at end",
                "expected_exit_code": 258
            },
            {
                "name": "file_not_found",
                "command": "cat nonexistent_file_12345.txt",
                "description": "File not found error",
                "expected_exit_code": 1
            }
        ]
    }
    
    return test_cases

def main():
    parser = argparse.ArgumentParser(
        description="École 42 Minishell Test Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s bash
  %(prog)s ./minishell
  %(prog)s ./minishell --verbose
  %(prog)s ./minishell --category echo
  %(prog)s bash --timeout 10
        """
    )
    
    parser.add_argument('shell_program', help='Shell program to test (e.g., bash, ./minishell)')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output with detailed test information')
    parser.add_argument('--detailed', '-d', action='store_true', help='Show detailed output including stdout/stderr for all tests')
    parser.add_argument('--category', '-c', help='Run only tests from specific category')
    parser.add_argument('--timeout', '-t', type=int, default=5, help='Timeout for each test (seconds)')
    parser.add_argument('--json-file', '-j', default='test_cases.json', help='JSON file with test cases')
    parser.add_argument('--create-json', action='store_true', help='Create default test cases JSON file')
    parser.add_argument('--list-categories', action='store_true', help='List all available test categories')
    
    args = parser.parse_args()
    
    if args.create_json:
        test_cases = create_test_json()
        with open('test_cases.json', 'w', encoding='utf-8') as f:
            json.dump(test_cases, f, indent=2, ensure_ascii=False)
        print("✅ Created test_cases.json file")
        return
    
    # Check if shell program exists
    if not shutil.which(args.shell_program) and not os.path.exists(args.shell_program):
        print(f"❌ Shell program '{args.shell_program}' not found!")
        sys.exit(1)
    
    # Create JSON file if it doesn't exist
    if not os.path.exists(args.json_file):
        print(f"📝 Creating default test cases file: {args.json_file}")
        test_cases = create_test_json()
        with open(args.json_file, 'w', encoding='utf-8') as f:
            json.dump(test_cases, f, indent=2, ensure_ascii=False)
    
    # List categories if requested
    if args.list_categories:
        tester = MinishellTester(args.shell_program, args.verbose, args.timeout, args.detailed)
        test_cases = tester.load_test_cases(args.json_file)
        categories = set(test.category for test in test_cases)
        print("\033[1;36mAvailable test categories:\033[0m")
        for category in sorted(categories):
            count = len([t for t in test_cases if t.category == category])
            print(f"  \033[1;33m{category}\033[0m ({count} tests)")
        return

    # Run tests
    with MinishellTester(args.shell_program, args.verbose, args.timeout, args.detailed) as tester:
        test_cases = tester.load_test_cases(args.json_file)
        tester.run_tests(test_cases, args.category)

if __name__ == "__main__":
    main()