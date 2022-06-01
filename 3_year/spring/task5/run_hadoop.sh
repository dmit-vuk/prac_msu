SOURCE_BASE_PATH="/task5"

INPUT_HADOOP_DIR="/task5/input"
OUTPUT_HADOOP_DIR="/task5/output"
STAGE1="/task5/output/stage_1"
STAGE2="/task5/output/stage_2"
STAGE3="/task5/output/stage_3"
STAGE4="/task5/output/stage_4"
STAGE5="/task5/output/stage_5"
STAGE6="/task5/output/stage_6"
FINAL="/task5/output/final"

HADOOP_STREAMING_PATH="${HADOOP_HOME}/share/hadoop/tools/lib/hadoop-streaming-3.2.1.jar"

hdfs dfs -test -d ${INPUT_HADOOP_DIR}
if [ $? -eq 0 ];
  then
    echo "Remove ${INPUT_HADOOP_DIR}"
    hdfs dfs -rm -r ${INPUT_HADOOP_DIR}
fi

hdfs dfs -test -d ${OUTPUT_HADOOP_DIR}
if [ $? -eq 0 ];
  then
    echo "Remove ${OUTPUT_HADOOP_DIR}"
    hdfs dfs -rm -r ${OUTPUT_HADOOP_DIR}
fi

hdfs dfs -test -d ${STAGE1}
if [ $? -eq 0 ];
  then
    echo "Remove ${STAGE1}"
    hdfs dfs -rm -r ${STAGE1}
fi

hdfs dfs -test -d ${STAGE2}
if [ $? -eq 0 ];
  then
    echo "Remove ${STAGE2}"
    hdfs dfs -rm -r ${STAGE2}
fi

hdfs dfs -test -d ${STAGE3}
if [ $? -eq 0 ];
  then
    echo "Remove ${STAGE3}"
    hdfs dfs -rm -r ${STAGE3}
fi

hdfs dfs -test -d ${STAGE4}
if [ $? -eq 0 ];
  then
    echo "Remove ${STAGE4}"
    hdfs dfs -rm -r ${STAGE4}
fi

hdfs dfs -test -d ${STAGE5}
if [ $? -eq 0 ];
  then
    echo "Remove ${STAGE5}"
    hdfs dfs -rm -r ${STAGE5}
fi

hdfs dfs -test -d ${STAGE6}
if [ $? -eq 0 ];
  then
    echo "Remove ${STAGE6}"
    hdfs dfs -rm -r ${STAGE6}
fi

hdfs dfs -test -d ${FINAL}
if [ $? -eq 0 ];
  then
    echo "Remove ${FINAL}"
    hdfs dfs -rm -r ${FINAL}
fi

test -d ${SOURCE_BASE_PATH}/data/output
if [ $? -eq 0 ];
  then
    echo "Remove ${SOURCE_BASE_PATH}/data/output"
    rm -rf ${SOURCE_BASE_PATH}/data/output
fi

hdfs dfs -mkdir -p ${INPUT_HADOOP_DIR}
hdfs dfs -mkdir -p ${INPUT_HADOOP_DIR}/ratings
hdfs dfs -mkdir -p ${INPUT_HADOOP_DIR}/movies
hdfs dfs -copyFromLocal ${SOURCE_BASE_PATH}/data/input/ratings_preproc.txt ${INPUT_HADOOP_DIR}/ratings
hdfs dfs -copyFromLocal ${SOURCE_BASE_PATH}/data/input/movies_preproc.txt ${INPUT_HADOOP_DIR}/movies

chmod 0777 ${SOURCE_BASE_PATH}/src/mapper_1.py
chmod 0777 ${SOURCE_BASE_PATH}/src/reducer_1.py
hadoop_streaming_arguments="\
  -D mapred.reduce.tasks=8 \
  -D mapreduce.map.java.opts=-Xmx4g \
  -D mapreduce.reduce.java.opts=-Xmx4g \
  -D mapred.text.key.comparator.options=k1,1 \
  -files ${SOURCE_BASE_PATH}/src \
  -mapper src/mapper_1.py -reducer src/reducer_1.py \
  -input ${INPUT_HADOOP_DIR}/ratings/* -output ${STAGE1} \
"
hadoop jar ${HADOOP_STREAMING_PATH} ${hadoop_streaming_arguments}

