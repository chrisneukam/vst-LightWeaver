cmake_minimum_required(VERSION 3.22)
project(LightWeaver VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE via FetchContent
include(FetchContent)
FetchContent_Declare(
    juce
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.0
)
FetchContent_MakeAvailable(juce)

# Plugin Target Setup
juce_add_plugin(LightWeaver
    COMPANY_NAME "LightWeaver"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    PLUGIN_MANUFACTURER_CODE Ltwv
    PLUGIN_CODE Ltw1
    FORMATS VST3
)

juce_generate_juce_header(LightWeaver)

target_compile_definitions(LightWeaver
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# Source Files Matching context layout
target_sources(LightWeaver
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
)

target_include_directories(LightWeaver
    PRIVATE
        src
)

target_link_libraries(LightWeaver
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)