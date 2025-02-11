#!/usr/bin/env bash
# Makes all permutations of the LKM and copies them to RedPill Load directory so that we can easily rebuild all images
# Yes, it has all the paths hardcoded - change it to fit your environment.
# When you are executing this script do it from the root of the LKM dir like ./tools/make_all.sh

LINUX_SRC_ROOT="$PWD/.."
RP_LOAD_ROOT="$HOME/build/redpill-load"

set -euo pipefail

# Build for v6 for 3615xs
make LINUX_SRC="$LINUX_SRC_ROOT/linux-3.10.x-bromolow-25426" clean
make LINUX_SRC="$LINUX_SRC_ROOT/linux-3.10.x-bromolow-25426"
cp redpill.ko "$RP_LOAD_ROOT/ext/rp-lkm/redpill-linux-v3.10.105.ko"

# Build for v7 for 3615xs
make LINUX_SRC="$LINUX_SRC_ROOT/bromolow-DSM-7.0-toolkit/build" clean
make LINUX_SRC="$LINUX_SRC_ROOT/bromolow-DSM-7.0-toolkit/build"
cp redpill.ko "$RP_LOAD_ROOT/ext/rp-lkm/redpill-linux-v3.10.108.ko"

# Build for v6 for 918+
make LINUX_SRC="$LINUX_SRC_ROOT/linux-4.4.x-apollolake-25426" clean
make LINUX_SRC="$LINUX_SRC_ROOT/linux-4.4.x-apollolake-25426"
cp redpill.ko "$RP_LOAD_ROOT/ext/rp-lkm/redpill-linux-v4.4.59+.ko"

# Build for v7 for 918+
make LINUX_SRC="$LINUX_SRC_ROOT/apollolake-DS-7.0-toolkit/build" clean
make LINUX_SRC="$LINUX_SRC_ROOT/apollolake-DS-7.0-toolkit/build"
cp redpill.ko "$RP_LOAD_ROOT/ext/rp-lkm/redpill-linux-v4.4.180+.ko"
