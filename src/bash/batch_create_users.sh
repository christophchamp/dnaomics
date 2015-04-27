#!/bin/bash
# DESCRIPTION: Batch user creation from file
#
# Create a script that meets the following requirements:
# 1. The script must accept the command line option -f which specifies the path
#    to a file that contains a list of usernames to be created
# 2. The list of usernames can be separated by a comma, a variable amount of
#    whitespace (including newlines), or a semicolon.
# 3. The script must create a system user for each user listed in the input
#    file, with a password of the username reversed.
# Example user list file:
# $ cat /path/to/users.txt
# alice   Bill,charlie;danny
# earl
# frank;greg

# With the above input file, the script would create a user "alice" with a
# password of "ecila", a user "Bill" with the password "lliB", etc.

usage() { echo "usage: $0 [-f <path_to_file>]" 1>&2; exit 1; }

USERADD=/usr/sbin/useradd

while getopts ":f:" o; do
    case "${o}" in
        f) path=${OPTARG};;
    esac
done
shift $((OPTIND-1))

# FIXME
#if [ -z "${f}" ]; then
#    usage
#fi

for username in `sed -e :a -e '$!N;s/\n/ /;ta' -e 's/[ ;,]\{1,\}/ /g' ${path}`; do
    password=`echo "${username}"|rev`;
    #echo "NAME=${username}; PASSWORD=${password}";
    #sudo $USERADD ${username}

    # Change/add password to new user
    # NOTE: `--stdin` has been deprecated on newer distros. Use `chpasswd` on those
    # sudo echo 'username:password' | chpasswd
    # sudo echo -e "password\npassword\n" | passwd
    #sudo echo ${password}|passwd ${username} --stdin
    $USERADD -p $(openssl passwd -1 ${password}) ${username}
done