chmod 0777 ${SOURCE_BASE_PATH}/src/mapper_2.py
chmod 0777 ${SOURCE_BASE_PATH}/src/reducer_2.py
hadoop_streaming_arguments="\
  -D mapred.reduce.tasks=8 \
  -D mapreduce.map.java.opts=-Xmx4g \
  -D mapreduce.reduce.java.opts=-Xmx4g \
  -D mapred.text.key.comparator.options=k1,1 \
  -files ${SOURCE_BASE_PATH}/src \
  -mapper src/mapper_2.py -reducer src/reducer_2.py \
  -input ${STAGE1}/* -output ${STAGE2} \
"
hadoop jar ${HADOOP_STREAMING_PATH} ${hadoop_streaming_arguments}

chmod 0777 ${SOURCE_BASE_PATH}/src/mapper_3.py
chmod 0777 ${SOURCE_BASE_PATH}/src/reducer_3.py
hadoop_streaming_arguments="\
  -D mapred.reduce.tasks=8 \
  -D mapreduce.map.java.opts=-Xmx4g \
  -D mapreduce.reduce.java.opts=-Xmx4g \
  -D mapred.text.key.comparator.options=k1,1 \
  -files ${SOURCE_BASE_PATH}/src \
  -mapper src/mapper_3.py -reducer src/reducer_3.py \
  -input ${INPUT_HADOOP_DIR}/ratings/* ${STAGE2}/* -output ${STAGE3} \
"
hadoop jar ${HADOOP_STREAMING_PATH} ${hadoop_streaming_arguments}

chmod 0777 ${SOURCE_BASE_PATH}/src/mapper_4.py
chmod 0777 ${SOURCE_BASE_PATH}/src/reducer_4.py
hadoop_streaming_arguments="\
  -D mapred.reduce.tasks=8 \
  -D mapreduce.map.java.opts=-Xmx4g \
  -D mapreduce.reduce.java.opts=-Xmx4g \
  -D mapred.text.key.comparator.options=k1,1 \
  -files ${SOURCE_BASE_PATH}/src \
  -mapper src/mapper_4.py -reducer src/reducer_4.py \
  -input ${STAGE3}/* -output ${STAGE4} \
"

hadoop jar ${HADOOP_STREAMING_PATH} ${hadoop_streaming_arguments}

chmod 0777 ${SOURCE_BASE_PATH}/src/mapper_5.py
chmod 0777 ${SOURCE_BASE_PATH}/src/reducer_5.py
hadoop_streaming_arguments="\
  -D mapred.reduce.tasks=8 \
  -D mapreduce.map.java.opts=-Xmx4g \
  -D mapreduce.reduce.java.opts=-Xmx4g \
  -D mapred.text.key.comparator.options=k1,1 \
  -files ${SOURCE_BASE_PATH}/src \
  -mapper src/mapper_5.py -reducer src/reducer_5.py \
  -input ${INPUT_HADOOP_DIR}/ratings/* ${STAGE4}/* -output ${STAGE5} \
"

hadoop jar ${HADOOP_STREAMING_PATH} ${hadoop_streaming_arguments}

chmod 0777 ${SOURCE_BASE_PATH}/src/mapper_6.py
chmod 0777 ${SOURCE_BASE_PATH}/src/reducer_6.py
hadoop_streaming_arguments="\
  -D mapred.reduce.tasks=8 \
  -D mapreduce.map.java.opts=-Xmx4g \
  -D mapreduce.reduce.java.opts=-Xmx4g \
  -D mapred.text.key.comparator.options=k1,1 \
  -files ${SOURCE_BASE_PATH}/src \
  -mapper src/mapper_6.py -reducer src/reducer_6.py \
  -input ${INPUT_HADOOP_DIR}/movies/* ${STAGE5}/* -output ${STAGE6} \
"

hadoop jar ${HADOOP_STREAMING_PATH} ${hadoop_streaming_arguments}

chmod 0777 ${SOURCE_BASE_PATH}/src/mapper_7.py
chmod 0777 ${SOURCE_BASE_PATH}/src/reducer_7.py
hadoop_streaming_arguments="\
  -D mapred.reduce.tasks=8 \
  -D mapreduce.map.java.opts=-Xmx4g \
  -D mapreduce.reduce.java.opts=-Xmx4g \
  -D stream.num.map.output.key.fields=3 \
  -D mapreduce.partition.keycomparator.options='-k1,1-k3,3nr-k2,2' \
  -files ${SOURCE_BASE_PATH}/src \
  -mapper src/mapper_7.py -reducer src/reducer_7.py \
  -input ${STAGE6}/* -output ${FINAL} \
"
hdfs dfs -copyToLocal ${OUTPUT_HADOOP_DIR} ${SOURCE_BASE_PATH}/data

hdfs dfs -rm -r ${INPUT_HADOOP_DIR}
hdfs dfs -rm -r ${OUTPUT_HADOOP_DIR}