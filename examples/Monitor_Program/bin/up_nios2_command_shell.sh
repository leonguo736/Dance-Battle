#!/bin/bash
############################################################
#                                                          #
# Nios2 Software Build Tools (SBT) Command Shell           #
#                                                          #
# Execute this script to setup an environment for          #
# using the Nios2eds Software Build Tools (SBT)            #
#                                                          #
# This shell uses nios2-elf-gcc version 4.7.3              #
#                                                          #
#                                                          #
# Copyright (c) 2010,2012,2013 Altera Corporation          #
# All Rights Reserved.                                     #
#                                                          #
############################################################


############################################################
#
# Get the Root ACDS directory
#
# Don't delimit on whitespace so that dirname is happy with
# spaces in paths
IFS= 

if [ -n "${COMSPEC}" ]; then
    _IS_WINDOWS=1
    PATH="/bin:/usr/bin:${PATH}"
fi

if [ "${_IS_WINDOWS}" = "1" ] && [ -n "$(which cygpath 2>/dev/null)" ]; then
    _IS_CYGWIN=1
fi

if [ "${_IS_CYGWIN}" = "1" ]; then
    printf "\033]2;Altera Nios II EDS 14.0 [gcc4]\a"
    _QUARTUS_ROOTDIR=$(cygpath --unix ${QUARTUS_ROOTDIR})
else    
    _QUARTUS_ROOTDIR=${QUARTUS_ROOTDIR}
fi

_ACDS_ROOT=$(dirname "${_QUARTUS_ROOTDIR}")
unset _QUARTUS_ROOTDIR

if [ ! -d "${_ACDS_ROOT}/nios2eds" ]; then
    echo "${_ACDS_ROOT}/nios2eds not found. Invalid or corrupt ACDS Install" 1>&2
    exit 1
fi

if [ ! -d "${_ACDS_ROOT}/University_Program" ]; then
    echo "${_ACDS_ROOT}/University_Program not found. Invalid or corrupt University Program Install" 1>&2
    exit 1
fi

if [ -z "${QUARTUS_ROOTDIR_OVERRIDE}" ]; then
    if [ ! -d "${_ACDS_ROOT}/quartus" ]; then
        echo "${_ACDS_ROOT}/quartus not found. Invalid or corrupt ACDS Install" 1>&2
        exit 1
    fi
fi

############################################################


############################################################
#
# Convenience bash functions for setting up the environment
#

env_var_delete () {
    local var=${2:-PATH}
    local value
    local IFS=':'
    for i in ${!var} ; do
        if [ "$i" != "$1" ] ; then
            value=${value:+$value:}$i
        fi
    done
    export $var="$value"
}

env_var_prepend () {
    env_var_delete $1 $2
    local var=${2:-PATH}
    export $var="$1${!var:+:${!var}}"
}

env_var_append () {
    env_var_delete $1 $2
    local var=${2:-PATH}
    export $var="${!var:+${!var}:}$1"
}
############################################################


############################################################
#
# Setup Quartus II
#

if [ -n "${QUARTUS_ROOTDIR_OVERRIDE}" ]; then
    QUARTUS_ROOTDIR=${QUARTUS_ROOTDIR_OVERRIDE}

    if [ "${_IS_CYGWIN}" = "1" ]; then
        QUARTUS_ROOTDIR="$(cygpath -u "${QUARTUS_ROOTDIR}" 2>/dev/null)"
    fi
else
    QUARTUS_ROOTDIR=${_ACDS_ROOT}/quartus
fi

env_var_prepend "${QUARTUS_ROOTDIR}/sopc_builder/bin"
env_var_prepend "${QUARTUS_ROOTDIR}/bin32"
env_var_prepend "${QUARTUS_ROOTDIR}/bin64"

if [ "${_IS_CYGWIN}" = "1" ]; then
    QUARTUS_ROOTDIR="$(cygpath -m "${QUARTUS_ROOTDIR}" 2>/dev/null)"
fi
export QUARTUS_ROOTDIR
############################################################


############################################################
#
# Setup Modelsim AE
#  (if modelsim_ae/modelsim_ase directory exists and vsim not in PATH)
#

