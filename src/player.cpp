#include "player.h"
#include "position.h"


std::shared_ptr<PieceGroup> &Player::GetPieces()
{
    return pieces_;
}