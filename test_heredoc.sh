#!/bin/bash
# Test heredoc functionality

echo "Testing basic heredoc..."
echo 'cat << EOF
test line 1
test line 2
EOF' > /tmp/heredoc_test.txt

echo "Test file created with content:"
cat /tmp/heredoc_test.txt