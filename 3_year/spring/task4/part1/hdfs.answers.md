1) number of splits:3, map tasks=3, reduce tasks=0
2) Missing replicas: 4. Отсутствует 4 реплики, так как в директории logs2 находится
	2 файла с фактором репликации 3. Но на нашем кластере находится всего 1 datanode, 
	на которой и располагается всего 1 реплика. Поэтому для каждого из файлов не 
	хватает по 2 реплики.
3) Configured Capacity: 269490393088 (250.98 GB)
   Present Capacity: 246372225024 (229.45 GB)
   DFS Remaining: 246034313216 (229.14 GB)
   DFS Used: 337911808 (322.26 MB)
   DFS Used%: 0.14%