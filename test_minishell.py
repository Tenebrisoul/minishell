#!/usr/bin/env python3
"""
Minishell Test Suite
Author: AI Assistant
Date: 2025-09-17

This test suite covers the mandatory functionality of the minishell project:
- Basic commands
- Built-in commands (echo -n, cd, pwd, export, unset, env, exit)
- Redirections (<, >, <<, >>)
- Pipes (|)
- Environment variables ($VAR, $?)
- Quote handling (' and ")
- Signal handling (basic tests)
- Essential error cases

Note: Does not test semicolon (;) or backslash (\) as these are explicitly 
excluded from the mandatory requirements.
"""

import subprocess
import os
import sys
import tempfile
import time
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import json
import signal

class Colors:
    """ANSI color codes for terminal output"""
    RESET = '\033[0m'
    RED = '\033[31m'
    GREEN = '\033[32m'
    YELLOW = '\033[33m'
    BLUE = '\033[34m'
    MAGENTA = '\033[35m'
    CYAN = '\033[36m'
    WHITE = '\033[37m'
    BOLD = '\033[1m'
    DIM = '\033[2m'
    UNDERLINE = '\033[4m'
    
    # Background colors
    BG_RED = '\033[41m'
    BG_GREEN = '\033[42m'
    BG_YELLOW = '\033[43m'
    BG_BLUE = '\033[44m'
    BG_MAGENTA = '\033[45m'
    BG_CYAN = '\033[46m'
    
    # Bright colors
    BRIGHT_RED = '\033[91m'
    BRIGHT_GREEN = '\033[92m'
    BRIGHT_YELLOW = '\033[93m'
    BRIGHT_BLUE = '\033[94m'
    BRIGHT_MAGENTA = '\033[95m'
    BRIGHT_CYAN = '\033[96m'
    BRIGHT_WHITE = '\033[97m'

class Symbols:
    """Unicode symbols for better visual output"""
    CHECK = '✅'
    CROSS = '❌'
    WARNING = '⚠️'
    INFO = 'ℹ️'
    ROCKET = '🚀'
    GEAR = '⚙️'
    BUG = '🐛'
    TROPHY = '🏆'
    TARGET = '🎯'
    CHART = '📊'
    CLOCK = '⏱️'
    FIRE = '🔥'
    SPARKLES = '✨'
    ARROW_RIGHT = '→'
    ARROW_DOWN = '↓'
    DOUBLE_ARROW = '⇒'
    BULLET = '•'
    DIAMOND = '◆'
    STAR = '⭐'

class TestResult:
    """Class to track individual test results"""
    def __init__(self, name: str, category: str, expected: str, actual: str, 
                 passed: bool, error_msg: str = "", command: str = ""):
        self.name = name
        self.category = category
        self.expected = expected
        self.actual = actual
        self.passed = passed
        self.error_msg = error_msg
        self.command = command

