



OUTPUT_FILE="mem_output.txt"


doLog() {
    rm -f ${OUTPUT_FILE}
    while true
    do
        free >> ${OUTPUT_FILE}
        sleep 1
    done
}



doLog &
LOG_PID="$!"
./exec_latency_test.sh "$@"
kill ${LOG_PID}
