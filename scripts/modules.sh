#!/bin/bash

create_module() {
  local INPUT="$1"
  local NAME=$(basename "$INPUT")
  local PATH_PART=$(dirname "$INPUT")
  local MODULE="${NAME}"
  local GUARD=$(echo "${MODULE}_H" | tr 'a-z' 'A-Z')

  local SRC_DIR="src/${PATH_PART}"
  local INCLUDE_DIR="include"

  mkdir -p "$SRC_DIR" "$INCLUDE_DIR"

  cat > "${INCLUDE_DIR}/${MODULE}.h" <<EOF
#ifndef ${GUARD}
#define ${GUARD}

void ${MODULE}_dummy(void);

#endif // ${GUARD}
EOF

  cat > "${SRC_DIR}/${MODULE}.c" <<EOF
#include "${MODULE}.h"

void ${MODULE}_dummy(void) {
    // TODO: Implement ${MODULE}
}
EOF

  echo "✔ Created: ${SRC_DIR}/${MODULE}.c and include/${MODULE}.h"
}

# If run with a pipe
if [ ! -t 0 ]; then
  while read -r line; do
    [[ -z "$line" || "$line" == \#* ]] && continue
    create_module "$line"
  done
# If run with a single argument
elif [ -n "$1" ]; then
  create_module "$1"
else
  echo "Usage: $0 path/name  or  echo path | $0"
  exit 1
fi
