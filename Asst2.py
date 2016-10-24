import sys

test = {
	1 : [2, 3, 4, 6, 7, 10],
	2 : [1, 3, 4, 5, 6],
	3 : [1, 2],
	4 : [1, 2],
	5 : [2, 6],
	6 : [1, 2, 5, 7, 8],
	7 : [1, 6, 8, 9, 10],
	8 : [6, 7, 9],
	9 : [7, 8, 10],
	10 : [1, 7, 9],
}

print "Number of args: %d" %len(sys.argv)
if len(sys.argv) < 2:
	fp = input("Please specify an input file:")
else:
	fp = sys.argv[1]

file = open(fp, "r")
VertColour = file.readline().strip('(').split()
VertColour = [int(x) for x in VertColour]

# setColour = ['r', 'y', 'g', 'b']
graph = dict()
color = dict()

while True:
	line = file.readline().replace('(','').replace(')','').split()

	if line:
		line = [int(x) for x in line]
		graph.update({line[0]: line[1:]})
		color.update({line[0]: range(0,VertColour[1])})
	if not line: break

for x in graph:
	print x, graph[x] #, '\t', len(graph[x]), color[x]

# print 'Greatest Degree: ', max(graph, key = lambda x: len(graph[x]))


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


# depth_first_search(graph)

def least_constrain(graph, color):
	lcv = []
	minConstrain = 15
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


def backtrack_maxDegree(graph, color):
	maxDeg = max(graph, key = lambda x: len(graph[x]))
	stack = [maxDeg]
	visited = []
	while stack:
		node = stack.pop(0)
		# print node
		color[node] = [color[node].pop(0)]
		if node not in visited:
			visited.append(node)
			for x in graph[node]:
				if x in visited:
					pass
				else:
					if color[node][0] in color[x]:
						color[x].remove(color[node][0])
					else:
						return False
				# if len(color[x]) > 1:
				# 	color[x].remove(color[node][0])
			nextNode = least_constrain(graph, color)
			if nextNode > 0:
				stack.append(nextNode)
				# print 'Next Node: ', nextNode
			# else:
			# 	print 'No node'
	return True

def constraint_check(graph, color):
	for x in graph:
		assigned = color[x][0]
		for value in graph[x]:
			if assigned == color[value][0]:
				return False
	return True

if backtrack_maxDegree(graph, color):
	for x in color:
		print x, '\t', color[x]
else:
	print 'No Legal Colorings Exist'
# if constraint_check(graph, color):
# 	for x in color:
# 		print x, '\t', color[x]
# else:
# 	print 'Legal Coloring Does Not Exist'






