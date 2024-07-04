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

class Board
{
  public:
    virtual ~Board() = default;
    Board();

    const Player &GetPlayer(Color color) const;

    // Implement move logic functions here (similar to the provided ShatranjPiece functions)
    void MoveSuccesful(const PiecePrimitive &piece, const Position &frompos, const Position &topos);
    bool MovePiece(Position frompos, Position topos);
    bool Revert(int move_count = 1);
    static Piece PromotePiyade(Piece &piyade);
    static Piece DemotePromoted(Piece &promoted);
    static PiecePrimitive PromotePiyade(PiecePrimitive &piyade);
    static PiecePrimitive DemotePromoted(PiecePrimitive &promoted);
    bool WouldBeInCheck(const Movement &controlling);
    bool OpponnentCanCapturePos(const Position &pos);
    bool IsCheck(Color color);
    GameState GetBoardState();
    std::optional<Player> Winner();
    Player Opponent(const Color &pColor);
    void SwitchTurn();
    bool IsPathClear(const Position &from, const Position &target);
    bool Play(const Movement &input);
    bool PlayWithoutIsCheckControl(const Movement &input);
    std::string BoardToString();
    double EvaluateBoard(Color color);
    bool CanPawnCapture(ChessPieceEnum pieceType, const Position &frompos, const Position &topos);
    bool CollisionCheck(ChessPieceEnum pieceType, const Position &frompos, const Position &topos);
    std::vector<Movement> GetPossibleMoves(Position frompos, ChessPieceEnum pieceType, Color color);
    bool CanGo(Position frompos, Position pos, ChessPieceEnum pieceType);
    bool CanJumpOrPathClear(Position frompos, Position topos, ChessPieceEnum pieceType);
    std::variant<double, Movement> MinimaxSearch(const std::optional<Movement> &playing_move, int *nodesvisited,
                                                 double alpha, double beta, int depth = 5,
                                                 Color maximizingColor = Color::kWhite, bool randomize = true);
    std::variant<double, Movement> PickOrEvaluate(const std::optional<Movement> &playing_move, int *nodesvisited,
                                                  double &alpha, double &beta, int depth = 5,
                                                  Color maximizingColor = Color::kWhite, bool randomize = true);
    std::shared_ptr<PieceGroup> &GetPieces()
    {
        return pieces_;
    }
    const std::shared_ptr<PieceGroup> &GetPieces() const
    {
        return pieces_;
    }
    void PrintValidMoves();
    bool AddPiece(const PiecePrimitive &piece, const Position &pos);
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

    friend std::ostream &operator<<(std::ostream &ostr, Board &board)
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

    const std::vector<Movement> GetPossibleMoves(Color color);
    const std::vector<Movement> GetPossibleMovesCalcOpponentToo(Color color);
    const std::vector<Movement> GetPossibleCheckMoves(Color color);
    std::pair<Movement, bool> LookForCheckMateMoveDfs(int depth, Color colorForCheckMate, int *totalnodes,
                                                      std::vector<Movement> &moves_so_far,
                                                      std::optional<const Movement> playing_move = std::nullopt);
    bool IsCheckAfterMove(const Movement &Movement);

  private:
    std::shared_ptr<PieceGroup> pieces_;
    std::vector<Player> players_;
    std::unique_ptr<MoveHistory> history_;
    Color currentTurn_;
    int halfMoveClock_ = 0;
    int fullMoveNumber_ = 1;
    constexpr static inline bool kDebug = kBoardDebug;
};
} // namespace shatranj
