import random

def aco_tsp(cities, data):

    n = len(cities)
    initial_pheromone, rho, alpha, beta, iteration_limit, n_ants, Q, K = data
    # ant selection
    ants = random.sample(range(len(cities)), n_ants) # can also select random number of ants placed randomly on the edges
    ants = [0, 1, 3]
    
    # for computing path probability in the future (stays constant)
    eta = [
        [K/i if i != 0 else 0 for i in row]
        for row in cities
    ]
    
    # pheromones (dynamically changes with pheromones)
    tau = [
        [initial_pheromone if i != j else 0 for j in range(n)]
        for i in range(n)
    ]


    best_tour = None
    best_cost = float('inf')

    for _ in range(iteration_limit):
        print(f"ITERATION {_ + 1} ==>\n")

        # del tau matrix for updating the tau matrix after an iteration
        del_tau = [[0 for _ in range(n)] for _ in range(n)]

        for ant in ants:

            print(f"ANT {ant} 🐜 :\n")
            curr_path = [ant]
            CLOSED = set([ant])
            
            # ant path making for this ant
            for _ in range(n - 1):
                probability = {}
                denom = 0
                curr_city = curr_path[-1]
                
                # finding the next city possibilities and their probabilities
                for next_city in range(n):
                    if next_city not in CLOSED:
                        probability[next_city] = 0
                        denom += (tau[curr_city][next_city] ** alpha) * (eta[curr_city][next_city] ** beta)
                for next_city in probability:
                    probability[next_city] = ((tau[curr_city][next_city] ** alpha) * (eta[curr_city][next_city] ** beta)) / denom
                
                # probabilistic choice making for the next city
                print(probability)
                next_city = random.choices(list(probability.keys()), weights=list(probability.values()), k=1)[0]
                curr_path.append(next_city)
                CLOSED.add(next_city)

            print("path made:", curr_path)
            curr_cost = 0
            
            # cost of current path
            for i in range(n):
                curr_cost += cities[curr_path[i]][curr_path[(i + 1) % n]]
            print("cost of path:", curr_cost)

            # best path checker
            if curr_cost < best_cost:
                best_cost = curr_cost
                best_tour = curr_path

            # pheromone deposited on the current path (del tau updation)
            phero = Q / curr_cost
            print(phero)
            for i in range(n):
                del_tau[curr_path[i]][curr_path[(i + 1) % n]] += phero
                del_tau[curr_path[(i + 1) % n]][curr_path[i]] += phero
        
            print()

        # updating the tau matrix using the del tau values
        for i in range(n):
            for j in range(n):
                if i != j:
                    tau[i][j] = (1 - rho) * tau[i][j] + del_tau[i][j]

        print("DEL TAU: ")
        for row in del_tau:
            print(row)
        print("TAU: ")
        for row in tau:
            print(row)
        
        print()

    print("BEST TOUR: ")
    print(best_tour)
    print("BEST COST: ")
    print(best_cost)


"""
+------+
|INPUTS|
+------+
"""

cities = [
    [0 , 10, 3 , 6 , 18],
    [10, 0 , 2 , 20, 9 ],
    [3 , 2 , 0 , 12, 7 ],
    [6 , 20, 12, 0 , 15],
    [18, 9 , 7 , 15, 0 ]
]

initial_pheromone = 1
rho = 0.8

alpha = 1
beta = 1

iteration_limit = 3
n_ants = 3

Q = 100
K = 10

aco_tsp(cities, [initial_pheromone, rho, alpha, beta, iteration_limit, n_ants, Q, K])