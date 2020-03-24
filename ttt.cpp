#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);
vector<string> split(const string &);

#define DEBUG(x) cout << "Line: " << __LINE__ << ": " << x

// Square NxN board, win with N consecutive.

/// \brief Class to play Tic Tac Toe.
class TicTacToe {
 public:
  typedef int Player;

  /// \brief Defines the result of a single move.
  enum class MoveResult { WIN, INVALID, DRAW, CONTINUE, NUM_MOVE_RESULT };

  /// \brief Defines a location on the board.
  struct Location {
    Location(int row, int col) : row(row), col(col) {}
    const int row;
    const int col;
  };

  /// \brief Constructor.
  /// \param boardSize Board size for this game.  boardSize N -> NxN board.
  /// \param numPlayers The number of players who will participate.
  TicTacToe(int boardSize, int numberPlayers);

  /// \brief MakeMove Interface for the game playing system to add a new move to
  /// the game.
  /// \param player   Player making this move.
  /// \param location The selected location on the board.
  /// \return Result of the move, including the new game status such as Win,
  /// Invalid, etc.
  MoveResult MakeMove(Player player, Location location);

  /// \brief Convert a move result into an actual game result.
  /// \param result The move result.
  /// \param player The player making the move.
  int ConvertMoveResultToGameResult(MoveResult result, Player player);

  /// \brief Print the current game board.
  void Print();

  // Game status definitions.
  static constexpr int NEXT_PLAYER = 0;
  const int CATS_GAME;

 private:
  typedef vector<vector<Player>> board_t;

  /// \brief Check if \p player just won playing at \location.
  /// \param location \p player's most recent move.
  /// \param player The player of this turn only.
  /// \return MoveResult::WIN or MoveResult::CONTINUE.
  ///
  /// This function only returns WIN or CONTINUE. It assumes that only those two
  /// possibilities are left / all error-check has already been performed.
  MoveResult CheckForWin(Location location, Player player);

  /// \brief Structure to store current game state.
  board_t board;

  /// \brief Counter for the number of valid moves made in the game.
  unsigned int valid_move_count = 0;

  /// \brief Board size.
  const int board_size;

  /// \brief The number of players.
  const int num_players;

  /// \brief The maximum number of total moves by all players combined.
  const int max_valid_moves;

  // Board data definitions.
  const Player NO_MOVE = 0;

  // Keep track of whose turn it is.
  int whose_turn;
};

TicTacToe::TicTacToe(int boardSize, int numberPlayers)
    : CATS_GAME(numberPlayers + 1),
      board_size(boardSize),
      num_players(numberPlayers),
      max_valid_moves(boardSize * boardSize) {
  // Initialize board to reflect that no players have played yet.
  const vector<int> initial_row(board_size, NO_MOVE);
  for (int i = 0; i < board_size; ++i) {
    board.push_back(initial_row);
  }

  // Set who must go first.  Player 1 goes first.  Note that players are
  // 1-indexed.
  whose_turn = 1;
}

void TicTacToe::Print() {
  for (const auto row : board) {
    for (const auto col : row) {
      cout << col << " ";
    }
    cout << "\n";
  }
}

TicTacToe::MoveResult TicTacToe::CheckForWin(Location location, Player player) {
  // Entire row, column, or diagonal must be filled for WIN. Iterate through
  // each for the given <location> to check. Once a bad entry is found, we know
  // that win isn't possible.
  bool row_win = true;
  bool col_win = true;
  // For a diagonal win, <location> must be on either main diagonal (forwards or
  // backwards).
  bool diag_win_down = (location.row == location.col);
  bool diag_win_up = (location.row == (board_size - location.col - 1));
  for (int idx = 0; idx < board_size; ++idx) {
    // Check if row win type is possible.
    if (row_win) {
      row_win = board[location.row][idx] == player;
    }
    // Check if col..
    if (col_win) {
      col_win = board[idx][location.col] == player;
    }
    // Check if diag..
    if (diag_win_down) {
      diag_win_down = board[idx][idx] == player;
    }
    if (diag_win_up) {
      diag_win_up = board[idx][board_size - idx - 1] == player;
    }
    // If no win type is still possible, we can return early.
    if (!(row_win || col_win || diag_win_down || diag_win_up)) {
      return MoveResult::CONTINUE;
    }
  }
  // If we've iterated through without exiting, it's a win!
  return MoveResult::WIN;
}

