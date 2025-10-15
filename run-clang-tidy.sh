#!/bin/bash

# run-clang-tidy.sh - Convenient script to run clang-tidy on Ray-Invaders project

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m' 
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

# Check if clang-tidy-21 is available
if ! command -v clang-tidy-21 &> /dev/null; then
    print_error "clang-tidy-21 not found. Please install it first."
    exit 1
fi

# Default options
CLANG_TIDY_ARGS="-std=c++23 -I. -I/usr/local/include"
FIX_MODE=false
SINGLE_FILE=""
VERBOSE=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --fix)
            FIX_MODE=true
            shift
            ;;
        --file)
            SINGLE_FILE="$2"
            shift 2
            ;;
        --verbose|-v)
            VERBOSE=true
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --fix         Apply automatic fixes"
            echo "  --file FILE   Analyze only the specific file"
            echo "  --verbose,-v  Verbose output"
            echo "  --help,-h     Show this help"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

print_status "Running clang-tidy analysis on Ray-Invaders project..."

if [[ -n "$SINGLE_FILE" ]]; then
    # Analyze single file
    if [[ ! -f "$SINGLE_FILE" ]]; then
        print_error "File not found: $SINGLE_FILE"
        exit 1
    fi
    
    print_status "Analyzing file: $SINGLE_FILE"
    
    if [[ "$FIX_MODE" == true ]]; then
        print_warning "Applying automatic fixes to $SINGLE_FILE..."
        clang-tidy-21 "$SINGLE_FILE" --fix -- $CLANG_TIDY_ARGS
    else
        clang-tidy-21 "$SINGLE_FILE" -- $CLANG_TIDY_ARGS
    fi
else
    # Analyze all .cpp files
    CPP_FILES=$(find . -name "*.cpp" -type f)
    FILE_COUNT=$(echo "$CPP_FILES" | wc -l)
    
    print_status "Found $FILE_COUNT C++ files to analyze"
    
    if [[ "$FIX_MODE" == true ]]; then
        print_warning "Applying automatic fixes to all files..."
        echo "$CPP_FILES" | while read -r file; do
            if [[ "$VERBOSE" == true ]]; then
                print_status "Fixing: $file"
            fi
            clang-tidy-21 "$file" --fix -- $CLANG_TIDY_ARGS
        done
    else
        echo "$CPP_FILES" | while read -r file; do
            if [[ "$VERBOSE" == true ]]; then
                print_status "Analyzing: $file"
            fi
            clang-tidy-21 "$file" -- $CLANG_TIDY_ARGS
        done
    fi
fi

print_success "Clang-tidy analysis complete!"

if [[ "$FIX_MODE" == true ]]; then
    print_status "Automatic fixes have been applied. Please review the changes."
    print_status "You may want to run: git diff"
fi