# Target Operating System
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# Define the cross-compiler locations
set(CMAKE_C_COMPILER x86_64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER x86_64-linux-gnu-g++)

set(CURL_LIBRARY "/Users/greenbox/curl-x86_64/lib/libcurl.a")
set(CURL_INCLUDE_DIR "/Users/greenbox/curl-x86_64/include")


# Configure lookups for libraries and headers
# NEVER look into the macOS host system paths for libraries/includes

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

