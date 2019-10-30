

CC="gcc"
CROSS_BUILD="arm-linux-gnueabihf-"
CFLAGS="-pthread"
SRC="shdsem.c "
SRC_SLAVE="latency_slave.c"
SRC_MASTER="latency.c latency_master.c"
BIN_SLAVE="slave"
BIN_MASTER="master"

DOCKER_CALL="docker build -t latencytest ."


${CROSS_BUILD}${CC} ${CFLAGS} ${SRC} ${SRC_SLAVE} -o ${BIN_SLAVE}
${CROSS_BUILD}${CC} ${CFLAGS} ${SRC} ${SRC_MASTER} -o ${BIN_MASTER}
cp ${BIN_MASTER} ./docker_build/
cp ${BIN_SLAVE} ./docker_build/
cd docker_build
${DOCKER_CALL}
