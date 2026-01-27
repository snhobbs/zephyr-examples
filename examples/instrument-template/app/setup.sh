#! /bin/sh
# setup.sh

SCRIPT_DIR="$(cd -- "$(dirname -- "$0")" && pwd)"
APP_PATH="$(basename ${SCRIPT_DIR})"

mkdir "${SCRIPT_DIR}/../.west"

west config manifest.path ${APP_PATH}
west config manifest.file -- "west.yml"
west config zephyr.base -- "deps/zephyr"
west config update.sync-submodules true
west config log.verbosity 2
west config build.dir-fmt -- "build/{board}/{app}" 
west config build.cmake-args -- "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_VERBOSE_MAKEFILE=ON"
west config build.board rp2350_eth/rp2350a/m33/mcuboot
west config build.generator Ninja
west config build.sysbuild true
