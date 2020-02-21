if(CMAKE_SIZEOF_VOID_P MATCHES "8")
    set(HL2SDK_64BIT ON)
    add_compile_options(-DPLATFORM_64BITS=1)
    message(STATUS "Building for 64 Bit")
else()
    set(HL2SDK_64BIT OFF)
    message(STATUS "Building for 32 Bit")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(OSX ON)
    add_compile_options(-DOSX -D_OSX)
    set(HL2SDK_LIB_STATIC_EXT ".a")
    set(HL2SDK_LIB_SHARED_EXT ".dylib")
    set(HL2SDK_LIB_PREFIX "lib")

    if(HL2SDK_64BIT)
        set(HL2SDK_LIB_DIR ${HL2SDK_PATH}/lib/osx64)
        set(HL2SDK_LIB_SUFFIX "")
    else()
        set(HL2SDK_LIB_DIR ${HL2SDK_PATH}/lib/mac)
        set(HL2SDK_LIB_SUFFIX "_i486")
    endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX ON)
    add_compile_options(-D_LINUX -DLINUX)
    set(HL2SDK_LIB_STATIC_EXT ".a")
    set(HL2SDK_LIB_SHARED_EXT ".so")
    set(HL2SDK_LIB_PREFIX "lib")

    if(HL2SDK_64BIT)
        set(HL2SDK_LIB_DIR ${HL2SDK_PATH}/lib/linux64)
        set(HL2SDK_LIB_SUFFIX "")
    else()
        set(HL2SDK_LIB_DIR ${HL2SDK_PATH}/lib/linux)
        set(HL2SDK_LIB_SUFFIX "_i486")
    endif()
elseif(WIN32)
    set(HL2SDK_LIB_DIR ${HL2SDK_PATH}/lib/public)
    set(HL2SDK_LIB_STATIC_EXT ".lib")
    set(HL2SDK_LIB_SHARED_EXT ".dll")
endif()

link_directories(${HL2SDK_LIB_DIR})

if(MSVC)
    add_compile_options(-DCOMPILER_MSVC)
else()
    add_compile_options(-DCOMPILER_GCC) ## clang or gcc
endif()

if(MINGW)
    add_compile_options(-fpermissive)
endif()

if(NOT WIN32)
    set(POSIX ON)

    add_compile_options(
            -DVPROF_LEVEL=1 -DSWDS -D_finite=finite -Dstricmp=strcasecmp -D_stricmp=strcasecmp
            -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp
    )
endif()

if(POSIX)
    add_compile_options(-DPOSIX)
endif()

add_library(tier0 INTERFACE)
target_include_directories(tier0 INTERFACE
        ${HL2SDK_PATH}/public
        ${HL2SDK_PATH}/public/tier0
        )
target_link_libraries(tier0 INTERFACE -ltier0)

add_library(tier1 STATIC
        ${HL2SDK_PATH}/tier1/bitbuf.cpp
        ${HL2SDK_PATH}/tier1/byteswap.cpp
        ${HL2SDK_PATH}/tier1/characterset.cpp
        ${HL2SDK_PATH}/tier1/checksum_crc.cpp
        ${HL2SDK_PATH}/tier1/checksum_md5.cpp
        ${HL2SDK_PATH}/tier1/commandbuffer.cpp
        ${HL2SDK_PATH}/tier1/convar.cpp
        ${HL2SDK_PATH}/tier1/datamanager.cpp
        ${HL2SDK_PATH}/tier1/diff.cpp
        ${HL2SDK_PATH}/tier1/generichash.cpp
        ${HL2SDK_PATH}/tier1/interface.cpp
        ${HL2SDK_PATH}/tier1/KeyValues.cpp
        ${HL2SDK_PATH}/tier1/mempool.cpp
        ${HL2SDK_PATH}/tier1/memstack.cpp
        ${HL2SDK_PATH}/tier1/NetAdr.cpp
        ${HL2SDK_PATH}/tier1/newbitbuf.cpp
        ${HL2SDK_PATH}/tier1/processor_detect.cpp
        ${HL2SDK_PATH}/tier1/rangecheckedvar.cpp
        ${HL2SDK_PATH}/tier1/stringpool.cpp
        ${HL2SDK_PATH}/tier1/strtools.cpp
        ${HL2SDK_PATH}/tier1/tier1.cpp
        ${HL2SDK_PATH}/tier1/undiff.cpp
        ${HL2SDK_PATH}/tier1/uniqueid.cpp
        ${HL2SDK_PATH}/tier1/utlbuffer.cpp
        ${HL2SDK_PATH}/tier1/utlbufferutil.cpp
        ${HL2SDK_PATH}/tier1/utlstring.cpp
        ${HL2SDK_PATH}/tier1/utlsymbol.cpp
        )
