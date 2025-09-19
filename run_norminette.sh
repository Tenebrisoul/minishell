#!/bin/bash

# Clear the output file
> out.txt

# Find all .c files and run norminette on each
find . -name "*.c" -type f | while read -r file; do
    echo "Checking $file..."
    # Run norminette and filter out OK! lines, "Function has more than 25 lines" errors, "Setting locale" lines, and "Too many functions in file" errors
    norminette "$file" | grep -v "OK!" | grep -v "Function has more than 25 lines" | grep -v "Setting locale" | grep -v "Too many functions in file" | grep -v "Too many variables declarations in a function" | grep -v "Function has more than 4 arguments" >> out.txt
done

echo "Norminette check completed. Results written to out.txt"