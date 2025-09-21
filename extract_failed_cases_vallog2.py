#!/usr/bin/env python3
import re
import os

def extract_test_case(file_path, line_num):
    """Extract test case starting from given line number until empty line or next test"""
    try:
        with open(file_path, 'r') as f:
            lines = f.readlines()

        # Convert to 0-based index
        start_idx = line_num - 1
        if start_idx >= len(lines):
            return []

        test_case = []
        # Start from the specified line
        for i in range(start_idx, len(lines)):
            line = lines[i].rstrip()

            # Stop at empty line or next test marker
            if not line.strip():
                break
            # If we find another command that looks like a test separator, stop
            if i > start_idx and (line.startswith('>>') or line.startswith('"')):
                break

            test_case.append(line)

        return test_case
    except Exception as e:
        return [f"Error reading {file_path}: {e}"]

def parse_test_info(line):
    """Parse test line and extract priority information"""
    # Extract test number
    test_num_match = re.search(r'^(\d+):', line)
    test_num = int(test_num_match.group(1)) if test_num_match else 0

    # Check for errors (❌ means failed)
    has_leak = 'LEAKS: ❌' in line
    has_exit_error = 'EXIT_CODE: ❌' in line
    has_stderr_error = 'STD_ERR: ❌' in line

    # Priority: 1=Leaks, 2=Exit Code, 3=Stderr
    if has_leak:
        priority = 1
    elif has_exit_error:
        priority = 2
    elif has_stderr_error:
        priority = 3
    else:
        priority = 4  # Other errors

    return priority, test_num, line

def main():
    with open('/root/Desktop/minishell/vallog2_failed_only.txt', 'r') as f:
        log_lines = f.readlines()

    test_cases = []

    for log_line in log_lines:
        log_line = log_line.strip()
        if not log_line:
            continue

        # Parse log line format: "TEST_NUM: STATUS /path/to/file.sh:LINE_NUM"
        match = re.search(r'/root/42_minishell_tester/cmds/mand/([^:]+):(\d+)', log_line)
        if not match:
            continue

        file_name = match.group(1)
        line_num = int(match.group(2))
        file_path = f'/root/42_minishell_tester/cmds/mand/{file_name}'

        # Extract the test case
        test_case = extract_test_case(file_path, line_num)
        priority, test_num, full_header = parse_test_info(log_line)

        if test_case:
            test_cases.append((priority, test_num, full_header, '\n'.join([f"  {cmd}" for cmd in test_case])))

    # Sort by priority, then by original test number
    test_cases.sort(key=lambda x: (x[0], x[1]))

    # Generate output with new numbering
    output = []
    for new_num, (priority, orig_num, header, content) in enumerate(test_cases, 1):
        priority_names = {1: "LEAKS", 2: "EXIT_CODE", 3: "STDERR", 4: "OTHER"}
        # Clean header from file paths
        clean_header = re.sub(r'/root/42_minishell_tester/cmds/mand/[a-zA-Z0-9_]*\.sh:\d+', '', header).strip()
        output.append(f"=== {new_num}: {priority_names[priority]} - {clean_header} ===")
        output.append(content)
        output.append("")

    # Write output to file
    with open('/root/Desktop/minishell/test_cases_vallog2_final.txt', 'w') as f:
        f.write('\n'.join(output))

    # Print summary
    leak_count = sum(1 for p, _, _, _ in test_cases if p == 1)
    exit_count = sum(1 for p, _, _, _ in test_cases if p == 2)
    stderr_count = sum(1 for p, _, _, _ in test_cases if p == 3)
    other_count = sum(1 for p, _, _, _ in test_cases if p == 4)

    print(f"Processed {len(test_cases)} failed test cases:")
    print(f"  {leak_count} LEAK errors (priority 1)")
    print(f"  {exit_count} EXIT_CODE errors (priority 2)")
    print(f"  {stderr_count} STDERR errors (priority 3)")
    print(f"  {other_count} OTHER errors (priority 4)")

if __name__ == "__main__":
    main()