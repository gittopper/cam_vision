include(ExternalProject)

set(ZLIB_BUILD_EXAMPLES CACHE BOOL OFF FORCE)
add_subdirectory(zlib)

set(PNG_TESTS CACHE BOOL OFF FORCE)
set(PNG_TOOLS CACHE BOOL OFF FORCE)
add_subdirectory(libpng)

add_subdirectory(freetype)

