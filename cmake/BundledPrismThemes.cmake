# SPDX-License-Identifier: GPL-3.0-only
#
# Bundles the Prism Launcher community themes directly into the launcher.
#
# The source is pinned by the git submodule at:
#   9e921ca23a1838f87e0699517a77da5e92921a11
#
# This intentionally does not download anything during CMake configure. The
# submodule must already be present in the source checkout.

set(Launcher_PRISM_THEMES_COMMIT "9e921ca23a1838f87e0699517a77da5e92921a11")

set(Launcher_PRISM_THEMES_SOURCE_DIR "${PROJECT_SOURCE_DIR}/3rdparty/PrismLauncher-Themes" CACHE PATH
    "PrismLauncher/Themes checkout to bundle into the launcher")

option(Launcher_BUNDLE_PRISM_THEMES
    "Bundle Prism Launcher community themes into the launcher"
    ON)

function(configure_bundled_prism_themes output_variable)
    if(NOT Launcher_BUNDLE_PRISM_THEMES)
        set(${output_variable} "" PARENT_SCOPE)
        return()
    endif()

    set(_source_root "${Launcher_PRISM_THEMES_SOURCE_DIR}")

    if(NOT EXISTS "${_source_root}/themes")
        message(FATAL_ERROR
            "PrismLauncher/Themes is missing at '${_source_root}'. "
            "Initialize the pinned submodule with: "
            "git submodule update --init --recursive")
    endif()

    set(_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(_version_file "${_generated_dir}/prism_themes_version.txt")
    set(_qrc_file "${_generated_dir}/prism_themes.qrc")

    file(MAKE_DIRECTORY "${_generated_dir}")
    file(WRITE "${_version_file}" "9e921ca23a1838f87e0699517a77da5e92921a11\n")

    file(GLOB_RECURSE _theme_files
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        RELATIVE "${_source_root}/themes"
        "${_source_root}/themes/*"
    )

    file(WRITE "${_qrc_file}" "<RCC>\n    <qresource prefix=\"/bundled-prism-themes\">\n")
    file(APPEND "${_qrc_file}" "        <file alias=\"_bundle_version\">${_version_file}</file>\n")

    foreach(_relative_path IN LISTS _theme_files)
        string(REPLACE "\\" "/" _alias "${_relative_path}")
        string(REPLACE "&" "&amp;" _alias "${_alias}")
        string(REPLACE "<" "&lt;" _alias "${_alias}")
        string(REPLACE ">" "&gt;" _alias "${_alias}")

        set(_source_file "${_source_root}/themes/${_relative_path}")
        file(TO_CMAKE_PATH "${_source_file}" _source_file)
        string(REPLACE "&" "&amp;" _source_xml "${_source_file}")
        string(REPLACE "<" "&lt;" _source_xml "${_source_xml}")
        string(REPLACE ">" "&gt;" _source_xml "${_source_xml}")

        file(APPEND "${_qrc_file}"
            "        <file alias=\"${_alias}\">${_source_xml}</file>\n")
    endforeach()

    file(GLOB _license_files
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        RELATIVE "${_source_root}"
        "${_source_root}/LICENSES/*"
    )

    foreach(_relative_path IN LISTS _license_files)
        string(REPLACE "\\" "/" _alias "${_relative_path}")
        string(REPLACE "&" "&amp;" _alias "${_alias}")
        string(REPLACE "<" "&lt;" _alias "${_alias}")
        string(REPLACE ">" "&gt;" _alias "${_alias}")

        set(_source_file "${_source_root}/${_relative_path}")
        file(TO_CMAKE_PATH "${_source_file}" _source_file)
        string(REPLACE "&" "&amp;" _source_xml "${_source_file}")
        string(REPLACE "<" "&lt;" _source_xml "${_source_xml}")
        string(REPLACE ">" "&gt;" _source_xml "${_source_xml}")

        file(APPEND "${_qrc_file}"
            "        <file alias=\"_licenses/${_alias}\">${_source_xml}</file>\n")
    endforeach()

    file(APPEND "${_qrc_file}" "    </qresource>\n</RCC>\n")
    set(${output_variable} "${_qrc_file}" PARENT_SCOPE)
endfunction()
