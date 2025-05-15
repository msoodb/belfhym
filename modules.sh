#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 path/name"
  exit 1
fi

INPUT="$1"
NAME=$(basename "$INPUT")            # e.g., "pid"
PATH_PART=$(dirname "$INPUT")        # e.g., "control"

MODULE="blfm_${NAME}"
GUARD=$(echo "${MODULE}_H" | tr 'a-z' 'A-Z')

SRC_DIR="src/${PATH_PART}"
INCLUDE_DIR="include"

C_FILE="${SRC_DIR}/${MODULE}.c"
H_FILE="${INCLUDE_DIR}/${MODULE}.h"

mkdir -p "$SRC_DIR" "$INCLUDE_DIR"

echo "Creating $H_FILE"
cat <<EOF > "$H_FILE"
#ifndef ${GUARD}
#define ${GUARD}

void ${MODULE}_init(void);

#endif // ${GUARD}
EOF

echo "Creating $C_FILE"
cat <<EOF > "$C_FILE"
#include "${MODULE}.h"

void ${MODULE}_init(void) {
    // TODO: Implement ${MODULE} initialization
}
EOF

echo "Created module: $MODULE"
echo "  Header: $H_FILE"
echo "  Source: $C_FILE"
