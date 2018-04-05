#!/bin/sh
# This is an entry point for containers used in my Master's thesis.
# This basically just spawns IPC slaves with ids from $1 to $2. No error checks are done.
# @author: toni.lammi@wapice.com

SLAVE_BIN="/mnt/slave"


if [ $# != 4 ]
then
	echo "Usage: $0 (first-slave-id) (last-slave-id) (master-to-slave-sem-prefix) (slave-to-master-sem-name)"
	exit 1
fi


for id in  $(seq $1 $2)
do
	$SLAVE_BIN $3$id $4 & 
done