TicTacToe::MoveResult TicTacToe::MakeMove(Player player, Location location) {
  // Check for invalid player moves.
  // * Wrong player has attempted to move.
  const bool wrong_player = (player != whose_turn);
  // TODO Should this be incremented if the wrong player attempted to move?
  {
    // Circular increment whose_turn. Note that whose_turn is 1-indexed.
    whose_turn = (whose_turn % num_players) + 1;
  }
  // * Location is off the board.
  const auto IsOffBoard = [this](const int idx) {
    return (idx < 0) || (idx >= this->board_size);
  };
  bool off_board = IsOffBoard(location.row) || IsOffBoard(location.col);
  // * Move is in an already-filled location.
  bool already_filled = (board[location.row][location.col] != NO_MOVE);

  MoveResult move_result = MoveResult::NUM_MOVE_RESULT;
  if (wrong_player || off_board || already_filled) {
    move_result = MoveResult::INVALID;
  } else if (valid_move_count == max_valid_moves) {
    // We've made all the valid moves possible, so the board is filled in. Draw!
    move_result = MoveResult::DRAW;
  } else {
    ++valid_move_count;
    board[location.row][location.col] = player;
    // Returns WIN or CONTINUE.
    move_result = CheckForWin(location, player);
  }

  if (move_result == MoveResult::NUM_MOVE_RESULT) {
    throw std::logic_error(
        "Invalid result NUM_MOVE_RESULT. Should never reach here.");
  }
  return move_result;
}

int TicTacToe::ConvertMoveResultToGameResult(MoveResult result, Player player) {
  switch (result) {
    case TicTacToe::MoveResult::WIN: {
      return player;
    }
    case TicTacToe::MoveResult::DRAW: {
      return CATS_GAME;
    }
    case TicTacToe::MoveResult::INVALID: {
      return -player;
    }
    case TicTacToe::MoveResult::CONTINUE: {
      return NEXT_PLAYER;
    }
    case TicTacToe::MoveResult::NUM_MOVE_RESULT: {
      throw std::logic_error(
          "Invalid result NUM_MOVE_RESULT. Should never reach here.");
    }
  }
  throw std::logic_error("Something went wrong. Should never reach here.");
  return 0;
}

/*
 * Complete the 'playTicTacToe' function below.
 *
 * The function is expected to return an INTEGER_ARRAY.
 *
 * The function accepts following parameters:
 *  1. TicTacToe& game object
 *  2. 2D_INTEGER_ARRAY moves - moves[i][0] = player for move i,
 *                              moves[i][1] = row for move i,
 *                              moves[i][2] = column for move i
 */

vector<int> playTicTacToe(TicTacToe &game, vector<vector<int>> moves) {
  vector<int> game_statuses;
  for (const auto &move : moves) {
    const int player = move[0];
    const TicTacToe::Location location(move[1], move[2]);
    const TicTacToe::MoveResult result = game.MakeMove(player, location);
    const int game_status = game.ConvertMoveResultToGameResult(result, player);
    game_statuses.push_back(game_status);
    if (game_status > TicTacToe::NEXT_PLAYER) {
      // Win or draw occurred, return early.
      break;
    }
  }
  return game_statuses;
}

int main() {
  int boardSize = 5;
  int numberPlayers = 3;
  vector<vector<int>> moves;
  moves.push_back({1, 1, 0});
  moves.push_back({2, 3, 3});
  moves.push_back({3, 1, 3});
  moves.push_back({1, 0, 2});
  moves.push_back({2, 0, 0});
  moves.push_back({3, 2, 2});
  moves.push_back({1, 4, 1});
  moves.push_back({2, 4, 2});
  moves.push_back({3, 3, 1});
  moves.push_back({1, 1, 2});
  moves.push_back({2, 4, 3});
  moves.push_back({3, 2, 1});
  moves.push_back({1, 4, 4});
  moves.push_back({2, 1, 1});
  moves.push_back({3, 0, 4});
  moves.push_back({1, 0, 1});
  moves.push_back({2, 2, 3});
  moves.push_back({3, 4, 0});

  TicTacToe game(boardSize, numberPlayers);
  vector<int> result = playTicTacToe(game, moves);

  for (int i = 0; i < result.size(); i++) {
    cout << result[i];

    if (i != result.size() - 1) {
      cout << "\n";
    }
  }

  cout << "\n";

  game.Print();

  return 0;
}
