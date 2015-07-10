all: compile_bfs compile_dfs

run_all: bfs dfs

compile_bfs:
	g++ bfs.c -o BFS

compile_dfs:
	g++ dfs.c -o DFS

bfs: compile_bfs
	./BFS map1.map 

dfs: compile_dfs
	./DFS map1.map 
