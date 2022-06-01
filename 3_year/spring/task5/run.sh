python3 preprocessing.py

# Copy all data and code to namenode (see also `docker cp`)
docker cp ../task5 namenode:/
# Run Hadoop Streaming on namenode (see also `docker exec`)
docker exec -it namenode bash task5/run_hadoop.sh
# Copy results from namenode (see also `docker cp`)
docker cp namenode:\\task5/data/output data/

