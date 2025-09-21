#!/usr/bin/env python3
import re

def main():
    with open('/root/Desktop/minishell/vallog2.txt', 'r') as f:
        log_lines = f.readlines()

    failed_tests = []

    for log_line in log_lines:
        log_line = log_line.strip()
        if not log_line:
            continue

        # Only keep lines that contain ❌ (failed tests)
        if '❌' in log_line:
            failed_tests.append(log_line)

    # Write filtered failed tests
    with open('/root/Desktop/minishell/vallog2_failed_only.txt', 'w') as f:
        for test in failed_tests:
            f.write(test + '\n')

    print(f"Filtered {len(failed_tests)} failed tests from vallog2.txt")

if __name__ == "__main__":
    main()