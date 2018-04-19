#!/bin/sh

# Binary names
MASTER_BIN="./master"
SLAVE_BIN="./slave"

# IPC semaphores
MTOS_SEM_PREFIX="/mtos"
STOM_SEM_NAME="/stom"
# Number of measurements
MEAS_COUNT="10000"

# Log file
LOG_FILE_native="native.txt"
LOG_FILE_one_cont="one_container.txt"
LOG_FILE_two_cont="two_containers.txt"
LOG_FILE_three_cont="three_containers.txt"


DOCKER_FLAGS="--ipc=host"
DOCKER_LATENCY_TEST_IMAGE="latency_test:latest"
DOCKER_ENTRY_POINT="container_entry_point.sh"

declare -a SLAVE_IDS=("0" "1" "2")
declare -a SLAVE_PIDS=()


# Execute tests natively
echo "Executing native tests"

echo "Spawning slaves"

for id in "${SLAVE_IDS[@]}"
do
	${SLAVE_BIN} ${MTOS_SEM_PREFIX}${id} ${STOM_SEM_NAME} &
	SLAVE_PIDS+=("$!")
	echo "Started slave, PID: $!"
done

echo "Starting master"

${MASTER_BIN} ${MTOS_SEM_PREFIX} ${STOM_SEM_NAME} ${MEAS_COUNT} > ${LOG_FILE_native}

echo "Master finished. Output can be found from file ${LOG_FILE_native}"
echo "Killing slaves"

for id in "${SLAVE_PIDS[@]}"
do
	kill ${id}
	echo "Killed ${id}"
done

echo "Finished"



#Execute tests in containers
SLAVE_PIDS=()

echo "Spawning slaves in one container"
docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE} ${DOCKER_ENTRY_POINT} \
    0 2 ${MTOS_SEM_PREFIX} ${STOM_SEM_NAME}

echo "Running master"
${MASTER_BIN} ${MTOS_SEM_PREFIX} ${STOM_SEM_NAME} ${MEAS_COUNT} > ${LOG_FILE_one_cont}

echo "Master finished. Output can be found from file ${LOG_FILE_one_cont}"

docker kill \$(docker ps -aq)