target_link_libraries(tier1 tier0)
target_include_directories(tier1 PUBLIC
        ${HL2SDK_PATH}/public
        ${HL2SDK_PATH}/public/tier1
        )

add_library(mathlib STATIC
        ${HL2SDK_PATH}/mathlib/anorms.cpp
        ${HL2SDK_PATH}/mathlib/bumpvects.cpp
        ${HL2SDK_PATH}/mathlib/color_conversion.cpp
        ${HL2SDK_PATH}/mathlib/halton.cpp
        ${HL2SDK_PATH}/mathlib/IceKey.cpp
        ${HL2SDK_PATH}/mathlib/imagequant.cpp
        ${HL2SDK_PATH}/mathlib/lightdesc.cpp
        ${HL2SDK_PATH}/mathlib/mathlib_base.cpp
        ${HL2SDK_PATH}/mathlib/polyhedron.cpp
        ${HL2SDK_PATH}/mathlib/powsse.cpp
        ${HL2SDK_PATH}/mathlib/quantize.cpp
        ${HL2SDK_PATH}/mathlib/randsse.cpp
        ${HL2SDK_PATH}/mathlib/simdvectormatrix.cpp
        ${HL2SDK_PATH}/mathlib/sparse_convolution_noise.cpp
        ${HL2SDK_PATH}/mathlib/sse.cpp
        ${HL2SDK_PATH}/mathlib/sseconst.cpp
        ${HL2SDK_PATH}/mathlib/ssenoise.cpp
        ${HL2SDK_PATH}/mathlib/vector.cpp
        ${HL2SDK_PATH}/mathlib/vmatrix.cpp
        )
target_compile_options(mathlib PRIVATE -Wno-c++11-narrowing)
target_link_libraries(mathlib PUBLIC tier0 tier1)
target_include_directories(mathlib PUBLIC ${HL2SDK_PATH}/public/mathlib)

add_library(vstdlib INTERFACE)
target_include_directories(vstdlib INTERFACE
        ${HL2SDK_PATH}/public
        ${HL2SDK_PATH}/public/vstdlib
        )
target_link_libraries(vstdlib INTERFACE -lvstdlib)

add_library(interfaces STATIC
        ${HL2SDK_PATH}/interfaces/interfaces.cpp
        )
target_include_directories(interfaces PUBLIC
        ${HL2SDK_PATH}/public
        ${HL2SDK_PATH}/public/interfaces
        )

add_library(amtl INTERFACE)
target_include_directories(amtl INTERFACE
        ${SOURCEMOD_PATH}/public/amtl
        ${SOURCEMOD_PATH}/public/amtl/amtl
        )

add_library(smsdk INTERFACE)
target_include_directories(smsdk INTERFACE
        ${SOURCEMOD_PATH}/core
        ${SOURCEMOD_PATH}/public
        ${SOURCEMOD_PATH}/sourcepawn/include
        )
target_link_libraries(smsdk INTERFACE amtl)

add_library(smsdk_ext INTERFACE)
target_sources(smsdk_ext INTERFACE ${SOURCEMOD_PATH}/public/smsdk_ext.cpp)
target_link_libraries(smsdk_ext INTERFACE smsdk tier0 tier1 mathlib vstdlib interfaces)