CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(fw_name "capi-security-privilege-manager")

PROJECT(${fw_name})

SET(CMAKE_INSTALL_PREFIX /usr)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})

SET(INC_DIR include)

INCLUDE_DIRECTORIES(
    "${INC_DIR}"
    )

SET(pc_requires "")

INCLUDE(FindPkgConfig)
pkg_check_modules(${fw_name} REQUIRED dlog pkgmgr-info glib-2.0)
FOREACH(flag ${${fw_name}_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall -Werror")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
    ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
MESSAGE("PROFILE_TYPE = ${PROFILE_TYPE}")
IF("${PROFILE_TYPE}" STREQUAL "WEARABLE")
    ADD_DEFINITIONS("-DPROFILE_TYPE_WEARABLE")
ELSEIF("${PROFILE_TYPE}" STREQUAL "MOBILE")
    ADD_DEFINITIONS("-DPROFILE_TYPE_MOBILE")
ELSEIF("${PROFILE_TYPE}" STREQUAL "TV")
    ADD_DEFINITIONS("-DPROFILE_TYPE_TV")
ENDIF()

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=${LIBDIR}")

aux_source_directory(src SOURCES)
ADD_LIBRARY(${fw_name} SHARED ${SOURCES})

TARGET_LINK_LIBRARIES(${fw_name} ${${fw_name}_LDFLAGS})

SET_TARGET_PROPERTIES(${fw_name} 
    PROPERTIES 
    SOVERSION ${API_VERSION} 
    VERSION ${VERSION} 
    CLEAN_DIRECT_OUTPUT 1
)

INSTALL(TARGETS ${fw_name} DESTINATION ${LIBDIR})
INSTALL(
        DIRECTORY ${INC_DIR}/ DESTINATION ${INCLUDEDIR}/privilegemgr
        FILES_MATCHING
        PATTERN "*_private.h" EXCLUDE
        PATTERN "${INC_DIR}/*.h"
        )

SET(PC_NAME ${fw_name})
SET(PC_REQUIRED ${pc_requires})
SET(PC_LDFLAGS -l${fw_name})
SET(PC_CFLAGS -I\${includedir}/privilegemgr)

CONFIGURE_FILE(${fw_name}.pc.in ${fw_name}.pc @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${fw_name}.pc DESTINATION ${LIBDIR}/pkgconfig)


IF(UNIX)

ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
ADD_CUSTOM_COMMAND(
        DEPENDS clean 
        COMMENT "distribution clean"
        COMMAND find
        ARGS    . 
        -not -name config.cmake -and \(
        -name tester.c -or
        -name Testing -or
        -name CMakeFiles -or
        -name cmake.depends -or
        -name cmake.check_depends -or
        -name CMakeCache.txt -or
        -name cmake.check_cache -or
        -name *.cmake -or
        -name Makefile -or
        -name core -or
        -name core.* -or
        -name gmon.out -or
        -name install_manifest.txt -or
        -name *.pc -or
        -name *~ \)
        | grep -v TC | xargs rm -rf
        TARGET  distclean
        VERBATIM
)

ENDIF(UNIX)


SET(ROOT_DIR ${CMAKE_SOURCE_DIR}/cmake_build_tmp/output)
SET(LOCALE_ROOT_PATH "${ROOT_DIR}/target/generic/root/usr/share/locale")

FILE(GLOB _pofiles res/string/*.po)

FOREACH(_file ${_pofiles})
    GET_FILENAME_COMPONENT(_in ${_file} ABSOLUTE)
    GET_FILENAME_COMPONENT(_basename ${_file} NAME_WE)

    SET(_out "${LOCALE_ROOT_PATH}/${_basename}/LC_MESSAGES/privilege.mo")

    ADD_CUSTOM_COMMAND(
        OUTPUT ${_out}
        COMMENT "Generating ${_out}"
        COMMAND mkdir -p ${LOCALE_ROOT_PATH}/${_basename}/LC_MESSAGES
        COMMAND msgfmt -o ${_out} ${_in}
        DEPENDS ${_in}
        )

    SET(mo_files ${mo_files} ${_out})

ENDFOREACH(_file ${_pofiles})

ADD_CUSTOM_TARGET(privilege-string ALL DEPENDS ${mo_files})

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/cmake_build_tmp/output/target/generic/root/usr/share/locale DESTINATION ${DATADIR})

ADD_SUBDIRECTORY(res/dbspace)
