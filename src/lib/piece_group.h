#pragma once

#include "board.h"
#include "helper.h"
#include "position.h"
#include "shatranc_piece.h"
#include "types.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <sys/types.h>

namespace shatranj
{

class Piece;
class PiecePrimitive;
enum class ChessPieceEnum : uint8_t;
class Board;
enum class Color : uint8_t;

class PieceGroup
{
  public:
    PieceGroup();
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
    static uint8_t Coord2to1(Position pos)
    {
        return pos.Getx() + 8 * pos.Gety();
    }
    static Position Coord1to2(uint8_t coord)
    {
        return Position(coord % 8, coord / 8);
    }

    size_t Size() const
    {
        return counter_;
    }

    bool Empty() const
    {
        return counter_ == 0;
    }

    void Clear();

    bool IsAllInstanceOf(ChessPieceEnum chessPiece) const;

    std::optional<Piece> Get(size_t index);

    const std::vector<Movement> &GetPossibleMoves(Color color, const std::shared_ptr<Board> &board);

    constexpr inline static size_t kSquareCount = 64;

  private:
    void RemovePieceNoCounterUpdate(const Position &pos);
    Piece FromPiecePrimitive(std::vector<PiecePrimitive>::iterator primitr);
    std::optional<PiecePrimitive *> GetPiece(const Position &pos);
    std::vector<PiecePrimitive> pieces_primitive_;
    KeyBasedMemory<std::string, std::vector<Movement>> possibleMovesMemory_;
    constexpr static bool inline kDebug = kDebugGlobal;
    size_t counter_ = 0;
    size_t black_count_ = 0;
    size_t white_count_ = 0;
};
} // namespace shatranj