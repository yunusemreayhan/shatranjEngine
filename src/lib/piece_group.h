#pragma once

#include "board.h"
#include "helper.h"
#include "position.h"
#include "shatranc_piece.h"
#include "types.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sys/types.h>

namespace shatranj
{

class Piece;
enum class ChessPieceEnum : uint8_t;
class Board;
enum class Color : uint8_t;

class PieceGroup
{
  public:
    PieceGroup()
    {
        pieces_ = std::vector<Piece>();
        pieces_.reserve(32);
    }
    using iterator = std::vector<Piece>::iterator;
    using cons_iterator = std::vector<Piece>::const_iterator;
    using reverse_iterator = std::vector<Piece>::reverse_iterator;
    using const_reverse_iterator = std::vector<Piece>::const_reverse_iterator;

    bool AddPiece(const Piece &piece);
    void RemovePiece(const Position &pos);
    bool HasPiece(const Position &pos);
    bool MovePiece(const Position &frompos, const Position &topos);
    std::optional<Piece> GetPieceByVal(const Position &pos);
    std::vector<Piece> GetSubPieces(Color color);
    std::set<Position> GetPositions()
    {
        return positions_;
    }

    size_t size() const
    {
        return pieces_.size();
    }

    bool empty() const
    {
        return pieces_.empty();
    }

    void clear()
    {
        pieces_.clear();
        positions_.clear();
    }

    bool is_all_instance_of(ChessPieceEnum chessPiece) const;

    Piece &get(size_t index);

    const std::vector<Movement> &GetPossibleMoves(Color color, const std::shared_ptr<Board> &board);

  private:
    std::optional<Piece *> GetPiece(const Position &pos, bool check = true);
    std::vector<Piece> pieces_;
    std::set<Position> positions_;
    KeyBasedMemory<std::string, std::vector<Movement>> possibleMovesMemory_;
    constexpr static bool inline kDebug = kDebugGlobal;
};
} // namespace shatranj