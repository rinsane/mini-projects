import math
import time
import copy

class Othello:
    def __init__(self, depth):
        self.board = [[-1 for _ in range(8)] for _ in range(8)]
        self.board[3][3], self.board[4][4] = 0, 0
        self.board[3][4], self.board[4][3] = 1, 1
        self.depth = depth

    def display_board(self):
        for row in self.board:
            print(' '.join(str(cell) if cell != -1 else '.' for cell in row))
        print()

    def get_valid_moves(self, board, player):
        opponent = 1 - player
        directions = [(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)]
        valid_moves = []

        for i in range(8):
            for j in range(8):
                if board[i][j] != -1:
                    continue
                for dx, dy in directions:
                    x, y = i + dx, j + dy
                    found_opponent = False
                    while 0 <= x < 8 and 0 <= y < 8 and board[x][y] == opponent:
                        x += dx
                        y += dy
                        found_opponent = True
                    if found_opponent and 0 <= x < 8 and 0 <= y < 8 and board[x][y] == player:
                        valid_moves.append((i, j))
                        break

        return valid_moves

    def make_move(self, board, move, player):
        opponent = 1 - player
        directions = [(-1, -1), (-1, 0), (-1, 1), (0, -1), (0, 1), (1, -1), (1, 0), (1, 1)]
        x, y = move
        board[x][y] = player

        for dx, dy in directions:
            i, j = x + dx, y + dy
            flips = []
            while 0 <= i < 8 and 0 <= j < 8 and board[i][j] == opponent:
                flips.append((i, j))
                i += dx
                j += dy
            if flips and 0 <= i < 8 and 0 <= j < 8 and board[i][j] == player:
                for flip_x, flip_y in flips:
                    board[flip_x][flip_y] = player

    def evaluate(self, board, player):
        opponent = 1 - player
        player_score = sum(row.count(player) for row in board)
        opponent_score = sum(row.count(opponent) for row in board)
        return player_score - opponent_score

    def init_minimax(self, board, maximizing_player, player, alpha=None, beta=None):
        return self.minimax(board, self.depth, maximizing_player, player, alpha, beta)

    def minimax(self, board, depth, maximizing_player, player, alpha=None, beta=None):
        if depth == 0 or not self.get_valid_moves(board, 1) and not self.get_valid_moves(board, 0):
            return self.evaluate(board, player), None

        valid_moves = self.get_valid_moves(board, player if maximizing_player else (1 - player))
        if not valid_moves:
            return self.minimax(board, depth - 1, not maximizing_player, player, alpha, beta)

        best_move = None
        if maximizing_player:
            max_eval = -math.inf
            for move in valid_moves:
                new_board = copy.deepcopy(board)
                self.make_move(new_board, move, player)
                eval_score, _ = self.minimax(new_board, depth - 1, False, player, alpha, beta)
                if eval_score > max_eval:
                    max_eval = eval_score
                    best_move = move
                if alpha is not None:
                    alpha = max(alpha, eval_score)
                    if beta <= alpha:
                        break
            return max_eval, best_move
        else:
            min_eval = math.inf
            for move in valid_moves:
                new_board = copy.deepcopy(board)
                self.make_move(new_board, move, 1 - player)
                eval_score, _ = self.minimax(new_board, depth - 1, True, player, alpha, beta)
                if eval_score < min_eval:
                    min_eval = eval_score
                    best_move = move
                if beta is not None:
                    beta = min(beta, eval_score)
                    if beta <= alpha:
                        break
            return min_eval, best_move

# Black = 1 ; White = 0
# Black plays with a depth-limited minimax
depth = 4

# Example usage
game = Othello(depth)
game.display_board()

# Timing AlphaBeta
print("AlphaBeta:")
player = 0
start_time = time.time()
for _ in range(60):
    score, best_move = game.init_minimax(game.board, True, player, -math.inf, math.inf)
    if best_move:
        print(f"Best move for '{player}': {best_move} with score {score}")
        game.make_move(game.board, best_move, player)
        # game.display_board()
    player = 1 - player
end_time = time.time()
print(f"AlphaBeta total execution time: {end_time - start_time:.2f} seconds\n")

# Reset the board
game = Othello(depth)
game.display_board()

# Timing Minimax
print("Minimax:")
player = 0
start_time = time.time()
for _ in range(60):
    score, best_move = game.init_minimax(game.board, True, player)
    if best_move:
        print(f"Best move for '{player}': {best_move} with score {score}")
        game.make_move(game.board, best_move, player)
        # game.display_board()
    player = 1 - player
end_time = time.time()
print(f"Minimax total execution time: {end_time - start_time:.2f} seconds")
