#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);
vector<string> split(const string &);

#define DEBUG(x) cout << "Line: " << __LINE__ << ": " << x

// Square NxN board, win with N consecutive.
class TicTacToe {
public:
  typedef int Player;
  enum class MoveResult { WIN, INVALID, DRAW, CONTINUE, NUM_MOVE_RESULT };

  struct Location {
    Location(int row, int col) : row(row), col(col) {}
    const int row;
    const int col;
  };

  TicTacToe(int boardSize, int numberPlayers);

  ///
  /// \brief MakeMove Interface for the game playing system to add a new move to
  /// the game.
  /// \param player   Player making this move.
  /// \param location The selected location on the board.
  /// \return Result of the move, including the new game status such as Win,
  /// Invalid, etc.
  ///
  MoveResult MakeMove(Player player, Location location);

  int ConvertMoveResultToGameResult(MoveResult result, Player player);

  // Game status definitions.
  const int CATS_GAME;
  static constexpr int NEXT_PLAYER = 0;

private:
  /// Create a representation of the game state and any internal structures to
  /// help determine win conditions. The implementation should easily extend to
  /// different board sizes and run time should scale linearly (or better) with
  /// N.

  /// \brief Storage for board.
  typedef vector<vector<Player>> board_t;
  board_t board;

  MoveResult CheckForWin(board_t board, Location location, Player player);

  // Counter for the number of valid moves made in the game.
  unsigned int valid_move_count = 0;

  /// \brief Board size.
  const int board_size;
  const int num_players;
  const int max_valid_moves;

  // Board data definitions.
  const Player NO_MOVE = 0;

  // Keep track of whose turn it is.
  int whose_turn;
};

TicTacToe::TicTacToe(int boardSize, int numberPlayers)
    : CATS_GAME(numberPlayers + 1), board_size(boardSize),
      num_players(numberPlayers), max_valid_moves(boardSize * boardSize) {
  // Initialize board to reflect that no players have played yet.
  const vector<int> initial_row(board_size, NO_MOVE);
  for (int i = 0; i < board_size; ++i) {
    board[i] = initial_row;
  }

  // Set who must go first.  Player 1 goes first.  Note that players are
  // 1-indexed.
  whose_turn = 1;
}

TicTacToe::MoveResult TicTacToe::CheckForWin(board_t board, Location location,
                                             Player player) {
  // Entire row, column, or diagonal must be filled for WIN. Iterate through
  // each for the given <location> to check. Once a bad entry is found, we know
  // that win isn't possible.
  bool row_win = true;
  bool col_win = true;
  // For a diagonal win, <location> must be on either main diagonal (forwards or
  // backwards).
  bool diag_win = (location.row == location.col) ||
                  (location.row == (board_size - location.col - 1));
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
    if (diag_win) {
      diag_win = board[idx][location.col] == player;
    }
    // If no win type is still possible, we can return early.
    if (!(row_win || col_win || diag_win)) {
      return MoveResult::CONTINUE;
    }
  }
  // If we've iterated through without exiting, it's a win!
  return MoveResult::WIN;
}

///
/// \brief MakeMove Interface for the game playing system to add a new move to
/// the game.
/// \param player   Player making this move.
/// \param location The selected location on the board.
/// \return Result of the move, including the new game status such as Win,
/// Invalid, etc.
///
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
  bool already_filled = board[location.row][location.col] != NO_MOVE;

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
    move_result = CheckForWin(board, location, player);
  }

  // TODO error-check move_result != NUM_MOVE_RESULT.
  return move_result;
}

int TicTacToe::ConvertMoveResultToGameResult(MoveResult result, Player player) {
  // Process the result
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
    // TODO std::throw?
  }
  }
  // Should never reach here.
  // TODO std::throw?
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

int main()
{
  cout << "hello";
    ifstream inputstream("sample_input.txt");
    string boardSize_temp;
    getline(inputstream, boardSize_temp);

    int boardSize = stoi(ltrim(rtrim(boardSize_temp)));
  
    string numberPlayers_temp;
    getline(inputstream, numberPlayers_temp);
  
    int numberPlayers = stoi(ltrim(rtrim(numberPlayers_temp)));

    string moves_rows_temp;
    getline(inputstream, moves_rows_temp);

    int moves_rows = stoi(ltrim(rtrim(moves_rows_temp)));

    string moves_columns_temp;
    getline(inputstream, moves_columns_temp);

    int moves_columns = stoi(ltrim(rtrim(moves_columns_temp)));

    vector<vector<int>> moves(moves_rows);

    for (int i = 0; i < moves_rows; i++) {
        moves[i].resize(moves_columns);

        string moves_row_temp_temp;
        getline(inputstream, moves_row_temp_temp);

        vector<string> moves_row_temp = split(rtrim(moves_row_temp_temp));

        for (int j = 0; j < moves_columns; j++) {
            int moves_row_item = stoi(moves_row_temp[j]);

            moves[i][j] = moves_row_item;
        }
    }

    TicTacToe game(boardSize, numberPlayers);
    vector<int> result = playTicTacToe(game, moves);

    for (int i = 0; i < result.size(); i++) {
        cout << result[i];

        if (i != result.size() - 1) {
            cout << "\n";
        }
    }

    cout << "\n";

    // cout.close();

    return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))
    );

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end()
    );

    return s;
}

vector<string> split(const string &str) {
    vector<string> tokens;

    string::size_type start = 0;
    string::size_type end = 0;

    while ((end = str.find(" ", start)) != string::npos) {
        tokens.push_back(str.substr(start, end - start));

        start = end + 1;
    }

    tokens.push_back(str.substr(start));

    return tokens;
}
