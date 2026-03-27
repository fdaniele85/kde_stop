cmake_minimum_required(VERSION 3.26)

execute_process(
        COMMAND git describe --tags --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE KDE_STOP_GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)

# Use the git describe string for display, but keep a semver for CMake's version logic.
set(KDE_STOP_VERSION_STRING "${KDE_STOP_GIT_DESCRIBE}")
set(KDE_STOP_VERSION_SEMVER "${KDE_STOP_GIT_DESCRIBE}")
if (KDE_STOP_GIT_DESCRIBE)
    set(KDE_STOP_VERSION_STRING "${KDE_STOP_GIT_DESCRIBE}")
    string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" KDE_STOP_VERSION_SEMVER "${KDE_STOP_GIT_DESCRIBE}")
    if (NOT KDE_STOP_VERSION_SEMVER)
        set(KDE_STOP_VERSION_SEMVER "0.1.0")
    endif ()
endif ()

project(kde_stop
        VERSION ${KDE_STOP_VERSION_SEMVER}
        LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(KDE_STOP_THREAD_SAFE "Enable thread-safe mode in kde_stop (mutex/lock_guard enabled)" OFF)

set(KDE_STOP_PUBLIC_HEADERS
        include/kde_stop/Bandwidth.h
        include/kde_stop/KDE.h
        include/kde_stop/ProbabilisticStop.h
)

add_library(kde_stop
        src/ProbabilisticStop.cpp
        src/KDE.cpp
        #        src/ProbabilisticFilter.cpp
        src/Bandwidth.cpp
)

if (KDE_STOP_THREAD_SAFE)
    message("Enabling thread-safe mode in kde_stop (mutex/lock_guard enabled)")
    target_compile_definitions(kde_stop PUBLIC KDE_STOP_THREAD_SAFE)
endif ()

add_library(kde_stop::kde_stop ALIAS kde_stop)

#find_package(FFTW3 QUIET)
#
#if (FFTW3_FOUND)
#        message(STATUS "Found FFTW3 via find_package")
#        target_link_libraries(kde_stop PUBLIC FFTW3::FFTW3)
#        target_compile_definitions(kde_stop PUBLIC KDE_STOP_USE_FFTW)
#else()
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW3 REQUIRED IMPORTED_TARGET fftw3)

message(STATUS "Found FFTW3 via pkg-config")
target_link_libraries(kde_stop PUBLIC PkgConfig::FFTW3)
target_compile_definitions(kde_stop PUBLIC KDE_STOP_USE_FFTW)
#endif()

target_compile_features(kde_stop PUBLIC cxx_std_20)

# Se hai una cartella include/kde_stop/... metti quella.
# Se invece gli header sono nella root, cambia BUILD_INTERFACE di conseguenza.
target_include_directories(kde_stop
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# ---- Install target + header ----
install(TARGETS kde_stop
        EXPORT kde_stopTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES ${KDE_STOP_PUBLIC_HEADERS}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kde_stop
)

install(DIRECTORY include/kde_stop/detail/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kde_stop/detail
)

# ---- Export targets ----
install(EXPORT kde_stopTargets
        FILE kde_stopTargets.cmake
        NAMESPACE kde_stop::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kde_stop
)

# ---- Config package ----
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/kde_stopConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/kde_stopConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kde_stop
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/kde_stopConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/kde_stopConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/kde_stopConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/kde_stop
)

# (Opzionale) per usare find_package dal build tree senza install
export(EXPORT kde_stopTargets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/kde_stopTargets.cmake
        NAMESPACE kde_stop::
)
