#!/bin/sh

MASTER_BIN="./master"
SLAVE_BIN="./slave"

MTOS_SEM_PREFIX="/mtos"
STOM_SEM_NAME="/stom"

MEAS_COUNT="10000"

LOG_FILE="out.txt"


VOLUME_MOUNT_HOST="/usr/home"
VOLUME_MOUNT_GUEST="/mnt"

DOCKER_FLAGS="--ipc=host"

declare -a SLAVE_IDS=("0" "1" "2")
declare -a SLAVE_PIDS=()

if [ "$#" -ne 1 ]; then
	echo "Usage: $0 (native|container|all)"
	exit 1
fi
if [ "$1" != "native" ] && [ "$1" != "container" ] && [ "$1" != "all" ]; then
	echo "Usage: $0 (native|container|all)"
	exit 2
fi


# Execute tests natively
if [ "$1" == "native" ] || [ "$1" == "all" ]; then
	echo "Executing native tests"

	echo "Spawning slaves"
	for id in "${SLAVE_IDS[@]}"
	do
		${SLAVE_BIN} ${MTOS_SEM_PREFIX}${id} ${STOM_SEM_NAME} &
		SLAVE_PIDS+=("$!")
		echo "Started slave, PID: $!"
	done
	
	echo "Starting master"
	${MASTER_BIN} ${MTOS_SEM_PREFIX} ${STOM_SEM_NAME} ${MEAS_COUNT} > ${LOG_FILE}
	echo "Master finished. Output can be found from file ${LOG_FILE}"	
	
	echo "Killing slaves"
	for id in "${SLAVE_PIDS[@]}"
	do
		kill ${id}
		echo "Killed ${id}"
	done

	echo "Finished"

fi

#Execute tests in containers
if [ "$1" == "container" ] || [ "$1" == "all" ]; then
	SLAVE_PIDS=()
	echo "Executing container tests"

	echo "Spawning slaves in their own containers"
	for id in "${SLAVE_IDS}"
	do
		docker $DOCKER_FLAGS --volume $VOLUME_MOUNT_HOST/vol${id}:${VOLUME_MOUNT_GUEST}\
			${SLAVE_BIN} ${MTOS_SEM_PREFIX}${id} ${STOM_SEM_NAME}
		SLAVE_PIDS+=($!)
		echo "Started slave, PID: $!"
	done
	
	echo "Starting master"
	${MASTER_BIN} ${MTOS_SEM_PREFIX} ${STOM_SEM_NAME} > ${LOG_FILE}
	echo "Master finished. Output can be found from file ${LOG_FILE}"

	for id in "${SLAVE_PIDS[@]}"
	do
		kill ${id}
		echo "Killed ${id}"
	done

	echo "Finished" 


	SLAVE_PIDS=()
	echo "Spawning slaves in two containers"
	echo "Not implemented yet"
	echo "Finished"




	echo "Spawning slaves in one container"
	echo "Not yet implented"

fi
