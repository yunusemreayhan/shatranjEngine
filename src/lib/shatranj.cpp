#include "shatranj.h"

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include "position.h"
#include "shatranc_piece.h"
#include "types.h"

namespace shatranj
{
Shatranj::Shatranj(std::string player1, std::string player2)
    : board_(std::make_shared<Board>(std::move(player1), std::move(player2)))
{
    InitializeBoard();
}
void Shatranj::InitializeBoard()
{
    board_->AddPiece(Rook(Position("a1"), Color::kWhite));
    board_->AddPiece(Rook(Position("h1"), Color::kWhite));
    board_->AddPiece(Horse(Position("b1"), Color::kWhite));
    board_->AddPiece(Horse(Position("g1"), Color::kWhite));
    board_->AddPiece(Fil(Position("c1"), Color::kWhite));
    board_->AddPiece(Fil(Position("f1"), Color::kWhite));
    board_->AddPiece(Vizier(Position("d1"), Color::kWhite));
    board_->AddPiece(Shah(Position("e1"), Color::kWhite));
    for (const char citr : std::vector<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'})
    {
        std::stringstream ssinst;
        ssinst << citr << "2";
        board_->AddPiece(Piyade(Position(ssinst.str()), Color::kWhite));
    }
    board_->AddPiece(Rook(Position("a8"), Color::kBlack));
    board_->AddPiece(Rook(Position("h8"), Color::kBlack));
    board_->AddPiece(Horse(Position("b8"), Color::kBlack));
    board_->AddPiece(Horse(Position("g8"), Color::kBlack));
    board_->AddPiece(Fil(Position("c8"), Color::kBlack));
    board_->AddPiece(Fil(Position("f8"), Color::kBlack));
    board_->AddPiece(Vizier(Position("d8"), Color::kBlack));
    board_->AddPiece(Shah(Position("e8"), Color::kBlack));
    for (const char citr : std::vector<char>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'})
    {
        std::stringstream ssinst;
        ssinst << citr << "7";
        board_->AddPiece(Piyade(Position(ssinst.str()), Color::kBlack));
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
            std::cout << "Invalid input. Please enter a move in the format 'fromPos toPos'." << std::endl;
            return false;
        }

        return board_->Play(from_pos, to_pos);
    }
    return false;
}

bool Shatranj::Play(const Movement &input)
{
    if (kDebug)
        std::cout << "play: " << input.ToString() << std::endl;

    return board_->Play(input, true);
}

void Shatranj::Run()
{
    while (board_->GetBoardState() == GameState::kNormal)
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

bool Shatranj::PlaySeq(const std::vector<std::string> &seq)
{
    int counter = 0;
    bool succ = true;
    for (const auto &seqitr : seq)
    {
        if (Play(seqitr))
        {
            if constexpr (kDebug)
                std::cout << "sequence success at:" << seqitr << std::endl;
            counter++;
        }
        else
        {
            if constexpr (kDebug)
                std::cout << "sequence failed at:" << seqitr << std::endl;
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

} // namespace shatranj