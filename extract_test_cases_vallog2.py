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

def main():
    with open('/root/Desktop/minishell/vallog2.txt', 'r') as f:
        log_lines = f.readlines()

    output = []

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

        if test_case:
            output.append(f"=== {log_line} ===")
            for cmd in test_case:
                output.append(f"  {cmd}")
            output.append("")

    # Write output to file
    with open('/root/Desktop/minishell/test_cases_vallog2.txt', 'w') as f:
        f.write('\n'.join(output))

    print(f"Extracted {len([l for l in output if l.startswith('===')])} test cases to test_cases_vallog2.txt")

if __name__ == "__main__":
    main()