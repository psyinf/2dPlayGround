include (cmake/setup_cpm.cmake)
include (cmake/deps_cpm_sdl.cmake)
include (cmake/deps_cpm_pg.cmake)

CPMAddPackage(
    NAME sdlpp    

    GITHUB_REPOSITORY "mika314/sdlpp"
    GIT_TAG v0.0.1
    #OPTIONS USE_SDLGFX
)
#pybind
CPMAddPackage(
    NAME pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.13.6
)

CPMAddPackage(
    #zeromq
    NAME ZeroMQ
    GITHUB_REPOSITORY zeromq/libzmq
    GIT_TAG v4.3.5
    OPTIONS "WITH_LIBSODIUM OFF"
    "WITH_PERF_TOOL OFF"
    "WITH_DOCS OFF"
    "WITH_TESTS OFF"
    "WITH_TOOLS OFF"
    "WITH_LIBUNWIND OFF"
    "WITH_SYSTEMD OFF"
    "WITH_OPENPGM OFF"
    "WITH_DRAFTS OFF"
    "WITH_CURVE OFF"
    "WITH_LZ4 OFF"
    "WITH_GSSAPI OFF"
    "WITH_LIBSODIUM OFF"
    "WITH_LIBZMQ_STATIC ON"
    "WITH_LIBZMQ_SHARED OFF"
    "WITH_TLS OFF"
    "WITH_VALGRIND OFF"
    "WITH_ASAN OFF"
    "WITH_UBSAN OFF"
    "WITH_MSAN OFF"
    "WITH_TSAN OFF"
    #"WITH_CPACK OFF"
    "BUILD_SHARED OFF"
    "BUILD_TESTS OFF"
    "ZMQ_BUILD_TESTS OFF"
    
)
set (BTCPP_EXTRA_LIBRARIES libzmq-static)
CPMAddPackage(
    Name btcpp
    GITHUB_REPOSITORY BehaviorTree/BehaviorTree.CPP
    GIT_TAG a44619cd8871c484f5c4b24f9958302455f19211
    SYSTEM TRUE
    OPTIONS 
    "BTCPP_SHARED_LIBS OFF"
    "BTCPP_BUILD_TOOLS OFF"
    "BTCPP_EXAMPLES OFF"
    "BTCPP_UNIT_TESTS OFF"
    "BTCPP_GROOT_INTERFACE ON"
    "BTCPP_SQLITE_LOGGING OFF"
    
)

CPMAddPackage(
    NAME Catch2

    GITHUB_REPOSITORY "catchorg/Catch2"
    GIT_TAG v3.5.2
     OPTIONS
    "CATCH_BUILD_TESTING OFF"
    "CATCH_BUILD_EXAMPLES OFF"
    "CATCH_BUILD_EXTRA_TESTS OFF"
    "CATCH_BUILD_FUZZERS OFF"
)

#CPMAddPackage(
#    NAME fmt
#    GITHUB_REPOSITORY fmtlib/fmt
#    GIT_TAG 10.2.1
#    OPTIONS
#    "FMT_DOC OFF"
#    "FMT_TEST OFF"
#    "FMT_INSTALL OFF"
#    "FMT_PEDANTIC OFF"
#    "FMT_WERROR OFF"
#    "FMT_USE_CCACHE OFF"
#    "FMT_USE_FIND_PACKAGE OFF"
#    "FMT_USE_CONFIG OFF"
#    "FMT_USE_PKG_CONFIG OFF"
#    "FMT_USE_CONAN OFF"
#    "FMT_USE_VCPKG OFF"
#    "FMT_USE_ASAN OFF"
#    "FMT_USE_UBSAN OFF"
#    "FMT_USE_MSAN OFF"
#    "FMT_USE_TSAN OFF"
#    "FMT_USE_CODE_ANALYSIS OFF"
#    "FMT_USE_CPPCHECK OFF"
#    
#)

CPMAddPackage(
    NAME magic_enum
    GITHUB_REPOSITORY Neargye/magic_enum
    GIT_TAG v0.9.5
)



if (NOT ($ENV{GITHUB_ACTIONS}))
CPMAddPackage(
	NAME resources
	GITHUB_REPOSITORY psyinf/resources-galaxy
    GIT_TAG main
)
CPMAddPackage(
	NAME resources-sound
	GITHUB_REPOSITORY psyinf/resources-sound
    GIT_TAG main
)
endif()

CPMAddPackage(
	NAME nlohmann_json
	GITHUB_REPOSITORY nlohmann/json
	GIT_TAG v3.11.3
)

CPMAddPackage(
    NAME yaml-cpp
    GITHUB_REPOSITORY jbeder/yaml-cpp
    GIT_TAG 0.8.0
    OPTIONS
    "YAML_CPP_BUILD_TESTS OFF"
)

CPMAddPackage(
    NAME spdlog

    GITHUB_REPOSITORY gabime/spdlog
    GIT_TAG v1.13.0
    OPTIONS
    "SPDLOG_BUILD_EXAMPLES OFF"
    "SPDLOG_BUILD_TESTS OFF"
    "SPDLOG_BUILD_BENCH OFF"
    "SPDLOG_BUILD_SHARED OFF"
    "SPDLOG_FMT_EXTERNAL ON"
)
CPMAddPackage(
    NAME cereal

    GITHUB_REPOSITORY "USCiLab/cereal"
    GIT_TAG HEAD
    OPTIONS "SKIP_PERFORMANCE_COMPARISON ON" "BUILD_SANDBOX OFF" "BUILD_TESTS OFF"
)


CPMAddPackage(
	NAME EnTT
	GITHUB_REPOSITORY skypjack/entt
	GIT_TAG v3.12.2
)

CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG v1.91.6-docking
)
#if (imgui_ADDED)
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp # optionally comment this out
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends/)
target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS) # optional imgui setting
set_target_properties(imgui PROPERTIES FOLDER third-party) # optoinal IDE dir
#alias
add_library(imgui::imgui ALIAS imgui)
#endif()
CPMAddPackage(
    NAME vfspp
    GITHUB_REPOSITORY nextgeniuspro/vfspp
    GIT_TAG v2.0.0
)

if(SCRIPTING)
CPMAddPackage(
    NAME cparse
    GITHUB_REPOSITORY psyinf/cparse
    GIT_TAG b98299c7b0e94d1300ab227bead61fdd72fc1ef1
)

CPMAddPackage(
    NAME pybind11
    GITHUB_REPOSITORY pybind/pybind11
    GIT_TAG v2.12.0
)
endif()

CPMAddPackage(NAME  IconFontCppHeaders
    GIT_TAG         main
    GITHUB_REPOSITORY  juliettef/IconFontCppHeaders
    GIT_SHALLOW     TRUE
    DOWNLOAD_ONLY   TRUE
)

if (IconFontCppHeaders_ADDED)
    add_library(IconFontCppHeaders INTERFACE)
    target_include_directories(IconFontCppHeaders INTERFACE "${IconFontCppHeaders_SOURCE_DIR}")
endif()


