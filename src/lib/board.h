#pragma once

#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <optional>

#include "helper.h"
#include "history.h"
#include "piece_group.h"
#include "player.h"
#include "position.h"
#include "shatranc_piece.h"
#include "types.h"

namespace shatranj
{
class PieceGroup;
class Player;
class Piece;
class Position;
class MoveHistory;

class Board : public std::enable_shared_from_this<Board>
{
  public:
    struct BoardRepresantation
    {
        static char *GetBoardReprensentation(Board *board);

        constexpr inline static char GetPieceFromCoordinate(char* board_repr, uint8_t xpos, uint8_t ypos)
        {
            return board_repr[CombinedCoordinate(xpos, ypos)];
        }

      private:
        constexpr inline static uint8_t CombinedCoordinate(uint8_t xpos, uint8_t ypos)
        {
            return xpos + 8 * ypos;
        }
    };
    friend struct BoardRepresantation;
    Board(const std::string &name1, const std::string &name2);

    const Player &GetPlayer(Color color) const;

    // Implement move logic functions here (similar to the provided ShatranjPiece functions)
    void MoveSuccesful(const Piece &piece, const Position &frompos, const Position &topos);
    bool MovePiece(Position frompos, Position topos);
    bool Revert(int move_count = 1);
    static Piece PromotePiyade(Piece &piyade);
    static Piece DemotePromoted(Piece &promoted);
    bool WouldBeInCheck(const Movement &controlling);
    bool OpponnentCanCapturePos(const Position &pos);
    bool IsCheck(Color color);
    GameState GetBoardState();
    std::optional<Player> Winner();
    Player Opponent(const Color &pColor);
    void SwitchTurn();
    bool IsPathClear(const Position &from, const Position &target);
    bool IsUnderAttack(int posx, int posy, Player *player); // not used yet
    bool Play(const std::string &input);
    bool Play(const Movement &input);
    bool Play(const std::string &from_pos, const std::string &to_pos);
    std::string BoardToString() const;
    double EvaluateBoard(Color color);
    std::variant<double, Movement> MinimaxSearch(std::optional<Movement> playing_move, int &nodesvisited, int depth = 5,
                                                 Color maximizingColor = Color::kWhite, bool randomize = true,
                                                 double alpha = std::numeric_limits<double>::min(),
                                                 double beta = std::numeric_limits<double>::max());
    std::variant<double, Movement> PickOrEvaluate(std::optional<Movement> playing_move_opt, int &nodesvisited,
                                                  int depth, Color maximizingColor, bool randomize, double alpha,
                                                  double beta);
    std::shared_ptr<PieceGroup> &GetPieces()
    {
        return pieces_;
    }
    void PrintValidMoves();
    bool AddPiece(Piece piece);
    std::string GenerateFEN(bool includeCounters = true) const;
    void ApplyFEN(const std::string &fen);

    void RemovePiece(const Position &pos);
    const Player &GetCurrentPlayer() const
    {
        return GetPlayer(currentTurn_);
    }
    Color GetCurrentTurn() const
    {
        return currentTurn_;
    }
    std::shared_ptr<Board> GetSharedFromThis()
    {
        return shared_from_this();
    }

    friend std::ostream &operator<<(std::ostream &ostr, const Board &board)
    {
        ostr << board.BoardToString();
        return ostr;
    }

    const std::vector<Player> &GetPlayers() const
    {
        return players_;
    }

    int GetHalfMoveClock() const
    {
        return halfMoveClock_;
    }

    int GetFullMoveNumber() const
    {
        return fullMoveNumber_;
    }

    MoveHistory &GetHistory() const;

  private:
    std::shared_ptr<PieceGroup> pieces_;
    std::vector<Player> players_;
    std::unique_ptr<MoveHistory> history_;
    Color currentTurn_;
    int halfMoveClock_ = 0;
    int fullMoveNumber_ = 1;
    KeyBasedMemory<std::string, GameState> boardStateMemory_;
    KeyBasedMemory<std::string, bool> wouldBeInCheckMemory_;
    constexpr static inline bool kDebug = kBoardDebug;
};
} // namespace shatranj