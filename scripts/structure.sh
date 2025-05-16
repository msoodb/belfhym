#!/bin/bash

# SPDX-License-Identifier: GPL-3.0-or-later
# Create directory structure for Belfhym project

INCLUDE_DIR="include"
BASE_DIR="src"

DIRS=(
  "$BASE_DIR/app"
  "$BASE_DIR/core"
  "$BASE_DIR/drivers"
  "$BASE_DIR/sensors"
  "$BASE_DIR/actuators"
  "$BASE_DIR/logic"
  "$BASE_DIR/communication"
  "$BASE_DIR/control"
  "$BASE_DIR/safety"
  "$BASE_DIR/monitoring"
  "$BASE_DIR/power"
  "$BASE_DIR/logging"
  "$BASE_DIR/interface"
)

echo "Creating directory structure..."

for dir in "${DIRS[@]}"; do
  mkdir -p "$dir"
  echo "Created: $dir"
done

# mkdir $INCLUDE_DIR

echo "Directory layout created successfully."
