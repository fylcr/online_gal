# Web worker target. Included only when the recipe passes -DARTC_BUILD_WEB=ON.
# Pin CMake has no artemis-web target; do not pretend it did.
if(ARTC_BUILD_WEB)
    if(NOT EMSCRIPTEN)
        message(FATAL_ERROR "ARTC_BUILD_WEB requires the Emscripten toolchain")
    endif()
    # Lua's setjmp (ldo.c) must use the same wasm-exception personality as the
    # C++ that catches runtime_error. A JS longjmp does not link against it.
    # Only lua51 gets `-fwasm-exceptions`; the C++ target links with
    # `-fwasm-exceptions` and the two match. Do not reach for `-fexceptions`
    # here: emscripten's own zlib is built with the wasm personality and a JS
    # longjmp build stops linking against it.
    #
    # Placement matters. `artemis_deps` is an INTERFACE library, so flags put
    # on it land ahead of the link libraries in the link line. `-fwasm-exceptions`
    # is a compile-and-link flag; as a trailing link option it emits nothing and
    # the static library's objects are then dropped for resolving
    # `__cpp_exception`. The flag belongs on the C++ target's compile options
    # (and link options), never on `artemis_deps`.
    if(TARGET lua51)
        target_compile_options(lua51 PRIVATE -fwasm-exceptions)
    endif()
    # Classic worker glue. Exceptions stay on: native_save and PSB throw into
    # catch and must not become abort. Numeric parsers are patched to strto*
    # so a bad attribute is a default, not a throw.
    #
    # Audio source selection. `ARTC_CORE_SOURCES` is a GLOB over the pinned
    # tree, so the upstream `src/audio/audio.cpp` (the OpenSL/desktop stub) is
    # in it and would define `Audio::Impl` a second time. The overlay file is
    # compiled *instead of* it: same class, same `PcmStream` contract, a pull
    # backend the worker can drive. Exactly one `Audio::Impl` per link, same
    # rule the pinned CMake already applies for the OHOS backend.
    #
    # The overlay is not inside the source tree, so its include directory is
    # added explicitly and listed *before* the pinned `src` -- the overlay's
    # `audio/audio_web.h` is reached by the same `audio/...` spelling the
    # upstream headers use, and the pinned tree is never written to.
    #
    # The recipe `cat`s this file into the build tree's `CMakeLists.txt` and
    # copies `overlay/audio/` in beside it, so the overlay is located *inside*
    # the build tree, not next to it. `ARTC_AUDIO_WEB_DIR` is overridable for a
    # caller with a different layout, and the guard below is what turns a wrong
    # layout into a configure error instead of a missing-symbol link failure.
    set(ARTC_AUDIO_WEB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/overlay"
        CACHE PATH "Directory holding overlay/audio/audio_web.{h,cpp}")
    if(NOT EXISTS "${ARTC_AUDIO_WEB_DIR}/audio/audio_web.cpp")
        message(FATAL_ERROR
            "audio overlay missing: ${ARTC_AUDIO_WEB_DIR}/audio/audio_web.cpp "
            "(pass -DARTC_AUDIO_WEB_DIR=<dir> if the overlay lives elsewhere)")
    endif()
    list(REMOVE_ITEM ARTC_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/audio/audio.cpp")
    list(APPEND ARTC_CORE_SOURCES "${ARTC_AUDIO_WEB_DIR}/audio/audio_web.cpp")

    add_executable(artemis-compat
        ${ARTC_CORE_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/web_main.cpp)
    target_include_directories(artemis-compat PRIVATE
        ${ARTC_AUDIO_WEB_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/render
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
    target_compile_features(artemis-compat PRIVATE cxx_std_17)
    target_compile_definitions(artemis-compat PRIVATE ARTC_HAS_GLES=1)
    # A worker has no `index.html` and no wasm-on-main-thread fallback, so the
    # export list is explicit. `FS` and `_malloc`/`_free` are exported for the
    # page: a host that writes the pack into MEMFS with `FS.createDataFile`
    # needs `ccall`/`cwrap`, `UTF8ToString` to read `artc_web_last_error`, and
    # its own `malloc`/`free` so a path string can outlive the call that made
    # it (`stringToNewUTF8` uses the same pair, so a page-side mismatched free
    # is a heap bug rather than an obvious error).
    target_compile_options(artemis-compat PRIVATE -Wall -Wextra -fwasm-exceptions)
    target_link_libraries(artemis-compat PRIVATE artemis_deps)
    target_link_options(artemis-compat PRIVATE
        "-sENVIRONMENT=worker"
        "--no-entry"
        "-sINVOKE_RUN=0"
        "-sASSERTIONS=1"
        "-sINITIAL_MEMORY=67108864"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sMAXIMUM_MEMORY=4294967296"
        "-sSTACK_SIZE=4194304"
        "-sUSE_WEBGL2=1"
        "-sFULL_ES2=1"
        "-sFORCE_FILESYSTEM=1"
        "-sEXPORTED_RUNTIME_METHODS=ccall,cwrap,HEAPU8,FS,UTF8ToString"
        "-sEXPORTED_FUNCTIONS=_malloc,_free,_artc_web_open,_artc_web_frame,_artc_web_pointer,_artc_web_pointer_move,_artc_web_key,_artc_web_resize,_artc_web_started,_artc_web_stage_width,_artc_web_stage_height,_artc_web_revision,_artc_web_presented,_artc_web_exit_requested,_artc_web_last_error,_artc_web_pause,_artc_web_now_ms,_artc_web_dispose,_artc_web_save,_artc_web_load,_artc_web_log_sink,_artc_web_audio_pump,_artc_web_audio_voice_count,_artc_web_audio_voice_handle,_artc_web_audio_voice_key,_artc_web_audio_voice_rate,_artc_web_audio_read_stereo,_artc_web_audio_ended,_artc_web_audio_played_frames,_artc_web_audio_ack"
        "-fwasm-exceptions"
        "-ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=/src"
        "-sEXPORT_ES6=0"
        "-sMODULARIZE=0")
    set_target_properties(artemis-compat PROPERTIES
        OUTPUT_NAME "artemis-compat"
        SUFFIX ".js")
endif()
