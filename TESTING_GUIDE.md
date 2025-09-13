# Minishell Test Cases & Development Guide

## Table of Contents
1. [Basic Command Execution](#basic-command-execution)
2. [Built-in Commands](#built-in-commands)
3. [Redirections](#redirections)
4. [Pipes](#pipes)
5. [Environment Variables](#environment-variables)
6. [Quote Handling](#quote-handling)
7. [Signal Handling](#signal-handling)
8. [Error Handling](#error-handling)
9. [Edge Cases](#edge-cases)
10. [Memory Management](#memory-management)
11. [Performance Considerations](#performance-considerations)
12. [Bash Compatibility](#bash-compatibility)

---

## Basic Command Execution

### Simple Commands
```bash
# Test basic command execution
echo hello
ls
pwd
whoami
date
```

### Commands with Arguments
```bash
# Test argument parsing
echo hello world
ls -la
grep "pattern" file.txt
cat /etc/passwd
```

### Absolute and Relative Paths
```bash
# Test path resolution
/bin/echo hello
./program
../bin/command
```

### PATH Resolution
```bash
# Test PATH environment variable usage
which ls
echo $PATH
# Should find commands in PATH directories
```

**Expected Behaviors:**
- ✅ Commands found in PATH execute correctly
- ✅ Absolute paths work
- ✅ Relative paths work
- ❌ Invalid commands return "command not found" (exit 127)
- ❌ Permission denied returns exit code 126

---

## Built-in Commands

### echo
```bash
# Basic echo
echo hello
echo hello world

# Echo with -n flag
echo -n hello
echo -n hello world

# Empty echo
echo
echo -n

# Multiple arguments
echo one two three
```

**Expected Behaviors:**
- ✅ `echo` prints arguments separated by spaces
- ✅ `echo -n` suppresses newline
- ✅ Empty echo prints just newline
- ✅ Multiple arguments handled correctly

### pwd
```bash
# Basic pwd
pwd

# After cd
cd /tmp && pwd
cd .. && pwd
```

**Expected Behaviors:**
- ✅ Returns current working directory
- ✅ Updates after directory changes
- ❌ Error message if getcwd fails

### env
```bash
# Display environment
env

# Check specific variables
env | grep PATH
env | grep HOME
```

**Expected Behaviors:**
- ✅ Displays all environment variables
- ✅ Format: KEY=VALUE
- ✅ One variable per line

### exit
```bash
# Basic exit
exit

# Exit with code
exit 0
exit 42
exit 255

# Exit with invalid code
exit abc
exit 999
```

**Expected Behaviors:**
- ✅ `exit` terminates shell with last status
- ✅ `exit N` terminates with code N
- ✅ Invalid codes handled gracefully
- ✅ Code modulo 256 for values > 255

### cd
```bash
# Basic cd
cd /tmp
cd ..
cd ~
cd

# Invalid directory
cd /nonexistent

# Permission denied
cd /root (as non-root user)

# Edge cases
cd ""
cd .
```

**Expected Behaviors:**
- ✅ Changes to specified directory
- ✅ `cd` with no args goes to HOME
- ✅ Updates PWD and OLDPWD
- ❌ Error for non-existent directories
- ❌ Error for permission denied

### export
```bash
# Basic export
export VAR=value
export PATH=/bin:/usr/bin

# Export existing variable
export HOME

# Multiple exports
export A=1 B=2 C=3

# Invalid exports
export =value
export 123=value
export VAR=
```

**Expected Behaviors:**
- ✅ Creates/updates environment variables
- ✅ Variables available to child processes
- ✅ Invalid names ignored
- ✅ Empty values allowed

### unset
```bash
# Basic unset
unset VAR
unset PATH

# Multiple unset
unset A B C

# Unset non-existent
unset NONEXISTENT

# Invalid names
unset 123
unset =
```

**Expected Behaviors:**
- ✅ Removes variables from environment
- ✅ Non-existent variables ignored
- ✅ Invalid names ignored

---

## Redirections

### Input Redirection (<)
```bash
# Basic input redirection
cat < file.txt
grep pattern < input.txt

# Non-existent file
cat < nonexistent.txt

# Permission denied
cat < /etc/shadow (as non-root)
```

**Expected Behaviors:**
- ✅ Reads from specified file
- ❌ Error for non-existent files
- ❌ Error for permission denied

### Output Redirection (>)
```bash
# Basic output redirection
echo hello > output.txt
ls > filelist.txt

# Overwrite existing file
echo new > existing.txt

# Permission denied
echo hello > /etc/passwd

# Invalid path
echo hello > /nonexistent/file.txt
```

**Expected Behaviors:**
- ✅ Creates new file or overwrites existing
- ✅ Truncates existing files
- ❌ Error for permission denied
- ❌ Error for invalid paths

### Append Redirection (>>)
```bash
# Basic append
echo line1 > file.txt
echo line2 >> file.txt

# Append to non-existent (creates file)
echo hello >> newfile.txt

# Permission denied
echo hello >> /etc/passwd
```

**Expected Behaviors:**
- ✅ Appends to existing file
- ✅ Creates file if doesn't exist
- ❌ Error for permission denied

### Multiple Redirections
```bash
# Multiple redirections
cat < input.txt > output.txt
grep pattern < file1.txt >> results.txt

# Order independence
> output.txt cat < input.txt
```

**Expected Behaviors:**
- ✅ Multiple redirections work together
- ✅ Order doesn't matter
- ✅ Input and output can be redirected simultaneously

### Heredoc (if implemented)
```bash
# Basic heredoc
cat << EOF
line 1
line 2
EOF

# With variables
cat << EOF
Home is $HOME
EOF
```

**Expected Behaviors:**
- ✅ Reads until delimiter
- ✅ Variable expansion works
- ✅ Delimiter matching exact

---

## Pipes

### Simple Pipes
```bash
# Basic pipes
ls | wc -l
cat file.txt | grep pattern
echo hello | cat

# Three-stage pipe
cat file.txt | grep pattern | wc -l
```

**Expected Behaviors:**
- ✅ Output of left command becomes input of right
- ✅ Multiple pipes work (A | B | C)
- ✅ Exit status is from rightmost command

### Pipes with Redirections
```bash
# Pipe with output redirection
ls | grep pattern > results.txt

# Pipe with input redirection
cat < input.txt | grep pattern

# Complex combinations
cat < input.txt | grep pattern | sort > output.txt
```

**Expected Behaviors:**
- ✅ Pipes and redirections work together
- ✅ Order of operations correct

### Pipes with Built-ins
```bash
# Built-in in pipe
env | grep PATH
echo hello | cat
pwd | cat
```

**Expected Behaviors:**
- ✅ Built-ins work in pipes
- ✅ Output correctly piped to next command

---

## Environment Variables

### Variable Expansion
```bash
# Basic expansion
echo $HOME
echo $PATH
echo $USER

# Non-existent variables
echo $NONEXISTENT

# Variables in arguments
ls $HOME
cat $HOME/.bashrc
```

**Expected Behaviors:**
- ✅ Variables expand to their values
- ✅ Non-existent variables expand to empty string
- ✅ Variables work in all argument positions

### Special Variables
```bash
# Exit status
echo $?
false
echo $?
true
echo $?

# Process ID (if implemented)
echo $$
```

**Expected Behaviors:**
- ✅ `$?` shows last command exit status
- ✅ Updated after each command
- ✅ Initially 0

### Variable Assignment
```bash
# Local assignment (if implemented)
VAR=value echo $VAR

# Export vs local
VAR=local
export EXPORTED=global
```

**Expected Behaviors:**
- ✅ Local assignments don't affect environment
- ✅ export makes variables available to children

---

## Quote Handling

### Single Quotes
```bash
# Basic single quotes
echo 'hello world'
echo 'special $chars !@#'

# Nested quotes
echo 'don'\''t'

# Empty quotes
echo ''
```

**Expected Behaviors:**
- ✅ Preserves literal values
- ✅ No variable expansion
- ✅ No special character interpretation

### Double Quotes
```bash
# Basic double quotes
echo "hello world"
echo "expansion: $HOME"

# Special characters
echo "quotes and $variables"

# Empty quotes
echo ""
```

**Expected Behaviors:**
- ✅ Allows variable expansion
- ✅ Preserves spaces
- ✅ Special characters mostly literal

### Mixed Quotes
```bash
# Adjacent quotes
echo 'hello'"world"
echo "part1"'part2'

# Nested combinations
echo "outer 'inner' outer"
echo 'outer "inner" outer'
```

**Expected Behaviors:**
- ✅ Adjacent quotes concatenate
- ✅ Nested quotes work correctly
- ✅ Each quote type preserves its rules

### Unclosed Quotes
```bash
# Unclosed single quote
echo 'unclosed

# Unclosed double quote
echo "unclosed
```

**Expected Behaviors:**
- ❌ Syntax error for unclosed quotes
- ❌ Clear error message

---

## Signal Handling

### Ctrl+C (SIGINT)
```bash
# During command execution
sleep 10
# Press Ctrl+C

# At prompt
# Press Ctrl+C at empty prompt
```

**Expected Behaviors:**
- ✅ Interrupts running command
- ✅ Returns to prompt
- ✅ Doesn't exit shell

### Ctrl+D (EOF)
```bash
# At empty prompt
# Press Ctrl+D
```

**Expected Behaviors:**
- ✅ Exits shell gracefully
- ✅ Prints "exit" message

### Ctrl+\ (SIGQUIT)
```bash
# During command execution
sleep 10
# Press Ctrl+\

# At prompt
# Press Ctrl+\ at prompt
```

**Expected Behaviors:**
- ✅ Ignored at prompt
- ✅ May quit running command (implementation dependent)

---

## Error Handling

### Command Not Found
```bash
# Non-existent command
nonexistentcommand
./nonexistent
/bin/nonexistent
```

**Expected Behaviors:**
- ❌ "command not found" message
- ❌ Exit status 127

### Permission Denied
```bash
# Non-executable file
echo "not executable" > test.txt
./test.txt

# No permission
chmod 000 script.sh
./script.sh
```

**Expected Behaviors:**
- ❌ "Permission denied" message
- ❌ Exit status 126

### Syntax Errors
```bash
# Invalid syntax
echo hello |
| grep pattern
> output.txt

# Unclosed quotes
echo "unclosed
echo 'unclosed
```

**Expected Behaviors:**
- ❌ "syntax error" message
- ❌ Exit status 2

### File Not Found (Redirections)
```bash
# Input redirection
cat < nonexistent.txt

# Invalid output path
echo hello > /nonexistent/path/file.txt
```

**Expected Behaviors:**
- ❌ "No such file or directory"
- ❌ Appropriate exit status

---

## Edge Cases

### Empty Input
```bash
# Empty lines

# Just spaces
   
	

# Comments (if implemented)
# this is a comment
echo hello # inline comment
```

**Expected Behaviors:**
- ✅ Empty lines ignored
- ✅ Whitespace-only lines ignored
- ✅ Comments ignored (if implemented)

### Long Commands
```bash
# Very long command line
echo very very very long command with many arguments and words

# Long paths
/very/long/path/to/command

# Long variables
export VERY_LONG_VARIABLE_NAME=very_long_value
echo $VERY_LONG_VARIABLE_NAME
```

**Expected Behaviors:**
- ✅ Handles reasonable line lengths
- ✅ No arbitrary limits on argument count
- ✅ Memory allocation succeeds

### Special Characters
```bash
# Special characters in arguments
echo "special chars: !@#$%^&*()"
echo 'more special: <>?:"{}'

# Non-ASCII characters
echo "unicode: éñü"
```

**Expected Behaviors:**
- ✅ Special characters handled correctly
- ✅ Unicode support (if implemented)

### Whitespace Handling
```bash
# Multiple spaces
echo    hello    world

# Tabs and spaces
echo	hello 	world

# Leading/trailing whitespace
   echo hello   
```

**Expected Behaviors:**
- ✅ Multiple whitespace collapsed to single space
- ✅ Leading/trailing whitespace ignored
- ✅ Preserved inside quotes

---

## Memory Management

### Memory Leaks
```bash
# Repeated operations
for i in {1..1000}; do echo hello; done

# Large files
cat /dev/zero | head -1000000

# Many variables
for i in {1..100}; do export VAR$i=value$i; done
```

**Testing Methods:**
- 🔧 Use valgrind to detect leaks
- 🔧 Monitor memory usage with ps/top
- 🔧 Run stress tests

### Resource Cleanup
```bash
# File descriptors
for i in {1..100}; do cat < /dev/null; done

# Child processes
sleep 10 &
sleep 10 &
# Check with ps
```

**Expected Behaviors:**
- ✅ All malloc/free pairs balanced
- ✅ File descriptors closed properly
- ✅ Child processes reaped
- ✅ No zombie processes

---

## Performance Considerations

### Startup Time
```bash
# Measure shell startup
time ./minishell -c "exit"
```

### Command Execution Speed
```bash
# Simple commands
time echo hello
time ls

# Complex pipelines
time cat large_file.txt | grep pattern | sort | uniq
```

### Memory Usage
```bash
# Monitor memory during operation
ps aux | grep minishell
```

**Benchmarks:**
- 📊 Startup < 100ms
- 📊 Simple commands < 10ms overhead
- 📊 Memory usage reasonable for workload

---

## Bash Compatibility

### Exit Codes
```bash
# Command success/failure
true; echo $?    # Should be 0
false; echo $?   # Should be 1
ls /nonexistent; echo $?  # Should be 2
nonexistentcmd; echo $?   # Should be 127
```

### Environment Behavior
```bash
# Environment inheritance
export TEST=parent
bash -c 'echo $TEST'  # Should show 'parent'

# Local vs exported
LOCAL=value
export EXPORTED=value
bash -c 'echo $LOCAL $EXPORTED'  # Should show ' value'
```

### Built-in Compatibility
```bash
# Compare with bash behavior
echo -n hello; echo world    # No newline between
cd /tmp; pwd                 # Should show /tmp
export VAR=value; env | grep VAR  # Should show VAR=value
```

**Testing Strategy:**
1. 🧪 Run identical commands in bash and minishell
2. 🧪 Compare outputs and exit codes
3. 🧪 Test edge cases where bash behavior is specific
4. 🧪 Document intentional differences

---

## Automated Testing Framework

### Test Script Structure
```bash
#!/bin/bash
# test_runner.sh

run_test() {
    local test_name="$1"
    local command="$2"
    local expected_output="$3"
    local expected_exit="$4"
    
    echo "Testing: $test_name"
    
    # Run in minishell
    actual_output=$(echo "$command" | ./minishell 2>&1)
    actual_exit=$?
    
    # Compare results
    if [[ "$actual_output" == "$expected_output" ]] && [[ "$actual_exit" == "$expected_exit" ]]; then
        echo "✅ PASS: $test_name"
    else
        echo "❌ FAIL: $test_name"
        echo "  Expected: '$expected_output' (exit $expected_exit)"
        echo "  Actual:   '$actual_output' (exit $actual_exit)"
    fi
}

# Test cases
run_test "echo hello" "echo hello" "hello" 0
run_test "command not found" "nonexistent" "minishell: nonexistent: command not found" 127
# ... more tests
```

### Continuous Integration
- 🔄 Run tests on every commit
- 🔄 Test on multiple environments
- 🔄 Memory leak detection
- 🔄 Performance regression testing

---

## Development Checklist

### Before Each Release
- [ ] All test cases pass
- [ ] No memory leaks (valgrind clean)
- [ ] No compiler warnings
- [ ] Documentation updated
- [ ] Performance benchmarks acceptable

### Code Quality
- [ ] Consistent error handling
- [ ] Proper resource cleanup
- [ ] Clear error messages
- [ ] Defensive programming practices
- [ ] Code comments for complex logic

### Feature Completeness
- [ ] All required built-ins implemented
- [ ] Signal handling working
- [ ] Redirections functional
- [ ] Pipes working correctly
- [ ] Environment variables supported
- [ ] Quote parsing accurate

---

## Debugging Tips

### Common Issues
1. **Memory Leaks**: Use valgrind, check malloc/free pairs
2. **File Descriptor Leaks**: Use lsof, check open/close pairs
3. **Signal Issues**: Test with actual Ctrl+C, not kill command
4. **Parsing Errors**: Print parse tree, trace tokenizer
5. **PATH Issues**: Check environment inheritance

### Debugging Tools
```bash
# Memory debugging
valgrind --leak-check=full ./minishell

# File descriptor monitoring
lsof -p $(pidof minishell)

# System call tracing
strace -e trace=file ./minishell

# Performance profiling
perf record ./minishell
perf report
```

### Test Environment
- Clean environment for consistent results
- Test as both root and regular user
- Test on different filesystems
- Test with various terminal settings

---

*This guide should be updated as new features are added and edge cases discovered. Regular testing ensures a robust and reliable shell implementation.*
