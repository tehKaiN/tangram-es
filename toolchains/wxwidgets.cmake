# global compile options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wignored-qualifiers -Wtype-limits -Wmissing-field-initializers")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fstack-protector -static")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}  -lc++ -lc++abi")
endif()

if (CMAKE_COMPILER_IS_GNUCC)
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
    OUTPUT_VARIABLE GCC_VERSION)
  string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
  list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
  list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)

  message(STATUS "Using gcc ${GCC_VERSION}")
  if (GCC_VERSION VERSION_GREATER 5.1)
    message(STATUS "USE CXX11_ABI")
    add_definitions("-D_GLIBCXX_USE_CXX11_ABI=1")
  endif()
endif()

check_unsupported_compiler_version()

add_definitions(-DTANGRAM_WXWIDGETS)

# load core library
add_subdirectory(${PROJECT_SOURCE_DIR}/core)

if(TANGRAM_APPLICATION)

  set(EXECUTABLE_NAME "wxtangram")

  find_package(OpenGL REQUIRED)
	find_package(CURL REQUIRED)
	
	# wxWidgets
	find_package(wxWidgets REQUIRED gl adv core base)
	include(${wxWidgets_USE_FILE})
	
	set(LIB_PATH "${CMAKE_BINARY_DIR}/lib")
	
  add_library(${EXECUTABLE_NAME}
    ${PROJECT_SOURCE_DIR}/platforms/wxwidgets/src/wxtangram.cpp
    ${PROJECT_SOURCE_DIR}/platforms/wxwidgets/src/wxtangramplatform.cpp
    ${PROJECT_SOURCE_DIR}/platforms/common/urlClient.cpp
    ${PROJECT_SOURCE_DIR}/platforms/common/platform_gl.cpp
    ${PROJECT_SOURCE_DIR}/platforms/common/glad.c
  )
		
  target_include_directories(${EXECUTABLE_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/platforms/common
		${CURL_INCLUDE_DIRS}
	)
		
  target_link_libraries(${EXECUTABLE_NAME}
    ${CORE_LIBRARY}
		${CURL_LIBRARIES}
    # only used when not using external lib
    -pthread
    ${OPENGL_LIBRARIES}
		${wxWidgets_LIBRARIES}
	)

  add_resources(${EXECUTABLE_NAME} "${PROJECT_SOURCE_DIR}/scenes")
	
endif()
