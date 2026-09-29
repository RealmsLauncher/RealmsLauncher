# SPDX-License-Identifier: GPL-3.0-only
#
# Bundles the Prism Launcher community themes directly into the launcher.
#
# The source is pinned by the git submodule at:
#   9e921ca23a1838f87e0699517a77da5e92921a11
#
# This intentionally does not download anything during CMake configure. The
# submodule must already be present in the source checkout.

set(Launcher_PRISM_THEMES_SOURCE_DIR "${PROJECT_SOURCE_DIR}/3rdparty/PrismLauncher-Themes" CACHE PATH
    "PrismLauncher/Themes checkout to bundle into the launcher")

option(Launcher_BUNDLE_PRISM_THEMES
    "Bundle Prism Launcher community themes into the launcher"
    ON)

function(_append_bundled_prism_directory qrc_file source_root source_directory resource_prefix)
    file(GLOB_RECURSE _files
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        RELATIVE "${source_root}/${source_directory}"
        "${source_root}/${source_directory}/*"
    )

    file(APPEND "${qrc_file}"
        "    <qresource prefix=\"${resource_prefix}\">\\n"
    )

    foreach(_relative_path IN LISTS _files)
        string(REPLACE "\\\\" "/" _alias "${_relative_path}")
        string(REPLACE "&" "&amp;" _alias "${_alias}")
        string(REPLACE "<" "&lt;" _alias "${_alias}")
        string(REPLACE ">" "&gt;" _alias "${_alias}")

        set(_source_file "${source_root}/${source_directory}/${_relative_path}")
        file(TO_CMAKE_PATH "${_source_file}" _source_file)
        string(REPLACE "&" "&amp;" _source_xml "${_source_file}")
        string(REPLACE "<" "&lt;" _source_xml "${_source_xml}")
        string(REPLACE ">" "&gt;" _source_xml "${_source_xml}")

        file(APPEND "${qrc_file}"
            "        <file alias=\"${_alias}\">${_source_xml}</file>\\n"
        )
    endforeach()

    file(APPEND "${qrc_file}" "    </qresource>\\n")
endfunction()

function(_append_bundled_prism_licenses qrc_file source_root)
    file(GLOB _license_files
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES false
        RELATIVE "${source_root}"
        "${source_root}/LICENSES/*"
    )

    file(APPEND "${qrc_file}"
        "    <qresource prefix=\"/bundled-prism-themes\">\\n"
    )

    foreach(_relative_path IN LISTS _license_files)
        string(REPLACE "\\\\" "/" _alias "${_relative_path}")
        string(REPLACE "&" "&amp;" _alias "${_alias}")
        string(REPLACE "<" "&lt;" _alias "${_alias}")
        string(REPLACE ">" "&gt;" _alias "${_alias}")

        set(_source_file "${source_root}/${_relative_path}")
        file(TO_CMAKE_PATH "${_source_file}" _source_file)
        string(REPLACE "&" "&amp;" _source_xml "${_source_file}")
        string(REPLACE "<" "&lt;" _source_xml "${_source_xml}")
        string(REPLACE ">" "&gt;" _source_xml "${_source_xml}")

        file(APPEND "${qrc_file}"
            "        <file alias=\"_licenses/${_alias}\">${_source_xml}</file>\\n"
        )
    endforeach()

    file(APPEND "${qrc_file}" "    </qresource>\\n")
endfunction()

function(configure_bundled_prism_themes output_variable)
    if(NOT Launcher_BUNDLE_PRISM_THEMES)
        set(${output_variable} "" PARENT_SCOPE)
        return()
    endif()

    set(_source_root "${Launcher_PRISM_THEMES_SOURCE_DIR}")

    foreach(_required_directory IN ITEMS themes icons cats)
        if(NOT EXISTS "${_source_root}/${_required_directory}")
            message(FATAL_ERROR
                "PrismLauncher/Themes is missing the required '${_required_directory}' "
                "directory at '${_source_root}'. "
                "Initialize the pinned submodule with: "
                "git submodule update --init --recursive")
        endif()
    endforeach()

    set(_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
    set(_qrc_file "${_generated_dir}/prism_themes.qrc")

    file(MAKE_DIRECTORY "${_generated_dir}")
    file(WRITE "${_qrc_file}" "<RCC>\\n")

    # Application/widget themes.
    _append_bundled_prism_directory(
        "${_qrc_file}"
        "${_source_root}"
        "themes"
        "/bundled-prism-themes"
    )

    # Icon themes preserve the source index.theme/scalable and size-specific
    # directory layout so Qt can resolve them as normal icon themes.
    _append_bundled_prism_directory(
        "${_qrc_file}"
        "${_source_root}"
        "icons"
        "/bundled-prism-icons"
    )

    # Cat packs include their manifests and all image assets.
    _append_bundled_prism_directory(
        "${_qrc_file}"
        "${_source_root}"
        "cats"
        "/bundled-prism-cats"
    )

    _append_bundled_prism_licenses("${_qrc_file}" "${_source_root}")

    file(APPEND "${_qrc_file}" "</RCC>\\n")
    set(${output_variable} "${_qrc_file}" PARENT_SCOPE)
endfunction()
