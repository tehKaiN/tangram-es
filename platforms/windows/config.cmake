if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xclang -std=gnu++1y")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DRAPIDJSON_HAS_CXX11_RVALUE_REFS=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc") # Exceptions handler - required for yaml-cpp
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_USE_MATH_DEFINES") # required for geojson-vt-cpp
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Clang may be also used in MSVC-Clang toolchain and that will pass match above,
	# But then clang works in clang-cl mode which emulates cl.exe and accepts MSVC args, not GNU-like
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++ -lc++abi")
endif()

if (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector")
	# Static build
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -DCURL_STATICLIB")
	message(STATUS "Using gcc ${CMAKE_CXX_COMPILER_VERSION}")
	if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
		message(STATUS "USE CXX11_ABI")
		add_definitions("-D_GLIBCXX_USE_CXX11_ABI=1")
	endif()
	if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.0)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-strict-aliasing")
	endif()
endif()


check_unsupported_compiler_version()

add_definitions(-DTANGRAM_WINDOWS)

find_package(OpenGL REQUIRED)
find_package(CURL REQUIRED)

include(cmake/glfw.cmake)

add_executable(tangram
	platforms/windows/src/windowsPlatform.cpp
	platforms/windows/src/main.cpp
	platforms/common/platform_gl.cpp
	platforms/common/imgui_impl_glfw.cpp
	platforms/common/imgui_impl_opengl3.cpp
	platforms/common/urlClient.cpp
	platforms/common/glfwApp.cpp
	platforms/common/glad.c
)

add_subdirectory(platforms/common/imgui)

target_include_directories(tangram
	PRIVATE
	platforms/common
	${CURL_INCLUDE_DIRS}
)

target_link_libraries(tangram
	PRIVATE
	tangram-core
	glfw
	imgui
	${GLFW_LIBRARIES}
	${OPENGL_LIBRARIES}
	${CURL_LIBRARIES}
	-pthread
	# For CURL static
	wsock32 ws2_32 crypt32 wldap32
)

target_compile_options(tangram
	PRIVATE
	-std=c++1y
	-fno-omit-frame-pointer
	-Wall
	-Wreturn-type
	-Wsign-compare
	-Wignored-qualifiers
	-Wtype-limits
	-Wmissing-field-initializers
)

get_nextzen_api_key(NEXTZEN_API_KEY)
target_compile_definitions(tangram
	PRIVATE
	NEXTZEN_API_KEY="${NEXTZEN_API_KEY}")

add_resources(tangram "${PROJECT_SOURCE_DIR}/scenes" "res")
