vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO libexif/libexif
    REF "v${VERSION}"
    SHA512 6e50134eab2fcf93036ecf8a9a9f89273ab8ddc4a171523f1f88f6d90bda799ef8f6a597c1c308fe8153dcc685a2d2b473e758e2286ce4d3143dd829e07a8c80
    HEAD_REF master
    PATCHES
        fix-ssize.patch
)

vcpkg_list(SET options)
if("nls" IN_LIST FEATURES)
    vcpkg_list(APPEND options "--enable-nls")
else()
    vcpkg_list(APPEND options "--disable-nls")
endif()

# For an Emscripten target vcpkg_configure_make passes no `--host`: its Windows branch is gated on
# VCPKG_TARGET_IS_WINDOWS (false here), the macOS/Linux branches do not apply, and this repo's
# wasm32-emscripten triplet sets no VCPKG_MAKE_BUILD_TRIPLET. autoconf therefore treats the build as
# native and *runs* its conftest, which is Emscripten CommonJS JavaScript; Node rejects it with
# "ReferenceError: require is not defined in ES module scope" because the containing directory
# declares "type": "module" and the vcpkg buildtrees live under that directory. Declaring the target
# host makes autoconf cross-compile and skip run tests.
set(VCPKG_MAKE_BUILD_TRIPLET "--host=wasm32-unknown-emscripten")
vcpkg_configure_make(
    SOURCE_PATH "${SOURCE_PATH}"
    AUTOCONFIG
    OPTIONS
        ${options}
        --enable-internal-docs=no
        --enable-ship-binaries=no
)

vcpkg_install_make()
vcpkg_fixup_pkgconfig()

file(INSTALL "${CMAKE_CURRENT_LIST_DIR}/unofficial-libexif-config.cmake" DESTINATION "${CURRENT_PACKAGES_DIR}/share/unofficial-${PORT}")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/COPYING")
