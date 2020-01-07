project (VisLink)

include(${CMAKE_FRAMEWORK_DIR}/project/project.cmake)

# --------------------- Source -----------------------


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set (SOURCEFILES
  image/Texture.cpp
  image/TextureManager.cpp
  net/NetInterface.cpp
  net/Client.cpp
  net/Server.cpp
)

set (HEADERFILES
  image/TextureManager.h
  net/Client.h
  net/Server.h
)

#set(SANDBOX_DIR ${CMAKE_SOURCE_DIR}/../sandbox)
#include_directories(${SANDBOX_DIR}/src)
#include_directories(${SANDBOX_DIR}/cmake/framework/external/stb/src)

#set(SOURCEFILES ${SOURCEFILES} 
#  ${SANDBOX_DIR}/src/sandbox/Entity.cpp
#  ${SANDBOX_DIR}/src/sandbox/graphics/vulkan/VulkanDevice.cpp
#  ${SANDBOX_DIR}/src/sandbox/graphics/vulkan/render/VulkanCommandPool.cpp
#  ${SANDBOX_DIR}/src/sandbox/graphics/vulkan/VulkanDeviceRenderer.cpp
#  ${SANDBOX_DIR}/src/sandbox/image/Image.cpp
#)
#message(${SOURCEFILES} )

# --------------------- Dependencies -----------------------

set(SandBoxArgs -DWITH_APPS=OFF -DCMAKE_DEBUG_POSTFIX=d)
add_external(SandBox 
	GIT_REPOSITORY https://github.com/dtorban/sandbox.git
	CMAKE_ARGS SandBoxArgs
  	GIT_TAG vulkan-refactor
	LIB_NAME sandbox
	DEBUG_POSTFIX d
)
set(DEP_INCLUDES ${DEP_INCLUDES} 
	${external_dir}/SandBox/src/src
	${external_dir}/SandBox/src/cmake/framework/external/stb/src)

find_package(Vulkan REQUIRED)
message("vk Found" ${Vulkan_FOUND} ${Vulkan_INCLUDE_DIRS} ${Vulkan_LIBRARIES})
include_directories(${Vulkan_INCLUDE_DIRS})
set(DEP_LIBS ${DEP_LIBS} ${Vulkan_LIBRARIES})

find_package(glfw3 REQUIRED)
set(DEP_LIBS ${DEP_LIBS} glfw)

include(${external_dir}/OpenGL/OpenGL.cmake)
useOpenGL()

# --------------------- Executable -----------------------

addSharedLibrary()
