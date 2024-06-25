#pragma once

#include "board.h"
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace shatranj
{
class Player;
class Shatranj
{
  public:
    Shatranj(std::shared_ptr<Player> player1, std::shared_ptr<Player> player2);
    void InitializeBoard();
    std::optional<std::pair<std::string, std::string>> ParseInput(const std::string &input);
    static std::string GetInput();
    bool Play(const std::string &input);
    void Run();

  private:
    std::shared_ptr<Board> board_;
};
} // namespace Shatranj