if [ -z "$(which vsim 2>/dev/null)" ]; then

    if [ "${_IS_WINDOWS}" = "1" ]; then
        _VSIM_EXEC_DIR=win32aloem
    else
        _VSIM_EXEC_DIR=linuxaloem
    fi

    if [ -d "${_ACDS_ROOT}/modelsim_ae/${_VSIM_EXEC_DIR}" ]; then
        env_var_prepend "${_ACDS_ROOT}/modelsim_ae/${_VSIM_EXEC_DIR}"
    else
        if [ -d "${_ACDS_ROOT}/modelsim_ase/${_VSIM_EXEC_DIR}" ]; then
            env_var_prepend "${_ACDS_ROOT}/modelsim_ase/${_VSIM_EXEC_DIR}"
        fi
    fi
    unset _VSIM_EXEC_DIR
fi
############################################################


############################################################
#
# Setup Nios2eds (GCC 4)
#

SOPC_KIT_NIOS2=${_ACDS_ROOT}/nios2eds

env_var_prepend "${SOPC_KIT_NIOS2}/bin"
env_var_prepend "${SOPC_KIT_NIOS2}/sdk2/bin"

if [ "${_IS_WINDOWS}" = "1" ]; then
    if [ -d "${SOPC_KIT_NIOS2}/bin/gnu/H-x86_64-mingw32/bin" ]; then
	env_var_prepend ${SOPC_KIT_NIOS2}/bin/gnu/H-x86_64-mingw32/bin
    else
    env_var_prepend ${SOPC_KIT_NIOS2}/bin/gnu/H-i686-mingw32/bin
    fi
else
    if [ -d "${SOPC_KIT_NIOS2}/bin/gnu/H-x86_64-pc-linux-gnu/bin" ]; then
	env_var_prepend ${SOPC_KIT_NIOS2}/bin/gnu/H-x86_64-pc-linux-gnu/bin
    else
    env_var_prepend ${SOPC_KIT_NIOS2}/bin/gnu/H-i686-pc-linux-gnu/bin
    fi
fi

if [ "${_IS_CYGWIN}" = "1" ]; then
    SOPC_KIT_NIOS2="$(cygpath -m "${SOPC_KIT_NIOS2}" 2>/dev/null)"
fi
export SOPC_KIT_NIOS2
############################################################


############################################################
#
# Setup ARM GNU baremetal compiler 
#

UP_ROOTDIR=${_ACDS_ROOT}/University_Program

# Modify the path to GNU_ARM_TOOL_ROOTDIR here
GNU_ARM_TOOL_ROOTDIR=${UP_ROOTDIR}/Monitor_Program/arm_tools/baremetal

if [ ! -d "${GNU_ARM_TOOL_ROOTDIR}/bin" ]; then
    echo "${GNU_ARM_TOOL_ROOTDIR}/bin not found. Please specify the path to CodeSourcery GNU ARM toolchain in ${UP_ROOTDIR}/Monitor_Program/bin/up_nios2_command_shell.sh." 1>&2
    exit 1
fi

# May need to add the linux path to the GNU Toolchain
#if [ "${_IS_WINDOWS}" = "1" ]; then
    env_var_prepend ${GNU_ARM_TOOL_ROOTDIR}/bin
#else
#    env_var_prepend ${GNU_ARM_TOOL_ROOTDIR}/bin/gnu/H-i686-pc-linux-gnu/bin
#fi

if [ "${_IS_CYGWIN}" = "1" ]; then
    UP_ROOTDIR="$(cygpath -m "${UP_ROOTDIR}" 2>/dev/null)"
    GNU_ARM_TOOL_ROOTDIR="$(cygpath -m "${GNU_ARM_TOOL_ROOTDIR}" 2>/dev/null)"
fi
export UP_ROOTDIR
export GNU_ARM_TOOL_ROOTDIR
############################################################


if [ -n "$*" ]; then
    exec $@
fi

unset _ACDS_ROOT
unset _IS_WINDOWS
unset _IS_CYGWIN
unset env_var_prepend
unset env_var_append
unset env_var_remove
