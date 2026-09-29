# SPDX-License-Identifier: GPL-3.0-only
#
# Bundles the Prism Launcher community themes directly into the launcher.
#
# The source is pinned to an upstream PrismLauncher/Themes commit instead of
# using GitHub release assets. This keeps the launcher reproducible while
# still bundling the complete theme set from the upstream repository.

set(Launcher_PRISM_THEMES_COMMIT "9e921ca23a1838f87e0699517a77da5e92921a11" CACHE STRING
    "PrismLauncher/Themes commit to bundle into the launcher")

set(Launcher_PRISM_THEMES_SOURCE_DIR "" CACHE PATH
    "Optional local PrismLauncher/Themes checkout to use instead of downloading the pinned commit")

option(Launcher_BUNDLE_PRISM_THEMES
    "Bundle Prism Launcher community themes into the launcher"
    ON)

function(configure_bundled_prism_themes output_variable)
    if(NOT Launcher_BUNDLE_PRISM_THEMES)
        set(${output_variable} "" PARENT_SCOPE)
        return()
    endif()

    set(_cache_root "${CMAKE_BINARY_DIR}/_deps/prism-themes")
    set(_source_root "${Launcher_PRISM_THEMES_SOURCE_DIR}")

    if(_source_root STREQUAL "")
        file(MAKE_DIRECTORY "${_cache_root}")

        set(_archive "${_cache_root}/Themes-${Launcher_PRISM_THEMES_COMMIT}.tar.gz")
        set(_source_root "${_cache_root}/Themes-${Launcher_PRISM_THEMES_COMMIT}")

        if(NOT EXISTS "${_source_root}/themes")
            message(STATUS
                "Downloading PrismLauncher/Themes at commit ${Launcher_PRISM_THEMES_COMMIT}")

            file(DOWNLOAD
                "https://codeload.github.com/PrismLauncher/Themes/tar.gz/${Launcher_PRISM_THEMES_COMMIT}"
                "${_archive}"
                SHOW_PROGRESS
                STATUS _download_status
            )

            list(GET _download_status 0 _download_code)
            list(GET _download_status 1 _download_message)
            if(NOT _download_code EQUAL 0)
                message(FATAL_ERROR
                    "Failed to download PrismLauncher/Themes: ${_download_message}")
            endif()

            file(ARCHIVE_EXTRACT
                INPUT "${_archive}"
                DESTINATION "${_cache_root}"
            )
        endif()
    endif()

    if(NOT EXISTS "${_source_root}/themes")
        message(FATAL_ERROR
            "PrismLauncher/Themes source was not found at '${_source_root}'")
    endif()

    set(_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(_version_file "${_generated_dir}/prism_themes_version.txt")
    set(_qrc_file "${_generated_dir}/prism_themes.qrc")

    file(MAKE_DIRECTORY "${_generated_dir}")
    file(WRITE "${_version_file}" "${Launcher_PRISM_THEMES_COMMIT}\n")

    file(GLOB_RECURSE _theme_files
        LIST_DIRECTORIES false
        RELATIVE "${_source_root}/themes"
        "${_source_root}/themes/*"
    )

    file(WRITE "${_qrc_file}" "<RCC>\n    <qresource prefix=\"/bundled-prism-themes\">\n")
    file(APPEND "${_qrc_file}" "        <file alias=\"_bundle_version\">${_version_file}</file>\n")

    foreach(_relative_path IN LISTS _theme_files)
        # The launcher never reads these files. Skipping them saves several
        # megabytes while retaining every functional theme and its assets.
        if(_relative_path MATCHES "/preview\\.png(\\.license)?$" OR
           _relative_path MATCHES "^preview\\.png(\\.license)?$")
            continue()
        endif()

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

    # Keep the upstream license texts available in the bundled resource set.
    file(GLOB _license_files
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
