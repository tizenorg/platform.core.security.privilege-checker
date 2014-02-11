CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(privilege-checker)

MESSAGE("build privilege-checker util")

SET(CMAKE_INSTALL_PREFIX /usr)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(LIBDIR "\${prefix}/lib")
SET(INCLUDEDIR "\${prefix}/include")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED dlog sqlite3 libxml-2.0 libtzplatform-config)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS_PROFILING    " -g -pg")
SET(CMAKE_CXX_FLAGS_PROFILING  " -std=c++0x -g -pg")
SET(CMAKE_C_FLAGS_DEBUG        " -g")
SET(CMAKE_CXX_FLAGS_DEBUG      " -std=c++0x -g")
SET(CMAKE_C_FLAGS_RELEASE      " -g")
SET(CMAKE_CXX_FLAGS_RELEASE    " -std=c++0x -g")
SET(CMAKE_C_FLAGS_CCOV         " -g --coverage")
SET(CMAKE_CXX_FLAGS_CCOV       " -std=c++0x -g --coverage")

SET(src_dir "${CMAKE_SOURCE_DIR}/util/src")
SET(include_dir "${CMAKE_SOURCE_DIR}/util/include")
SET(xml_include_dir "/usr/include/libxml2")

## Additional flag
ADD_DEFINITIONS("-fvisibility=hidden")
ADD_DEFINITIONS("-Wall -Werror")
ADD_DEFINITIONS("-DDLOG_ERROR_ENABLED")

###################################################################################################
## for privilege-checker (executable)
INCLUDE_DIRECTORIES(
	${pkgs_INCLUDE_DIRS} 
	${include_dir}
    ${xml_include_dir}	
	)
SET(PRIVILEGE_CHECKER_SOURCES 
	${src_dir}/Parser.cpp
	${src_dir}/Main.cpp
	${src_dir}/Util.cpp
	${src_dir}/DbManager.cpp
	)
SET(PRIVILEGE_CHECKER_HEADERS
)
SET(PRIVILEGE_CHECKER_LDFLAGS " -module -avoid-version ")

SET(PRIVILEGE_CHECKER_CFLAGS  " ${CFLAGS} -fPIC ")

ADD_DEFINITIONS("-DLOG_TAG=\"PRIVILEGE_CHECKER\"")
ADD_EXECUTABLE(privilege-checker ${PRIVILEGE_CHECKER_SOURCES})
TARGET_LINK_LIBRARIES(privilege-checker ${pkgs_LDFLAGS} ${pkgs_LIBRARIES})
SET_TARGET_PROPERTIES(privilege-checker PROPERTIES COMPILE_FLAGS "${PRIVILEGE_CHECKER_CFLAGS}")
###################################################################################################

INSTALL(TARGETS privilege-checker DESTINATION ../bin)
