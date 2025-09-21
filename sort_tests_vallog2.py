#!/usr/bin/env python3
import re

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
    with open('/root/Desktop/minishell/test_cases_vallog2.txt', 'r') as f:
        content = f.read()

    # Split by test case sections
    sections = content.split('=== ')[1:]  # Remove empty first element

    test_cases = []
    for section in sections:
        lines = section.strip().split('\n')
        if not lines:
            continue

        header_line = lines[0].replace(' ===', '')
        test_content = '\n'.join(lines[1:])

        priority, test_num, full_header = parse_test_info(header_line)
        test_cases.append((priority, test_num, full_header, test_content))

    # Sort by priority, then by original test number
    test_cases.sort(key=lambda x: (x[0], x[1]))

    # Generate output with new numbering
    output = []
    for new_num, (priority, orig_num, header, content) in enumerate(test_cases, 1):
        priority_names = {1: "LEAKS", 2: "EXIT_CODE", 3: "STDERR", 4: "OTHER"}
        output.append(f"=== {new_num}: {priority_names[priority]} - {header} ===")
        output.append(content)
        output.append("")

    # Write sorted output
    with open('/root/Desktop/minishell/test_cases_vallog2_sorted.txt', 'w') as f:
        f.write('\n'.join(output))

    # Print summary
    leak_count = sum(1 for p, _, _, _ in test_cases if p == 1)
    exit_count = sum(1 for p, _, _, _ in test_cases if p == 2)
    stderr_count = sum(1 for p, _, _, _ in test_cases if p == 3)
    other_count = sum(1 for p, _, _, _ in test_cases if p == 4)

    print(f"Sorted {len(test_cases)} test cases:")
    print(f"  {leak_count} LEAK errors (priority 1)")
    print(f"  {exit_count} EXIT_CODE errors (priority 2)")
    print(f"  {stderr_count} STDERR errors (priority 3)")
    print(f"  {other_count} OTHER errors (priority 4)")

if __name__ == "__main__":
    main()