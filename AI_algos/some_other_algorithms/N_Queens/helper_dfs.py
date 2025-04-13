from collections import deque

# DFS Solver
def dfs_solver(board):
    """
    Solves the N-Queens problem using Depth-First Search (DFS).

    Parameters:
    - board: 2D list representing the initial state of the chessboard.
    """
    open_list = [(board, 0)]  # Stack: (board, row)
    closed_list = set()
    solutions = []
    max_open_length = 0
    max_closed_length = 0

    while open_list:
        current_board, row = open_list.pop()
        
        # Update max lengths
        max_open_length = max(max_open_length, len(open_list))
        max_closed_length = max(max_closed_length, len(closed_list))
        
        if is_board_valid(current_board):
            solutions.append(current_board)
        
        if row == len(board):  # Goal check: all rows processed
            continue
        
        # Generate next states by moving queens in the current row
        for next_board in movegen(current_board, row):
            board_tuple = tuple(map(tuple, next_board))  # Convert to tuple for immutability
            if board_tuple not in closed_list:
                open_list.append((next_board, row + 1))
                closed_list.add(board_tuple)
    
    print("DFS Solution Found!")
    for current_board in solutions:
        print_board(current_board)
    
    print(f"Max Open Length: {max_open_length}, Max Closed Length: {max_closed_length}")

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