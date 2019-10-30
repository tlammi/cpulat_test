#!/bin/sh

# Binary names
MASTER_BIN="./master"
SLAVE_BIN="./slave"


# Log file
LOG_FILE_native="native.txt"
LOG_FILE_one_cont="one_container.txt"
LOG_FILE_three_cont="three_containers.txt"

LOG_FILE_postfix=".txt"

LOG_FILE_native_prefix="native"
LOG_FILE_single_prefix="cont_single"
LOG_FILE_ind_prefix="cont_ind"


# run -d --ipc=host --cap-add=sys_nice --ulimit rtprio=99 10.165.163.188:5000/latencytest:initial
DOCKER_FLAGS="-d --ipc=host --cap-add=sys_nice --ulimit rtprio=99"
DOCKER_LATENCY_TEST_IMAGE="10.165.163.188:5000/latencytest"
DOCKER_ENTRY_POINT="/slave"

rmContainers() {
    echo "Removing containers..."
    docker rm $(docker ps -aq)
    echo "Containers removed"
}

execNative() {
    ${SLAVE_BIN}  $(seq 1 "$1")&
    sleep 2
    ${MASTER_BIN} $(seq 1 "$1")> ${LOG_FILE_native_prefix}${1}${LOG_FILE_postfix}
    sleep 2
}

execSingle() {
    docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
        ${DOCKER_ENTRY_POINT} $(seq 1 "$1")

    ${MASTER_BIN} $(seq 1 "$1") > ${LOG_FILE_single_prefix}${1}${LOG_FILE_postfix}
    sleep 10

    rmContainers
}

execIndividual() {
    for i in $(seq 1 "$1")
    do
        docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
                        ${DOCKER_ENTRY_POINT} $i

    done

    ${MASTER_BIN} $(seq 1 "$1") > ${LOG_FILE_ind_prefix}${1}${LOG_FILE_postfix}
    sleep 10

    rmContainers

}


echo "Executing tests"

for i in "$@"
do
    echo "Testing with $i slaves"
    echo "Running native tests"
    execNative $i
    sleep 2
    echo "Running test in single container"
    execSingle $i

    if [ "$i" -gt 1 ]
    then
        echo "Running test in individual containers"
        execIndividual $i

    fi
done

echo "Finished"
