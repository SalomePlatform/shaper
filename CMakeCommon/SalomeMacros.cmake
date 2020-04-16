# Copyright (C) 2012-2020  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author: A.Geay, V. Sandler, A. Bruneton
#

#----------------------------------------------------------------------------
# LIST_CONTAINS is a macro useful for determining whether a list has a
# particular entry
#----------------------------------------------------------------------------
MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH(value2 ${ARGN})
    IF(${value} STREQUAL "${value2}")
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL "${value2}")
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

#----------------------------------------------------------------------------
# The PARSE_ARGUMENTS macro will take the arguments of another macro and
# define several variables.
#
# USAGE:  PARSE_ARGUMENTS(prefix arg_names options arg1 arg2...)
#
# ARGUMENTS:
#
# prefix: IN: a prefix to put on all variables it creates.
#
# arg_names: IN: a list of names.
# For each item in arg_names, PARSE_ARGUMENTS will create a
# variable with that name, prefixed with prefix_. Each variable will be filled
# with the arguments that occur after the given arg_name is encountered
# up to the next arg_name or the end of the arguments. All options are
# removed from these lists. PARSE_ARGUMENTS also creates a
# prefix_DEFAULT_ARGS variable containing the list of all arguments up
# to the first arg_name encountered.
#
# options: IN: a list of options.
# For each item in options, PARSE_ARGUMENTS will create a
# variable with that name, prefixed with prefix_. So, for example, if prefix is
# MY_MACRO and options is OPTION1;OPTION2, then PARSE_ARGUMENTS will
# create the variables MY_MACRO_OPTION1 and MY_MACRO_OPTION2. These
# variables will be set to true if the option exists in the command line
# or false otherwise.
# arg_names and options lists should be quoted.
#
# The rest of PARSE_ARGUMENTS are arguments from another macro to be parsed.
#----------------------------------------------------------------------------
MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)
  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})
    LIST_CONTAINS(is_arg_name ${arg} ${arg_names})
    IF (is_arg_name)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name)
      LIST_CONTAINS(is_option ${arg} ${option_names})
      IF (is_option)
      SET(${prefix}_${arg} TRUE)
      ELSE (is_option)
      SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option)
    ENDIF (is_arg_name)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

