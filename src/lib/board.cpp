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
        auto curpieceopt = currentTurn_ == Color::kBlack ? GetPieces()->GetBlackPtr(i) : GetPieces()->GetWhitePtr(i);
        if (!curpieceopt)
        {
            continue;
        }

        auto *curpiece = *curpieceopt;
        auto calc = Piece::CanMove(PieceGroup::Coord1to2(i), pos, GetSharedFromThis(), curpiece->GetPieceType(),
                                   curpiece->GetColor()) ||
                    Piece::CanCapture(PieceGroup::Coord1to2(i), pos, GetSharedFromThis(), curpiece->GetPieceType(),
                                      curpiece->GetColor());
        if (calc)
            return true;
    }
    return false;
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
        if (GetPieces()->GetPieceByVal(cur))
        {
            return false;
        }
        cur.Move(std::make_pair(step_x, step_y));
    }
    return true;
}

bool Board::MovePiece(Position frompos, Position topos)
{
    auto piece_opt = GetPieces()->GetPieceByVal(frompos);
    auto piece = *piece_opt;
    if (currentTurn_ != piece.GetColor())
    {
        if constexpr (kDebugGlobal)
        {
            std::cout << "its turn of " << currentTurn_ << " not " << piece.GetColor() << std::endl;
        }
        return false;
    }

    const bool can_move = Piece::CanMove(frompos, topos, GetSharedFromThis(), piece.GetPieceType(), piece.GetColor());

    const bool can_capture =
        Piece::CanCapture(frompos, topos, GetSharedFromThis(), piece.GetPieceType(), piece.GetColor());

    if (!can_move && !can_capture)
    {
        return false;
    }

    std::unique_ptr<Piece> captured_piece_uptr = nullptr;
    auto captured_piece = GetPieces()->GetPieceByVal(topos);
    if (captured_piece)
    {
        captured_piece_uptr = std::make_unique<Piece>(*captured_piece);
        if ((piece.IsPiyade() && can_capture) || (!piece.IsPiyade() && can_move))
            RemovePiece(topos);
    }

    bool promoted = false;
    GetPieces()->MovePiece(frompos, topos);
    piece_opt = GetPieces()->GetPieceByVal(topos);
    piece = *piece_opt;
    if (piece.IsPiyade())
    {
        if (topos.Gety() == 0 || topos.Gety() == 7)
        {
            auto promoted_piece = PromotePiyade(piece);
            RemovePiece(piece.GetPos());
            AddPiece(promoted_piece);
            promoted = true;
        }
    }
    GetHistory().AddMove(frompos, topos, piece.GetPieceType(), std::move(captured_piece_uptr), promoted,
                         piece.GetColor());
    MoveSuccesful(piece, frompos, topos);

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
        auto piece_opt = GetPieces()->GetPieceByVal(last_move->from);
        if (!piece_opt)
        {
            if constexpr (kDebug)
            {
                std::cout << *this << std::endl;
            }
            std::cout << "Piece not found, illogical board state at " << last_move->to.ToString() << std::endl;
            throw std::runtime_error("Piece not found, illogical board state at " + last_move->to.ToString());
        }
        auto piece = *piece_opt;
        if (last_move->promoted)
        {
            Piece demoted_piece = DemotePromoted(piece);
            RemovePiece(piece.GetPos());
            AddPiece(demoted_piece);
        }

        if (last_move->captured != nullptr)
        {
            AddPiece(*last_move->captured);
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

void Board::MoveSuccesful(const Piece &piece, const Position & /*fromPos*/, const Position & /*toPos*/)
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

GameState Board::GetBoardState()
{
    GameState ret = GameState::kNormal;
    auto fenkey = GenerateFEN(false);
    if (boardStateMemory_.Have(fenkey))
    {
        return boardStateMemory_.Get(fenkey);
    }
    auto current_turn_pieces = GetPieces()->GetSubPieces(currentTurn_);
    auto opponent_pieces = GetPieces()->GetSubPieces(OpponentColor(currentTurn_));

    if (current_turn_pieces.size() == 1 && opponent_pieces.size() > 2)
    {
        ret = GameState::kCheckmate;
    }
    else if (current_turn_pieces.size() == 1 && opponent_pieces.size() == 1)
    {
        ret = GameState::kDraw;
    }
    else if (current_turn_pieces.size() == 1 && opponent_pieces.size() == 2)
    {
        if (Piece::CanGo(current_turn_pieces[0].GetPos(), opponent_pieces[0].GetPos(), GetSharedFromThis(),
                         current_turn_pieces[0].GetPieceType(), current_turn_pieces[0].GetColor()) ||
            Piece::CanGo(current_turn_pieces[1].GetPos(), opponent_pieces[0].GetPos(), GetSharedFromThis(),
                         current_turn_pieces[0].GetPieceType(), current_turn_pieces[0].GetColor()))
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
        const auto &moves = GetPieces()->GetPossibleMoves(currentTurn_, GetSharedFromThis());

        if (IsCheck(currentTurn_) && moves.empty())
        {
            ret = GameState::kCheckmate;
        }
        else if (moves.empty())
        {
            ret = GameState::kStalemate;
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

    auto piece = pieces_->GetPieceByVal(input.from);
    if (!piece || piece->GetColor() != currentTurn_)
    {
        return false;
    }

    return MovePiece(input.from, input.to);
}

bool Board::Play(const std::string &from_pos, const std::string &to_pos)
{
    if (GetBoardState() != GameState::kNormal)
        return false;
    const auto checking_movemend = Movement(from_pos, to_pos);

    return !WouldBeInCheck(checking_movemend) && Play(checking_movemend);
}

char *Board::BoardRepresantation::GetBoardReprensentation(Board *board)
{
    static char *ret = new char[PieceGroup::kSquareCount];
    memset(ret, '.', PieceGroup::kSquareCount);
    for (size_t i = 0; i < PieceGroup::kSquareCount; i++)
    {
        const auto &pieceopt = board->GetPieces()->Get(i);
        if (!pieceopt)
        {
            continue;
        }
        const auto &piece = *pieceopt;
        ret[CombinedCoordinate(piece.GetPos().Getx(), piece.GetPos().Gety())] =
            piece.GetColor() == Color::kWhite ? std::toupper(piece.GetSymbol()) : std::tolower(piece.GetSymbol());
    }
    return ret;
}

std::string Board::BoardToString() const
{
    char *board_repr = BoardRepresantation::GetBoardReprensentation(const_cast<Board *>(this));
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
            ret += BoardRepresantation::GetPieceFromCoordinate(board_repr, xitr, yitr);
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

bool Board::AddPiece(Piece piece)
{
    auto res = pieces_->AddPiece(piece.GetPrimitive(), piece.GetPos());
    if (!res)
    {
        std::cout << "Adding " << piece.GetPos().ToString() << " failed" << std::endl;
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
    const auto &moves = GetPieces()->GetPossibleMoves(currentTurn_, GetSharedFromThis());

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
    auto *board_repr = BoardRepresantation::GetBoardReprensentation(const_cast<Board *>(this));

    std::string ret;
    for (int8_t yitr = 7; yitr >= 0; yitr--)
    {
        int spaces = 0;
        for (int8_t xitr = 0; xitr < 8; xitr++)
        {
            const auto &piece = BoardRepresantation::GetPieceFromCoordinate(board_repr, xitr, yitr);
            if (piece == '.')
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
                ret += piece;
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
                AddPiece(FromChar(piece, Position(std::make_pair(7 - col, 7 - row))));
            }
            col--;
        }
    }
    if constexpr (kDebug)
        std::cout << *this << std::endl;
}

double Board::EvaluateBoard(Color color)
{
    double score = 0;

    for (size_t i = 0; i < PieceGroup::kSquareCount; i++)
    {
        auto pieceopt = GetPieces()->Get(i);
        if (!pieceopt)
        {
            continue;
        }
        auto piece = *pieceopt;
        if (piece.GetColor() == color)
        {
            score += piece.GetPiecePoint();
        }
        else
        {
            score -= piece.GetPiecePoint();
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

std::variant<double, Movement> Board::PickOrEvaluate(std::optional<Movement> playing_move_opt, int &nodesvisited,
                                                     int depth, Color maximizingColor, bool randomize, double alpha,
                                                     double beta)
{
    bool isMaximizing = maximizingColor == currentTurn_;

    bool toplevel = !playing_move_opt;
    auto moves = GetPieces()->GetPossibleMoves(currentTurn_, GetSharedFromThis());
    if (moves.size() == 0 && toplevel)
    {
        std::cout << *this << std::endl;
        std::cout << GenerateFEN() << std::endl;
        std::cout << "No possible moves game over, " << OpponentColor(currentTurn_) << " won!" << std::endl;

        throw std::runtime_error("No possible moves, game over");
    }
    if (moves.size() == 0)
    {
        return EvaluateBoard(maximizingColor);
    }
    auto temp_eval = isMaximizing ? std::numeric_limits<double>::min() : std::numeric_limits<double>::max();
    auto pickedmove = moves[0];
    if (randomize)
        Shuffle(moves);

    for (const auto &possible_move : moves)
    {
        auto eval =
            MinimaxSearch(Movement(possible_move), nodesvisited, depth - 1, maximizingColor, randomize, alpha, beta);
        if (isMaximizing)
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

std::variant<double, Movement> Board::MinimaxSearch(std::optional<Movement> playing_move_opt, int &nodesvisited,
                                                    int depth, Color maximizingColor, bool randomize, double alpha,
                                                    double beta)
{
    bool played_something = false;
    if (playing_move_opt)
    {
        played_something = Play(*playing_move_opt);
        if (!played_something)
        {
            std::cout << *this << std::endl;
            std::cout << "Invalid move " << playing_move_opt->ToString() << "  boardstatefen " << GenerateFEN()
                      << std::endl;
            throw std::runtime_error("Invalid move " + playing_move_opt->ToString());
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

    return PickOrEvaluate(playing_move_opt, nodesvisited, depth, maximizingColor, randomize, alpha, beta);
}

} // namespace shatranj