cmake_minimum_required(VERSION 3.10)
project(OblivionSleepInOwnedBeds C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

set(VERSION_MAJOR 1)
set(VERSION_MINOR 2)
set(VERSION_PATCH 0)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# DLL Details
SET(CMAKE_RC_COMPILER "windres")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
set(RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
set(RES_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.res)
add_custom_command(OUTPUT ${RES_FILE}
    COMMAND ${CMAKE_RC_COMPILER} -O coff -i ${RC_FILE} -o ${RES_FILE}
    MAIN_DEPENDENCY ${RC_FILE}
)
set(VERSION_RESOURCE ${RES_FILE})

# Source Files
file(GLOB SOURCE_FILES *.cpp *.h)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
 
# DLL Details
set_source_files_properties(${VERSION_RESOURCE} PROPERTIES EXTERNAL_OBJECT TRUE)
target_sources(${PROJECT_NAME} PRIVATE ${VERSION_RESOURCE})
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h @ONLY)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wextra -O3 -flto -ffunction-sections -fdata-sections)
target_link_options(${PROJECT_NAME} PRIVATE -static -Wl,--gc-sections)

# POST BUILD
set(OBR_PATH $ENV{OBR_PATH})
set(SRC_INI "${CMAKE_CURRENT_SOURCE_DIR}/sleepinownedbeds.ini")
set(dest_dirs
    "${CMAKE_CURRENT_BINARY_DIR}/rel-asi/"
    "${CMAKE_CURRENT_BINARY_DIR}/rel-obse/OblivionRemastered/Binaries/Win64/OBSE/plugins"
    "${OBR_PATH}"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Stripping ${PROJECT_NAME}"
)

foreach(dir IN LISTS dest_dirs)
    set(ext ".asi")

    if(dir MATCHES "/OBSE/plugins")
        set(ext ".dll")
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${dir}/${PROJECT_NAME}${ext}"
        COMMAND ${CMAKE_COMMAND} -E copy "${SRC_INI}" "${dir}/sleepinownedbeds.ini"
        COMMENT "Copying ${PROJECT_NAME}${ext} and sleepinownedbeds.ini to ${dir}"
    )
endforeach()

set(RELEASE_OBSE_DIR "${CMAKE_CURRENT_BINARY_DIR}/rel-obse/")
set(RELEASE_ASI_DIR  "${CMAKE_CURRENT_BINARY_DIR}/rel-asi/")
set(RELEASE_OBSE_ZIP "${CMAKE_CURRENT_BINARY_DIR}/rel-obse.zip")
set(RELEASE_ASI_ZIP  "${CMAKE_CURRENT_BINARY_DIR}/rel-asi.zip")

add_custom_command(
    OUTPUT "${RELEASE_OBSE_ZIP}"
    WORKING_DIRECTORY "${RELEASE_OBSE_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar cf "${RELEASE_OBSE_ZIP}" --format=zip -- .
    DEPENDS ${PROJECT_NAME}
    COMMENT "Creating rel-obse.zip"
)

add_custom_command(
    OUTPUT "${RELEASE_ASI_ZIP}"
    WORKING_DIRECTORY "${RELEASE_ASI_DIR}"
    COMMAND ${CMAKE_COMMAND} -E tar cf "${RELEASE_ASI_ZIP}" --format=zip -- .
    DEPENDS ${PROJECT_NAME}
    COMMENT "Creating rel-asi.zip"
)

# Group both in a single target 
add_custom_target(package ALL
    DEPENDS "${RELEASE_OBSE_ZIP}" "${RELEASE_ASI_ZIP}"
    COMMENT "Packaging rel-obse.zip and rel-asi.zip"
)