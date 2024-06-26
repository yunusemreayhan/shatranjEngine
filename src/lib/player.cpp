#include "player.h"
#include "position.h"

namespace shatranj
{
std::shared_ptr<PieceGroup> &Player::GetPieces()
{
    return pieces_;
}
}