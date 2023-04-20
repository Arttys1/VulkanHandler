cmake_minimum_required (VERSION 3.10)

project("Vulkan_image")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#disable warning : enum unscoped because of vulkan write in c.
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise /wd\"26812\"")

#disable warning : arithmetic overflow because of stb_image
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise /wd\"26451\"")

file(GLOB_RECURSE SOURCES sources/*.cpp)
file(GLOB_RECURSE HEADERS includes/*.hpp includes/*.h sources/*.h sources/*hpp)

include_directories(${PROJECT_NAME} PUBLIC includes)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

######GLFW#######

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/glfw-3.3.8)

target_link_libraries(${PROJECT_NAME} glfw)

######GLM#######

add_subdirectory(third_party/glm)

target_link_libraries(${PROJECT_NAME} glm)

######VULKAN#####

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.211.0")
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC "C:/VulkanSDK/1.3.211.0/Include")