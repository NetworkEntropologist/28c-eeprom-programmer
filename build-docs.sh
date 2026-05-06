#!/bin/bash
# Build documentation with Doxygen and Sphinx

set -e

echo "Generating Doxygen XML..."
doxygen Doxyfile

echo "Building Sphinx documentation..."
cd docs
make clean
make html

echo "Documentation built successfully!"
echo "Open docs/_build/html/index.html to view"

cd ..
