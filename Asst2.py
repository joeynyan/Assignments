import sys # allows command line args to be inputted

# print "Number of args: %d" %len(sys.argv)
if len(sys.argv) < 2:
	fp = input("Please specify an input file:")
else:
	fp = sys.argv[1]

# Code to open file for reading and start parsing
file = open(fp, "r")
VertColour = file.readline().strip('(').split()
VertColour = [int(x) for x in VertColour]

# setColour = ['r', 'y', 'g', 'b']
graph = dict() # stores adjacency list of graph
color = dict() # stores possible coloring options for graph

# parses the graph
while True:
	line = file.readline().replace('(','').replace(')','').split()

	if line:
		line = [int(x) for x in line]
		graph.update({line[0]: line[1:]})
		color.update({line[0]: range(1,VertColour[1]+1)})
	if not line: break

print 'Inputed graph is:'
for x in graph:
	print x, graph[x] #, '\t', len(graph[x]) , color[x]

# Depth first search
def depth_first_search(graph):
	maxDeg = max(graph, key = lambda x: len(graph[x]))
	stack = [maxDeg]
	visited = []
	while stack:
		node = stack.pop(0)
		print node
		if node not in visited:
			visited.append(node)
			print graph[node]
			stack.extend(graph[node])

# searches through all possible nodes in graph using minimum remaining variable heuristic
# then uses degree heuristic to break the ties and return one
def least_constrain(graph, color):
	lcv = []
	minConstrain = (len(graph)+1)
	maxDeg = 0
	key = -1
	for x in color:
		if len(color[x]) > 1:
			if minConstrain > len(color[x]):
				minConstrain = len(color[x]) #find the minimum number of variables available in color
	# print 'MinConstrain = ', minConstrain
	for x in color:
		if len(color[x]) == minConstrain:
			lcv.append(x)
	for x in lcv:
		if len(graph[x]) > maxDeg:
			maxDeg = len(graph[x])
			key = x
	return key

# searches through the graph and assigns colorings according to mrv, degree heuristic, and lcv
def backtrack_maxDegree(graph, color):
	maxDeg = max(graph, key = lambda x: len(graph[x]))
	stack = [maxDeg]
	visited = []
	while stack:
		node = stack.pop(0)
		if len(color[node]) > 1:
			color[node] = [color[node].pop(0)]
		if node not in visited:
			visited.append(node)
			for x in graph[node]:
				if x in visited:
					pass
				else:
					if color[node][0] in color[x]:
						color[x].remove(color[node][0])
			nextNode = least_constrain(graph, color)
			if nextNode > 0:
				stack.append(nextNode)
				# print 'Stack: ', stack
			# else:
			# 	print 'No node'
	return True

# Double check that all constraints are correct
def constraint_check(graph, color):
	for x in graph:
		assigned = color[x][0]
		for value in graph[x]:
			if assigned == color[value][0]:
				return False
	return True

# Decides if legal coloring exist or doesn't
print(' ')
if backtrack_maxDegree(graph, color) and constraint_check(graph, color):
	print 'Legal Coloring Available'
	for x in color:
		print 'Node: ', x, '\t', color[x]
else:
	print 'No Legal Colorings Exist'







