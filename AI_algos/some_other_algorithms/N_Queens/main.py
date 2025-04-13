# Importing BFS, DFS, and DFID solvers from helper modules.
from helper_bfs import bfs_solver
from helper_dfs import dfs_solver
from helper_dfid import dfid_solver

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

# Generate the initial board from the student ID
def student_id_to_queens(student_id, board_size):
    """
    Generates the initial board configuration based on a student's ID.

    Parameters:
    - student_id: String representing the student's ID.
    - board_size: Integer representing the size of the board.

    Returns:
    - board: 2D list representing the initial state of the chessboard.
    """
    board = [[0 for _ in range(board_size)] for _ in range(board_size)]
    idx = 0

    # Generate columns based on student ID digits
    col_str = student_id[:4] + student_id[7:]

    for row in range(board_size):
        col = int(col_str[idx])
        board[row][col] = 1
        idx += 1
    
    return board

def main():
    """
    Main function to set up the chessboard and run the solvers for the N-Queens problem.
    """
    student_id = "2022UCS0106"
    board_size = 6  # Adjust based on problem size
    board = student_id_to_queens(student_id, board_size)

    print("Initial Board Configuration:")
    print_board(board)

    input("Press Enter to start the BFS:")
    bfs_solver(board)  # Run BFS solver.

    input("Press Enter to start the DFS:")
    dfs_solver(board)  # Run DFS solver.

    input("Press Enter to start the DFID:")
    dfid_solver(board)  # Run DFID solver.

if __name__ == "__main__":
    main()
