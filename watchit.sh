#!/usr/bin/env bash

# Replace with your process name
PROCESS_NAME="faststatus"

while pgrep -x "$PROCESS_NAME" > /dev/null; do
    sleep 1
done

# Your command here
notify-send -u critical "Status program has stopped - $(date)."
