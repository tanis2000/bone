project({{project_name}})

cmake_minimum_required(VERSION 2.8.12)

# cmake modules directory
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Binocle/build/cmake")

# Skip the platform compiler checks for cross compiling
set (CMAKE_CXX_COMPILER_WORKS TRUE)
set (CMAKE_C_COMPILER_WORKS TRUE)

# Enables CMake debugging with VSCode
include(CMakeToolsHelpers OPTIONAL)

# Add our own DEBUG define for Debug build types
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

include(BinocleUtils)

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "1")
SET(VERSION_PATCH "0")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
# this must be 4 numbers
SET(VERSION_RC "${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH},0")
SET(WEBSITE "http://www.altralogica.it/")

if (MSVC)
	message("Including Windows defines")
	include(BinocleWindows)
elseif(APPLE)
	if(IOS)
		message("Including iOS defines")
		include(BinocleIOS)
	else()
		message("Including Mac defines")
		include(BinocleMac)
	endif()
elseif(ANDROID)
	message("Including Android defines")
	include(BinocleAndroid)
elseif(EMSCRIPTEN)
	message("Including Emscripten defines")
	include(BinocleWeb)
endif()


file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADERS 
	${PROJECT_SOURCE_DIR}/src/*.hpp
	${PROJECT_SOURCE_DIR}/src/*.h
	)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(
    ${CMAKE_SOURCE_DIR}/Binocle/src
    ${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty
	${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/TinyXml
	${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/tmxparser
	${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/Overlap2D
	${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/include
	${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl_mixer
	${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/imgui
	${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/glm
	)

add_subdirectory(${PROJECT_SOURCE_DIR}/Binocle/src/Binocle ${PROJECT_BINARY_DIR}/binocle)
add_subdirectory(${PROJECT_SOURCE_DIR}/Binocle/src/ThirdParty ${PROJECT_BINARY_DIR}/ThirdParty)

set(BINOCLE_LINK_LIBRARIES ${BINOCLE_LINK_LIBRARIES} binocle tmxparser tinyxml overlap2d flac freetype sdl_mixer jpeg vorbis ogg lua luasocket imgui)

if (NOT EMSCRIPTEN)
	set(BINOCLE_LINK_LIBRARIES ${BINOCLE_LINK_LIBRARIES} sdl zlib png)
endif()

if (APPLE)
	if(IOS)
		include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/include/configs/default)
	else()
		include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/include/configs/mac)
		include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/glew/include)
		set(BINOCLE_LINK_LIBRARIES ${BINOCLE_LINK_LIBRARIES} glew)
	endif()
endif()

if(ANDROID)
	include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/include/configs/default)
endif()

if(MSVC)
	add_definitions(-DHX_WINDOWS)
	include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/include/configs/windows)
	include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/glew/include)
endif()

if (EMSCRIPTEN)
	include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/include/configs/default)
endif()

if(NOT DEFINED BINOCLE_DATA_DIR)
	if(APPLE) 
		if(IOS)
			set(BINOCLE_DATA_DIR "")
		else ()
			set(BINOCLE_DATA_DIR "../Resources/")
		endif()
	elseif(MSVC)
		set(BINOCLE_DATA_DIR "../../../../../../assets/")
	elseif(EMSCRIPTEN)
		# TODO: make this relative but also make sure that it 
		# becomes relative in BinocleWeb.cmake as they have to be the same
		set(BINOCLE_DATA_DIR "/Users/tanis/Documents/binocle/assets/")
	endif()
endif()
message("Data dir is ${BINOCLE_DATA_DIR}")



CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/sys_config.h.cmake ${CMAKE_SOURCE_DIR}/src/sys_config.h)
SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/src/sys_config.h PROPERTIES GENERATED TRUE)
#include_directories(${CMAKE_CURRENT_BINARY_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/test ${CMAKE_CURRENT_BINARY_DIR}/src/ThirdParty/TinyXml ${CMAKE_CURRENT_BINARY_DIR}/src/ThirdParty/tmxparser ${CMAKE_CURRENT_BINARY_DIR}/src/ThirdParty/Overlap2D)

if(APPLE)
	#if(IOS)
		set_property(GLOBAL PROPERTY USE_FOLDERS ON)
		file(GLOB RESOURCES_LIST ${CMAKE_SOURCE_DIR}/assets/*)
		file(GLOB RESOURCES_LIST_LUASOCKET ${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/luasocket/src/*.lua)
		set(RESOURCES
			${RESOURCES_LIST}
				${RESOURCES_LIST_LUASOCKET}
			)
		message("Resources list: ${RESOURCES}")
	#endif()
endif()


if (ANDROID)
	include_directories(${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/include)
	add_library(${PROJECT_NAME} SHARED ${SOURCES} ${CMAKE_SOURCE_DIR}/Binocle/src/ThirdParty/sdl/src/main/android/SDL_android_main.c)
else()
	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${EXE_TYPE} ${SOURCES} ${HEADERS} ${BINOCLE_ICON} ${RESOURCES})
endif()

target_link_libraries(${PROJECT_NAME} ${BINOCLE_LINK_LIBRARIES})



if(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_PRODUCT_NAME "${PROJECT_NAME}")
endif()

# Build the shared library next to sdl-endless-runner
set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/src
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/src
	XCODE_ATTRIBUTE_PRODUCT_NAME "${PROJECT_NAME}"
	XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
)

if(APPLE)
	set(CMAKE_MACOSX_RPATH 1)

	set_target_properties(${PROJECT_NAME} PROPERTIES
		MACOSX_RPATH 1
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@loader_path/../Frameworks"
		MACOSX_BUNDLE_GUI_IDENTIFIER "it.altralogica.${PROJECT_NAME}"
		)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
		#set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE "${RESOURCES}")
		# file(GLOB_RECURSE RESOURCES
  # 			${CMAKE_SOURCE_DIR}/data/*
		# )
		# set_source_files_properties(
  # 			${RESOURCES}
  # 			PROPERTIES
  # 			MACOSX_PACKAGE_LOCATION Resources
		# )
	set_source_files_properties(
		${RESOURCES}
		PROPERTIES
		MACOSX_PACKAGE_LOCATION Resources)
	if(IOS)
		set_target_properties(
			${PROJECT_NAME} 
			PROPERTIES
			MACOSX_BUNDLE YES
			MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/Binocle/build/ios/Info.plist"
			XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
			XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS YES
			XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0
			RESOURCE "${RESOURCES}"
			)

		# Copy resources
		#add_custom_command(
		#    TARGET ${NAME}
		#    POST_BUILD
		#    COMMAND /Developer/Library/PrivateFrameworks/DevToolsCore.framework/Resources/pbxcp -exclude .DS_Store -exclude CVS -exclude .svn -resolve-src-symlinks ${CMAKE_SOURCE_DIR}/data ${APP_NAME}
		#)
	endif()
endif()


if (MSVC)
    set (EXE_TYPE WIN32)
#    set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_SOURCE_DIR}/build/cmake/Binocle.rc)
elseif(APPLE)
    #ICNS
    set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME}.icns)
	if (NOT IOS)
	    set(BINOCLE_ICON ${CMAKE_SOURCE_DIR}/build/cmake/Binocle.icns)
	endif()
    set_source_files_properties(${BINOCLE_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set (EXE_TYPE MACOSX_BUNDLE)
endif()

if (ANDROID)
	message("Post build will copy to ${CMAKE_CURRENT_BINARY_DIR}/../../../android-project/libs/${ANDROID_ABI}/libmain.so")
	add_custom_command(TARGET ${PROJECT_NAME} 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "$<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../../../android-project/libs/${ANDROID_ABI}/libmain.so")
	add_custom_command(TARGET ${PROJECT_NAME} 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../../../android-project/libs/${ANDROID_ABI}/libmain.so)
#	file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/../android-project/libs/${ANDROID_ABI}/lib${PROJECT_NAME}.so ${CMAKE_CURRENT_BINARY_DIR}/../android-project/libs/${ANDROID_ABI}/libmain.so)
endif()

message("Linking with the following libraries: ${BINOCLE_LINK_LIBRARIES}")