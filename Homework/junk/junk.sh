#!/bin/bash
###############################################################################
# Date: 05/25/2021
# Pledge: "I pledge my honor that I have abided by the Stevens Honor System.:
# Description: Creating a recycling bin
###############################################################################

mkdir -p ./../.junk
readonly JUNK_DIR=$(realpath ../.junk)
file="$(pwd)/junk.sh"
readonly USAGE=$(cat << ENDOFTEXT
Usage: $(basename "$file") [-help] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
ENDOFTEXT
)
readonly TOO_MANY_OPTS="Error: Too many options enabled."

if [ $# -eq 0 ]; then	
	echo "$USAGE"
fi

help_flag=0
list_flag=0
purge_flag=0

while getopts ":hlp" option; do
	case "$option" in 
		h) help_flag=1
			;;
		l) list_flag=1
			;;
		p) purge_flag=1
			;;
		?) help_flag=1
		   echo "Error unknown option '$OPTARG'" >&2
		   echo "$USAGE" 
		   exit 1
		   ;;
	esac
done

count=$(( help_flag + list_flag + purge_flag ))

if [ $count -gt 1 ]; then
	echo "$TOO_MANY_OPTS"
	echo "$USAGE"
	exit 1;
fi

shift "$(($OPTIND-1))"

if [ $help_flag -eq 1 ]; then
	if [ $# -gt 0 ]; then
		echo "$TOO_MANY_OPTS"
		echo "$USAGE"
		exit 1
	else
		echo "$USAGE"
	fi
fi

if [ $list_flag -eq 1 ]; then
	if [ $# -gt 0 ]; then
		echo "$TOO_MANY_OPTS"
		echo "$USAGE"
		exit 1
	else
		echo "$(ls -lAF $JUNK_DIR)"
	fi
fi


if [ $purge_flag -eq 1 ]; then
	if [ $# -gt 0 ]; then
		echo "$TOO_MANY_OPTS"
		echo "$USAGE"
		exit 1
	else
		$(rm -r $JUNK_DIR/*)
		$(rm -r $JUNK_DIR/.* 2>/dev/null)
	fi
fi

for i in "$@"; do
	if [ -e "$i" ]; then
		$(mv "$i" $JUNK_DIR)
	else
		echo "Warning: '"$i"' not found."
	fi
done

exit 0
