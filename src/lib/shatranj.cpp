#include "shatranj.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include "board.h"
#include "position.h"
#include "shatranc_piece.h"
#include "types.h"

namespace shatranj
{
Shatranj::Shatranj() : board_(std::make_shared<Board>())
{
    InitializeBoard();
}
void Shatranj::InitializeBoard()
{
    board_->AddPiece(FromChar('R'), Position("a1"));
    board_->AddPiece(FromChar('H'), Position("b1"));
    board_->AddPiece(FromChar('F'), Position("c1"));
    board_->AddPiece(FromChar('V'), Position("d1"));
    board_->AddPiece(FromChar('S'), Position("e1"));
    board_->AddPiece(FromChar('F'), Position("f1"));
    board_->AddPiece(FromChar('H'), Position("g1"));
    board_->AddPiece(FromChar('R'), Position("h1"));
    for (const char citr : std::vector<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'})
    {
        std::stringstream ssinst;
        ssinst << citr << "2";
        board_->AddPiece(FromChar('P'), Position(ssinst.str()));
    }

    board_->AddPiece(FromChar('r'), Position("a8"));
    board_->AddPiece(FromChar('h'), Position("b8"));
    board_->AddPiece(FromChar('f'), Position("c8"));
    board_->AddPiece(FromChar('v'), Position("d8"));
    board_->AddPiece(FromChar('s'), Position("e8"));
    board_->AddPiece(FromChar('f'), Position("f8"));
    board_->AddPiece(FromChar('h'), Position("g8"));
    board_->AddPiece(FromChar('r'), Position("h8"));
    for (const char citr : std::vector<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'})
    {
        std::stringstream ssinst;
        ssinst << citr << "7";
        board_->AddPiece(FromChar('p'), Position(ssinst.str()));
    }
}
std::optional<std::pair<std::string, std::string>> Shatranj::ParseInput(const std::string &input)
{
    if (input.size() == 4)
    {
        return std::make_pair(input.substr(0, 2), input.substr(2, 2));
    }
    return std::nullopt;
}
std::string Shatranj::GetInput()
{
    std::cout << "please enter your move: ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}
bool Shatranj::Play(const std::string &input)
{
    if (kDebug)
        std::cout << "play: " << input << std::endl;
    if (input == "valids")
    {
        board_->PrintValidMoves();
        return true;
    }
    std::optional<std::pair<std::string, std::string>> parsed_input = ParseInput(input);
    if (parsed_input)
    {
        const std::string &from_pos = parsed_input->first;
        const std::string &to_pos = parsed_input->second;
        if (from_pos.size() != 2 || to_pos.size() != 2)
        {
            std::cerr << "Invalid input. Please enter a move in the format 'fromPos toPos'.\"" << input << "\""
                      << std::endl;
            return false;
        }

        return board_->Play(Movement(from_pos, to_pos));
    }
    return false;
}

bool Shatranj::Play(const Movement &input)
{
    if (kDebug)
        std::cout << "play: " << input.ToString() << std::endl;

    return board_->Play(input);
}

void Shatranj::Run()
{
    const auto state = board_->GetBoardState();
    while (state == GameState::kNormal || state == GameState::kCheck)
    {
        std::cout << *board_ << std::endl;
        std::string input = GetInput();
        auto res = Play(input);
        if (!res)
        {
            std::cout << "Invalid input. Please try again." << std::endl;
        }
        else
        {
            std::cout << "Move successful " << input << std::endl;
        }
        if (GameState() == GameState::kDraw)
        {
            std::cout << "Draw" << std::endl;
        }
    }
    std::cout << *(board_->Winner()) << " win" << std::endl;
    std::cout << *board_ << std::endl;
}

bool Shatranj::PlaySeq(const std::vector<std::string> &seq, bool printboard)
{
    int counter = 0;
    bool succ = true;
    for (const auto &seqitr : seq)
    {
        if (Play(seqitr))
        {
            if (printboard)
                std::cout << "sequence success at:" << seqitr << std::endl;
            counter++;
        }
        else
        {
            if (printboard)
                std::cout << "sequence failed at:" << seqitr << std::endl;
            succ = false;
            break;
        }
    }
    if (printboard)
        std::cout << *(this->GetBoard()) << std::endl;
    // TODO revert with counter
    (void)counter;
    return succ;
}

bool Shatranj::PlaySeq2(const std::vector<Movement> &seq)
{
    int counter = 0;
    bool succ = true;
    for (const auto &seqitr : seq)
    {
        if (Play(seqitr))
        {
            if constexpr (kDebug)
                std::cout << "sequence success at:" << seqitr.ToString() << std::endl;
            counter++;
        }
        else
        {
            if constexpr (kDebug)
                std::cout << "sequence failed at:" << seqitr.ToString() << std::endl;
            succ = false;
            break;
        }
        if constexpr (kDebug)
            std::cout << *(this->GetBoard()) << std::endl;
    }
    // TODO revert with counter
    (void)counter;
    return succ;
}

std::optional<shatranj::Movement> Shatranj::PickMoveInBoard(int depth, int *countofnodesvisited,
                                                            std::chrono::microseconds *duration)
{
    std::variant<double, shatranj::Movement> pickedmove;
    auto res = shatranj::RunWithTiming(
        "minmax search ",
        [&]() -> shatranj::GameState {
            try
            {
                auto alpha = -std::numeric_limits<double>::max();
                auto beta = std::numeric_limits<double>::max();
                pickedmove = board_->MinimaxSearch(std::nullopt, countofnodesvisited, alpha, beta, depth,
                                                   board_->GetCurrentTurn(), true);
            }
            catch (...)
            {
                const auto state = board_->GetBoardState();
                std::cout << "exception in search, state : " << static_cast<int>(state) << std::endl;
                return state;
            }
            return shatranj::GameState::kNormal;
        },
        duration);
    std::cout << "nodes visited: " << *countofnodesvisited << std::endl;
    if (duration != nullptr && countofnodesvisited != nullptr)
        std::cout << "individual call, nodes visited per second: " << 1000000 * *countofnodesvisited / duration->count()
                  << std::endl;
    std::cout << *(board_) << std::endl;
    if (shatranj::GameState::kNormal == res)
    {
        return std::get<shatranj::Movement>(pickedmove);
    }
    return std::nullopt;
}

std::optional<shatranj::Movement> Shatranj::PickMoveForMateSequenceIfAny(int depth, int *countofnodesvisited,
                                                                         std::chrono::microseconds *duration)
{
    std::pair<shatranj::Movement, bool> pickedmove = {Movement::GetEmpty(), false};
    shatranj::RunWithTiming(
        "LookForCheckMateMoveDfs ",
        [&]() -> bool {
            try
            {
                std::vector<Movement> moves;
                pickedmove =
                    board_->LookForCheckMateMoveDfs(depth, board_->GetCurrentTurn(), countofnodesvisited, moves);
            }
            catch (...)
            {
                std::cout << "exception in search" << std::endl;
                return false;
            }
            return true;
        },
        duration);
    std::cout << "nodes visited: " << *countofnodesvisited << std::endl;
    std::cout << *(board_) << std::endl;
    if (pickedmove.second)
    {
        std::cout << "found a winning sequence in move " << pickedmove.first.ToString() << std::endl;
        return pickedmove.first;
    }

    std::cout << "no winning sequence found for depth " << depth << "" << std::endl;
    return std::nullopt;
}

bool Shatranj::PickAndPlay(int depth, int *countofnodesvisited, std::chrono::microseconds *duration)
{
    auto move = PickMoveInBoard(depth, countofnodesvisited, duration);
    if (move)
    {
        std::cout << "picked and playing: " << move->ToString() << std::endl;
        return Play(*move);
    }
    return false;
}

bool Shatranj::PickAndPlayMateSequence(int depth, int *countofnodesvisited, std::chrono::microseconds *duration)
{
    auto move = PickMoveForMateSequenceIfAny(depth, countofnodesvisited, duration);
    if (move)
    {
        std::cout << "picked and playing: " << move->ToString() << std::endl;
        return Play(*move);
    }
    return false;
}

bool Shatranj::PickAndPlayWinningSequence(int depth, int *countofnodesvisited, std::chrono::microseconds *duration)
{
    auto move = PickMoveForMateSequenceIfAny(depth, countofnodesvisited, duration);
    if (move)
    {
        std::cout << "picked and playing: " << move->ToString() << std::endl;
        return Play(*move);
    }
    return false;
}

} // namespace shatranj
