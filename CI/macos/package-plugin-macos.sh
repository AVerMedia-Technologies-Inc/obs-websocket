#!/bin/bash

set -e

OSTYPE=$(uname)

if [ "${OSTYPE}" != "Darwin" ]; then
	echo "[obs-websocket-avt - Error] macOS package script can be run on Darwin-type OS only."
	exit 1
fi

echo "[obs-websocket-avt] Preparing package build"

GIT_HASH=$(git rev-parse --short HEAD)
GIT_BRANCH_OR_TAG=$(git name-rev --name-only HEAD | awk -F/ '{print $NF}')

VERSION="$GIT_HASH-$GIT_BRANCH_OR_TAG"

FILENAME_UNSIGNED="obs-websocket-avt-$VERSION-Unsigned.pkg"
FILENAME="obs-websocket-avt-$VERSION.pkg"

echo "[obs-websocket-avt] Modifying obs-websocket-avt.so linking"
install_name_tool \
	-change /tmp/obsdeps/lib/QtWidgets.framework/Versions/5/QtWidgets \
		@executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets \
	-change /tmp/obsdeps/lib/QtGui.framework/Versions/5/QtGui \
		@executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui \
	-change /tmp/obsdeps/lib/QtCore.framework/Versions/5/QtCore \
		@executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore \
	./build/obs-websocket-avt.so

# Check if replacement worked
echo "[obs-websocket-avt] Dependencies for obs-websocket-avt"
otool -L ./build/obs-websocket-avt.so

if [[ "$RELEASE_MODE" == "True" ]]; then
	echo "[obs-websocket-avt] Signing plugin binary: obs-websocket-avt.so"
	codesign --sign "$CODE_SIGNING_IDENTITY" ./build/obs-websocket-avt.so
else
	echo "[obs-websocket-avt] Skipped plugin codesigning"
fi

