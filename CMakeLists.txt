cmake_minimum_required(VERSION 3.22)
project(VocloningNative VERSION 0.2.9.24 LANGUAGES C CXX)

if(WIN32)
    enable_language(RC)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED JUCE_DIR)
    if(DEFINED ENV{JUCE_DIR})
        set(JUCE_DIR "$ENV{JUCE_DIR}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/JUCE/CMakeLists.txt")
        set(JUCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/JUCE")
    else()
        message(FATAL_ERROR "JUCE 9.0.6 not found. Set -DJUCE_DIR=<path-to-JUCE> or extract JUCE to third_party/JUCE.")
    endif()
endif()

add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")

juce_add_binary_data(VocloningAssets SOURCES
    Assets/VocloningLogo.png
)

juce_add_gui_app(VocloningNative
    PRODUCT_NAME "Vocloning"
    COMPANY_NAME "INDRO"
    BUNDLE_ID "com.indro.vocloning"
)

target_sources(VocloningNative PRIVATE
    Source/Utf8.h
    Source/Main.cpp
    Source/MainComponent.h
    Source/MainComponent.cpp
    Source/VoiceLibrary.h
    Source/VoiceLibrary.cpp
    Source/PocketTTSBackend.h
    Source/PocketTTSBackend.cpp
    Source/SoproTTSBackend.h
    Source/SoproTTSBackend.cpp
)

if(WIN32)
    # Embed the official VOC LONING icon in the executable resource table.
    target_sources(VocloningNative PRIVATE app_icon.rc)
endif()

# MSVC must interpret UTF-8 source literals (Portuguese UI strings) as UTF-8.
if(MSVC)
    target_compile_options(VocloningNative PRIVATE /utf-8)
endif()

target_compile_definitions(VocloningNative PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:VocloningNative,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="${PROJECT_VERSION}"
)

target_link_libraries(VocloningNative PRIVATE
    VocloningAssets
    juce::juce_gui_extra
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_data_structures
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Generate the JuceHeader.h included by the application source files.
# JUCE's CMake API does not create this header unless explicitly requested.
juce_generate_juce_header(VocloningNative)


# If the official standalone runtime has been installed in the project Runtime
# folder, copy it beside the built application for portable local execution.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Runtime/pocket-tts.exe")
    add_custom_command(TARGET VocloningNative POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/Runtime"
                "$<TARGET_FILE_DIR:VocloningNative>/Runtime"
        COMMENT "Copying Pocket TTS runtime beside the application"
    )
else()
    message(WARNING "Pocket TTS runtime not found at configure time; application will build; the selected local engine must be installed separately.")
endif()
