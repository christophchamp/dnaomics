#!/bin/bash
if [ ! -z "$(sed -ne '/CentOS/Ip' /etc/issue)" ]; then echo FOUND; else echo ERROR; fi

function which_cmd() {
  # Utility function - 'which_cmd VAR cmd'
  #                    Sets VAR="`which cmd`"

  # Since some OSes (like CentOS 5.8) don't have 'which' available, this is the bash equivalent to `which ${2}`.
  local cmd="$(builtin type -P ${2} 2>/dev/null)"

  if [ -z "${cmd}" ]; then
    echo "ERROR: Unable to find '${2}' binary in system path.  Aborting."
    exit 1
  fi

  # Since ${1}='VAR' and ${cmd}='/path/to/cmd', this is basically VAR="/path/to/cmd"
  eval ${1}='"'"${cmd}"'"'
}

# Determine OS type
function os_type(){
  if [ ! -z "$("${_SED}" -ne '/ubuntu/Ip' /etc/issue)" ]; then
    os='ubuntu'
  elif [ -f /etc/debian_version ]; then
    os='debian'
  elif [ -f /etc/arch-release ]; then
    os='arch'
  elif [ -f /etc/gentoo-release ]; then
    os='gentoo'
  elif [ ! -z "$("${_SED}" -ne '/opensuse/Ip' /etc/issue)" ]; then
    os='opensuse'
  elif [ -f /etc/sysconfig/network-scripts/ifcfg-eth0 -o -f /etc/sysconfig/network-scripts/ifcfg-eth1 ]; then
    # Includes all RedHat variants (RHEL, CentOS, or Fedora).
    os='redhat'
  else
    echo "ERROR: Unable to detect OS!"
    exit 3
  fi
  echo "Auto-detected OS type as '${os}'."
}

os_type
