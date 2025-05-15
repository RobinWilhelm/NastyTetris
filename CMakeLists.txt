cmake_minimum_required(VERSION 3.20)

include(extern/cmake-precompiled-header/PrecompiledHeader.cmake)
project(NastyTetris LANGUAGES CXX VERSION 0.1)

configure_file(Config.h.in Config.h)

set(PCH "src/iepch.h")
set(PCH_SOURCE "src/iepch.cpp") 

set(SOURCES 
	"${PCH}"	
	"${PCH_SOURCE}"
	"src/EntryPoint.cpp"  
	"src/Application.h"
	"src/Application.cpp"
	"src/Renderer.h"
	"src/Renderer.cpp"
	"src/Sprite2DPipeline.h"
	"src/Sprite2DPipeline.cpp"
	"src/Sprite.h"
	"src/Sprite.cpp"
	"src/TetrisGameScene.h"
	"src/TetrisGameScene.cpp"
	"src/Tetromino.h"
	"src/Tetromino.cpp"
	"src/Scene.h"
	"src/Scene.cpp"
	"src/GPUPipeline.h"
	"src/GPUPipeline.cpp"
	"src/RenderCommandBuffer.h"
	"src/RenderCommandBuffer.cpp"
	"src/Window.h"
	"src/Window.cpp"
	"src/AssetManager.h"
	"src/AssetManager.cpp"
	"src/AssetRepository.h"
	"src/Shader.h"
	"src/Shader.cpp"
	"src/ObjectPool.h"
	"src/Asset.h"
	"src/Asset.cpp"
	"src/AssetView.h"
	"src/CoreAPI.h"
	"src/OrthographicCamera.h"
	"src/OrthographicCamera.cpp"
	"src/Log.h"
	"src/Assert.h"
)

set(DIRECTXTK_SOURCES 	
	"extern/directxtk/SimpleMath.h"
	"extern/directxtk/SimpleMath.cpp"
	"extern/directxtk/SimpleMath.inl" 
)

set(IMGUI_SOURCES 
	"extern/imgui/imgui.h"
	"extern/imgui/imgui.cpp"
	"extern/imgui/imconfig.h"
	"extern/imgui/imgui_demo.cpp"
	"extern/imgui/imgui_draw.cpp"
	"extern/imgui/imgui_impl_sdl3.cpp"
	"extern/imgui/imgui_impl_sdl3.h"
	"extern/imgui/imgui_impl_sdlgpu3.cpp"
	"extern/imgui/imgui_impl_sdlgpu3.h"
	"extern/imgui/imgui_internal.h"
	"extern/imgui/imgui_impl_sdlgpu3_shaders.h"
	"extern/imgui/imgui_tables.cpp"
	"extern/imgui/imgui_widgets.cpp"
	"extern/imgui/imstb_rectpack.h"
	"extern/imgui/imstb_textedit.h"
	"extern/imgui/imstb_truetype.h"
)

set(SHADER_SOURCES 
	"src/Shader/TextureXColor.frag.hlsl"
	"src/Shader/SpriteBatch.vert.hlsl"
	"src/Shader/GPUPipelineBase.vertincl.hlsl"
)	

set (EXTERNAL_SOURCES ${DIRECTXTK_SOURCES} ${IMGUI_SOURCES})

list (APPEND SOURCES ${SHADER_SOURCES} ${EXTERNAL_SOURCES})
	
# add shaders to the project but do not compile them
if(MSVC)
	set_property(SOURCE ${SHADER_SOURCES} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")	
endif()

set(SDL_STATIC ON CACHE INTERNAL "")
#find_package(SDL3 REQUIRED)
#find_package(SDL3_image REQUIRED)


set(DIRECTXTK_INCLUDE_DIRS "extern/directxtk")

set(SDL3_INCLUDE_DIRS "extern/SDL3/include")
set(SDL3_BUILD "extern/SDL3/build")

set(SDL3_IMAGE_INCLUDE_DIRS "extern/SDL3_image/include")
set(SDL3_IMAGE_BUILD "extern/SDL3_image/build")

set(SDL3_SHADERCROSS_INCLUDE_DIRS "extern/SDL3_shadercross/include")
set(SDL3_SHADERCROSS_BUILD_DIRS "extern/SDL3_shadercross/build")

include_directories(${DIRECTXTK_INCLUDE_DIRS})
include_directories(${SDL3_INCLUDE_DIRS})
include_directories(${SDL3_IMAGE_INCLUDE_DIRS})
include_directories(${SDL3_SHADERCROSS_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} "${SOURCES}")

set(PCH_ABSOLUTE "${CMAKE_SOURCE_DIR}/${PCH}") # clang will complain about windows specific path if we dont set it to an absolute path here
add_precompiled_header(${PROJECT_NAME} "${PCH_ABSOLUTE}" SOURCE_CXX "${PCH_SOURCE}")

target_link_directories(${PROJECT_NAME} 
	PUBLIC ${SDL3_BUILD} 
	PUBLIC ${SDL3_IMAGE_BUILD} 
	PUBLIC ${SDL3_SHADERCROSS_BUILD_DIRS}
)
target_link_libraries(${PROJECT_NAME} 
	SDL3 
	SDL3_image 
	SDL3_shadercross-static
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# compiler warning settings
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()


# set project structure to be the same as the folder structure
foreach(_source IN ITEMS ${SOURCES})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

# remove custom compile flags from all 3rdparty sourcefiles (eg precompiled headers)
foreach(_source in ${EXTERNAL_SOURCES})
	set_source_files_properties("${_source}" PROPERTIES COMPILE_FLAGS "")
endforeach()