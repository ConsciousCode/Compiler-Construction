#!/bin/bash

OUTPUT_DIR="output"
TAR_FILE="while_outputs.tar"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# List of input files
files=(
    "examples/factorial.while"
    "examples/fibonacci.while"
    "examples/countTriangularNumbers.while"
    "examples/chain_prime.while"
)

# Process each file
for input_file in "${files[@]}"; do
    base_name=$(basename "$input_file" .while)
    
    echo "Processing $input_file..."
    
    # Copy original file to output directory
    cp "$input_file" "$OUTPUT_DIR/${base_name}.while"
    echo "  Copied $OUTPUT_DIR/${base_name}.while"
    
    # Run whilegen
    ./build/whilegen "$input_file" > "$OUTPUT_DIR/${base_name}_annotated.while"
    echo "  Created $OUTPUT_DIR/${base_name}_annotated.while"
    
    # Run wisc to generate AST dot file
    ./build/wisc --graph-ast "$input_file" > "$OUTPUT_DIR/${base_name}_ast.dot"
    echo "  Created $OUTPUT_DIR/${base_name}_ast.dot"
    
    # Generate SVG from dot file
    dot -Tsvg "$OUTPUT_DIR/${base_name}_ast.dot" -o "$OUTPUT_DIR/${base_name}_ast.svg"
    echo "  Created $OUTPUT_DIR/${base_name}_ast.svg"

    rm "$OUTPUT_DIR/${base_name}_ast.dot"
    echo "  Deleted $OUTPUT_DIR/${base_name}_ast.dot"
    
    echo ""
done

echo "Creating tar file..."
tar -cf "$TAR_FILE" "$OUTPUT_DIR"
echo "Created $TAR_FILE"

echo "Done!"