# Task dimension
N=${1}
M=${2}
K=${3}
# Number of mappers
N_M=${4}
# Number of reducers
N_R=${5}

# Generate data for matrix multiplication task
python3 generate_task.py -n ${N} -m ${M} -k ${K}

# Copy all data and code to namenode (see also `docker cp`)
docker cp ../matmul_streaming namenode:/
# Run Hadoop Streaming on namenode (see also `docker exec`)
docker exec -it namenode bash matmul_streaming/run_hadoop.sh ${N} ${M} ${K} ${N_M} ${N_R}
# Copy results from namenode (see also `docker cp`)
docker cp namenode:\\matmul_streaming/data/output data/

# Check solution
python3 check_answer.py -n ${N} -m ${M} -k ${K}
