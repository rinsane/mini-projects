"""
INPUTS ON LINE: 126
"""

import random

# i am nature, i will mutate you, if god allows me to ~~
def mutater(nodes, mut_probab):
    for i in range(len(nodes)):
        
        # mutate or not?
        flip = random.choices([1, 0], weights=[mut_probab, 1-mut_probab], k=1)[0]
        if flip:
            j = random.randint(0, len(nodes[i]) - 1)
            elem = nodes[i][j]
            if elem == '1':
                elem = '0'
            else:
                elem = '1'
            nodes[i] = nodes[i][:j] + elem + nodes[i][j+1:]
    
    return nodes

def mater(nodes):
    
    # random partition into two parts
    random.shuffle(nodes)
    partition1 = nodes[:len(nodes) // 2]
    partition2 = nodes[len(nodes) // 2:]

    offsprings = []
    # do for all parents
    while partition1 and partition2:
        
        # random parent selection
        random.shuffle(partition1)
        parent1 = partition1.pop()
        random.shuffle(partition2)
        parent2 = partition2.pop()

        # single point crossover (by middle)
        child1 = parent1[:len(parent1)//2] + parent2[len(parent2)//2:]
        child2 = parent2[:len(parent1)//2] + parent1[len(parent2)//2:]
        offsprings.append(child1)
        offsprings.append(child2)

    return offsprings

# fitness is square of the decimal number
def fitness(node):
    node = [int(i) for i in node]
    p = 0
    fit = 0
    for i in node[::-1]:
        fit += i * (2 ** p)
        p += 1

    # returning square of decimal value of node
    return fit ** 2
    
# checker for answer
def satisfier(node, clauses):
    node = [int(i) for i in node]
    falses = 0
    for clause in clauses:
        curr = 0
        for i in range(len(clause)):
            if clause[i] == 1:
                curr = curr or node[i]
            elif clause[i] == -1:
                curr = curr or (not node[i])
        if curr == 0 or curr == False:
            falses += 1    
    return falses

def genetic(start, clauses, iteration_limit, mut_probab, K):

    # n candidate solutions
    nodes = start[:]

    for _ in range(iteration_limit):

        # checker for SAT
        ans = None
        for node in nodes:
            if satisfier(node, clauses) == 0:
                ans = node
        if ans != None:
            print("Node Found! ")
            print(node)
            return
        
        # fitness values of each node from the candidates
        fitnesses = [[i, fitness(i)] for i in nodes]
        fitnesses.sort(key=lambda x: x[1])

        # probability computation for each fitness of a node
        denom = 0
        for _, fit in fitnesses:
            denom += fit
        for i in range(len(fitnesses)):
            fitnesses[i][1] = fitnesses[i][1] / denom
        print(fitnesses)
        
        # choosing n members from candidates (as per probability, with replacement)
        parents = [i[0] for i in fitnesses]
        probabilities = [i[1] for i in fitnesses]
        choice = random.choices(parents, weights=probabilities, k=len(nodes))
        print(choice)

        # offspring generation
        offsprings = mater(choice)
        print(offsprings)

        # offspring mutation
        offsprings = mutater(offsprings, mut_probab)

        # sorting best offsprings and replacing k parents with k best
        offsprings.sort(key=lambda x: fitness(x), reverse=True)
        for i in range(K):
            parents[i] = offsprings[i]
        
        # next generation of parents
        nodes = parents[:]

"""
+------+
|INPUTS|
+------+
"""

clauses = [
   # a   b   c   d 
    [1 , 1,  0 , 0 ],
    [1 , 0 , 1 , 0 ],
    [0 , 0 , 1 , 1 ],
    [0 , 1 , 0 , -1],
    [1 , 0 , 0 , 1 ],
    [0 , 1 , 1 , 0 ],
    [0 , 0 , 1 , -1],
    [0 , 1 , 0 , 1 ],
    [0 , -1, 0 , 1 ],
]

# using the first 6 numbers as start nodes
start = []
for node in range(6):
    temp = node
    node = str(bin(node))[2:].zfill(len(clauses[0]))
    start.append(node)

iteration_limit = 100

# mutation probability
mut_probab = 0.002

# number of weak members to be replaced every iteration
K = len(clauses[0]) // 2

genetic(start, clauses, iteration_limit, mut_probab, K)
