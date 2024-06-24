#include "board.h"
#include "position.h"
#include <optional>

namespace Shatranj
{
// would be in check if I move piece to pos
bool Board::WouldBeInCheck(const std::shared_ptr<Piece> &piece, Position pos)
{
    /*
        def wouldBeInCheck(self, piece, x, y):
            original_x, original_y = piece.x, piece.y
            capturedPiece = self.getCell( x, y )
            if capturedPiece:
                capturedPiece.player.pieces.remove( capturedPiece )
                self.pieces.remove( capturedPiece )

            piece.x, piece.y = x, y
            inCheck = self.isCheck(piece.player)

            piece.x, piece.y = original_x, original_y
            if capturedPiece:
                capturedPiece.player.pieces.append( capturedPiece )
                self.pieces.append( capturedPiece )

            return inCheck
     */
    auto player_sp = piece->GetPlayer().lock();
    if (!player_sp)
    {
        return false;
    }
    const auto captured_piece = GetPieces()->GetPiece(pos);
    if (captured_piece)
    {
        const auto &captured_piece_sp = (*captured_piece);
        if (auto captured_player_sp = captured_piece_sp->GetPlayer().lock())
        {
            captured_player_sp->GetPieces()->RemovePiece(pos);
        }
        // if target location have a piece we will replace it with our piece
        // remove from user's pieces
        // remove from board's pieces
        GetPieces()->RemovePiece(pos);
    }

    bool ret_is_check = IsCheck(player_sp);

    if (captured_piece)
    {
        const auto &captured_piece_sp = (*captured_piece);
        // TODO(yunus): put back the captured piece after check

        if (auto captured_player_sp = captured_piece_sp->GetPlayer().lock())
        {
            // put back to user's pieces
            const auto &captured_piece_sp = (*captured_piece);
            if (auto captured_player_sp = captured_piece_sp->GetPlayer().lock())
            {
                captured_player_sp->GetPieces()->AddPiece(captured_piece_sp);
            }

            // put back to board's pieces
            GetPieces()->AddPiece(captured_piece_sp);
        }
    }

    return ret_is_check;
}

bool Board::IsCheck(std::shared_ptr<Player> &player) const
{
    return false;
}

} // namespace Shatranj