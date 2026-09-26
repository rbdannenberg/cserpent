#!/bin/bash
# gen_tobetested.sh
#
# (Re)generate the queue of Serpent regression-test programs to run.
# Scans ~/serpent/programs/regr-test/*.srp and writes one program name
# (without the .srp extension) per line to tests/serpent/tobetested.txt.
#
# Run this once to start a test session, or again any time you add new
# programs to regr-test/ and want to pick them up. Re-running overwrites
# the queue, so don't run it mid-session if you want to keep whatever
# progress "testone" has made.
#
# Always operates in ~/cserpent (where compiler.srp lives), regardless
# of the directory you're in when you invoke it.

set -euo pipefail

SRC_DIR="$HOME/serpent/programs/regr-test"
REPO_DIR="$HOME/cserpent"
QUEUE_FILE="tests/serpent/tobetested.txt"

cd "$REPO_DIR" || { echo "Error: cannot cd to $REPO_DIR" >&2; exit 1; }

mkdir -p "$(dirname "$QUEUE_FILE")"

shopt -s nullglob
progs=("$SRC_DIR"/*.srp)
shopt -u nullglob

if [ ${#progs[@]} -eq 0 ]; then
    echo "No .srp files found in $SRC_DIR" >&2
    exit 1
fi

printf '%s\n' "${progs[@]##*/}" | sed 's/\.srp$//' | sort > "$QUEUE_FILE"

echo "Wrote $(wc -l < "$QUEUE_FILE") program name(s) to $QUEUE_FILE"
