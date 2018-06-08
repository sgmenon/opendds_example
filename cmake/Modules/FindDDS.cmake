# Find a DDS implementation.
# Currently only RTI's DDS implementation is searched for.
#
# The following directories are searched:
# DDS_ROOT (CMake variable)
# DDS_ROOT (Environment variable)
# ACE_ROOT (CMake variable)
# ACE_ROOT (Environment variable)
#
# Prior to calling this script, you may set the DDS_HOST variable. This is used
# when searching for some implementations as a name for the directory
# containing the library files. For example, it could be set to
# "x64Linux2.6gcc4.1.1".
#
# This sets the following variables:
# DDS_FOUND - True if DDS was found.
# DDS_VENDOR - Name of the DDS vendor found (e.g. "RTI")
# DDS_INCLUDE_DIRS - Directories containing the DDS include files.
# DDS_LIBRARIES - Libraries needed to use DDS.
# DDS_DEFINITIONS - Compiler flags for DDS.
# DDS_VERSION - The version of DDS found.
# DDS_VERSION_MAJOR - The major version of DDS found.
# DDS_VERSION_MINOR - The minor version of DDS found.
# DDS_VERSION_PATCH - The revision version of DDS found.
# DDS_VERSION_CAN - The candidate version of DDS found.

if (NOT DEFINED ${DDS_ROOT})
    set(DDS_ROOT $ENV{DDS_ROOT})
endif()
if (NOT DEFINED ${ACE_ROOT})
    set(ACE_ROOT $ENV{ACE_ROOT})
endif()

if (MSVC)
    set(CMAKE_MSVCIDE_RUN_PATH ${CMAKE_MSVCIDE_RUN_PATH} ${DDS_ROOT}/lib ${ACE_ROOT}/lib)
endif()

find_path(DDS_INCLUDE_DIR dds
    HINTS ${DDS_ROOT} $ENV{DDS_ROOT})
find_path(ACE_INCLUDE_DIR ace
    HINTS ${ACE_ROOT} $ENV{ACE_ROOT})
find_path(TAO_INCLUDE_DIR tao
    HINTS ${ACE_ROOT}/TAO $ENV{ACE_ROOT}/TAO)

find_library(DDS_CORE_LIBRARY OpenDDS_Dcps${DEBUG_SUFFIX}
    HINTS ${DDS_ROOT}/lib $ENV{DDS_ROOT}/lib)
find_library(DDS_TCP_LIBRARY OpenDDS_Tcp${DEBUG_SUFFIX}
    HINTS ${DDS_ROOT}/lib $ENV{DDS_ROOT}/lib)
find_library(DDS_REPO_LIBRARY OpenDDS_InfoRepoDiscovery${DEBUG_SUFFIX}
    HINTS ${DDS_ROOT}/lib $ENV{DDS_ROOT}/lib)
find_library(TAO_CORE_LIBRARY TAO${DEBUG_SUFFIX}
    HINTS ${ACE_ROOT}/lib $ENV{ACE_ROOT}/lib)
find_library(ACE_CORE_LIBRARY ACE${DEBUG_SUFFIX}
    HINTS ${ACE_ROOT}/lib $ENV{ACE_ROOT}/lib)
find_library(TAO_SERVER_LIBRARY TAO_PortableServer${DEBUG_SUFFIX}
    HINTS ${ACE_ROOT}/lib $ENV{ACE_ROOT}/lib)
find_library(TAO_ANY_LIBRARY TAO_AnyTypeCode${DEBUG_SUFFIX}
    HINTS ${ACE_ROOT}/lib $ENV{ACE_ROOT}/lib)
find_library(TAO_UTIL_LIBRARY TAO_Svc_Utils${DEBUG_SUFFIX}
    HINTS ${ACE_ROOT}/lib $ENV{ACE_ROOT}/lib)

if(WIN32)
    set(DDS_EXTRA_LIBRARIES netapi32 advapi32 user32 ws2_32)
    set(DDS_DEFINITIONS -DRTI_WIN32 -DNDDS_DLL_VARIABLE /MD)
else(WIN32)
    set(DDS_EXTRA_LIBRARIES dl nsl m pthread rt)
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(bits_flag -m64)
		set(DDS_DEFINITIONS -DRTI_UNIX -m64)
    else(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(bits_flag -m32)
		set(DDS_DEFINITIONS -DRTI_UNIX -D__i386__=1 -m32)
    endif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
endif(WIN32)

set(DDS_INCLUDE_DIRS ${DDS_INCLUDE_DIR} ${ACE_INCLUDE_DIR} ${TAO_INCLUDE_DIR} ${TAO_INCLUDE_DIR}/orbsvcs)
set(DDS_LIBRARIES ${DDS_CORE_LIBRARY} ${DDS_TCP_LIBRARY} ${DDS_REPO_LIBRARY}
    ${TAO_CORE_LIBRARY} ${ACE_CORE_LIBRARY} ${TAO_SEVER_LIBRARY} ${TAO_ANY_LIBRARY}
    ${TAO_UTIL_LIBRARY} ${DDS_EXTRA_LIBRARIES})

file(GLOB DDS_VERSION_FILE ${DDS_ROOT}/VERSION
    $ENV{DDS_ROOT}/VERSION)
if(DDS_VERSION_FILE)
    string(REGEX MATCH "OpenDDS version ([0-9]+).([0-9]+)"
        DDS_VERSION "${DDS_VERSION_FILE}")
    set(DDS_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(DDS_VERSION_MINOR ${CMAKE_MATCH_2})
    set(DDS_VERSION_CAN)
endif(DDS_VERSION_FILE)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DDS
    REQUIRED_VARS DDS_INCLUDE_DIR DDS_CORE_LIBRARY TAO_CORE_LIBRARY
    ACE_CORE_LIBRARY
    VERSION_VAR ${DDS_VERSION})