#include "board.h"
#include "helper.h"
#include "piece_group.h"
#include "position.h"
#include "shatranc_piece.h"
#include "shatranj.h"
#include "types.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <stdexcept>
#include <utility>

namespace shatranj
{

Board::Board(const std::string &name1, const std::string &name2)
    : pieces_(std::make_shared<PieceGroup>()), players_{{name1, Color::kWhite}, {name2, Color::kBlack}},
      history_(std::make_unique<MoveHistory>()), currentTurn_(Color::kWhite)
{
}
// would be in check if I move piece to pos
bool Board::WouldBeInCheck(const Movement &controlling)
{
    auto curkey = GenerateFEN(false) + controlling.ToString();
    if (wouldBeInCheckMemory_.Have(curkey))
    {
        return wouldBeInCheckMemory_.Get(curkey);
    }

    Color color = currentTurn_;
    auto temp = fullMoveNumber_;
    auto res = Play(controlling);

    if (!res)
    {
        std::cout << *this << std::endl;
        std::cout << "Error, can not control " << controlling.ToString() << std::endl;
    }

    bool ret_is_check = IsCheck(color);

    if (res)
    {
        Revert(1);
        wouldBeInCheckMemory_.Add(curkey, ret_is_check);
    }

    if (temp != fullMoveNumber_)
    {
        std::cout << *this << std::endl;
        std::cout << "Error, full move number changed " << controlling.ToString() << std::endl;
        throw std::runtime_error("Error, full move number changed");
    }

    return ret_is_check;
}

bool Board::OpponnentCanCapturePos(const Position &pos)
{
    for (size_t i = 0; i < PieceGroup::kSquareCount; i++)
    {
        auto curpieceopt = OpponentColor(GetPieces()->GetPiece(pos)->GetColor()) == Color::kBlack
                               ? GetPieces()->GetBlackPtr(i)
                               : GetPieces()->GetWhitePtr(i);
        if (!curpieceopt)
        {
            continue;
        }

        auto *curpiece = *curpieceopt;
        auto frompos = PieceGroup::Coord1to2(i);
        auto canmove = Piece::CanMove(frompos, pos, curpiece->GetPieceType()) &&
                       CollisionCheck(curpiece->GetPieceType(), frompos, pos);
        auto cancapture = Piece::CanPawnCapture(frompos, pos, curpiece->GetPieceType());

        if (canmove || cancapture)
            return true;
    }
    return false;
}

std::vector<Movement> Board::GetPossibleMoves(Position frompos, ChessPieceEnum pieceType, Color color)
{
    if (GetCurrentTurn() != color)
    {
        if constexpr (kGetPossibleDebug)
            std::cout << "GetPossibleMoves, can not get possible moves for " << frompos.ToString()
                      << " not same color with turn " << color << std::endl;
        return {};
    }
    std::vector<Movement> possible_moves;
    const auto &pos_moves = Piece::GetPreComputedMoveTable(pieceType, frompos);
    for (const auto &topos : pos_moves)
    {
        Movement tocheck(frompos, topos);
        if constexpr (kGetPossibleDebug)
            std::cout << "checking adding movement " << tocheck.ToString() << std::endl;
        if (frompos == topos)
        {
            if constexpr (kGetPossibleDebug)
                std::cout << "skipping adding movement " << tocheck.ToString() << " it is same" << std::endl;
            continue;
        }
        if (CollisionCheck(pieceType, frompos, topos))
        {
            if (!WouldBeInCheck(tocheck))
                possible_moves.push_back(tocheck);
            else if constexpr (kGetPossibleDebug)
            {
                std::cout << "skipping adding movement " << tocheck.ToString() << " would be in check" << std::endl;
            }
        }
        else if constexpr (kGetPossibleDebug)
        {
            std::cout << "skipping adding movement " << tocheck.ToString() << " collision" << std::endl;
        }
    }

    if (pieceType == ChessPieceEnum::kPiyadeBlack || pieceType == ChessPieceEnum::kPiyadeWhite)
    {
        const auto &cap_moves = Piece::GetPreComputedCaptureTable(pieceType, frompos);
        for (const auto &topos : cap_moves)
        {
            Movement tocheck(frompos, topos);
            if (frompos == topos)
            {
                if constexpr (kGetPossibleDebug)
                    std::cout << "skipping adding movement " << tocheck.ToString() << " it is same" << std::endl;
                continue;
            }
            if (CanPawnCapture(pieceType, frompos, topos))
            {
                if (!WouldBeInCheck(tocheck))
                    possible_moves.push_back(tocheck);
                else if constexpr (kGetPossibleDebug)
                {
                    std::cout << "skipping adding movement " << tocheck.ToString() << " would be in check" << std::endl;
                }
            }
            else if constexpr (kGetPossibleDebug)
            {
                std::cout << "skipping adding movement " << tocheck.ToString() << " collision" << std::endl;
            }
        }
    }
    return possible_moves;
}

bool Board::CanGo(Position frompos, Position pos, ChessPieceEnum pieceType)
{
    auto can_capture = Piece::CanPawnCapture(frompos, pos, pieceType);
    auto can_move = Piece::CanMove(frompos, pos, pieceType) && CollisionCheck(pieceType, frompos, pos);
    return can_capture || can_move;
}

bool Board::CanPawnCapture(ChessPieceEnum pieceType, const Position &frompos, const Position &topos)
{
    return Piece::CanPawnCapture(frompos, topos, pieceType) &&
           GetPieces()->GetPtr(topos, OpponentColor(currentTurn_)) != nullptr;
}

bool Board::CanJumpOrPathClear(Position frompos, Position topos, ChessPieceEnum pieceType)
{
    return Piece::CanJumpOverOthers(pieceType) || IsPathClear(frompos, topos);
}

bool Board::IsCheck(Color color)
{
    Position shahpos = color == Color::kBlack ? GetPieces()->GetBlackShahPos() : GetPieces()->GetWhiteShahPos();

    if (!shahpos.IsValid())
    {
        std::cout << *this << std::endl;
        throw std::runtime_error("Shah piece not found, logical error!");
    }

    return OpponnentCanCapturePos(shahpos);
}

bool Board::IsPathClear(const Position &from, const Position &target)
{
    int step_x = 0;
    int step_y = 0;
    if (from.Getx() < target.Getx())
    {
        step_x = 1;
    }
    else if (from.Getx() > target.Getx())
    {
        step_x = -1;
    }
    if (from.Gety() < target.Gety())
    {
        step_y = 1;
    }
    else if (from.Gety() > target.Gety())
    {
        step_y = -1;
    }
    const int cur_x = from.Getx() + step_x;
    const int cur_y = from.Gety() + step_y;
    Position cur(std::make_pair(cur_x, cur_y));
    while (cur != target)
    {
        if (GetPieces()->GetPiece(cur))
        {
            return false;
        }
        cur.Move(std::make_pair(step_x, step_y));
    }
    return true;
}

bool Board::MovePiece(Position frompos, Position topos)
{
    auto *piece = GetPieces()->GetPiece(frompos);
    if (currentTurn_ != piece->GetColor())
    {
        if constexpr (kDebugGlobal)
        {
            std::cout << "its turn of " << currentTurn_ << " not " << piece->GetColor() << std::endl;
        }
        return false;
    }

    const bool can_move =
        Piece::CanMove(frompos, topos, piece->GetPieceType()) && CollisionCheck(piece->GetPieceType(), frompos, topos);

    const bool can_capture = Piece::CanPawnCapture(frompos, topos, piece->GetPieceType());

    if (!can_move && !can_capture)
    {
        return false;
    }

    std::unique_ptr<Piece> captured_piece_uptr = nullptr;
    auto captured_piece = GetPieces()->GetPieceByVal(topos);
    if (captured_piece)
    {
        captured_piece_uptr = std::make_unique<Piece>(*captured_piece);
        if ((piece->IsPiyade() && can_capture) || (!piece->IsPiyade() && can_move))
            RemovePiece(topos);
    }

    bool promoted = false;
    GetPieces()->MovePiece(frompos, topos);
    piece = GetPieces()->GetPiece(topos);
    if (piece->IsPiyade())
    {
        if (topos.Gety() == 0 || topos.Gety() == 7)
        {
            auto promoted_piece = PromotePiyade(*piece);
            RemovePiece(topos);
            AddPiece(promoted_piece, topos);
            promoted = true;
        }
    }
    GetHistory().AddMove(frompos, topos, piece->GetPieceType(), std::move(captured_piece_uptr), promoted,
                         piece->GetColor());
    MoveSuccesful(*piece, frompos, topos);

    return true;
}

bool Board::Revert(int move_count)
{
    if constexpr (kDebug)
    {
        std::cout << "Reverting " << move_count << " moves" << std::endl;
    }
    if (move_count <= 0)
    {
        return true;
    }
    while (move_count > 0)
    {
        if (GetHistory().GetHistory().size() == 0)
        {
            if constexpr (kDebug)
            {
                std::cout << "No more moves to revert" << std::endl;
            }
            return false;
        }
        const std::unique_ptr<HistoryPoint> &last_move = GetHistory().GetLastMove();

        if constexpr (kDebug)
        {
            std::cout << "Reverting from " << last_move->to.ToString() << " to " << last_move->from.ToString()
                      << std::endl;
        }

        GetPieces()->MovePiece(last_move->to, last_move->from);
        auto *piece = GetPieces()->GetPiece(last_move->from);
        if (nullptr == piece)
        {
            if constexpr (kDebug)
            {
                std::cout << *this << std::endl;
            }
            std::cout << "Piece not found, illogical board state at " << last_move->to.ToString() << std::endl;
            throw std::runtime_error("Piece not found, illogical board state at " + last_move->to.ToString());
        }
        if (last_move->promoted)
        {
            PiecePrimitive demoted_piece = DemotePromoted(*piece);
            RemovePiece(last_move->from);
            AddPiece(demoted_piece, last_move->from);
        }

        if (last_move->captured != nullptr)
        {
            AddPiece(last_move->captured->GetPrimitive(), last_move->captured->GetPos());
        }
        if (last_move->color == Color::kBlack)
        {
            fullMoveNumber_--;
        }

        currentTurn_ = OpponentColor(currentTurn_);

        if constexpr (kDebug)
        {
            std::cout << "Revert done from " << last_move->to.ToString() << " to " << last_move->from.ToString()
                      << std::endl;
        }
        move_count--;
        if constexpr (kDebug)
        {
            std::cout << "Reverting " << move_count << " moves" << std::endl;
        }
        GetHistory().PopLastMove();
    }
    return true;
}

void Board::MoveSuccesful(const PiecePrimitive &piece, const Position & /*fromPos*/, const Position & /*toPos*/)
{
    // TODO(yunus) :  not sure if needed but last move textual iformation could be saved later
    if (piece.IsPiyade())
    {
        halfMoveClock_ = 0;
    }
    else
    {
        ++halfMoveClock_;
    }

    if (currentTurn_ == Color::kBlack)
    {
        fullMoveNumber_++;
    }
    SwitchTurn();
}

void Board::SwitchTurn()
{
    currentTurn_ = Color::kWhite == currentTurn_ ? Color::kBlack : Color::kWhite;
}

Piece Board::PromotePiyade(Piece &piyade)
{
    return Vizier(piyade.GetPos(), piyade.GetColor());
}
Piece Board::DemotePromoted(Piece &promoted)
{
    return Piyade(promoted.GetPos(), promoted.GetColor());
}

PiecePrimitive Board::PromotePiyade(PiecePrimitive &piyade)
{
    return PiecePrimitive(ChessPieceEnum::kVizier, piyade.GetColor(), piyade.IsMoved());
}
PiecePrimitive Board::DemotePromoted(PiecePrimitive &promoted)
{
    return PiecePrimitive(promoted.GetColor() == Color::kWhite ? ChessPieceEnum::kPiyadeWhite
                                                               : ChessPieceEnum::kPiyadeBlack,
                          promoted.GetColor(), promoted.IsMoved());
}

GameState Board::GetBoardState()
{
    GameState ret = GameState::kNormal;
    auto fenkey = GenerateFEN(false);
    if (boardStateMemory_.Have(fenkey))
    {
        return boardStateMemory_.Get(fenkey);
    }

    size_t current_turn_pieces_count;
    size_t opponent_pieces_count;

    if (currentTurn_ == Color::kWhite)
    {
        current_turn_pieces_count = GetPieces()->GetWhiteCount();
        opponent_pieces_count = GetPieces()->GetBlackCount();
    }
    else
    {
        current_turn_pieces_count = GetPieces()->GetBlackCount();
        opponent_pieces_count = GetPieces()->GetWhiteCount();
    }

    if (current_turn_pieces_count == 1 && opponent_pieces_count > 2)
    {
        ret = GameState::kCheckmate;
    }
    else if (current_turn_pieces_count == 1 && opponent_pieces_count == 1)
    {
        ret = GameState::kDraw;
    }
    else if (current_turn_pieces_count == 1 && opponent_pieces_count == 2)
    {
        auto current_turn_pieces = GetPieces()->GetSubPieces(currentTurn_);
        auto opponent_pieces = GetPieces()->GetSubPieces(OpponentColor(currentTurn_));
        if (CanGo(current_turn_pieces[0].GetPos(), opponent_pieces[0].GetPos(),
                  current_turn_pieces[0].GetPieceType()) ||
            CanGo(current_turn_pieces[1].GetPos(), opponent_pieces[0].GetPos(), current_turn_pieces[0].GetPieceType()))
        {
            ret = GameState::kDraw;
        }
        else
        {
            ret = GameState::kCheckmate;
        }
    }
    else
    {
        const auto &moves = GetPossibleMoves(currentTurn_);

        if (moves.empty())
        {
            if (IsCheck(currentTurn_))
            {
                ret = GameState::kCheckmate;
            }
            else
            {
                ret = GameState::kStalemate;
            }
        }
        else
        {
            if (IsCheck(currentTurn_))
            {
                ret = GameState::kCheck;
            }
        }
    }
    boardStateMemory_.Add(fenkey, ret);
    return ret;
}

std::optional<Player> Board::Winner()
{
    switch (GetBoardState())
    {
    case GameState::kCheckmate:
    case GameState::kStalemate:
        return Opponent(currentTurn_);
    default:
        return std::nullopt;
    }
    return std::nullopt;
}

Player Board::Opponent(const Color &color)
{
    return color == players_[1].GetColor() ? players_[0] : players_[1];
}

bool Board::Play(const std::string &input)
{
    if (kDebug)
        std::cout << "play: " << input << std::endl;
    std::optional<std::pair<std::string, std::string>> parsed_input = Shatranj::ParseInput(input);
    if (parsed_input)
    {
        const std::string &from_pos = parsed_input->first;
        const std::string &to_pos = parsed_input->second;
        if (from_pos.size() != 2 || to_pos.size() != 2)
        {
            std::cout << "Invalid input. Please enter a move in the format 'fromPos toPos'." << std::endl;
            return false;
        }

        return Play(from_pos, to_pos);
    }
    return false;
}

bool Board::Play(const Movement &input)
{
    if (!input.from.IsValid() || !input.to.IsValid())
    {
        std::cout << "Invalid input. Please enter a move in the format 'fromPos toPos'." << std::endl;
        return false;
    }

    auto *piece = pieces_->GetPiece(input.from);
    if (nullptr == piece || piece->GetColor() != currentTurn_)
    {
        return false;
    }

    return MovePiece(input.from, input.to);
}

bool Board::Play(const std::string &from_pos, const std::string &to_pos)
{
    if (GetBoardState() != GameState::kNormal && GetBoardState() != GameState::kCheck)
        return false;
    const auto checking_movemend = Movement(from_pos, to_pos);

    return !WouldBeInCheck(checking_movemend) && Play(checking_movemend);
}

std::string Board::BoardToString() const
{
    std::string ret;
    for (int8_t yitr = 7; yitr >= 0; yitr--)
    {
        if (yitr == 7)
        {
            ret += "  ";
            for (int8_t xitr = 0; xitr < 8; xitr++)
            {
                ret += static_cast<char>(xitr + 'a');
                ret += ' ';
            }
            ret += '\n';
        }

        ret += static_cast<char>('1' + yitr);
        ret += ' ';

        for (int8_t xitr = 0; xitr < 8; xitr++)
        {
            const auto &pos = Position(xitr, yitr);
            const auto *piece = GetPieces()->GetPiece(pos);
            if (piece == nullptr)
            {
                ret += '.';
            }
            else
            {
                ret += piece->GetSymbol();
            }
            ret += ' ';
        }
        if constexpr (kDebugTablePrint)
        {
            ret += static_cast<char>('0' + yitr);
            ret += ' ';
        }
        ret += '\n';

        if constexpr (kDebugTablePrint)
        {
            if (yitr == 0)
            {
                ret += "  ";
                for (int8_t xitr = 0; xitr < 8; xitr++)
                {
                    ret += static_cast<char>(xitr + '0');
                    ret += ' ';
                }
                ret += '\n';
            }
        }
    }

    ret += "  current turn : " + GetCurrentPlayer().GetName() + " color " +
           std::string(currentTurn_ == Color::kWhite ? "White which is uppercase" : "Black which is lowercase") + '\n';
    ret += "  current move count : " + std::to_string(fullMoveNumber_) + '\n';
    ret += "  half move count : " + std::to_string(halfMoveClock_) + '\n';
    ret += "  board FEN : " + GenerateFEN() + '\n';

    return ret;
}

const Player &Board::GetPlayer(Color color) const
{
    for (const auto &player : players_)
    {
        if (player.GetColor() == color)
        {
            return player;
        }
    }
    return players_[0];
}

bool Board::AddPiece(const PiecePrimitive &piece, const Position &pos)
{
    auto res = pieces_->AddPiece(piece, pos);
    if (!res)
    {
        std::cout << "Adding " << pos.ToString() << " failed" << std::endl;
        std::cout << *this << std::endl;
        return false;
    }

    return res;
}

void Board::RemovePiece(const Position &pos)
{
    if constexpr (kDebug)
    {
        std::cout << "Removing " << pos.ToString() << std::endl;
        std::cout << *this << std::endl;
    }
    pieces_->RemovePiece(pos);
    // piece.GetPlayer().lock()->GetPieces()->RemovePiece(piece);
    if constexpr (kDebug)
    {
        std::cout << "Removed " << pos.ToString() << std::endl;
        std::cout << *this << std::endl;
    }
}

void Board::PrintValidMoves()
{
    const auto &moves = GetPossibleMoves(currentTurn_);

    for (const auto &move : moves)
    {
        std::cout << move.ToString() << " ";
    }

    std::cout << std::endl;
}

MoveHistory &Board::GetHistory() const
{
    return *history_;
}

std::string Board::GenerateFEN(bool includeCounters) const
{
    std::string ret;
    for (int8_t yitr = 7; yitr >= 0; yitr--)
    {
        int spaces = 0;
        for (int8_t xitr = 0; xitr < 8; xitr++)
        {

            const auto &pos = Position(xitr, yitr);
            const auto *piece = GetPieces()->GetPiece(pos);
            if (piece == nullptr)
            {
                spaces++;
            }
            else
            {
                if (spaces != 0)
                {
                    ret += std::to_string(spaces);
                    spaces = 0;
                }
                ret += piece->GetSymbolOld();
            }
            if (xitr == 7)
            {
                if (spaces != 0)
                {
                    ret += std::to_string(spaces);
                    spaces = 0;
                }
            }
        }
        if (yitr != 0)
        {
            ret += '/';
        }
    }
    ret += " " + std::string(currentTurn_ == Color::kWhite ? "w" : "b");
    // ret += " KQkq"; // TODO add castling rights here, no castling in shatranj

    // if (history_->GetHistory().size() > 0)
    //{
    //  TODO here calculate if a pawn moved 2 squares and capturable point if so for example a pawn moved from e2 to
    //  e4 than e3 is capturable point, no 2 squares move in shatranj
    //}
    // ret += " -";
    if (includeCounters)
    {
        ret += " " + std::to_string(halfMoveClock_) + " " + std::to_string(fullMoveNumber_);
    }
    return ret;
}

void Board::ApplyFEN(const std::string &fen)
{
    // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w 0 1
    const auto &fen_pieces = SplitStringByChar(fen, ' ');

    if (fen_pieces.size() != 4) // TODO normally this is 6, but shatranj has 4
    {
        throw std::runtime_error("Invalid FEN");
    }

    const auto &piece_lines = SplitStringByChar(fen_pieces[0], '/');
    if (piece_lines.size() != 8)
    {
        throw std::runtime_error("Invalid FEN");
    }
    currentTurn_ = fen_pieces[1] == "w" ? Color::kWhite : Color::kBlack;
    halfMoveClock_ = std::stoi(fen_pieces[2]);
    fullMoveNumber_ = std::stoi(fen_pieces[3]);
    pieces_->Clear();
    for (uint8_t row = 0; row < 8; row++)
    {
        uint8_t col = 7;
        for (const char &piece : piece_lines[row])
        {
            if (isdigit(piece) != 0)
            {
                col -= piece - '0' - 1;
            }
            else
            {
                if constexpr (kDebug)
                {
                    std::cout << "Adding " << piece << " at row:" << row << " col:" << col << std::endl;
                }
                AddPiece(FromChar(piece), Position(std::make_pair(7 - col, 7 - row)));
            }
            col--;
        }
    }
    if constexpr (kDebug)
        std::cout << *this << std::endl;
}

bool Board::CollisionCheck(ChessPieceEnum pieceType, const Position &frompos, const Position &topos)
{
    const auto *frompospiece = GetPieces()->GetPiece(frompos);
    const auto *piece = GetPieces()->GetPiece(topos);
    if (piece != nullptr)
    {
        if (pieceType != ChessPieceEnum::kPiyadeWhite && pieceType != ChessPieceEnum::kPiyadeBlack)
        {
            if ((piece)->GetColor() == frompospiece->GetColor())
            {
                return false;
            }
        }
        else if (pieceType == ChessPieceEnum::kPiyadeWhite || pieceType == ChessPieceEnum::kPiyadeBlack)
        {
            // piyade can not move over another piece
            return false;
        }
    }

    return CanJumpOrPathClear(frompos, topos, pieceType);
}

double Board::EvaluateBoard(Color color)
{
    double score = 0;

    for (size_t i = 0; i < PieceGroup::kSquareCount; i++)
    {
        const auto &pos = PieceGroup::Coord1to2(i);
        auto *piece = GetPieces()->GetPiece(pos);
        if (nullptr == piece)
        {
            continue;
        }
        if (piece->GetColor() == color)
        {
            score += piece->GetPiecePoint();
        }
        else
        {
            score -= piece->GetPiecePoint();
        }
        auto state = GetBoardState();
        if (currentTurn_ == color)
        {
            if (state == GameState::kCheckmate || state == GameState::kStalemate)
            {
                score -= 1000;
            }
            else if (state == GameState::kCheck)
            {
                score -= 100;
            }
        }
        else
        {
            if (state == GameState::kCheckmate || state == GameState::kStalemate)
            {
                score += 1000;
            }
            else if (state == GameState::kCheck)
            {
                score += 100;
            }
        }
    }
    return score;
}

void Shuffle(std::vector<Movement> &movements)
{
    std::random_device rnd;  // Seed the random number generator (once per program execution)
    std::mt19937 gen(rnd()); // Use Mersenne Twister engine

    std::shuffle(movements.begin(), movements.end(), gen);
}

const std::vector<Movement> &Board::GetPossibleMoves(Color color)
{
    auto fen = GenerateFEN(false) + (color == Color::kBlack ? std::string("b") : std::string("w"));
    if (possibleMovesMemory_.Have(fen))
    {
        const auto &ret = possibleMovesMemory_.Get(fen);
        if constexpr (kGetPossibleDebug)
            std::cout << "Using cached possible moves " << ret.size() << " for " << color << " " << fen << std::endl;
        return ret;
    }
    std::vector<Movement> ret;
    if constexpr (kPieceGroupDebug)
        std::cout << "Getting possible moves for " << color << std::endl;
    for (size_t i = 0; i < PieceGroup::kSquareCount; i++)
    {
        auto pos = PieceGroup::Coord1to2(i);
        auto *piece_primitive = GetPieces()->GetPiece(pos);
        if (piece_primitive == nullptr)
            continue;
        if constexpr (kGetPossibleDebug)
            std::cout << "Checking " << pos.ToString() << " " << piece_primitive->GetName() << std::endl;
        if (piece_primitive->GetColor() != color)
        {
            continue;
        }
        auto insertable = GetPossibleMoves(pos, piece_primitive->GetPieceType(), piece_primitive->GetColor());
        for (auto it = insertable.begin(); it != insertable.end(); it++)
        {
            if (it->from == it->to)
            {
                throw std::runtime_error("from == to in GetPossibleMoves " + it->from.ToString());
            }
            ret.push_back(*it);
        }
    }
    if constexpr (kPieceGroupDebug)
        std::cout << "Got " << ret.size() << " possible moves" << std::endl;
    Shuffle(ret);
    if constexpr (kGetPossibleDebug)
        std::cout << "Using calculated possible moves with size " << ret.size() << std::endl;

    possibleMovesMemory_.Add(fen, ret);
    if constexpr (kGetPossibleDebug)
        if (ret.size() == 0)
        {
            std::cout << "No possible moves for " << OpponentColor(currentTurn_) << " " << GenerateFEN() << std::endl;
            std::cout << *this << std::endl;
        }
    return possibleMovesMemory_.Get(fen);
}

const std::vector<Movement> &Board::GetPossibleMovesCalcOpponentToo(Color color)
{
    auto fen = GenerateFEN(false) + (color == Color::kBlack ? std::string("b") : std::string("w"));
    if (possibleMovesMemory_.Have(fen))
    {
        const auto &ret = possibleMovesMemory_.Get(fen);
        if constexpr (kGetPossibleDebug)
            std::cout << "Using cached possible opponent moves " << ret.size() << " for " << color << " " << fen
                      << std::endl;
        return ret;
    }
    std::vector<Movement> ret;
    if constexpr (kPieceGroupDebug)
        std::cout << "Getting possible moves for " << color << std::endl;
    bool turned_switch_temp = false;
    if (GetCurrentTurn() != color)
    {
        SwitchTurn();
        turned_switch_temp = true;
    }
    DeferedCall my_defered_call([&]() {
        if (turned_switch_temp)
        {
            SwitchTurn();
        }
    });
    for (size_t i = 0; i < PieceGroup::kSquareCount; i++)
    {
        auto pos = PieceGroup::Coord1to2(i);
        auto *piece_primitive = GetPieces()->GetPiece(pos);
        if (piece_primitive == nullptr)
            continue;
        if (piece_primitive->GetColor() != color)
        {
            continue;
        }
        auto insertable = GetPossibleMoves(pos, piece_primitive->GetPieceType(), piece_primitive->GetColor());
        for (auto it = insertable.begin(); it != insertable.end(); it++)
        {
            if (it->from == it->to)
            {
                throw std::runtime_error("from == to in GetPossibleMoves " + it->from.ToString());
            }
            ret.push_back(*it);
        }
    }
    if constexpr (kPieceGroupDebug)
        std::cout << "Got " << ret.size() << " possible moves" << std::endl;
    Shuffle(ret);

    if constexpr (kGetPossibleDebug)
        std::cout << "Using calculated possible opponent moves with size " << ret.size() << std::endl;
    possibleMovesMemory_.Add(fen, ret);
    if constexpr (kGetPossibleDebug)
        if (ret.size() == 0)
        {
            std::cout << "No possible moves for " << OpponentColor(currentTurn_) << " " << GenerateFEN() << std::endl;
            std::cout << *this << std::endl;
        }
    return possibleMovesMemory_.Get(fen);
}
std::variant<double, Movement> Board::PickOrEvaluate(const std::optional<Movement> &playing_move, int &nodesvisited,
                                                     double &alpha, double &beta, int depth, Color maximizingColor,
                                                     bool randomize)
{
    bool is_maximizing = maximizingColor == currentTurn_;

    bool toplevel = !playing_move.has_value();
    const auto &moves = GetPossibleMoves(currentTurn_);
    if (moves.size() == 0 && toplevel)
    {
        std::cout << *this << std::endl;
        std::cout << GenerateFEN() << std::endl;
        std::cout << "No possible moves game over, " << OpponentColor(currentTurn_) << " won! current depth " << depth
                  << " " << (*playing_move).ToString() << "  toplevel " << toplevel << " moves.size() " << moves.size()
                  << std::endl;

        throw std::runtime_error("No possible moves, game over");
    }
    if (moves.size() == 0)
    {
        auto res = EvaluateBoard(maximizingColor);
        return res;
    }
    auto temp_eval = is_maximizing ? -std::numeric_limits<double>::max() : std::numeric_limits<double>::max();
    auto pickedmove = moves[0];

    for (const auto &possible_move : moves)
    {
        auto eval = MinimaxSearch(possible_move, nodesvisited, alpha, beta, depth - 1, maximizingColor, randomize);
        if (is_maximizing)
        {
            if (temp_eval < std::get<double>(eval))
            {
                pickedmove = possible_move;
                temp_eval = std::get<double>(eval);
            }
            alpha = std::max(alpha, std::get<double>(eval));
            if (beta <= alpha)
            {
                break;
            }
        }
        else
        {

            if (temp_eval > std::get<double>(eval))
            {
                pickedmove = possible_move;
                temp_eval = std::get<double>(eval);
            }
            beta = std::min(beta, std::get<double>(eval));
            if (beta <= alpha)
            {
                break;
            }
        }
    }
    if (!toplevel)
        return temp_eval;
    return Movement(pickedmove);
}

std::variant<double, Movement> Board::MinimaxSearch(const std::optional<Movement> &playing_move, int &nodesvisited,
                                                    double alpha, double beta, int depth, Color maximizingColor,
                                                    bool randomize)
{
    bool played_something = false;
    if (playing_move)
    {
        played_something = Play(*playing_move);
        if (!played_something)
        {
            std::cout << *this << std::endl;
            std::cout << "Invalid move " << playing_move->ToString() << "  boardstatefen " << GenerateFEN()
                      << std::endl;
            throw std::runtime_error("Invalid move " + playing_move->ToString());
        }
        nodesvisited++;
    }
    DeferedCall callback([&]() {
        if (played_something)
            Revert();
    });

    if (depth == 0)
    {
        auto res = EvaluateBoard(maximizingColor);
        return res;
    }

    return PickOrEvaluate(playing_move, nodesvisited, alpha, beta, depth, maximizingColor, randomize);
}

} // namespace shatranj