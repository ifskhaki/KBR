cmake_minimum_required(VERSION 3.18)
project(bitrecover 
    VERSION 1.0.0
    DESCRIPTION "Multi-GPU Bitcoin Key Recovery Tool"
    LANGUAGES CUDA CXX
)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# CUDA detection
find_package(CUDA REQUIRED)
enable_language(CUDA)

# Auto-detect GPU architectures
if(UNIX)
    execute_process(
        COMMAND nvidia-smi --query-gpu=compute_cap --format=csv,noheader
        OUTPUT_VARIABLE GPU_ARCHS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    
    if(GPU_ARCHS_RAW)
        string(REPLACE "\n" ";" GPU_ARCHS_LIST ${GPU_ARCHS_RAW})
        list(REMOVE_DUPLICATES GPU_ARCHS_LIST)
        
        set(CUDA_ARCHS "")
        foreach(ARCH ${GPU_ARCHS_LIST})
            string(REPLACE "." "" ARCH_CLEAN ${ARCH})
            list(APPEND CUDA_ARCHS "sm_${ARCH_CLEAN}")
        endforeach()
    endif()
endif()

# Default architectures if detection failed
if(NOT CUDA_ARCHS)
    set(CUDA_ARCHS "sm_70;sm_75;sm_80;sm_86;sm_89;sm_90")
endif()

message(STATUS "Building for CUDA architectures: ${CUDA_ARCHS}")

# Include directories
set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(LEGACY_ROOT ${PROJECT_ROOT}/../)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LEGACY_ROOT}
    ${LEGACY_ROOT}/AddressUtil
    ${LEGACY_ROOT}/KeyFinderLib
    ${LEGACY_ROOT}/CudaKeySearchDevice
    ${LEGACY_ROOT}/cudaMath
    ${LEGACY_ROOT}/cudaUtil
    ${LEGACY_ROOT}/CryptoUtil
    ${LEGACY_ROOT}/secp256k1lib
    ${LEGACY_ROOT}/util
    ${LEGACY_ROOT}/Logger
    ${LEGACY_ROOT}/CmdParse
    ${LEGACY_ROOT}/KeyFinder
    ${CUDA_INCLUDE_DIRS}
)

# Source files
set(BITRECOVER_SOURCES
    src/main.cpp
    src/BitrecoverEngine.cpp
    src/MultiGPUManager.cpp
    src/EmailNotifier.cpp
    src/StatusDisplay.cpp
    src/ConfigManager.cpp
    src/RandomKeyGenerator.cpp
)

# Legacy sources (from existing BitCrack codebase)
set(LEGACY_SOURCES
    ${LEGACY_ROOT}/KeyFinderLib/KeyFinder.cpp
    ${LEGACY_ROOT}/CudaKeySearchDevice/CudaKeySearchDevice.cpp
    ${LEGACY_ROOT}/CudaKeySearchDevice/CudaKeySearchDevice.cu
    ${LEGACY_ROOT}/CudaKeySearchDevice/cudabridge.cu
    ${LEGACY_ROOT}/CudaKeySearchDevice/CudaDeviceKeys.cu
    ${LEGACY_ROOT}/CudaKeySearchDevice/CudaHashLookup.cu
    ${LEGACY_ROOT}/CudaKeySearchDevice/CudaAtomicList.cu
    ${LEGACY_ROOT}/AddressUtil/Base58.cpp
    ${LEGACY_ROOT}/AddressUtil/hash.cpp
    ${LEGACY_ROOT}/CryptoUtil/sha256.cpp
    ${LEGACY_ROOT}/CryptoUtil/ripemd160.cpp
    ${LEGACY_ROOT}/CryptoUtil/checksum.cpp
    ${LEGACY_ROOT}/CryptoUtil/Rng.cpp
    ${LEGACY_ROOT}/CryptoUtil/hash.cpp
    ${LEGACY_ROOT}/secp256k1lib/secp256k1.cpp
    ${LEGACY_ROOT}/util/util.cpp
    ${LEGACY_ROOT}/Logger/Logger.cpp
    ${LEGACY_ROOT}/CmdParse/CmdParse.cpp
    ${LEGACY_ROOT}/KeyFinder/DeviceManager.cpp
    ${LEGACY_ROOT}/KeyFinder/ConfigFile.cpp
)

# Separate CUDA and C++ sources
set(CUDA_SOURCES "")
set(CPP_SOURCES "")

foreach(SRC ${BITRECOVER_SOURCES} ${LEGACY_SOURCES})
    if(SRC MATCHES "\\.cu$")
        list(APPEND CUDA_SOURCES ${SRC})
    else()
        list(APPEND CPP_SOURCES ${SRC})
    endif()
endforeach()

# Executable
add_executable(bitrecover ${CPP_SOURCES} ${CUDA_SOURCES})

# CUDA properties
set_target_properties(bitrecover PROPERTIES
    CUDA_ARCHITECTURES "${CUDA_ARCHS}"
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_RESOLVE_DEVICE_SYMBOLS ON
)

# Compiler flags
target_compile_options(bitrecover PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-O3 -march=native -Wall -Wextra>
    $<$<COMPILE_LANGUAGE:CUDA>:-O3 --use_fast_math>
)

# Link libraries
target_link_libraries(bitrecover PRIVATE
    ${CUDA_LIBRARIES}
    ${CUDA_CUDART_LIBRARY}
    pthread
)

# Installation
install(TARGETS bitrecover DESTINATION bin)
install(DIRECTORY scripts/ DESTINATION share/bitrecover/scripts)
install(DIRECTORY config/ DESTINATION share/bitrecover/config)
install(FILES README.md LICENSE DESTINATION share/bitrecover)

# Print build info
message(STATUS "")
message(STATUS "=== Bitrecover Build Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "CUDA architectures: ${CUDA_ARCHS}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "CUDA standard: ${CMAKE_CUDA_STANDARD}")
message(STATUS "=====================================")
message(STATUS "")

