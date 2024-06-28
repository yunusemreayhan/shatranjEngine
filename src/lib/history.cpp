#include "history.h"
#include "types.h"

namespace shatranj
{
HistoryPoint::HistoryPoint(Position frompos, Position topos, std::unique_ptr<Piece> captured, bool promoted,
                           Color color)
    : from(frompos), to(topos), captured(std::move(captured)), promoted(promoted), color(color)
{
}
} // namespace shatranj