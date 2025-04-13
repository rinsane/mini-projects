import math
import time
import copy

class TicTacToe:
    def __init__(self, board):
        self.board = [[0 for _ in range(4)] for _ in range(4)]
        curr = 0
        for i in range(4):
            for j in range(4):
                self.board[i][j] = board[curr]
                curr += 1

    def display_board(self):
        for row in self.board:
            for ele in row:
                if ele == -1:
                    print(f" {ele}", end='')
                else:
                    print(f"  {ele}", end='')
            print()
        print()
    
    def print_board(self, board):
        for row in board:
            for ele in row:
                if ele == -1:
                    print(f" {ele}", end='')
                else:
                    print(f"  {ele}", end='')
            print()
        print()

    def get_valid_moves(self, board):
        valid_moves = []

        for i in range(4):
            for j in range(4):
                if board[i][j] == 0:
                    valid_moves.append((i, j))

        return valid_moves

    def check_win(self, board, player):
        winners = []
        for i in range(4):
            for j in range(4):
                if j + 2 < 4:
                    members = [board[i][j], board[i][j+1], board[i][j+2]]
                    if members.count(0) == 1:
                        if -1 not in members:
                            winners.append(1)
                        elif 1 not in members:
                            winners.append(-1)
                if i + 2 < 4:
                    members = [board[i][j], board[i+1][j], board[i+2][j]]
                    if members.count(0) == 1:
                        if -1 not in members:
                            winners.append(1)
                        elif 1 not in members:
                            winners.append(-1)
                if i + 2 < 4 and j + 2 < 4:
                    members = [board[i][j], board[i+1][j+1], board[i+2][j+2]]
                    if members.count(0) == 1:
                        if -1 not in members:
                            winners.append(1)
                        elif 1 not in members:
                            winners.append(-1)
        
        if player in winners:
            return player
        return None if not winners else winners[0]

    def evaluate(self, board, player):
        winner = self.check_win(board, player)
        print(f"WINNER = {winner}")
        if winner == 1:
            return 100
        elif winner == -1:
            return -100

        # c1 is for MAX and c2 is for MIN
        c1, c2 = 0, 0
        
        # For rows
        for row in board:
            if row.count(-1) == 0:
                c1 += 1
            if row.count(1) == 0:
                c2 += 1
        
        # For columns
        for j in range(4):
            col = [board[i][j] for i in range(4)]
            if col.count(-1) == 0:
                c1 += 1
            if col.count(1) == 0:
                c2 += 1

        # For diagonals
        diagonal1 = [board[i][i] for i in range(4)]
        if diagonal1.count(-1) == 0:
            c1 += 1
        if diagonal1.count(1) == 0:
            c2 += 1

        diagonal2 = []
        i, j = 3, 0
        for _ in range(4):
            diagonal2.append(board[i][j])
            i -= 1
            j += 1
        if diagonal2.count(-1) == 0:
            c1 += 1
        if diagonal2.count(1) == 0:
            c2 += 1

        self.print_board(board)
        print(c1, c2)
        return c1 - c2

    def alphabeta(self, board, depth, alpha, beta, maximizing_player, player):
        valid_moves = self.get_valid_moves(board)
        print(f"DEPTH = {depth}, player = {player} {maximizing_player}")
        print(f"alpha = {alpha}, beta = {beta}")
        print()

        if depth == 0 or not valid_moves:
            print("RETURNING FROM HORIZON...")
            return self.evaluate(board, player), None

        best_move = None
        if maximizing_player:
            max_eval = -math.inf
            for move in valid_moves:
                new_board = copy.deepcopy(board)
                new_board[move[0]][move[1]] = player
                self.print_board(new_board)
                eval_score, _ = self.alphabeta(new_board, depth - 1, alpha, beta, False, player * -1)

                print(eval_score)
                if eval_score > max_eval:
                    max_eval = eval_score
                    best_move = move
                alpha = max(alpha, eval_score)
                if beta <= alpha:
                    break
            return max_eval, best_move
        else:
            min_eval = math.inf
            for move in valid_moves:
                new_board = copy.deepcopy(board)
                new_board[move[0]][move[1]] = player
                self.print_board(new_board)
                eval_score, _ = self.alphabeta(new_board, depth - 1, alpha, beta, True, player * -1)

                print(eval_score)
                if eval_score < min_eval:
                    min_eval = eval_score
                    best_move = move
                beta = min(beta, eval_score)
                if beta <= alpha:
                    break
            return min_eval, best_move

"""
+--------+
| INPUTS |
+--------+
"""

# Value of k (number of plies)
depth = 2

# Values of alpha and beta
alpha = -1000
beta = 1000

# Board
board = [
     1,  0, -1,  0,
     0,  0, -1, -1,
     0,  0,  0,  0,
     0,  0,  1,  0 
         ]

# If it is MAX's move
maximizing_player = True

# If MAX is playing, set it to 1
player = 1

game = TicTacToe(board)
game.display_board()
print(game.check_win(game.board, player))

start_time = time.time()
score, best_move = game.alphabeta(game.board, depth, alpha, beta, maximizing_player, player)
if best_move:
    print(f"Best move for '{player}': {best_move} with score {score}")
    game.display_board()
player = 1 - player

end_time = time.time()
print(f"AlphaBeta total execution time: {end_time - start_time:.2f} seconds\n")
