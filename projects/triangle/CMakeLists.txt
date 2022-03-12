cmake_minimum_required(VERSION 3.22)
#set project
project(HelloTriangle VERSION 1.0)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
configure_file(helloTriangle.h.in include/helloTriangle.h @ONLY)


# add executable
add_executable(triangleT helloTriangle.cpp)



#set path for find modules  see below for another way to set the path 
#create variable which is stored in the CACHE to determine root directory
# for windows.  The root directory is always F: or C: with colon.
set(ROOT_DIR_WIN "F:" CACHE STRING 
	"Select Root directory for Windows Installs")
# installs are only F or C for now.
set_property(CACHE ROOT_DIR_WIN PROPERTY STRINGS
	"F:" "C:") 
message(STATUS "Root dir: " ${ROOT_DIR_WIN})
cmake_path(SET rootPath  "${ROOT_DIR_WIN}")
cmake_path(APPEND glfw_full_path "${rootPath}/Program Files" "glfw"
"install")
cmake_path(APPEND glm_full_path "${rootPath}/Program Files" "glm" 
	"install" "include")

# Vulkan::Vulkan finds the file object and all the include directories
# No need to put it in by hand

if (WIN32)
	list(APPEND CMAKE_PREFIX_PATH ${glfw_full_path})
	list(APPEND EXTRA_INCLUDES ${glm_full_path})
	message(STATUS "glfw full path: " ${glfw_full_path})
	message(STATUS "glm full path: "  ${glm_full_path})

endif()


target_include_directories(triangleT PUBLIC "${PROJECT_BINARY_DIR}/include"
	${EXTRA_INCLUDES})
find_package(glfw3 3.3 REQUIRED)

find_package(Vulkan  REQUIRED)
if (Vulkan_FOUND)
	target_link_libraries(triangleT PRIVATE glfw Vulkan::Vulkan)
else()
	target_link_libraries(triangleT glfw)
endif()


#Paths can also be set using string(CONCAT ..) with several strings. 
# The separator needs to be escaped for it to compile correctly. 
# The root directory should be F:, not F for the path to work with cm_path.
# cmake understands that even if the separator won't work in windows the paths iwll
# be searched for correctly as if the correct separator were used. In times of
# trouble these commands produce the windows style paths.
#set(glfw_rel_path "\\Program Files\\glfw\\install")
#string(CONCAT  glfw_full_path ${ROOT_DIR_WIN} ${glfw_rel_path})
#set(glm_rel_path "\\Program Files\\glm\\install\\include")
#string(CONCAT glm_full_path ${ROOT_DIR_WIN} ${glm_rel_path})
