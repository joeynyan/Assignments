import sys

print "hello world"

if len(sys.argv) < 2:
	fp = input("Please specify an input file:")
else:
	fp = sys.argv[1]

# Code to open file for reading and start parsing
file = open(fp, "r")
# rule = file.readline().strip().split()
# rule = [int(x) for x in rule]

rule = []
i = 0;
while True:
	line = file.readline().split()

	if line:
		# line = [x for x in line]
		rule.insert(i, line);
		i = i+1
	if not line: break

print 'Inputed rules is:'
# print rules
for x in range(len(rule)):
	print rule[x]

#  Do input validication later. make sure valid entries are entered
query = raw_input("What is your query: ")
print "Your query is: ", query

goals = []
goals.insert(0, query);

#backward chaining
def solve(goals):
	if not goals:
		return True
	a = goals.pop(0)
	for x in range(len(rule)):
		if rule[x][0] == a:
			print "SubGoals are: ", rule[x][1:]
			if solve(goals.extend(rule[x][1:])) == True:
				print a, "is true"
				return True
	return False

print solve(goals)