#!/bin/sh
# $1 printer name
# $2 raw file to print
lp -o raw -d $1 $2
