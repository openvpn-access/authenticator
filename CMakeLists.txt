cmake_minimum_required(VERSION 3.15)
project(openvpn-access-authenticator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Options
option(BUILD_TESTS "Build authenticator tests" ON)
option(RUN_TESTS "Run tests automatically" ON)
option(GENERATE_DOCUMENTATION "Generate documentation" ON)

# Documentation
add_custom_target(documentation ALL
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		COMMAND doxygen ./Doxyfile)

# Main executable
set(AUTHENTICATOR_SOURCES
		src/main.cpp
		src/authentication.cpp
		src/database.cpp
		src/log.cpp
		src/util.cpp
		src/config.cpp
		src/user.cpp)

set(AUTHENTICATOR_INCLUDES
		src/include)

add_executable(authenticator ${AUTHENTICATOR_SOURCES})
target_include_directories(authenticator PRIVATE ${AUTHENTICATOR_INCLUDES})

if(GENERATE_DOCUMENTATION)
	add_dependencies(authenticator documentation)
endif()

# Tests executable
if(BUILD_TESTS)
	add_custom_target(copy-test-files ALL
			COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/test/authenticator_test.yml ${PROJECT_BINARY_DIR})

	add_executable(authenticator-tests
			${AUTHENTICATOR_SOURCES}
			test/main.test.cpp)
	target_include_directories(authenticator-tests PRIVATE ${AUTHENTICATOR_INCLUDES})
	target_compile_options(authenticator-tests PUBLIC -DTESTING)
	add_dependencies(authenticator-tests copy-test-files)

	add_dependencies(authenticator authenticator-tests)

	if(RUN_TESTS)
		add_custom_command(TARGET authenticator POST_BUILD
				WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
				COMMAND ./authenticator-tests)

		# add_dependencies(authenticator run-tests)
	endif()
endif()

# Allow us to download dependencies
include(FetchContent)

# Grab googletest
if(BUILD_TESTS)
	FetchContent_Declare(googletest
			GIT_REPOSITORY  https://github.com/google/googletest.git
			GIT_TAG master)

	FetchContent_MakeAvailable(googletest)

	target_include_directories(authenticator-tests PUBLIC SYSTEM ${googletest_SOURCE_DIR}/googletest/include)
	target_link_libraries(authenticator-tests gtest)
endif()

# Grab libbcrypt
FetchContent_Declare(libbcrypt
		GIT_REPOSITORY  https://github.com/rg3/libbcrypt.git
		GIT_TAG master)

FetchContent_MakeAvailable(libbcrypt)

add_custom_target(libbcrypt-build ALL
		WORKING_DIRECTORY ${libbcrypt_SOURCE_DIR}
		COMMAND make)

target_include_directories(authenticator PUBLIC SYSTEM ${libbcrypt_SOURCE_DIR})
add_dependencies(authenticator libbcrypt-build)
target_link_libraries(authenticator ${libbcrypt_SOURCE_DIR}/bcrypt.a)

if(BUILD_TESTS)
	target_include_directories(authenticator-tests PUBLIC SYSTEM ${libbcrypt_SOURCE_DIR})
	add_dependencies(authenticator-tests libbcrypt-build)
	target_link_libraries(authenticator-tests ${libbcrypt_SOURCE_DIR}/bcrypt.a)
endif()

# Grab mariadbcpp
FetchContent_Declare(mariadbpp
		GIT_REPOSITORY  https://github.com/viaduck/mariadbpp.git
		GIT_TAG master)

FetchContent_MakeAvailable(mariadbpp)

target_include_directories(authenticator PUBLIC SYSTEM ${mariadbpp_SOURCE_DIR}/include)
target_link_libraries(authenticator mariadbclientpp)

if(BUILD_TESTS)
	target_include_directories(authenticator-tests PUBLIC SYSTEM ${mariadbpp_SOURCE_DIR}/include)
	target_link_libraries(authenticator-tests mariadbclientpp)
endif()

# Grab yaml-cpp
FetchContent_Declare(yamlcpp
		GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
		GIT_TAG yaml-cpp-0.6.3)

SET(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Build yaml-cpp tests" FORCE)
SET(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Build yaml-cpp tools" FORCE)
SET(YAML_CPP_INSTALL OFF CACHE BOOL "Install yaml-cpp" FORCE)

FetchContent_MakeAvailable(yamlcpp)

target_include_directories(authenticator PUBLIC SYSTEM ${yamlcpp_SOURCE_DIR}/include)
target_link_libraries(authenticator yaml-cpp)

if(BUILD_TESTS)
	target_include_directories(authenticator-tests PUBLIC SYSTEM ${yamlcpp_SOURCE_DIR}/include)
	target_link_libraries(authenticator-tests yaml-cpp)
endif()
