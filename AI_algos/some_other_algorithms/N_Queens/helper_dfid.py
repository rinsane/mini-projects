from collections import deque

# DFID Solver (Depth-First Iterative Deepening) with Iterative DFS
def dfid_solver(board):
    """
    Solves the N-Queens problem using Depth-First Iterative Deepening (DFID).

    Parameters:
    - board: 2D list representing the initial state of the chessboard.
    """
    # Iterative DFS with depth limit
    def dfs_limited_iterative(current_board, max_depth):
        """
        Performs depth-limited search iteratively up to the specified depth.

        Parameters:
        - current_board: 2D list representing the current state of the chessboard.
        - max_depth: Integer representing the maximum depth to explore.

        Returns:
        - max_open_length: Maximum length of the stack during the search.
        - max_closed_length: Maximum length of the closed set during the search.
        """
        stack = [(current_board, 0, 0)]  # Stack: (board, row, depth)
        closed_list = set()
        max_open_length = 0
        max_closed_length = 0

        while stack:
            current_board, row, depth = stack.pop()

            # Update max lengths
            max_open_length = max(max_open_length, len(stack))
            max_closed_length = max(max_closed_length, len(closed_list))

            if is_board_valid(current_board):
                solutions.append(current_board)

            if depth >= max_depth or row == len(board):  # Reached max depth or end of rows
                continue

            # Generate next states by moving queens in the current row
            for next_board in movegen(current_board, row):
                board_tuple = tuple(map(tuple, next_board))  # Convert to tuple for immutability
                if board_tuple not in closed_list:
                    closed_list.add(board_tuple)
                    stack.append((next_board, row + 1, depth + 1))
                    # No need to remove from closed_list in iterative DFS

        return max_open_length, max_closed_length

    max_depth = 0
    solutions = []
    overall_max_open_length = 0
    overall_max_closed_length = 0

    while max_depth <= len(board):
        max_open_length, max_closed_length = dfs_limited_iterative(board, max_depth)
        overall_max_open_length = max(overall_max_open_length, max_open_length)
        overall_max_closed_length = max(overall_max_closed_length, max_closed_length)
        max_depth += 1

    print("DFID Solutions Found!")
    for current_board in solutions:
        print_board(current_board)

    print(f"Max Open Length: {overall_max_open_length}, Max Closed Length: {overall_max_closed_length}")

# Move generation: generates next possible queen placements.
def movegen(board, row):
    """
    Generates the next possible board states by moving the queen in the specified row.

    Parameters:
    - board: 2D list representing the current state of the chessboard.
    - row: Integer representing the current row where the queen is being moved.

    Returns:
    - next_states: List of new board configurations after moving the queen.
    """
    n = len(board)
    next_states = []
    
    # Find current column where the queen is placed in the row
    current_col = board[row].index(1)

    # Try to move the queen in the current row to all other columns
    for col in range(n):
        new_board = [r[:] for r in board]  # Deep copy of the board
        new_board[row][current_col] = 0  # Remove queen from current column
        new_board[row][col] = 1  # Place queen in the new column
        next_states.append(new_board[:])
    
    return next_states

def is_board_valid(board):
    """
    Checks if the current board configuration is valid, ensuring no two queens 
    threaten each other.

    Parameters:
    - board: 2D list representing the current state of the chessboard.

    Returns:
    - True if the board is valid, False otherwise.
    """
    n = len(board)
    
    # Check each row for queen placement
    for row in range(n):
        for col in range(n):
            if board[row][col] == 1:
                # Check for conflicts in the same column
                for i in range(n):
                    if i != row and board[i][col] == 1:
                        return False
                
                # Check for diagonal conflicts
                for i in range(1, n):
                    if row + i < n and col + i < n and board[row + i][col + i] == 1:
                        return False
                    if row + i < n and col - i >= 0 and board[row + i][col - i] == 1:
                        return False
                    if row - i >= 0 and col + i < n and board[row - i][col + i] == 1:
                        return False
                    if row - i >= 0 and col - i >= 0 and board[row - i][col - i] == 1:
                        return False
    
    return True

# Function to print the board in a readable format
def print_board(board):
    """
    Prints the chessboard in a human-readable format, using 'Q' for queens and '.' for empty spaces.

    Parameters:
    - board: 2D list representing the current state of the chessboard.
    """
    for row in board:
        for col in row:
            if col == 1:
                print("Q", end=" ")
            else:
                print(".", end=" ")
        print()
    print()