#!/bin/sh

# Binary names
MASTER_BIN="./master"
SLAVE_BIN="./slave"


# Log file
LOG_FILE_native="native.txt"
LOG_FILE_one_cont="one_container.txt"
LOG_FILE_three_cont="three_containers.txt"

# Native execution, 1-3 slaves
LOG_FILE_native1="native1_inv.txt"
LOG_FILE_native2="native2_inv.txt"
LOG_FILE_native3="native3_inv.txt"

# One container, 1-3 slaves
LOG_FILE_cont_1_1="cont_1_1_inv.txt"
LOG_FILE_cont_1_2="cont_1_2_inv.txt"
LOG_FILE_cont_1_3="cont_1_3_inv.txt"

# More containers, 2 and 3
LOG_FILE_cont_2_2="cont_2_2_inv.txt"
LOG_FILE_cont_3_3="cont_3_3_inv.txt"

# run -d --ipc=host --cap-add=sys_nice --ulimit rtprio=99 10.165.163.188:5000/latencytest:initial
DOCKER_FLAGS="-d --ipc=host --cap-add=sys_nice --ulimit rtprio=99"
DOCKER_LATENCY_TEST_IMAGE="10.165.163.188:5000/latencytest:initial"
DOCKER_ENTRY_POINT="/slave"






echo "Executing container tests..."



echo "Executing tests with individual containers"

docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
                        ${DOCKER_ENTRY_POINT} 1

docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
                        ${DOCKER_ENTRY_POINT} 2

docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
                        ${DOCKER_ENTRY_POINT} 3

${MASTER_BIN} 1 2 3 > ${LOG_FILE_cont_3_3}

echo "Three slaves in individual containers executed"

sleep 10

docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
                        ${DOCKER_ENTRY_POINT} 1

docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
                        ${DOCKER_ENTRY_POINT} 2

${MASTER_BIN} 1 2 > ${LOG_FILE_cont_2_2}

echo "Two slaves in individual containers executed"
sleep 10


echo "All slaves in a single container:"



docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
 						${DOCKER_ENTRY_POINT} 1 2 3

${MASTER_BIN} 1 2 3 > ${LOG_FILE_cont_1_3}
echo "Three slaves in one container executed"
sleep 10

docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
 						${DOCKER_ENTRY_POINT} 1 2

${MASTER_BIN} 1 2 > ${LOG_FILE_cont_1_2}
echo "Two slaves one container executed"
sleep 10

docker run ${DOCKER_FLAGS} ${DOCKER_LATENCY_TEST_IMAGE}\
 						${DOCKER_ENTRY_POINT} 1

${MASTER_BIN} 1 > ${LOG_FILE_cont_1_1}
echo "Three slaves in one container executed"
sleep 10





echo "Executing native tests..."


${SLAVE_BIN} 1 2 3 &
sleep 2
${MASTER_BIN} 1 2 3 > ${LOG_FILE_native3}
echo "Three slaves executed"
${SLAVE_BIN} 1 2 &
sleep 2
${MASTER_BIN} 1 2 > ${LOG_FILE_native2}
echo "Two slaves executed"
sleep 2

${SLAVE_BIN} 1 &
sleep 2
${MASTER_BIN} 1 > ${LOG_FILE_native1}
echo "One slave executed"
echo "Native tests executed."
