#!/bin/bash

if http --check-status --ignore-stdin --timeout=2.5 HEAD marcxtof.com &> /dev/null; then
    echo 'OK!'
else
    case $? in
        2) echo 'Request timed out!' ;;
        3) echo 'Unexpected HTTP 3xx Redirection!' ;;
        4) echo 'HTTP 4xx Client Error!' ;;
        5) echo 'HTTP 5xx Server Error!' ;;
        *) echo 'Other Error!' ;;
    esac
fi
