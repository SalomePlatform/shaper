#!/bin/bash

find_duplicated_images_usage()
{
    echo "Find duplicated images."
    echo
    echo "Usage: $(basename ${0}) [options]"
    echo
    echo "Options:"
    echo
    echo "  --help (-h)        Print this help information and exit."
    echo "  --print (-p)       List duplicated files. Default: OFF."
    echo
    exit 0
}



find_duplicated_images_main()
{
    local verbose=0

    local where=$(readlink -f $(dirname "${0}"))

    # parse command line
    local option
    while getopts ":-:hp" option ${@}
    do
        if [ "${option}" = "-" ]
        then
            case ${OPTARG} in
                help ) find_duplicated_images_usage ;;
                print ) verbose=1 ;;
                * ) echo "Wrong option: --${OPTARG}" > /dev/stderr ; exit 1 ;;
            esac
        else
            case ${option} in
                h ) find_duplicated_images_usage ;;
                p ) verbose=1 ;;
                ? ) echo "Wrong option" > /dev/stderr ; exit 1 ;;
            esac
        fi
    done
    shift $((OPTIND - 1))
    
    local result=0

    if [ "${verbose}" = "1" ]
    then
	local filename
	for filename in $(find ${where} -type f -wholename "*/doc/*.png" -exec basename {} \; | sort | uniq -d)
	do
	    find ${where} -type f -wholename "*/doc/*/${filename}"
	    result=$((result+1))
	done
    else
	result=$(find -type f -wholename "*/doc/*.png" -exec basename {} \; | sort | uniq -d | wc -l)
    fi

    return ${result}
}

find_duplicated_images_main "${@}"
