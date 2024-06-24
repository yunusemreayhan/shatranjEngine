#include "player.h"
#include "position.h"

namespace Shatranj
{
std::shared_ptr<PieceGroup> &Player::GetPieces()
{
    return pieces_;
}
}