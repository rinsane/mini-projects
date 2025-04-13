import heapq

class PuzzleState:
    def __init__(self, state, parent=None, g=0, h=0):
        self.state = state  # state is now a 3x3 tuple
        self.parent = parent
        self.g = g  # Cost to reach this state
        self.h = h  # Heuristic value
        self.f = g + h  # Total cost

    def __lt__(self, other):
        return self.f < other.f  # Compare based on f-value for priority queue

    def get_possible_moves(self):
        """Generate all possible states by sliding tiles."""
        moves = []
        blank_row, blank_col = self.find_blank_tile()
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]  # Up, Down, Left, Right
        
        for dr, dc in directions:
            new_row, new_col = blank_row + dr, blank_col + dc
            if 0 <= new_row < 3 and 0 <= new_col < 3:
                new_state = [list(row) for row in self.state]  # Convert tuples to lists for mutation
                # Swap blank tile with the adjacent tile
                new_state[blank_row][blank_col], new_state[new_row][new_col] = new_state[new_row][new_col], new_state[blank_row][blank_col]
                moves.append(PuzzleState(tuple(tuple(row) for row in new_state), self, self.g + 1, self.calculate_heuristic(new_state)))
        
        return moves

    def find_blank_tile(self):
        """Find the position of the blank tile (0)."""
        for row_idx, row in enumerate(self.state):
            for col_idx, value in enumerate(row):
                if value == 0:
                    return row_idx, col_idx
        return None

    def calculate_heuristic(self, state):
        """Calculate the Manhattan distance heuristic."""
        distance = 0
        for r in range(3):
            for c in range(3):
                value = state[r][c]
                if value != 0:
                    target_r, target_c = divmod(value - 1, 3)
                    distance += abs(r - target_r) + abs(c - target_c)
        return distance

    def is_goal_state(self):
        """Check if the current state is the goal state."""
        return self.state == ((1, 2, 3), (4, 5, 6), (7, 8, 0))

class PuzzleSolver:
    def __init__(self, start_state):
        self.start_state = PuzzleState(start_state)
        self.open_list = []
        self.closed_set = set()

    def solve(self):
        heapq.heappush(self.open_list, self.start_state)

        while self.open_list:
            # Step 1: Get node with lowest f-value
            current_state = heapq.heappop(self.open_list)
            self.closed_set.add(current_state.state)

            # Step 2: Check if goal state
            if current_state.is_goal_state():
                return self.reconstruct_solution(current_state)

            # Step 3: For each neighbor (MoveGen)
            for neighbor in current_state.get_possible_moves():
                if neighbor.state not in self.closed_set and neighbor not in self.open_list:
                    # If the neighbor is neither in OPEN nor in CLOSED
                    heapq.heappush(self.open_list, neighbor)
                elif neighbor.state in self.closed_set:
                    # If the neighbor is in CLOSED, propagate improvement
                    if current_state.g + 1 < neighbor.g:
                        self.propagate_improvement(neighbor, current_state)
        
        return None  # No solution found

    def propagate_improvement(self, neighbor, current_state):
        """Propagate the improvement to the neighbors of M, including those in CLOSED."""
        # Update the g, f, and parent for the neighbor
        neighbor.g = current_state.g + 1
        neighbor.f = neighbor.g + neighbor.h
        neighbor.parent = current_state

        # Re-add the neighbor to the open list if it has a better g value
        heapq.heappush(self.open_list, neighbor)

        # Now propagate to all the neighbors of M
        for next_neighbor in neighbor.get_possible_moves():
            if neighbor.g + 1 < next_neighbor.g:
                # Update the next neighbor with the improved path
                next_neighbor.g = neighbor.g + 1
                next_neighbor.f = next_neighbor.g + next_neighbor.h
                next_neighbor.parent = neighbor

                # If the next neighbor is in the closed list, propagate the improvement
                if next_neighbor.state in self.closed_set:
                    self.propagate_improvement(next_neighbor, neighbor)

    def reconstruct_solution(self, goal_state):
        """Reconstruct the path to the goal state."""
        path = []
        current = goal_state
        while current:
            path.append(current.state)
            current = current.parent
        return path[::-1]  # Return the path from start to goal


# Example usage with a 3x3 grid board
start_state = (
    (1, 3, 5),
    (4, 2, 0),
    (7, 8, 6)
)

solver = PuzzleSolver(start_state)
solution = solver.solve()

if solution:
    print("Solution found:")
    for step in solution:
        for row in step:
            print(row)
        print()  # Blank line for separation between steps
else:
    print("No solution found")
