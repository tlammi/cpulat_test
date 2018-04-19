#!/bin/sh
# This is an entry point for containers used in my Master's thesis.
# This basically just spawns IPC slaves with ids from $1 to $2.
# @author: toni.lammi@wapice.com

SLAVE_BIN="/slave"


if [ $# != 4 ]
then
	echo "Usage: $0 (first-slave-id) (last-slave-id)"
	exit 1
fi

declare -a SLAVE_PIDS=()
for id in  $(seq $1 $2)
do
	$SLAVE_BIN $id &
    SLAVE_PIDS+=("$1")
done

wait SLAVE_PIDS