#----------------------------------------------------------------------------
# SALOME_INSTALL_SCRIPTS is a macro useful for installing scripts.
#
# USAGE: SALOME_INSTALL_SCRIPTS(file_list path [WORKING_DIRECTORY dir] [DEF_PERMS] [TARGET_NAME name])
#
# ARGUMENTS:
# file_list: IN : list of files to be installed. This list should be quoted.
# path: IN : full pathname for installing.
#
# By default files to be installed as executable scripts.
# If DEF_PERMS option is provided, than permissions for installed files are
# only OWNER_WRITE, OWNER_READ, GROUP_READ, and WORLD_READ.
# WORKING_DIRECTORY option may be used to specify the relative or absolute
# path to the directory containing source files listed in file_list argument.
# If TARGET_NAME option is specified, the name of the target being created
# with this macro is returned via the given variable.
#----------------------------------------------------------------------------
MACRO(SALOME_INSTALL_SCRIPTS file_list path)
  PARSE_ARGUMENTS(SALOME_INSTALL_SCRIPTS "WORKING_DIRECTORY;TARGET_NAME" "DEF_PERMS" ${ARGN})
  SET(PERMS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
  IF(NOT SALOME_INSTALL_SCRIPTS_DEF_PERMS)
    SET(PERMS ${PERMS} OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
  ENDIF(NOT SALOME_INSTALL_SCRIPTS_DEF_PERMS)
  SET(_all_pyc)
  SET(_all_pyo)
  FOREACH(file ${file_list})
    SET(PREFIX "")
    SET(_source_prefix "")
    GET_FILENAME_COMPONENT(file_name ${file} NAME)
    IF(NOT IS_ABSOLUTE ${file})
      IF(SALOME_INSTALL_SCRIPTS_WORKING_DIRECTORY)
        SET(PREFIX "${SALOME_INSTALL_SCRIPTS_WORKING_DIRECTORY}/")
      ENDIF(SALOME_INSTALL_SCRIPTS_WORKING_DIRECTORY)
      SET(_source_prefix "${CMAKE_CURRENT_SOURCE_DIR}/")
    ENDIF(NOT IS_ABSOLUTE ${file})
    INSTALL(FILES ${PREFIX}${file} DESTINATION ${path} PERMISSIONS ${PERMS})
    GET_FILENAME_COMPONENT(ext ${file} EXT)
    GET_FILENAME_COMPONENT(we_ext ${file} NAME_WE)

    IF(ext STREQUAL .py)
      # Generate and install the pyc and pyo
      # [ABN] Important: we avoid references or usage of CMAKE_INSTALL_PREFIX which is not correctly set
      # when using CPack.
      SET(_pyc_file "${CMAKE_CURRENT_BINARY_DIR}/${we_ext}.cpython-${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}.pyc")
      SET(_pyo_file "${CMAKE_CURRENT_BINARY_DIR}/${we_ext}.cpython-${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}.opt-1.pyc")
      LIST(APPEND _all_pyc ${_pyc_file})
      LIST(APPEND _all_pyo ${_pyo_file})
      ADD_CUSTOM_COMMAND(
        OUTPUT ${_pyc_file} ${_pyo_file}
        COMMAND ${PYTHON_EXECUTABLE} -c "from py_compile import compile; compile('${_source_prefix}${file}', '${_pyc_file}', doraise=True, optimize=0); compile('${_source_prefix}${file}', '${_pyo_file}', doraise=True, optimize=1)"
        DEPENDS ${PREFIX}${file}
        VERBATIM
      )
      # Install the .pyo and the .pyc
      INSTALL(FILES ${_pyc_file} DESTINATION ${path}/__pycache__ PERMISSIONS ${PERMS})
      INSTALL(FILES ${_pyo_file} DESTINATION ${path}/__pycache__ PERMISSIONS ${PERMS})
    ENDIF(ext STREQUAL .py)

  # get relative path (from CMAKE_SOURCE_DIR to CMAKE_CURRENT_SOURCE_DIR)
  STRING(REGEX REPLACE ${CMAKE_SOURCE_DIR} "" rel_dir ${CMAKE_CURRENT_SOURCE_DIR})
  # convert "/" to "_"
  IF(rel_dir)
    STRING(REGEX REPLACE "/" "_" unique_name ${rel_dir})
  ELSE(rel_dir)
    SET(unique_name _)
  ENDIF(rel_dir)

  ENDFOREACH(file ${file_list})
  # Generate only one target for all requested Python script compilation.
  # Make sure that the target name is unique too.
  IF(_all_pyc)
     SET(_cnt 0)
     WHILE(TARGET "PYCOMPILE${unique_name}_${_cnt}")
       MATH(EXPR _cnt ${_cnt}+1)
     ENDWHILE()
     SET(_target_name "PYCOMPILE${unique_name}_${_cnt}")
     ADD_CUSTOM_TARGET(${_target_name} ALL DEPENDS ${_all_pyc} ${_all_pyo})
     IF(SALOME_INSTALL_SCRIPTS_TARGET_NAME)
       SET(${SALOME_INSTALL_SCRIPTS_TARGET_NAME} ${_target_name})
     ENDIF(SALOME_INSTALL_SCRIPTS_TARGET_NAME)
  ENDIF()
ENDMACRO(SALOME_INSTALL_SCRIPTS)

#----------------------------------------------------------------------------
# SALOME_CONFIGURE_FILE is a macro useful for copying a file to another location
# and modify its contents.
#
# USAGE: SALOME_CONFIGURE_FILE(in_file out_file [INSTALL dir])
#
# ARGUMENTS:
# in_file: IN : input file (if relative path is given, full file path is computed from current source dir).
# out_file: IN : output file (if relative path is given, full file path is computed from current build dir).
# If INSTALL is specified, then 'out_file' will be installed to the 'dir' directory.
#----------------------------------------------------------------------------
MACRO(SALOME_CONFIGURE_FILE IN_FILE OUT_FILE)
  IF(IS_ABSOLUTE ${IN_FILE})
    SET(_in_file ${IN_FILE})
  ELSE()
    SET(_in_file ${CMAKE_CURRENT_SOURCE_DIR}/${IN_FILE})
  ENDIF()
  IF(IS_ABSOLUTE  ${OUT_FILE})
    SET(_out_file ${OUT_FILE})
  ELSE()
    SET(_out_file ${CMAKE_CURRENT_BINARY_DIR}/${OUT_FILE})
  ENDIF()
  MESSAGE(STATUS "Creation of ${_out_file}")
  CONFIGURE_FILE(${_in_file} ${_out_file} @ONLY)
  PARSE_ARGUMENTS(SALOME_CONFIGURE_FILE "INSTALL" "" ${ARGN})
  IF(SALOME_CONFIGURE_FILE_INSTALL)
    INSTALL(FILES ${_out_file} DESTINATION ${SALOME_CONFIGURE_FILE_INSTALL})
  ENDIF(SALOME_CONFIGURE_FILE_INSTALL)
ENDMACRO(SALOME_CONFIGURE_FILE)


#######################################################################################
# Useful macros for SALOME own package detection system
#

###
# SALOME_CHECK_EQUAL_PATHS(result path1 path2)
#  Check if two paths are identical, resolving links. If the paths do not exist a simple
#  text comparison is performed.
#  result is a boolean.
###
MACRO(SALOME_CHECK_EQUAL_PATHS varRes path1 path2)
  SET("${varRes}" OFF)
  IF(EXISTS "${path1}")
    GET_FILENAME_COMPONENT(_tmp1 "${path1}" REALPATH)
  ELSE()
    SET(_tmp1 "${path1}")
  ENDIF()

  IF(EXISTS "${path2}")
    GET_FILENAME_COMPONENT(_tmp2 "${path2}" REALPATH)
  ELSE()
    SET(_tmp2 "${path2}")
  ENDIF()

  IF("${_tmp1}" STREQUAL "${_tmp2}")
    SET("${varRes}" ON)
  ENDIF()
#  MESSAGE(${${varRes}})
ENDMACRO()

####
# SALOME_LOG_OPTIONAL_PACKAGE(pkg flag)
#
# Register in global variables the detection status (found or not) of the optional package 'pkg'
# and the configuration flag that should be turned off to avoid detection of the package.
# The global variables are read again by SALOME_PACKAGE_REPORT_AND_CHECK to produce
# a summary report of the detection status and stops the process if necessary.
MACRO(SALOME_LOG_OPTIONAL_PACKAGE pkg flag)
  # Was the package found
  STRING(TOUPPER ${pkg} _pkg_UC)
  IF(${pkg}_FOUND OR ${_pkg_UC}_FOUND)
    SET(_isFound TRUE)
  ELSE()
    SET(_isFound FALSE)
  ENDIF()

  # Is the package already in the list? Then update its status:
  LIST(FIND _SALOME_OPTIONAL_PACKAGES_names ${pkg} _result)
  IF(NOT ${_result} EQUAL -1)
    LIST(REMOVE_AT _SALOME_OPTIONAL_PACKAGES_found ${_result})
    LIST(REMOVE_AT _SALOME_OPTIONAL_PACKAGES_flags ${_result})
    LIST(INSERT    _SALOME_OPTIONAL_PACKAGES_found ${_result} ${_isFound})
    LIST(INSERT    _SALOME_OPTIONAL_PACKAGES_flags ${_result} ${flag})
  ELSE()
    # Otherwise insert it
    LIST(APPEND _SALOME_OPTIONAL_PACKAGES_names ${pkg})
    LIST(APPEND _SALOME_OPTIONAL_PACKAGES_found ${_isFound})
    LIST(APPEND _SALOME_OPTIONAL_PACKAGES_flags ${flag})
  ENDIF()

ENDMACRO(SALOME_LOG_OPTIONAL_PACKAGE)

####
# SALOME_JUSTIFY_STRING()
#
# Justifies the string specified as an argument to the given length
# adding required number of spaces to the end. Does noting if input
# string is longer as required length.
# Puts the result to the output variable.
#
# USAGE: SALOME_JUSTIFY_STRING(input length result)
#
# ARGUMENTS:
#   input  [in] input string
#   length [in] required length of resulting string
#   result [out] name of variable where the result string is put to
#
MACRO(SALOME_JUSTIFY_STRING input length result)
  SET(${result} ${input})
  STRING(LENGTH ${input} _input_length)
  MATH(EXPR _nb_spaces "${length}-${_input_length}-1")
  IF (_nb_spaces GREATER 0)
    FOREACH(_idx RANGE ${_nb_spaces})
      SET(${result} "${${result}} ")
    ENDFOREACH()
  ENDIF()
ENDMACRO(SALOME_JUSTIFY_STRING)

####
# SALOME_PACKAGE_REPORT_AND_CHECK()
#
# Print a quick summary of the detection of optional prerequisites.
# If a package was not found, the configuration is stopped. The summary also indicates
# which flag should be turned off to skip the detection of the package.
#
# If optional JUSTIFY argument is specified, names of packages
# are left-justified to the given length; default value is 10.
#
# USAGE: SALOME_PACKAGE_REPORT_AND_CHECK([JUSTIFY length])
#
MACRO(SALOME_PACKAGE_REPORT_AND_CHECK)
  SET(_will_fail OFF)
  PARSE_ARGUMENTS(SALOME_PACKAGE_REPORT "JUSTIFY" "" ${ARGN})
  IF(SALOME_PACKAGE_REPORT_JUSTIFY)
    SET(_length ${SALOME_PACKAGE_REPORT_JUSTIFY})
  ELSE()
    SET(_length 23)
  ENDIF()
  MESSAGE(STATUS "")
  MESSAGE(STATUS "  Optional packages - Detection report ")
  MESSAGE(STATUS "  ==================================== ")
  MESSAGE(STATUS "")
  IF(DEFINED _SALOME_OPTIONAL_PACKAGES_names)
    LIST(LENGTH _SALOME_OPTIONAL_PACKAGES_names _list_len)
    # Another CMake stupidity - FOREACH(... RANGE r) generates r+1 numbers ...
    MATH(EXPR _range "${_list_len}-1")
    FOREACH(_idx RANGE ${_range})
      LIST(GET _SALOME_OPTIONAL_PACKAGES_names ${_idx} _pkg_name)
      LIST(GET _SALOME_OPTIONAL_PACKAGES_found ${_idx} _pkg_found)
      LIST(GET _SALOME_OPTIONAL_PACKAGES_flags ${_idx} _pkg_flag)
      SALOME_JUSTIFY_STRING(${_pkg_name} ${_length} _pkg_name)
      IF(_pkg_found)
        SET(_found_msg "Found")
        SET(_flag_msg "")
      ELSE()
        SET(_will_fail ON)
        SET(_found_msg "NOT Found")
        SET(_flag_msg " - ${_pkg_flag} can be switched OFF to skip this prerequisite.")
      ENDIF()

      MESSAGE(STATUS "  * ${_pkg_name}  ->  ${_found_msg}${_flag_msg}")
    ENDFOREACH()
  ENDIF(DEFINED _SALOME_OPTIONAL_PACKAGES_names)
  MESSAGE(STATUS "")
  MESSAGE(STATUS "")

  # Failure if some packages were missing:
  IF(_will_fail)
    MESSAGE(FATAL_ERROR "Some required prerequisites have NOT been found. Take a look at the report above to fix this.")
  ENDIF()
ENDMACRO(SALOME_PACKAGE_REPORT_AND_CHECK)

####
# SALOME_FIND_PACKAGE(englobingPackageName standardPackageName modus [onlyTryQuietly])
#
# example:  SALOME_FIND_PACKAGE(SalomeVTK VTK CONFIG)
#
# Encapsulate the call to the standard FIND_PACKAGE(standardPackageName) passing all the options
# given when calling the command FIND_PACKAGE(SalomeXYZ). Those options are stored implicitly in
# CMake variables: xyz__FIND_QUIETLY, xyz_FIND_REQUIRED, etc ...
#
# If a list of components was specified when invoking the initial FIND_PACKAGE(SalomeXyz ...) this is
# also handled properly.
#
# Modus is either MODULE or CONFIG (cf standard FIND_PACKAGE() documentation).
# The last argument is optional and if set to TRUE will force the search to be OPTIONAL and QUIET.
# If the package is looked for in CONFIG mode, the standard system paths are skipped. If you still want a
# system installation to be found in this mode, you have to set the ROOT_DIR variable explicitly to /usr (for
# example).
#
# This macro is to be called from within the FindSalomeXXXX.cmake file.
#
####
MACRO(SALOME_FIND_PACKAGE englobPkg stdPkg mode)
  SET(_OPT_ARG ${ARGV3})
  # Only bother if the package was not already found:
  # Some old packages use the lower case version - standard should be to always use
  # upper case:
  STRING(TOUPPER ${stdPkg} stdPkgUC)
  IF(NOT (${stdPkg}_FOUND OR ${stdPkgUC}_FOUND))
    IF(${englobPkg}_FIND_QUIETLY OR _OPT_ARG)
      SET(_tmp_quiet "QUIET")
    ELSE()
      SET(_tmp_quiet)
    ENDIF()
    IF(${englobPkg}_FIND_REQUIRED AND NOT _OPT_ARG)
      SET(_tmp_req "REQUIRED")
    ELSE()
      SET(_tmp_req)
    ENDIF()
    IF(${englobPkg}_FIND_VERSION_EXACT)
      SET(_tmp_exact "EXACT")
    ELSE()
      SET(_tmp_exact)
    ENDIF()

    # Call the CMake FIND_PACKAGE() command:
    STRING(TOLOWER ${stdPkg} _pkg_lc)
    IF(("${mode}" STREQUAL "NO_MODULE") OR ("${mode}" STREQUAL "CONFIG"))
      # Hope to find direclty a CMake config file, indicating the SALOME CMake file
      # paths (the command already looks in places like "share/cmake", etc ... by default)
      # Note the options NO_CMAKE_BUILDS_PATH, NO_CMAKE_PACKAGE_REGISTRY to avoid (under Windows)
      # looking into a previous CMake build done via a GUI, or into the Win registry.
      # NO_CMAKE_SYSTEM_PATH and NO_SYSTEM_ENVIRONMENT_PATH ensure any _system_ files like 'xyz-config.cmake'
      # don't get loaded (typically Boost). To force their loading, set the XYZ_ROOT_DIR variable to '/usr'.
      # See documentation of FIND_PACKAGE() for full details.

      # Do we need to call the signature using components?
      IF(${englobPkg}_FIND_COMPONENTS)
        FIND_PACKAGE(${stdPkg} ${${englobPkg}_FIND_VERSION} ${_tmp_exact}
              NO_MODULE ${_tmp_quiet} ${_tmp_req} COMPONENTS ${${englobPkg}_FIND_COMPONENTS}
              PATH_SUFFIXES "salome_adm/cmake_files" "adm_local/cmake_files" "adm/cmake"
              NO_CMAKE_BUILDS_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH
                NO_SYSTEM_ENVIRONMENT_PATH)
      ELSE()
        FIND_PACKAGE(${stdPkg} ${${englobPkg}_FIND_VERSION} ${_tmp_exact}
              NO_MODULE ${_tmp_quiet} ${_tmp_req}
              PATH_SUFFIXES "salome_adm/cmake_files" "adm_local/cmake_files" "adm/cmake"
              NO_CMAKE_BUILDS_PATH NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PACKAGE_REGISTRY NO_CMAKE_SYSTEM_PATH
                 NO_SYSTEM_ENVIRONMENT_PATH)
      ENDIF()
      MARK_AS_ADVANCED(${stdPkg}_DIR)

    ELSEIF("${mode}" STREQUAL "MODULE")

      # Do we need to call the signature using components?
      IF(${englobPkg}_FIND_COMPONENTS)
        FIND_PACKAGE(${stdPkg} ${${englobPkg}_FIND_VERSION} ${_tmp_exact}
              MODULE ${_tmp_quiet} ${_tmp_req} COMPONENTS ${${englobPkg}_FIND_COMPONENTS})
      ELSE()
        FIND_PACKAGE(${stdPkg} ${${englobPkg}_FIND_VERSION} ${_tmp_exact}
              MODULE ${_tmp_quiet} ${_tmp_req})
      ENDIF()

    ELSE()

      MESSAGE(FATAL_ERROR "Invalid mode argument in the call to the macro SALOME_FIND_PACKAGE. Should be CONFIG or MODULE.")

    ENDIF()

  ENDIF()
ENDMACRO()


####################################################################
# SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS(pkg referenceVariable upCount)
#    pkg              : name of the system package to be detected
#    referenceVariable: variable containing a path that can be browsed up to
# retrieve the package root directory (xxx_ROOT_DIR)
#    upCount          : number of times we have to go up from the path <referenceVariable>
# to obtain the package root directory.
# If this is a path to a file, going up one time gives the directory containing the file
# going up 2 times gives the parent directory.
#
# For example:  SALOME_FIND_PACKAGE_DETECT_CONFLICTS(SWIG SWIG_EXECUTABLE 2)
#     with SWIG_EXECUTABLE set to '/usr/bin/swig'
#     will produce '/usr'
#
# Generic detection (and conflict check) procedure for package XYZ:
# 1. Load a potential env variable XYZ_ROOT_DIR as a default choice for the cache entry XYZ_ROOT_DIR
#    If empty, load a potential XYZ_ROOT_DIR_EXP as default value (path exposed by another package depending
# directly on XYZ)
# 2. Invoke FIND_PACKAGE() in this order:
#    * in CONFIG mode first (if possible): priority is given to a potential
#    "XYZ-config.cmake" file
#    * then switch to the standard MODULE mode, appending on CMAKE_PREFIX_PATH
# the above XYZ_ROOT_DIR variable
# 3. Extract the path actually found into a temp variable _XYZ_TMP_DIR
# 4. Warn if XYZ_ROOT_DIR is set and doesn't match what was found (e.g. when CMake found the system installation
#    instead of what is pointed to by XYZ_ROOT_DIR - happens when a typo in the content of XYZ_ROOT_DIR).
# 5. Conflict detection:
#    * check the temp variable against a potentially existing XYZ_ROOT_DIR_EXP
# 6. Finally expose what was *actually* found in XYZ_ROOT_DIR.
# 7. Specific stuff: for example exposing a prerequisite of XYZ to the rest of the world for future
# conflict detection. This is added after the call to the macro by the callee.
#
MACRO(SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS pkg referenceVariable upCount)
  ##
  ## 0. Initialization
  ##
  PARSE_ARGUMENTS(SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS "ENVVAR" "" ${ARGN})

  # Package name, upper case
  STRING(TOUPPER ${pkg} pkg_UC)

  ##
  ## 1. Load environment or any previously detected root dir for the package
  ##
  SET(_envvar ${pkg_UC}_ROOT_DIR)
  IF(SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS_ENVVAR)
    SET(_envvar "${SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS_ENVVAR}")
  ENDIF()
  IF(DEFINED ENV{${_envvar}})
    FILE(TO_CMAKE_PATH "$ENV{${_envvar}}" _${pkg_UC}_ROOT_DIR_ENV)
    SET(_dflt_value "${_${pkg_UC}_ROOT_DIR_ENV}")
  ELSE()
    # will be blank if no package was previously loaded:
    SET(_dflt_value "${${pkg_UC}_ROOT_DIR_EXP}")
  ENDIF()
  # Detect if the variable has been set on the command line or elsewhere:
  IF(DEFINED ${_envvar})
     SET(_var_already_there TRUE)
  ELSE()
     SET(_var_already_there FALSE)
  ENDIF()
  #   Make cache entry
  SET(${_envvar} "${_dflt_value}" CACHE PATH "Path to ${pkg_UC} directory")

  ##
  ## 2. Find package - try CONFIG mode first (i.e. looking for XYZ-config.cmake)
  ##

  # Override the variable - don't append to it, as it would give precedence
  # to what was stored there before!
  IF(DEFINED ${_envvar})
    SET(CMAKE_PREFIX_PATH "${${_envvar}}")
  ENDIF()

  # Try find_package in config mode. This has the priority, but is
  # performed QUIET and not REQUIRED:
  SALOME_FIND_PACKAGE("Salome${pkg}" ${pkg} NO_MODULE TRUE)

  IF (${pkg_UC}_FOUND OR ${pkg}_FOUND)
    MESSAGE(STATUS "Found ${pkg} in CONFIG mode!")
  ENDIF()

  # Otherwise try the standard way (module mode, with the standard CMake Find*** macro):
  # We do it quietly to produce our own error message, except if we are in debug mode:
  IF(SALOME_CMAKE_DEBUG)
    SALOME_FIND_PACKAGE("Salome${pkg}" ${pkg} MODULE FALSE)
  ELSE()
    SALOME_FIND_PACKAGE("Salome${pkg}" ${pkg} MODULE TRUE)
  ENDIF()

  # Set the "FOUND" variable for the SALOME wrapper:
  IF(${pkg_UC}_FOUND OR ${pkg}_FOUND)
    SET(SALOME${pkg_UC}_FOUND TRUE)
  ELSE()
    SET(SALOME${pkg_UC}_FOUND FALSE)
    IF(NOT Salome${pkg}_FIND_QUIETLY)
      IF(Salome${pkg}_FIND_REQUIRED)
         MESSAGE(FATAL_ERROR "Package ${pkg} couldn't be found - did you set the corresponing root dir correctly? "
         "It currently contains ${_envvar}=${${_envvar}}  "
         "Append -DSALOME_CMAKE_DEBUG=ON on the command line if you want to see the original CMake error.")
      ELSE()
         MESSAGE(WARNING "Package ${pkg} couldn't be found - did you set the corresponing root dir correctly? "
         "It currently contains ${_envvar}=${${_envvar}}  "
         "Append -DSALOME_CMAKE_DEBUG=ON on the command line if you want to see the original CMake error.")
      ENDIF()
    ENDIF()
  ENDIF()

  IF (${pkg_UC}_FOUND OR ${pkg}_FOUND)
    ## 3. Set the root dir which was finally retained by going up "upDir" times
    ## from the given reference path. The variable "referenceVariable" may be a list.
    ## In this case we take its first element.

    # First test if the variable exists, warn otherwise:
    IF(NOT DEFINED ${referenceVariable})
      MESSAGE(WARNING "${pkg}: the reference variable '${referenceVariable}' used when calling the macro "
      "SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS() is not defined.")
    ENDIF()

    LIST(LENGTH ${referenceVariable} _tmp_len)
    IF(_tmp_len)
       LIST(GET ${referenceVariable} 0 _tmp_ROOT_DIR)
    ELSE()
       #  Note the double de-reference of "referenceVariable":
       SET(_tmp_ROOT_DIR "${${referenceVariable}}")
    ENDIF()
    # Up cound can be reset by detection procedure
    SET(_upCount ${upCount})
    IF(DEFINED ${pkg_UC}_UPCOUNT)
      SET(_upCount ${${pkg_UC}_UPCOUNT})
    ENDIF()
    IF(${_upCount})
      FOREACH(_unused RANGE 1 ${_upCount})
        GET_FILENAME_COMPONENT(_tmp_ROOT_DIR "${_tmp_ROOT_DIR}" PATH)
      ENDFOREACH()
    ENDIF()

    ##
    ## 4. Warn if CMake found something not located under ENV(XYZ_ROOT_DIR)
    ##
    IF(DEFINED ENV{${_envvar}})
      SALOME_CHECK_EQUAL_PATHS(_res "${_tmp_ROOT_DIR}" "${_${pkg_UC}_ROOT_DIR_ENV}")
      IF(NOT _res)
        MESSAGE(WARNING "${pkg} was found, but not at the path given by the "
            "environment ${_envvar}! Is the variable correctly set? "
            "The two paths are: ${_tmp_ROOT_DIR} and: ${_${pkg_UC}_ROOT_DIR_ENV}")

      ELSE()
        MESSAGE(STATUS "${pkg} found directory matches what was specified in the ${_envvar} variable, all good!")
      ENDIF()
    ELSE()
        IF(NOT _var_already_there)
          MESSAGE(STATUS "Variable ${_envvar} was not explicitly defined. "
          "An installation was found anyway: ${_tmp_ROOT_DIR}")
        ENDIF()
    ENDIF()

    ##
    ## 5. Conflict detection
    ##     From another prerequisite using the package:
    ##
    IF(${pkg_UC}_ROOT_DIR_EXP)
        SALOME_CHECK_EQUAL_PATHS(_res "${_tmp_ROOT_DIR}" "${${pkg_UC}_ROOT_DIR_EXP}")
        IF(NOT _res)
           MESSAGE(WARNING "Warning: ${pkg}: detected version conflicts with a previously found ${pkg}!"
                           " The two paths are " ${_tmp_ROOT_DIR} " vs " ${${pkg_UC}_ROOT_DIR_EXP})
        ELSE()
            MESSAGE(STATUS "${pkg} directory matches what was previously exposed by another prereq, all good!")
        ENDIF()
    ENDIF()

    ##
    ## 6. Save the detected installation
    ##
    SET(${_envvar} "${_tmp_ROOT_DIR}")

  ELSE()
    MESSAGE(STATUS "${pkg} was not found.")
  ENDIF()

  SET(Salome${pkg}_FOUND "${pkg}_FOUND")
ENDMACRO(SALOME_FIND_PACKAGE_AND_DETECT_CONFLICTS)


####################################################################
# SALOME_ADD_MPI_TO_HDF5()
#
# Overload the HDF5 flags so that they also contain MPI references.
# This is to be used when HDF5 was compiled with MPI support;
MACRO(SALOME_ADD_MPI_TO_HDF5)
  SET(HDF5_INCLUDE_DIRS ${HDF5_INCLUDE_DIRS} ${MPI_INCLUDE_DIRS})
  SET(HDF5_DEFINITIONS "${HDF5_DEFINITIONS} ${MPI_DEFINITIONS}")
  SET(HDF5_LIBRARIES ${HDF5_LIBRARIES} ${MPI_LIBRARIES})
ENDMACRO(SALOME_ADD_MPI_TO_HDF5)

####################################################################
# SALOME_TOHEXA()
# Convert a number (smaller than 16) into hexadecimal representation
# with a leading 0.
MACRO(SALOME_TOHEXA num result)
  SET(_hexa_map a b c d e f)
  IF(${num} LESS 10)
    SET(${result} "0${num}")
  ELSE()
    MATH(EXPR _res "${num}-10" )
    LIST(GET _hexa_map ${_res} _out)
    SET(${result} "0${_out}")
  ENDIF()
ENDMACRO(SALOME_TOHEXA)

####################################################################
# SALOME_XVERSION()
#
# Computes hexadecimal version of SALOME package
#
# USAGE: SALOME_XVERSION(package)
#
# ARGUMENTS:
#
# package: IN: SALOME package name
#
# The macro reads SALOME package version from PACKAGE_VERSION variable
# (note package name in uppercase as assumed for SALOME modules);
# hexadecimal version value in form 0xAABBCC (where AA, BB and CC are
# major, minor and maintenance components of package version in
# hexadecimal form) is put to the PACKAGE_XVERSION variable
MACRO(SALOME_XVERSION pkg)
  STRING(TOUPPER ${pkg} _pkg_UC)
  IF(${_pkg_UC}_VERSION)
    SET(_major)
    SET(_minor)
    SET(_patch)
    SALOME_TOHEXA(${${_pkg_UC}_MAJOR_VERSION} _major)
    SALOME_TOHEXA(${${_pkg_UC}_MINOR_VERSION} _minor)
    SALOME_TOHEXA(${${_pkg_UC}_PATCH_VERSION} _patch)
    SET(${_pkg_UC}_XVERSION "0x${_major}${_minor}${_patch}")
  ENDIF()
ENDMACRO(SALOME_XVERSION)


#########################################################################
# SALOME_ACCUMULATE_HEADERS()
#
# This macro is called in the various FindSalomeXYZ.cmake modules to accumulate
# internally the list of include headers to be saved for future export.
# The full set of include is saved in a variable called
#      _${PROJECT_NAME}_EXTRA_HEADERS
#
MACRO(SALOME_ACCUMULATE_HEADERS lst)
  FOREACH(l IN LISTS ${lst})
    LIST(FIND _${PROJECT_NAME}_EXTRA_HEADERS "${l}" _res)
    IF(_res EQUAL "-1")
      IF(NOT "${l}" STREQUAL "/usr/include")
        LIST(APPEND _${PROJECT_NAME}_EXTRA_HEADERS "${l}")
      ENDIF()
    ENDIF()
  ENDFOREACH()
ENDMACRO(SALOME_ACCUMULATE_HEADERS)

#########################################################################
# SALOME_ACCUMULATE_ENVIRONMENT()
#
# USAGE: SALOME_ACCUMULATE_ENVIRONMENT(envvar value [value ...])
#
# ARGUMENTS:
#   envvar [in] environment variable name, e.g. PATH
#   value  [in] value(s) to be added to environment variable
#
# This macro is called in the various FindSalomeXYZ.cmake modules to
# accumulate environment variables, to be used later to run some command
# in proper environment.
#
# 1. Each envrironment variable is stored in specific CMake variable
#    _${PROJECT_NAME}_EXTRA_ENV_<var>, where <var> is name of variable.
# 2. Full list of environment variable names is stored in CMake variable
#    _${PROJECT_NAME}_EXTRA_ENV.
#
# Notes:
# - The arguments list can include optional CHECK or NOCHECK keywords:
#   * For all arguments following CHECK keyword the macro perform an
#     additional check (see below); this is the default mode, it is suitable
#     for path variables (PATH, LD_LIBRARY_PATH, etc).
#   * For all arguments following NOCHECK keyword, no additional check is
#     performed.
#   Checking an argument means that we check:
#    - That the path actually exists
#    - That this is not a standard system path (starting with "/usr"); this avoids
#   polluting LD_LIBRARY_PATH or PATH with things like "/usr/lib64" ...
#
MACRO(SALOME_ACCUMULATE_ENVIRONMENT envvar)
  SET(_is_check ON)
  FOREACH(_item ${ARGN})
    IF(${_item} STREQUAL "NOCHECK")
      SET(_is_check OFF)
    ELSEIF(${_item} STREQUAL "CHECK")
      SET(_is_check ON)
    ELSE()
      IF(_is_check)
        IF(NOT IS_DIRECTORY ${_item})
          IF(TARGET ${_item})
            GET_TARGET_PROPERTY(_item ${_item} LOCATION)
          ENDIF()
          GET_FILENAME_COMPONENT(_item ${_item} PATH)
        ENDIF()
        IF(EXISTS ${_item})
          STRING(REGEX MATCH "^(/usr|/lib|/bin)" _usr_find ${_item})
          LIST(FIND _${PROJECT_NAME}_EXTRA_ENV_${envvar} ${_item} _res)
          IF(NOT _usr_find AND _res EQUAL -1)
              LIST(APPEND _${PROJECT_NAME}_EXTRA_ENV_${envvar} ${_item})
          ENDIF()
        ENDIF()
      ELSE(_is_check)
        LIST(FIND _${PROJECT_NAME}_EXTRA_ENV_${envvar} ${_item} _res)
        IF( _res EQUAL -1)
          LIST(APPEND _${PROJECT_NAME}_EXTRA_ENV_${envvar} ${_item})
        ENDIF()
      ENDIF(_is_check)
    ENDIF()
  ENDFOREACH()

  LIST(FIND _${PROJECT_NAME}_EXTRA_ENV ${envvar} _res)
  IF(_res EQUAL -1)
    LIST(APPEND _${PROJECT_NAME}_EXTRA_ENV ${envvar})
  ENDIF()
  SET(_${PROJECT_NAME}_EXTRA_ENV_FULL "SET\(${PROJECT_NAME}_EXTRA_ENV ${_${PROJECT_NAME}_EXTRA_ENV}\)")
  FOREACH(_res ${_${PROJECT_NAME}_EXTRA_ENV})
    STRING(REPLACE "${CMAKE_INSTALL_PREFIX}" "\${PACKAGE_PREFIX_DIR}" _res_path "${_${PROJECT_NAME}_EXTRA_ENV_${_res}}")
    SET(_${PROJECT_NAME}_EXTRA_ENV_FULL "${_${PROJECT_NAME}_EXTRA_ENV_FULL}\nSET\(${PROJECT_NAME}_EXTRA_ENV_${_res} ${_res_path}\)")
  ENDFOREACH()
ENDMACRO(SALOME_ACCUMULATE_ENVIRONMENT)

#########################################################################
# SALOME_GENERATE_ENVIRONMENT_SCRIPT()
#
# USAGE: SALOME_GENERATE_ENVIRONMENT_SCRIPT(output script cmd opts)
#
# ARGUMENTS:
#   output [out] output command, e.g. for creation of target.
#   script [in]  output environement script name
#   cmd    [in]  input command, e.g. sphinx or python command.
#   opts   [in]  options for input command (cmd).
#
# This macro is called when it's necessary to use given environment to run some command.
# Macro generates environement script using previously created variables
# _${PROJECT_NAME}_EXTRA_ENV_<var>, where <var> is name of variable and
# _${PROJECT_NAME}_EXTRA_ENV (see marco SALOME_ACCUMULATE_ENVIRONMENT);
# and puts generated command in proper environment into <output> argument.
#
# Notes:
# - If <script> is specified as relative path, it is computed from the current build
#   directory.
#
MACRO(SALOME_GENERATE_ENVIRONMENT_SCRIPT output script cmd opts)
  IF(IS_ABSOLUTE ${script})
    SET(_script ${script})
  ELSE()
    SET(_script ${CMAKE_CURRENT_BINARY_DIR}/${script})
  ENDIF()

  IF(WIN32)
    SET(_ext "bat")
    SET(_call_cmd "call")
  ELSE()
    SET(_ext "sh")
    SET(_call_cmd ".")
  ENDIF()

  SET(_env)
  FOREACH(_item ${_${PROJECT_NAME}_EXTRA_ENV})
    FOREACH(_val ${_${PROJECT_NAME}_EXTRA_ENV_${_item}})
      IF(WIN32)
        IF(${_item} STREQUAL "LD_LIBRARY_PATH")
          SET(_item PATH)
        ENDIF()
        STRING(REPLACE "/" "\\" _env "${_env} @SET ${_item}=${_val};%${_item}%\n")
      ELSEIF(APPLE)
        IF(${_item} STREQUAL "LD_LIBRARY_PATH")
          SET(_env "${_env} export DYLD_LIBRARY_PATH=${_val}:\${DYLD_LIBRARY_PATH}\n")
        ELSE()
          SET(_env "${_env} export ${_item}=${_val}:\${${_item}}\n")
        ENDIF()
      ELSE()
        SET(_env "${_env} export ${_item}=${_val}:\${${_item}}\n")
      ENDIF()
    ENDFOREACH()
  ENDFOREACH()

  SET(_script ${_script}.${_ext})
  FILE(WRITE ${_script} "${_env}")

  SET(${output} ${_call_cmd} ${_script} && ${cmd} ${opts})

ENDMACRO(SALOME_GENERATE_ENVIRONMENT_SCRIPT)

#########################################################################
# SALOME_GENERATE_TESTS_ENVIRONMENT()
#
# USAGE: SALOME_GENERATE_TESTS_ENVIRONMENT(output)
#
# ARGUMENTS:
#   output [out] output environement variable.
#
# This macro generates <output> variable to use given environment to run some tests.
# Macro generates environement variable using previously created variables
# _${PROJECT_NAME}_EXTRA_ENV_<var>, where <var> is name of variable and
# _${PROJECT_NAME}_EXTRA_ENV (see marco SALOME_ACCUMULATE_ENVIRONMENT);
# and puts this variable into <output> argument.
#
MACRO(SALOME_GENERATE_TESTS_ENVIRONMENT output)
 SET(_env)
 SET(_WIN_LD_LIBRARY OFF)
 FOREACH(_item ${_${PROJECT_NAME}_EXTRA_ENV})
   IF(${_item} STREQUAL "LD_LIBRARY_PATH")
     SET(_WIN_LD_LIBRARY ON)
   ENDIF()
   SET(_env_${_item})
   FOREACH(_val ${_${PROJECT_NAME}_EXTRA_ENV_${_item}})
     IF(WIN32)
       STRING(REPLACE "/" "\\" _val "${_val}")
       SET(_env_${_item} "${_val};${_env_${_item}}")
     ELSE()
       SET(_env_${_item} "${_val}:${_env_${_item}}")
     ENDIF()
   ENDFOREACH()
 ENDFOREACH()

 IF(_WIN_LD_LIBRARY AND WIN32)
   SET(_env_PATH "${_env_PATH}$ENV{LD_LIBRARY_PATH};${_env_LD_LIBRARY_PATH}")
 ENDIF()

 IF(WIN32)
   SET(sep ",")
 ELSE()
   SET(sep ";")
 ENDIF()

 FOREACH(_item ${_${PROJECT_NAME}_EXTRA_ENV})
   IF(WIN32)
     IF(NOT ${_item} STREQUAL "LD_LIBRARY_PATH")
       SET(_env "${_item}=$ENV{${_item}};${_env_${_item}}${sep}${_env}")
     ENDIF()
   ELSE()
     STRING(REPLACE ";" ":" _iii "$ENV{${_item}}")
     SET(_env "${_item}=${_iii}:${_env_${_item}}${sep}${_env}")
   ENDIF()
 ENDFOREACH()

 # Get module name as substring of "Salome<ModuleName>"
 STRING(REGEX MATCH "^Salome" _is_salome_project ${PROJECT_NAME})
 IF(_is_salome_project)
   STRING(SUBSTRING "${PROJECT_NAME}" 6 -1 PRNAME)
 ELSE()
   SET(PRNAME ${PROJECT_NAME})
 ENDIF()
 SET(_env "${PRNAME}_ROOT_DIR=${CMAKE_INSTALL_PREFIX}${sep}${_env}")

 # Creating follow string for Windows environement:
 # "VAR1_ENV=1\;2\;3\;...\;...\;...;VAR2_ENV=1\;2\;3\;...\;...\;...;VAR3_ENV=1\;2\;3\;...\;...\;...;..."
 IF(WIN32)
   STRING(REGEX REPLACE "\\\\*;" "\\\\;" _env "${_env}")
   STRING(REGEX REPLACE "\\\\*;*," ";" _env "${_env}")
 ENDIF()

 SET(${output} "${_env}")

ENDMACRO(SALOME_GENERATE_TESTS_ENVIRONMENT)

#########################################################################
# SALOME_APPEND_LIST_OF_LIST()
#
# USAGE: SALOME_APPEND_LIST_OF_LIST(result element_list)
#
# Build a list of lists. The element_list is first parsed to convert it
# from
#     a;b;c;d;e
# to
#     a,b,c,d,e
#
# It is then added to the big list 'result'. Hence 'result' looks like:
#     a,b,c,d,e;f,g,h; ...
#
MACRO(SALOME_APPEND_LIST_OF_LIST result element_list)
  SET(_tmp_res)
  STRING(REPLACE ";" "," _tmp_res "${${element_list}}")

  # Yet another CMake stupidity - LIST(LENGTH ";" var) returns 0
  STRING(LENGTH result _list_len)
  IF(NOT _list_len EQUAL 0)
    SET(${result} "${${result}}${_tmp_res};")  # LIST(APPEND ...) doesn't handle well empty elements!?
  ELSE()
    SET(${result} "${_tmp_res};")              # to avoid redundant ';' at the beginning of the list
  ENDIF()

ENDMACRO(SALOME_APPEND_LIST_OF_LIST)

#########################################################################
# SALOME_CONFIGURE_PREPARE()
#
# USAGE: SALOME_CONFIGURE_PREPARE(pkg1 pkg2 ...)
#
# Prepare the variable that will be used to configure the file Salome<MODULE>Config.cmake,
# namely:
#    - _PREREQ_LIST      : the list of level 1 external prerequisites
#    - _PREREQ_DIR_LIST  : their corresponding CMake directories (i.e. where the CMake configuration
#    file for this package can be found, if there is any!)
#    - _PREREQ_COMPO_LIST: the list of components requested when this package was invoked
#
# All this information is built from the package_list, the list of level 1 packages for this module.
# Only the packages found in CONFIG mode are retained.
#
MACRO(SALOME_CONFIGURE_PREPARE)
  SET(_tmp_prereq "${ARGV}")
  SET(_PREREQ_LIST)
  SET(_PREREQ_DIR_LIST)
  SET(_PREREQ_COMPO_LIST)
  FOREACH(_prereq IN LISTS _tmp_prereq)
    IF(${_prereq}_DIR)
      SET(_PREREQ_LIST "${_PREREQ_LIST} ${_prereq}")
      FILE(TO_CMAKE_PATH ${${_prereq}_DIR} CURR_DIR)
      SET(_PREREQ_DIR_LIST "${_PREREQ_DIR_LIST} \"${CURR_DIR}\"")
      SALOME_APPEND_LIST_OF_LIST(_PREREQ_COMPO_LIST Salome${_prereq}_COMPONENTS)
    ENDIF()
  ENDFOREACH()
ENDMACRO(SALOME_CONFIGURE_PREPARE)

#######################################################################
#
# From a version string like "2.7.12+" extract the major, minor and patch number
# taking ONLY the numerical part.
# This macro was created to treat Ubuntu Python version number where the libs are
# version 2.7.12+ and the interp is 2.7.12 ...
#
MACRO(SALOME_EXTRACT_VERSION version_string major minor patch)
  IF(${version_string} MATCHES "[0-9]+[^0-9]*\\.[0-9]+[^0-9]*\\.*[0-9]*[^0-9]*")
    STRING(REGEX REPLACE "^([0-9]+)[^0-9]*\\.[0-9]+[^0-9]*\\.*[0-9]*[^0-9]*" "\\1" ${major} "${version_string}")
    STRING(REGEX REPLACE "^[0-9]+[^0-9]*\\.([0-9]+)[^0-9]*\\.*[0-9]*[^0-9]*" "\\1" ${minor} "${version_string}")

    IF(${version_string} MATCHES "[0-9]+[^0-9]*\\.[0-9]+[^0-9]*\\.[0-9]+[^0-9]*")
        # X.Y.Z format (python 3.5.2 ...)
        STRING(REGEX REPLACE "^[0-9]+[^0-9]*\\.[0-9]+[^0-9]*\\.([0-9]+)[^0-9]*" "\\1" ${patch} "${version_string}")
    ELSE()
        # X.Y format (python 3.5 ...)
        SET(${patch} "0")
    ENDIF()
  ELSE()
    MESSAGE("MACRO(SALOME_EXTRACT_VERSION ${version_string} ${major} ${minor} ${patch}")
    MESSAGE(FATAL_ERROR "Problem parsing version string, I can't parse it properly.")
  ENDIF()
ENDMACRO(SALOME_EXTRACT_VERSION)

#######################################################################
#
# This macro checks that swig files were generated.
# It is requared under Windows platform, because sometimes under Windows platform
# the genetarion of the swig wrappings tooks long time. And seems swig opens
# file at the begining of generation process and after that swig
# begins the generation of the content. In its turn Microsoft Visual Studio
# tryes to compile file immediately after creation and as a result compilation breaks.
MACRO(SWIG_CHECK_GENERATION swig_module)
  IF(WIN32)
    SET(SCRIPT
"@echo off
:check
( (call ) >> @SWIG_GEN_FILE_NAME@ ) 2>null && (
  echo The file @SWIG_GEN_FILE_NAME@ was created. & goto :eof
) || (
  echo The file @SWIG_GEN_FILE_NAME@ is still being created !!! & goto :check
)
:eof")
    LIST(LENGTH swig_generated_sources NB_GEN_FILES)
    IF(${NB_GEN_FILES})
      LIST(GET swig_generated_sources 0 SWIG_GEN_FILE_NAME)
      STRING(CONFIGURE ${SCRIPT} SCRIPT)
      GET_FILENAME_COMPONENT(SWIG_GEN_FILE_NAME_DIR ${SWIG_GEN_FILE_NAME} DIRECTORY)
      GET_FILENAME_COMPONENT(SWIG_GEN_FILE_NAME_WE ${SWIG_GEN_FILE_NAME} NAME_WE)
      SET(SCRIPT_FILE_NAME ${SWIG_GEN_FILE_NAME_DIR}/${SWIG_GEN_FILE_NAME_WE}.bat)
      FILE(WRITE ${SCRIPT_FILE_NAME} ${SCRIPT})
      ADD_CUSTOM_TARGET(${SWIG_MODULE_${swig_module}_REAL_NAME}_ready
                        DEPENDS ${SWIG_GEN_FILE_NAME}
                        COMMAND ${SCRIPT_FILE_NAME}
                        COMMENT "Waiting for swig wrappings !!!")
      ADD_DEPENDENCIES(${SWIG_MODULE_${swig_module}_REAL_NAME} ${SWIG_MODULE_${swig_module}_REAL_NAME}_ready)
    ELSE()
       MESSAGE(FATAL "swig sources for targer ${swig_module} are not found !!!")
     ENDIF()
  ENDIF()
ENDMACRO(SWIG_CHECK_GENERATION)
