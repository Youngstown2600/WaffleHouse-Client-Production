cmake_minimum_required(VERSION 3.20)

project(WaffleHouseClient VERSION 2.5.4 LANGUAGES CXX)

set(APP_DISPLAY_NAME "WaffleHouse-Client" CACHE STRING "Application display name")
set(APP_ID "WaffleHouseClient" CACHE STRING "Application settings/config identifier")
set(APP_EXECUTABLE "wafflehouse-client" CACHE STRING "Executable name")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)

find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)
pkg_check_modules(NCURSESW REQUIRED IMPORTED_TARGET ncursesw)
pkg_check_modules(XKBCOMMON REQUIRED IMPORTED_TARGET xkbcommon)

# The integrated softphone uses the r14-synchronized PJSIP 2.17/PJSUA2 engine (r12 live audio + r13 prefix behavior). Static PJSIP needs its complete pkg-config --static link
# chain (audio, codecs, SRTP, crypto, UUID, etc.) on the final link line.
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    set(ENV{PKG_CONFIG_ALLOW_SYSTEM_LIBS} "1")
endif()
pkg_check_modules(PJPROJECT REQUIRED libpjproject)
if(NOT PJPROJECT_VERSION VERSION_EQUAL "2.17")
    message(FATAL_ERROR "WaffleHouse-Client softphone requires PJSIP 2.17; pkg-config reported ${PJPROJECT_VERSION}")
endif()
if(NOT PJPROJECT_STATIC_LDFLAGS)
    message(FATAL_ERROR "pkg-config did not return static PJSIP flags. Check: pkg-config --cflags --libs --static libpjproject")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    string(JOIN " " _wh_pj_static_flags ${PJPROJECT_STATIC_LDFLAGS})
    string(FIND " ${_wh_pj_static_flags} " " -lstdc++ " _wh_stdcxx_pos)
    if(NOT _wh_stdcxx_pos EQUAL -1)
        message(FATAL_ERROR "FreeBSD PJSIP advertises libstdc++; rebuild the managed PJSIP with ./build.sh --pjsip so it uses base Clang/libc++.")
    endif()
    string(FIND " ${_wh_pj_static_flags} " " -lc++ " _wh_libcxx_pos)
    if(_wh_libcxx_pos EQUAL -1)
        message(FATAL_ERROR "FreeBSD PJSIP does not advertise libc++ linkage; rebuild with ./build.sh --pjsip.")
    endif()
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "FreeBSD WaffleHouse-Client must use base Clang/libc++ for Qt/PJSIP ABI compatibility.")
    endif()
endif()

qt_standard_project_setup()
include(GNUInstallDirs)

add_library(wafflehouse_sipcore
    src/sipcore/Profile.cpp
    src/sipcore/TextPool.cpp
    src/sipcore/Logger.cpp
    src/sipcore/SipTrace.cpp
    src/sipcore/CallSession.cpp
    src/sipcore/SipAccount.cpp
    src/sipcore/SipEngine.cpp
    src/sipcore/SipWireMonitor.cpp
    src/sipcore/RuntimePaths.cpp
)
target_include_directories(wafflehouse_sipcore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PJPROJECT_INCLUDE_DIRS}
)
target_compile_options(wafflehouse_sipcore PUBLIC ${PJPROJECT_CFLAGS_OTHER})
if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    if(NOT WAFFLEHOUSE_FREEBSD_LOCALBASE)
        if(DEFINED ENV{LOCALBASE} AND NOT "$ENV{LOCALBASE}" STREQUAL "")
            set(WAFFLEHOUSE_FREEBSD_LOCALBASE "$ENV{LOCALBASE}")
        else()
            set(WAFFLEHOUSE_FREEBSD_LOCALBASE "/usr/local")
        endif()
    endif()
    target_link_directories(wafflehouse_sipcore PUBLIC "${WAFFLEHOUSE_FREEBSD_LOCALBASE}/lib")
endif()
target_link_libraries(wafflehouse_sipcore PUBLIC ${PJPROJECT_STATIC_LDFLAGS} Threads::Threads)

qt_add_executable(${APP_EXECUTABLE}
    src/main.cpp
    src/backend.cpp
    src/ircbackend.cpp
    src/oscarprotocol.cpp
    src/oscarbackend.cpp
    src/telnetbackend.cpp
    src/ansiterminal.cpp
    src/ansiterminalwidget.cpp
    src/bbsdirectory.cpp
    src/securechannel.cpp
    src/filetransfer.cpp
    src/directtransfer.cpp
    src/transferwindow.cpp
    src/platforminfo.cpp
    src/chatwindow.cpp
    src/mainwindow.cpp
    src/terminalui.cpp
    src/sipcontroller.cpp
    src/sipbackend.cpp
    src/softphonewindow.cpp
)

target_include_directories(${APP_EXECUTABLE} PRIVATE src include)

target_compile_definitions(${APP_EXECUTABLE} PRIVATE
    APP_DISPLAY_NAME="${APP_DISPLAY_NAME}"
    APP_ID="${APP_ID}"
    APP_EXECUTABLE="${APP_EXECUTABLE}"
    APP_VERSION_STRING="2.5.4-r5"
)

target_link_libraries(${APP_EXECUTABLE} PRIVATE
    wafflehouse_sipcore
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    PkgConfig::SODIUM
    PkgConfig::NCURSESW
    PkgConfig::XKBCOMMON
)

if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    target_compile_definitions(${APP_EXECUTABLE} PRIVATE WAFFLEHOUSE_FREEBSD=1)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(${APP_EXECUTABLE} PRIVATE WAFFLEHOUSE_LINUX=1)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${APP_EXECUTABLE} PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(wafflehouse_sipcore PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS ${APP_EXECUTABLE}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES data/wafflehouse-client.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)
