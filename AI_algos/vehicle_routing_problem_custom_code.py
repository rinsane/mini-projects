"""
INPUTS AT BOTTOM
"""

import random

def aco_tsp(cities, demand, vehicle_capacity):

    n = len(cities)
    initial_pheromone = 1
    rho = 0.8

    alpha = 1
    beta = 1

    # iteration limit for each n_vehicle number of variables
    iteration_limit = 3

    Q = 100
    K = 10

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

    global_cost = float('inf')
    global_tour = None
    n_vehicles = 0
    """
    +-------------------------------------------------+
    |ITERATIONS INCREASING THE NUMBER OF VEHICLES SENT|
    +-------------------------------------------------+
    """
    # loop runs for maximum len(demand) number of vehicles, i.e., one vehicle for each customer
    while n_vehicles <= len(demand):
        # start with a single vehicle
        n_vehicles += 1
        print(f"""
              ---------------------------
               VEHICLES SENT {n_vehicles}
              ---------------------------""")


        """
        +-------------------------------------------+
        |ITERATIONS FOR n_vehicle NUMBER OF VEHICLES|
        +-------------------------------------------+
        """
        for _ in range(iteration_limit):
            print(f"ITERATION {_ + 1} ==>\n")

            # del tau matrix for updating the tau matrix after an iteration
            del_tau = [[0 for _ in range(n)] for _ in range(n)]

            customers_served = set()
            customers_served.add(0)

            vehicle = 0
            tours = {}

            """
            +------------------------------------------------------------------------+
            |ITERATIONS FOR nth VEHICLE TO SEE HOW MANY CUSTOMERS IT IS ABLE TO SERVE|
            +------------------------------------------------------------------------+
            """
            while vehicle < n_vehicles:

                vehicle += 1
                print(f"VEHICLE {vehicle} 🚚 :\n")
                curr_path = [0]
                current_capacity = vehicle_capacity
                
                # vehicle hopping to the next city
                for _ in range(n - 1):
                    probability = {}
                    denom = 0
                    curr_customer = curr_path[-1]
                    customer_found = 0
                    
                    # finding the next city possibilities and their probabilities
                    for next_customer in range(n):
                        if next_customer not in customers_served and demand[next_customer] <= current_capacity:
                            customer_found = 1
                            probability[next_customer] = 0
                            denom += (tau[curr_customer][next_customer] ** alpha) * (eta[curr_customer][next_customer] ** beta)
                    
                    if not customer_found:
                        break

                    for next_customer in probability:
                        probability[next_customer] = ((tau[curr_customer][next_customer] ** alpha) * (eta[curr_customer][next_customer] ** beta)) / denom

                    # probabilistic choice making for the next customer
                    print(probability)
                    next_customer = random.choices(list(probability.keys()), weights=list(probability.values()), k=1)[0]
                    curr_path.append(next_customer)
                    customers_served.add(next_customer)
                    current_capacity -= demand[next_customer]

                if curr_path == [0]:
                    print("nothing done\n")
                    continue

                print("path made:", curr_path)
                curr_cost = 0
                
                # cost of current path
                for i in range(len(curr_path)):
                    curr_cost += cities[curr_path[i]][curr_path[(i + 1) % len(curr_path)]]
                print("cost of path:", curr_cost)
                
                # adding this path to the tours
                tours[tuple(curr_path)] = curr_cost

                # pheromone deposited on the current path (del tau updation)
                phero = Q / curr_cost
                print(phero)
                for i in range(len(curr_path)):
                    del_tau[curr_path[i]][curr_path[(i + 1) % len(curr_path)]] += phero
                    del_tau[curr_path[(i + 1) % len(curr_path)]][curr_path[i]] += phero
            
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

            # i.e. all customers have been served in this tour
            if len(customers_served) == n:
                if sum(list(tours.values())) < global_cost:
                    global_cost = sum(list(tours.values()))
                    global_tour = tours
            
            print()

    if global_tour is not None:
        print("BEST TOURS: ")
        print(global_tour)
        print("NUMBER OF VEHICLES USED: ")
        print(len(global_tour))
        print("BEST COST: ")
        print(global_cost)
    else:
        print("NO best tour found that serves all customers, consider increasing the vehicle_limit")



"""
+------+
|INPUTS|
+------+
"""

cities = [
    [0 , 10, 3 , 6 , 18], # distance between depot (0) and each customer
    [10, 0 , 2 , 20, 9 ], # distance of cust 1 from every other cust 
    [3 , 2 , 0 , 12, 7 ], # distance of cust 2 from every other cust
    [6 , 20, 12, 0 , 15], # distance of cust 3 from every other cust
    [18, 9 , 7 , 15, 0 ]  # distance of cust 4 from every other cust
]

# demand for each customer
demand = [0, 15, 5, 8, 12]
# vehicle capacity
vehicle_capacity = 100

if max(demand) > vehicle_capacity:
    print("Vechicle(s) cannot serve all the customers")
else:
    aco_tsp(cities, demand, vehicle_capacity)