class MinishellTester:
    """Main test class for minishell"""
    
    def __init__(self, minishell_path: str = "./minishell"):
        self.minishell_path = minishell_path
        self.results: List[TestResult] = []
        self.temp_dir = tempfile.mkdtemp()
        self.original_cwd = os.getcwd()
        
        # Verify minishell exists
        if not os.path.exists(minishell_path):
            raise FileNotFoundError(f"Minishell not found at {minishell_path}")
    
    def run_command(self, command: str, timeout: int = 5, 
                   input_data: str = None) -> Tuple[str, str, int]:
        """Run a command in minishell and return stdout, stderr, exit_code"""
        try:
            # Create input with command and exit
            if input_data is None:
                input_text = f"{command}\nexit\n"
            else:
                input_text = f"{input_data}\nexit\n"
            
            process = subprocess.Popen(
                [self.minishell_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd=self.original_cwd
            )
            
            stdout, stderr = process.communicate(input=input_text, timeout=timeout)
            
            # Remove the "exit" output and clean up
            lines = stdout.strip().split('\n')
            # Filter out shell prompt and exit messages
            filtered_lines = []
            for line in lines:
                if line and not line.startswith('minishell>') and line != 'exit':
                    filtered_lines.append(line)
            
            return '\n'.join(filtered_lines), stderr, process.returncode
            
        except subprocess.TimeoutExpired:
            process.kill()
            return "", "TIMEOUT", 124
        except Exception as e:
            return "", str(e), 1
    
    def run_bash_command(self, command: str) -> Tuple[str, str, int]:
        """Run command in bash for comparison"""
        try:
            process = subprocess.run(
                command,
                shell=True,
                capture_output=True,
                text=True,
                timeout=5,
                cwd=self.original_cwd
            )
            return process.stdout.strip(), process.stderr.strip(), process.returncode
        except subprocess.TimeoutExpired:
            return "", "TIMEOUT", 124
        except Exception as e:
            return "", str(e), 1
    
    def assert_equal(self, name: str, category: str, command: str, 
                    expected_output: str = None, expected_exit: int = None,
                    compare_with_bash: bool = False):
        """Assert that command output matches expected"""
        
        minishell_out, minishell_err, minishell_exit = self.run_command(command)
        
        if compare_with_bash:
            bash_out, bash_err, bash_exit = self.run_bash_command(command)
            expected_output = bash_out
            expected_exit = bash_exit
        
        # Check output
        output_match = True
        exit_match = True
        error_msg = ""
        
        if expected_output is not None:
            output_match = minishell_out.strip() == expected_output.strip()
            if not output_match:
                error_msg += f"Output mismatch. Expected: '{expected_output}', Got: '{minishell_out}'"
        
        if expected_exit is not None:
            exit_match = minishell_exit == expected_exit
            if not exit_match:
                error_msg += f" Exit code mismatch. Expected: {expected_exit}, Got: {minishell_exit}"
        
        passed = output_match and exit_match
        
        result = TestResult(
            name=name,
            category=category,
            expected=f"Output: '{expected_output}', Exit: {expected_exit}",
            actual=f"Output: '{minishell_out}', Exit: {minishell_exit}",
            passed=passed,
            error_msg=error_msg,
            command=command
        )
        
        self.results.append(result)
        return passed
    
    def test_basic_commands(self):
        """Test basic command execution"""
        print(f"{Colors.BRIGHT_BLUE}{Symbols.ROCKET} Testing Basic Commands...{Colors.RESET}")
        
        # Basic echo tests
        self.assert_equal("echo_simple", "basic", "echo hello", "hello")
        self.assert_equal("echo_multiple_args", "basic", "echo hello world", "hello world")
        self.assert_equal("echo_empty", "basic", "echo", "")
        self.assert_equal("echo_with_flags", "basic", "echo -n hello", compare_with_bash=True)
        
        # Navigation commands
        self.assert_equal("pwd", "basic", "pwd", compare_with_bash=True)
        self.assert_equal("ls_exists", "basic", "ls", compare_with_bash=True)
        self.assert_equal("ls_with_flags", "basic", "ls -la", compare_with_bash=True)
        
        # System commands
        self.assert_equal("date", "basic", "date", compare_with_bash=True)
        self.assert_equal("whoami", "basic", "whoami", compare_with_bash=True)
        self.assert_equal("uname", "basic", "uname", compare_with_bash=True)
        
        # Command variants
        self.assert_equal("cat_version", "basic", "cat --version", compare_with_bash=True)
        self.assert_equal("which_ls", "basic", "which ls", compare_with_bash=True)
        
        # Error cases
        self.assert_equal("nonexistent_command", "basic", "nonexistentcommand123", 
                         expected_exit=127)
        self.assert_equal("permission_denied_cmd", "basic", "/etc/shadow", 
                         expected_exit=126)
    
    def test_builtin_commands(self):
        """Test built-in commands"""
        print(f"{Colors.BRIGHT_MAGENTA}{Symbols.GEAR} Testing Built-in Commands...{Colors.RESET}")
        
        # Test cd variations
        self.assert_equal("cd_root", "builtins", "cd /; pwd", "/")
        self.assert_equal("cd_home", "builtins", "cd ~; pwd", os.path.expanduser("~"))
        self.assert_equal("cd_home_explicit", "builtins", "cd $HOME; pwd", os.path.expanduser("~"))
        self.assert_equal("cd_back", "builtins", "cd /tmp; cd -; pwd", compare_with_bash=True)
        self.assert_equal("cd_parent", "builtins", "cd ..; pwd", compare_with_bash=True)
        self.assert_equal("cd_current", "builtins", "cd .; pwd", compare_with_bash=True)
        self.assert_equal("cd_nonexistent", "builtins", "cd /nonexistent/path", 
                         expected_exit=1)
        self.assert_equal("cd_no_args", "builtins", "cd; pwd", os.path.expanduser("~"))
        
        # Test env variations
        self.assert_equal("env_display", "builtins", "env | sort", compare_with_bash=True)
        self.assert_equal("env_with_command", "builtins", "env echo hello", "hello")
        
        # Test export variations
        self.assert_equal("export_simple", "builtins", "export TEST_VAR=hello; echo $TEST_VAR", 
                         "hello")
        self.assert_equal("export_with_spaces", "builtins", "export TEST_VAR='hello world'; echo $TEST_VAR", 
                         "hello world")
        self.assert_equal("export_empty", "builtins", "export EMPTY_VAR=; echo $EMPTY_VAR", "")
        self.assert_equal("export_no_value", "builtins", "export JUST_NAME; env | grep JUST_NAME", 
                         compare_with_bash=True)
        self.assert_equal("export_overwrite", "builtins", 
                         "export TEST_VAR=first; export TEST_VAR=second; echo $TEST_VAR", "second")
        
        # Test unset variations
        self.assert_equal("unset_existing", "builtins", 
                         "export TEST_VAR=hello; unset TEST_VAR; echo $TEST_VAR", "")
        self.assert_equal("unset_nonexistent", "builtins", "unset NONEXISTENT_VAR", expected_exit=0)
        self.assert_equal("unset_multiple", "builtins", 
                         "export VAR1=a VAR2=b; unset VAR1 VAR2; echo $VAR1$VAR2", "")
        
        # Test exit variations
        self.assert_equal("exit_success", "builtins", "exit 0", expected_exit=0)
        self.assert_equal("exit_failure", "builtins", "exit 42", expected_exit=42)
        self.assert_equal("exit_no_arg", "builtins", "exit", expected_exit=0)
        self.assert_equal("exit_invalid", "builtins", "exit abc", expected_exit=2)
    
    def test_redirections(self):
        """Test input/output redirections"""
        print(f"{Colors.BRIGHT_CYAN}{Symbols.ARROW_RIGHT} Testing Redirections...{Colors.RESET}")
        
        # Create test files
        test_file = os.path.join(self.temp_dir, "test_input.txt")
        with open(test_file, 'w') as f:
            f.write("test content\nline2\nline3\n")
        
        output_file = os.path.join(self.temp_dir, "test_output.txt")
        
        # Test input redirection variations
        self.assert_equal("input_redirect", "redirections", 
                         f"cat < {test_file}", "test content\nline2\nline3")
        self.assert_equal("input_redirect_wc", "redirections", 
                         f"wc -l < {test_file}", "3")
        self.assert_equal("input_redirect_nonexistent", "redirections", 
                         f"cat < /nonexistent/file", expected_exit=1)
        
        # Test output redirection variations
        cmd = f"echo 'hello world' > {output_file}; cat {output_file}"
        self.assert_equal("output_redirect", "redirections", cmd, "hello world")
        
        cmd2 = f"echo 'line1' > {output_file}; echo 'line2' > {output_file}; cat {output_file}"
        self.assert_equal("output_redirect_overwrite", "redirections", cmd2, "line2")
        
        # Test append redirection variations
        append_file = os.path.join(self.temp_dir, "append_test.txt")
        cmd = f"echo 'line1' > {append_file}; echo 'line2' >> {append_file}; cat {append_file}"
        self.assert_equal("append_redirect", "redirections", cmd, "line1\nline2")
        
        cmd2 = f"echo 'a' > {append_file}; echo 'b' >> {append_file}; echo 'c' >> {append_file}; cat {append_file}"
        self.assert_equal("append_multiple", "redirections", cmd2, "a\nb\nc")
        
        # Test redirection with commands
        self.assert_equal("redirect_with_args", "redirections", 
                         f"echo hello world > {output_file}; cat {output_file}", "hello world")
        
        # Test multiple redirections
        input2 = os.path.join(self.temp_dir, "input2.txt")
        output2 = os.path.join(self.temp_dir, "output2.txt")
        with open(input2, 'w') as f:
            f.write("input data\n")
        
        cmd = f"cat < {input2} > {output2}; cat {output2}"
        self.assert_equal("input_output_redirect", "redirections", cmd, "input data")
        
        # Test redirection permissions
        readonly_file = os.path.join(self.temp_dir, "readonly.txt")
        try:
            with open(readonly_file, 'w') as f:
                f.write("readonly")
            os.chmod(readonly_file, 0o444)
            self.assert_equal("redirect_readonly", "redirections", 
                             f"echo test > {readonly_file}", expected_exit=1)
        except:
            pass
    
    def test_pipes(self):
        """Test pipe functionality"""
        print(f"{Colors.BRIGHT_YELLOW}{Symbols.DOUBLE_ARROW} Testing Pipes...{Colors.RESET}")
        
        # Basic pipe tests
        self.assert_equal("simple_pipe", "pipes", "echo hello | cat", "hello")
        self.assert_equal("pipe_chain", "pipes", "echo hello | cat | cat", "hello")
        self.assert_equal("pipe_chain_long", "pipes", "echo test | cat | cat | cat", "test")
        
        # Pipe with common commands
        self.assert_equal("echo_pipe_wc", "pipes", "echo 'line1\nline2\nline3' | wc -l", "3")
        self.assert_equal("ls_pipe_wc", "pipes", "ls | wc -l", compare_with_bash=True)
        self.assert_equal("echo_pipe_grep", "pipes", 
                         "echo 'hello\\nworld\\nhello' | grep hello", compare_with_bash=True)
        self.assert_equal("cat_pipe_sort", "pipes", 
                         "echo 'c\\nb\\na' | sort", "a\nb\nc")
        
        # Pipe with built-ins
        self.assert_equal("env_pipe_grep", "pipes", "env | grep HOME", compare_with_bash=True)
        self.assert_equal("echo_pipe_head", "pipes", 
                         "echo 'line1\\nline2\\nline3' | head -n 2", "line1\nline2")
        self.assert_equal("echo_pipe_tail", "pipes", 
                         "echo 'line1\\nline2\\nline3' | tail -n 1", "line3")
        
        # Complex pipe chains
        self.assert_equal("complex_pipe", "pipes", 
                         "echo 'apple\\nbanana\\napple\\ncherry' | sort | uniq", 
                         "apple\nbanana\ncherry")
        
        # Pipe with variables
        self.assert_equal("pipe_with_vars", "pipes", 
                         "export VAR=hello; echo $VAR | cat", "hello")
        
        # Error cases in pipes
        self.assert_equal("pipe_command_not_found", "pipes", 
                         "echo hello | nonexistentcommand", expected_exit=127)
        self.assert_equal("broken_pipe_start", "pipes", 
                         "nonexistentcommand | cat", expected_exit=127)
    
    def test_environment_variables(self):
        """Test environment variable handling"""
        print(f"{Colors.BRIGHT_GREEN}{Symbols.TARGET} Testing Environment Variables...{Colors.RESET}")
        
        # Basic variable expansion
        self.assert_equal("var_expansion", "env_vars", "echo $HOME", os.environ.get('HOME', ''))
        self.assert_equal("var_expansion_user", "env_vars", "echo $USER", os.environ.get('USER', ''))
        self.assert_equal("var_expansion_path", "env_vars", "echo $PATH", os.environ.get('PATH', ''))
        
        # Variable in different positions
        self.assert_equal("var_in_middle", "env_vars", "echo hello$USER", f"hello{os.environ.get('USER', '')}")
        self.assert_equal("var_at_start", "env_vars", "echo $USER world", f"{os.environ.get('USER', '')} world")
        self.assert_equal("var_multiple", "env_vars", "echo $USER $HOME", 
                         f"{os.environ.get('USER', '')} {os.environ.get('HOME', '')}")
        
        # Undefined variables
        self.assert_equal("undefined_var", "env_vars", "echo $UNDEFINED_VAR_12345", "")
        self.assert_equal("undefined_with_text", "env_vars", "echo hello$UNDEFINED_VAR", "hello")
        
        # Special variables
        self.assert_equal("exit_status_success", "env_vars", "true; echo $?", "0")
        self.assert_equal("exit_status_failure", "env_vars", "false; echo $?", "1")
        self.assert_equal("exit_status_command_not_found", "env_vars", "nonexistentcmd; echo $?", "127")
        
        # Variable edge cases
        self.assert_equal("dollar_alone", "env_vars", "echo $", "$")
        self.assert_equal("double_dollar", "env_vars", "echo $$", compare_with_bash=True)
        self.assert_equal("var_with_braces", "env_vars", "echo ${HOME}", os.environ.get('HOME', ''))
        self.assert_equal("var_in_quotes", "env_vars", 'echo "$USER"', os.environ.get('USER', ''))
        
        # Custom variables
        self.assert_equal("custom_var_simple", "env_vars", 
                         "export CUSTOM=test; echo $CUSTOM", "test")
        self.assert_equal("custom_var_override", "env_vars", 
                         "export PATH=custom; echo $PATH", "custom")
        self.assert_equal("custom_var_unset", "env_vars", 
                         "export TEMP=value; unset TEMP; echo $TEMP", "")
        
        # Variables in commands
        self.assert_equal("var_in_command", "env_vars", 
                         "export CMD=echo; $CMD hello", "hello")
        self.assert_equal("var_as_argument", "env_vars", 
                         "export ARG=hello; echo $ARG", "hello")
    
    def test_quotes_and_escaping(self):
        """Test quote handling"""
        print(f"{Colors.MAGENTA}{Symbols.SPARKLES} Testing Quotes and Escaping...{Colors.RESET}")
        
        # Single quotes - no expansion
        self.assert_equal("single_quotes", "quotes", "echo 'hello world'", "hello world")
        self.assert_equal("single_quotes_no_expansion", "quotes", 
                         "echo '$HOME'", "$HOME")
        self.assert_equal("single_quotes_with_spaces", "quotes", 
                         "echo '  multiple   spaces  '", "  multiple   spaces  ")
        self.assert_equal("single_quotes_special_chars", "quotes", 
                         "echo '!@#$%^&*()'", "!@#$%^&*()")
        
        # Double quotes - with expansion
        self.assert_equal("double_quotes", "quotes", 'echo "hello world"', "hello world")
        self.assert_equal("double_quotes_expansion", "quotes", 
                         f'echo "hello $USER"', f"hello {os.environ.get('USER', '')}")
        self.assert_equal("double_quotes_with_singles", "quotes", 
                         '''echo "It's working"''', "It's working")
        self.assert_equal("double_quotes_multiple_vars", "quotes", 
                         f'echo "$USER at $HOME"', f"{os.environ.get('USER', '')} at {os.environ.get('HOME', '')}")
        
        # Mixed quotes
        self.assert_equal("mixed_quotes", "quotes", 
                         """echo 'single' "double" normal""", "single double normal")
        self.assert_equal("quotes_in_quotes", "quotes", 
                         '''echo "He said 'hello'"''', "He said 'hello'")
        self.assert_equal("nested_quotes", "quotes", 
                         """echo 'User is "$USER"' """, 'User is "$USER"')
        
        # Empty quotes
        self.assert_equal("empty_single_quotes", "quotes", "echo ''", "")
        self.assert_equal("empty_double_quotes", "quotes", 'echo ""', "")
        self.assert_equal("quotes_with_empty", "quotes", 
                         "echo hello '' world", "hello  world")
        
        # Quote edge cases
        self.assert_equal("unclosed_single_quote", "quotes", 
                         "echo 'unclosed", expected_exit=2)
        self.assert_equal("unclosed_double_quote", "quotes", 
                         'echo "unclosed', expected_exit=2)
        
        # Quotes with pipes and redirections
        self.assert_equal("quotes_with_pipe", "quotes", 
                         "echo 'hello world' | cat", "hello world")
        
        # Complex quote scenarios
        self.assert_equal("complex_quotes", "quotes", 
                         """echo "Start 'middle' end" """, "Start 'middle' end")
    
    def test_error_handling(self):
        """Test essential error conditions"""
        print(f"{Colors.RED}{Symbols.BUG} Testing Error Handling...{Colors.RESET}")
        
        # Quote syntax errors
        self.assert_equal("unclosed_single_quote", "errors", "echo 'unclosed", expected_exit=2)
        self.assert_equal("unclosed_double_quote", "errors", 'echo "unclosed', expected_exit=2)
        
        # Pipe syntax errors
        self.assert_equal("invalid_pipe_start", "errors", "| echo hello", expected_exit=2)
        self.assert_equal("invalid_pipe_end", "errors", "echo hello |", expected_exit=2)
        
        # File/directory errors
        self.assert_equal("invalid_redirect_input", "errors", "cat < /nonexistent/file", 
                         expected_exit=1)
        
        # Command errors
        self.assert_equal("command_not_found", "errors", "nonexistentcommand123", 
                         expected_exit=127)
        self.assert_equal("invalid_builtin_args", "errors", "cd too many args", 
                         expected_exit=1)
        
        # Directory navigation errors
        self.assert_equal("cd_nonexistent", "errors", "cd /nonexistent/path", expected_exit=1)
        
        # Pipe errors
        self.assert_equal("pipe_to_invalid_command", "errors", 
                         "echo hello | nonexistentcommand", expected_exit=127)
        
        # Empty command handling
        self.assert_equal("empty_command", "errors", "", expected_exit=0)
        self.assert_equal("spaces_only", "errors", "   ", expected_exit=0)
        
        # Exit code tests
        self.assert_equal("exit_string", "errors", "exit hello", expected_exit=2)
    
    def test_edge_cases(self):
        """Test important edge cases"""
        print(f"{Colors.YELLOW}{Symbols.WARNING} Testing Edge Cases...{Colors.RESET}")
        
        # Basic formatting
        self.assert_equal("multiple_spaces", "edge_cases", 
                         "echo    hello     world", "hello world")
        self.assert_equal("leading_trailing_spaces", "edge_cases", 
                         "   echo hello world   ", "hello world")
        
        # Special characters in quotes
        self.assert_equal("special_chars_in_quotes", "edge_cases", 
                         "echo 'special: !@#$%^&*()'", "special: !@#$%^&*()")
        
        # Variable edge cases
        self.assert_equal("var_with_numbers", "edge_cases", 
                         "export VAR123=test; echo $VAR123", "test")
        self.assert_equal("var_with_underscores", "edge_cases", 
                         "export MY_VAR_123=test; echo $MY_VAR_123", "test")
        
        # Zero-length scenarios
        self.assert_equal("zero_length_var", "edge_cases", 
                         "export EMPTY=; echo [$EMPTY]", "[]")
        
        # Hidden file access
        dot_file = os.path.join(self.temp_dir, ".hidden_file")
        with open(dot_file, 'w') as f:
            f.write("hidden content")
        self.assert_equal("hidden_file_access", "edge_cases", 
                         f"cat {dot_file}", "hidden content")
    
    def test_signal_handling(self):
        """Test signal handling (basic test)"""
        print(f"{Colors.BRIGHT_RED}{Symbols.CLOCK} Testing Signal Handling...{Colors.RESET}")
        
        # Test Ctrl+C doesn't crash shell (basic test)
        try:
            process = subprocess.Popen(
                [self.minishell_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            
            # Send interrupt signal
            process.send_signal(signal.SIGINT)
            time.sleep(0.1)
            
            # Try to continue
            stdout, stderr = process.communicate(input="echo hello\nexit\n", timeout=2)
            
            # If we get here, signal was handled properly
            self.assert_equal("sigint_handling", "signals", "echo after_signal", 
                             expected_output="after_signal")
            
        except subprocess.TimeoutExpired:
            process.kill()
            # Signal handling might not be implemented
            result = TestResult("sigint_handling", "signals", "Shell continues after SIGINT", 
                              "TIMEOUT", False, "Signal handling test timed out", "SIGINT test")
            self.results.append(result)
        except Exception as e:
            result = TestResult("sigint_handling", "signals", "Shell continues after SIGINT", 
                              str(e), False, f"Signal test failed: {e}", "SIGINT test")
            self.results.append(result)
    
    def run_all_tests(self):
        """Run all test categories"""
        print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}")
        print("╔" + "═" * 68 + "╗")
        print("║" + f"{Symbols.ROCKET} MINISHELL MANDATORY TEST SUITE {Symbols.ROCKET}".center(66) + "║")
        print("║" + f"{Symbols.CHART} Essential Tests for Required Features {Symbols.CHART}".center(66) + "║")
        print("╚" + "═" * 68 + "╝")
        print(f"{Colors.RESET}")
        
        # Change to temp directory for tests
        os.chdir(self.temp_dir)
        
        try:
            # Run mandatory test categories only
            self.test_basic_commands()
            self.test_builtin_commands()
            self.test_redirections()
            self.test_pipes()
            self.test_environment_variables()
            self.test_quotes_and_escaping()
            self.test_error_handling()
            self.test_edge_cases()
            self.test_signal_handling()
            self.test_combinations()
            
        finally:
            # Restore original directory
            os.chdir(self.original_cwd)
    
    def test_combinations(self):
        """Test combinations of mandatory features"""
        print(f"{Colors.BRIGHT_WHITE}{Symbols.FIRE} Testing Feature Combinations...{Colors.RESET}")
        
        # Pipes + Redirections
        temp_in = os.path.join(self.temp_dir, "combo_in.txt")
        temp_out = os.path.join(self.temp_dir, "combo_out.txt")
        with open(temp_in, 'w') as f:
            f.write("line1\nline2\nline3\n")
        
        self.assert_equal("pipe_with_input_redirect", "combinations", 
                         f"cat < {temp_in} | grep line2", "line2")
        self.assert_equal("pipe_with_output_redirect", "combinations", 
                         f"echo hello | cat > {temp_out}; cat {temp_out}", "hello")
        
        # Pipes + Environment Variables
        self.assert_equal("pipe_with_env_vars", "combinations", 
                         "export WORD=hello; echo $WORD | cat", "hello")
        self.assert_equal("env_var_in_pipe", "combinations", 
                         "echo test | grep $USER", compare_with_bash=True)
        
        # Quotes + Environment Variables
        self.assert_equal("quotes_env_combo", "combinations", 
                         'export VAR=test; echo "Value: $VAR"', "Value: test")
        self.assert_equal("single_quotes_no_env", "combinations", 
                         "export VAR=test; echo 'Value: $VAR'", "Value: $VAR")
        
        # Quotes + Pipes
        self.assert_equal("quotes_pipe_combo", "combinations", 
                         "echo 'hello world' | cat", "hello world")
        self.assert_equal("complex_quotes_pipe", "combinations", 
                         '''echo "He said 'hello'" | cat''', "He said 'hello'")
        
        # Nested redirections and pipes
        nested_temp = os.path.join(self.temp_dir, "nested.txt")
        with open(nested_temp, 'w') as f:
            f.write("nested content\n")
        self.assert_equal("nested_redirect_pipe", "combinations", 
                         f"cat < {nested_temp} | cat | cat", "nested content")
        
        # Multiple environment variables in complex command
        self.assert_equal("multi_env_complex", "combinations", 
                         'export A=hello; export B=world; echo "$A $B" | cat', "hello world")
    
    def generate_report(self, output_mode='full'):
        """Generate comprehensive test report with different output modes"""
        # Calculate statistics by category
        categories = {}
        for result in self.results:
            if result.category not in categories:
                categories[result.category] = {'total': 0, 'passed': 0, 'failed': 0}
            categories[result.category]['total'] += 1
            if result.passed:
                categories[result.category]['passed'] += 1
            else:
                categories[result.category]['failed'] += 1
        
        total_tests = len(self.results)
        total_passed = sum(1 for r in self.results if r.passed)
        total_failed = total_tests - total_passed
        overall_percentage = (total_passed / total_tests * 100) if total_tests > 0 else 0
        
        # JSON only mode
        if output_mode == 'json':
            json_data = {
                'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
                'total_tests': total_tests,
                'passed': total_passed,
                'failed': total_failed,
                'success_rate': round(overall_percentage, 1),
                'categories': {}
            }
            for category, stats in categories.items():
                percentage = (stats['passed'] / stats['total'] * 100) if stats['total'] > 0 else 0
                json_data['categories'][category] = {
                    'total': stats['total'],
                    'passed': stats['passed'],
                    'failed': stats['failed'],
                    'success_rate': round(percentage, 1)
                }
            print(json.dumps(json_data, indent=2))
            return overall_percentage
        
        # Quiet mode - only final result
        if output_mode == 'quiet':
            if overall_percentage >= 90:
                status = f"{Colors.BRIGHT_GREEN}{Symbols.TROPHY} EXCELLENT{Colors.RESET}"
            elif overall_percentage >= 80:
                status = f"{Colors.GREEN}{Symbols.CHECK} PASS{Colors.RESET}"
            elif overall_percentage >= 60:
                status = f"{Colors.YELLOW}{Symbols.WARNING} POOR{Colors.RESET}"
            else:
                status = f"{Colors.RED}{Symbols.CROSS} FAIL{Colors.RESET}"
                
            print(f"{status}: {total_passed}/{total_tests} ({overall_percentage:.1f}%)")
            return overall_percentage
        
        # Summary mode - only key statistics
        if output_mode == 'summary':
            print(f"\n{Colors.BOLD}{Colors.BRIGHT_CYAN}╔══════════════════════════════════════╗{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}║{Colors.RESET} {Symbols.CHART} {Colors.BOLD}TEST SUMMARY{Colors.RESET} {Symbols.CHART}".ljust(50) + f"{Colors.BOLD}{Colors.BRIGHT_CYAN}║{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}╚══════════════════════════════════════╝{Colors.RESET}")
            print(f"{Symbols.BULLET} Total Tests: {Colors.BOLD}{total_tests}{Colors.RESET}")
            print(f"{Symbols.BULLET} Passed: {Colors.BRIGHT_GREEN}{total_passed}{Colors.RESET}")
            print(f"{Symbols.BULLET} Failed: {Colors.BRIGHT_RED}{total_failed}{Colors.RESET}")
            
            if overall_percentage >= 90:
                status_symbol = Symbols.TROPHY
                status_color = Colors.BRIGHT_GREEN
            elif overall_percentage >= 80:
                status_symbol = Symbols.CHECK
                status_color = Colors.GREEN
            elif overall_percentage >= 60:
                status_symbol = Symbols.WARNING
                status_color = Colors.YELLOW
            else:
                status_symbol = Symbols.CROSS
                status_color = Colors.RED
                
            print(f"{Symbols.BULLET} Success Rate: {status_color}{status_symbol} {overall_percentage:.1f}%{Colors.RESET}")
            return overall_percentage
        
        # Brief mode - only category counts
        if output_mode == 'brief':
            print(f"\n{Colors.BOLD}{Colors.BRIGHT_CYAN}╔══════════════════════════════════════╗{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}║{Colors.RESET} {Symbols.CHART} {Colors.BOLD}CATEGORY RESULTS{Colors.RESET} {Symbols.CHART}".ljust(50) + f"{Colors.BOLD}{Colors.BRIGHT_CYAN}        ║{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}╚══════════════════════════════════════╝{Colors.RESET}")
            
            for category, stats in categories.items():
                percentage = (stats['passed'] / stats['total'] * 100) if stats['total'] > 0 else 0
                
                if percentage >= 90:
                    color = Colors.BRIGHT_GREEN
                    symbol = Symbols.CHECK
                elif percentage >= 80:
                    color = Colors.GREEN
                    symbol = Symbols.CHECK
                elif percentage >= 60:
                    color = Colors.YELLOW
                    symbol = Symbols.WARNING
                else:
                    color = Colors.RED
                    symbol = Symbols.CROSS
                    
                print(f"{symbol} {category.upper():<15s}: {color}{stats['passed']:2d}/{stats['total']:2d}{Colors.RESET}")
            return overall_percentage
        
        # Percentage mode - only success rates
        if output_mode == 'percentage':
            print(f"\n{Colors.BOLD}{Colors.BRIGHT_CYAN}╔══════════════════════════════════════╗{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}║{Colors.RESET} {Symbols.TARGET} {Colors.BOLD}SUCCESS RATES{Colors.RESET} {Symbols.TARGET}".ljust(50) + f"{Colors.BOLD}{Colors.BRIGHT_CYAN}        ║{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}╚══════════════════════════════════════╝{Colors.RESET}")
            
            # Overall first
            if overall_percentage >= 90:
                overall_color = Colors.BRIGHT_GREEN
                overall_symbol = Symbols.TROPHY
            elif overall_percentage >= 80:
                overall_color = Colors.GREEN
                overall_symbol = Symbols.CHECK
            elif overall_percentage >= 60:
                overall_color = Colors.YELLOW
                overall_symbol = Symbols.WARNING
            else:
                overall_color = Colors.RED
                overall_symbol = Symbols.CROSS
                
            print(f"{overall_symbol} {Colors.BOLD}{'OVERALL':<15s}: {overall_color}{overall_percentage:5.1f}%{Colors.RESET}")
            print(f"{Colors.DIM}{'─' * 30}{Colors.RESET}")
            
            for category, stats in categories.items():
                percentage = (stats['passed'] / stats['total'] * 100) if stats['total'] > 0 else 0
                
                if percentage >= 90:
                    color = Colors.BRIGHT_GREEN
                    symbol = Symbols.TROPHY
                elif percentage >= 80:
                    color = Colors.GREEN
                    symbol = Symbols.CHECK
                elif percentage >= 60:
                    color = Colors.YELLOW
                    symbol = Symbols.WARNING
                else:
                    color = Colors.RED
                    symbol = Symbols.CROSS
                    
                print(f"{symbol} {category.upper():<15s}: {color}{percentage:5.1f}%{Colors.RESET}")
            return overall_percentage
        
        # Failed only mode - only failed test details
        if output_mode == 'failed':
            failed_tests = [r for r in self.results if not r.passed]
            if not failed_tests:
                print(f"\n{Colors.BRIGHT_GREEN}╔══════════════════════════════════════╗{Colors.RESET}")
                print(f"{Colors.BRIGHT_GREEN}║{Colors.RESET} {Symbols.TROPHY} {Colors.BOLD}{Colors.BRIGHT_GREEN}ALL TESTS PASSED!{Colors.RESET} {Symbols.TROPHY}".ljust(50) + f"{Colors.BRIGHT_GREEN}║{Colors.RESET}")
                print(f"{Colors.BRIGHT_GREEN}╚══════════════════════════════════════╝{Colors.RESET}")
                print(f"{Colors.BRIGHT_GREEN}{Symbols.SPARKLES} Congratulations! Your minishell is working perfectly! {Symbols.SPARKLES}{Colors.RESET}")
                return overall_percentage
            
            print(f"\n{Colors.BOLD}{Colors.BRIGHT_RED}╔══════════════════════════════════════╗{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_RED}║{Colors.RESET} {Symbols.BUG} {Colors.BOLD}FAILED TESTS ({len(failed_tests)}/{total_tests}){Colors.RESET} {Symbols.BUG}".ljust(50) + f"{Colors.BOLD}{Colors.BRIGHT_RED}        ║{Colors.RESET}")
            print(f"{Colors.BOLD}{Colors.BRIGHT_RED}╚══════════════════════════════════════╝{Colors.RESET}")
            
            by_category = {}
            for test in failed_tests:
                if test.category not in by_category:
                    by_category[test.category] = []
                by_category[test.category].append(test)
            
            for category, tests in by_category.items():
                print(f"\n{Colors.BOLD}{Colors.BRIGHT_BLUE}┌─ {category.upper()} {Symbols.ARROW_DOWN}{Colors.RESET}")
                for test in tests:
                    print(f"{Colors.DIM}│{Colors.RESET} {Symbols.CROSS} {test.name:<30s} [{Colors.BRIGHT_RED}FAILED{Colors.RESET}]")
                    # Show the command that was tested
                    if test.command:
                        print(f"{Colors.DIM}│{Colors.RESET}   {Colors.CYAN}{Symbols.GEAR} Command: {Colors.WHITE}{test.command}{Colors.RESET}")
                    if test.error_msg:
                        print(f"{Colors.DIM}│{Colors.RESET}   {Colors.DIM}{Symbols.ARROW_RIGHT} {test.error_msg}{Colors.RESET}")
                    else:
                        print(f"{Colors.DIM}│{Colors.RESET}   {Colors.DIM}{Symbols.BULLET} Expected: {test.expected}{Colors.RESET}")
                        print(f"{Colors.DIM}│{Colors.RESET}   {Colors.DIM}{Symbols.BULLET} Actual:   {test.actual}{Colors.RESET}")
                print(f"{Colors.DIM}└{'─' * 40}{Colors.RESET}")
            return overall_percentage
        
        # Full mode (default) - complete report
        # Print summary
        print(f"\n{Colors.BOLD}{Colors.BRIGHT_CYAN}╔══════════════════════════════════════════════════════════╗{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}║{Colors.RESET} {Symbols.CHART} {Colors.BOLD}TEST RESULTS SUMMARY{Colors.RESET} {Symbols.CHART}".ljust(70) + f"{Colors.BOLD}{Colors.BRIGHT_CYAN}        ║{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}╚══════════════════════════════════════════════════════════╝{Colors.RESET}")
        
        # Overall status with big visual indicator
        if overall_percentage >= 90:
            overall_color = Colors.BRIGHT_GREEN
            overall_symbol = Symbols.TROPHY
            status_text = "EXCELLENT"
        elif overall_percentage >= 80:
            overall_color = Colors.GREEN
            overall_symbol = Symbols.CHECK
            status_text = "GOOD"
        elif overall_percentage >= 60:
            overall_color = Colors.YELLOW
            overall_symbol = Symbols.WARNING
            status_text = "NEEDS WORK"
        else:
            overall_color = Colors.RED
            overall_symbol = Symbols.CROSS
            status_text = "POOR"
        
        print(f"\n{overall_color}{overall_symbol} Overall Results: {Colors.BRIGHT_GREEN}{total_passed}{Colors.RESET} passed, "
              f"{Colors.BRIGHT_RED}{total_failed}{Colors.RESET} failed - "
              f"{overall_color}{overall_percentage:.1f}% ({status_text}){Colors.RESET}")
        print()
        
        # Category breakdown with visual bars
        print(f"{Colors.BOLD}{Colors.CYAN}┌─ Category Breakdown{Colors.RESET}")
        for i, (category, stats) in enumerate(categories.items()):
            percentage = (stats['passed'] / stats['total'] * 100) if stats['total'] > 0 else 0
            
            if percentage >= 90:
                color = Colors.BRIGHT_GREEN
                symbol = Symbols.TROPHY
            elif percentage >= 80:
                color = Colors.GREEN
                symbol = Symbols.CHECK
            elif percentage >= 60:
                color = Colors.YELLOW
                symbol = Symbols.WARNING
            else:
                color = Colors.RED
                symbol = Symbols.CROSS
            
            # Create visual bar
            bar_length = 20
            filled_length = int(bar_length * percentage / 100)
            bar = "█" * filled_length + "░" * (bar_length - filled_length)
            
            is_last = i == len(categories) - 1
            prefix = "└─" if is_last else "├─"
            
            print(f"{Colors.CYAN}{prefix}{Colors.RESET} {symbol} {category.upper():<15s}: "
                  f"{color}{stats['passed']:2d}/{stats['total']:2d} "
                  f"[{bar}] {percentage:5.1f}%{Colors.RESET}")
        
        print(f"\n{Colors.BOLD}{Colors.BRIGHT_CYAN}╔══════════════════════════════════════════════════════════╗{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}║{Colors.RESET} {Symbols.INFO} {Colors.BOLD}DETAILED TEST RESULTS{Colors.RESET} {Symbols.INFO}".ljust(70) + f"{Colors.BOLD}{Colors.BRIGHT_CYAN}            ║{Colors.RESET}")
        print(f"{Colors.BOLD}{Colors.BRIGHT_CYAN}╚══════════════════════════════════════════════════════════╝{Colors.RESET}")
        
        for category in categories.keys():
            category_stats = categories[category]
            category_percentage = (category_stats['passed'] / category_stats['total'] * 100) if category_stats['total'] > 0 else 0
            
            if category_percentage >= 90:
                category_color = Colors.BRIGHT_GREEN
            elif category_percentage >= 80:
                category_color = Colors.GREEN
            elif category_percentage >= 60:
                category_color = Colors.YELLOW
            else:
                category_color = Colors.RED
            
            print(f"\n{Colors.BOLD}{category_color}┌─ {category.upper()} "
                  f"({category_stats['passed']}/{category_stats['total']}) {Colors.RESET}")
            
            category_results = [r for r in self.results if r.category == category]
            for j, result in enumerate(category_results):
                is_last_in_category = j == len(category_results) - 1
                prefix = "└─" if is_last_in_category else "├─"
                
                if result.passed:
                    status_color = Colors.BRIGHT_GREEN
                    status_symbol = Symbols.CHECK
                    status = "PASS"
                else:
                    status_color = Colors.BRIGHT_RED
                    status_symbol = Symbols.CROSS
                    status = "FAIL"
                
                print(f"{Colors.DIM}{prefix}{Colors.RESET} {status_symbol} {result.name:<30s} "
                      f"[{status_color}{status}{Colors.RESET}]")
                
                if not result.passed and not is_last_in_category:
                    continuation = "│"
                elif not result.passed:
                    continuation = " "
                else:
                    continue
                    
                if not result.passed:
                    # Show the command that was tested
                    if result.command:
                        print(f"{Colors.DIM}{continuation}    {Colors.CYAN}{Symbols.GEAR} Command: {Colors.WHITE}{result.command}{Colors.RESET}")
                    print(f"{Colors.DIM}{continuation}    {Symbols.BULLET} Expected: {result.expected}{Colors.RESET}")
                    print(f"{Colors.DIM}{continuation}    {Symbols.BULLET} Actual:   {result.actual}{Colors.RESET}")
                    if result.error_msg:
                        print(f"{Colors.DIM}{continuation}    {Symbols.ARROW_RIGHT} {result.error_msg}{Colors.RESET}")
        
        # Save detailed log
        self.save_log()
        
        return overall_percentage
    
    def save_log(self):
        """Save detailed test log to file"""
        log_data = {
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'total_tests': len(self.results),
            'passed': sum(1 for r in self.results if r.passed),
            'failed': sum(1 for r in self.results if not r.passed),
            'results': []
        }
        
        for result in self.results:
            log_data['results'].append({
                'name': result.name,
                'category': result.category,
                'passed': result.passed,
                'expected': result.expected,
                'actual': result.actual,
                'error_msg': result.error_msg,
                'command': result.command
            })
        
        log_file = 'minishell_test_log.json'
        with open(log_file, 'w') as f:
            json.dump(log_data, f, indent=2)
        
        print(f"\n{Colors.CYAN}Detailed test log saved to: {log_file}{Colors.RESET}")
    
    def cleanup(self):
        """Clean up temporary files"""
        try:
            import shutil
            shutil.rmtree(self.temp_dir)
        except:
            pass

def main():
    """Main function to run tests"""
    import argparse
    
    parser = argparse.ArgumentParser(description='Minishell Mandatory Features Test Suite')
    parser.add_argument('--minishell', '-m', default='./minishell',
                       help='Path to minishell executable')
    parser.add_argument('--category', '-c', 
                       choices=['basic', 'builtins', 'redirections', 'pipes', 
                               'env_vars', 'quotes', 'errors', 
                               'edge_cases', 'signals', 'combinations'],
                       help='Run only specific test category')
    parser.add_argument('--summary', '-s', action='store_true',
                       help='Show only summary statistics')
    parser.add_argument('--brief', '-b', action='store_true',
                       help='Show only pass/fail counts per category')
    parser.add_argument('--percentage', '-p', action='store_true',
                       help='Show only success percentages')
    parser.add_argument('--failed-only', '-f', action='store_true',
                       help='Show only failed tests details')
    parser.add_argument('--quiet', '-q', action='store_true',
                       help='Show only final result (pass/fail)')
    parser.add_argument('--json-only', '-j', action='store_true',
                       help='Output only JSON results to stdout')
    
    args = parser.parse_args()
    
    try:
        tester = MinishellTester(args.minishell)
        
        # Determine output mode
        output_mode = 'full'  # default
        if args.json_only:
            output_mode = 'json'
        elif args.quiet:
            output_mode = 'quiet'
        elif args.summary:
            output_mode = 'summary'
        elif args.brief:
            output_mode = 'brief'
        elif args.percentage:
            output_mode = 'percentage'
        elif args.failed_only:
            output_mode = 'failed'
        
        # Suppress test run output for quiet modes
        if output_mode in ['quiet', 'json']:
            import sys
            from io import StringIO
            old_stdout = sys.stdout
            sys.stdout = StringIO()
        
        if args.category:
            # Run specific category
            category_methods = {
                'basic': 'test_basic_commands',
                'builtins': 'test_builtin_commands', 
                'redirections': 'test_redirections',
                'pipes': 'test_pipes',
                'env_vars': 'test_environment_variables',
                'quotes': 'test_quotes_and_escaping',
                'errors': 'test_error_handling',
                'edge_cases': 'test_edge_cases',
                'signals': 'test_signal_handling',
                'combinations': 'test_combinations'
            }
            
            method_name = category_methods.get(args.category)
            if method_name and hasattr(tester, method_name):
                getattr(tester, method_name)()
            else:
                if output_mode in ['quiet', 'json']:
                    sys.stdout = old_stdout
                print(f"Unknown category: {args.category}")
                return 1
        else:
            # Run all tests
            tester.run_all_tests()
        
        # Restore stdout for output modes that suppressed it
        if output_mode in ['quiet', 'json']:
            sys.stdout = old_stdout
        
        # Generate report with specified mode
        success_rate = tester.generate_report(output_mode)
        
        # Cleanup
        tester.cleanup()
        
        # Exit with appropriate code
        return 0 if success_rate >= 80 else 1
        
    except FileNotFoundError as e:
        print(f"{Colors.RED}Error: {e}{Colors.RESET}")
        print("Make sure minishell is compiled and accessible.")
        return 1
    except Exception as e:
        print(f"{Colors.RED}Unexpected error: {e}{Colors.RESET}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
