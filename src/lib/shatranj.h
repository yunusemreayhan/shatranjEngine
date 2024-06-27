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
    Shatranj(std::string player1, std::string player2);
    void InitializeBoard();
    std::optional<std::pair<std::string, std::string>> ParseInput(const std::string &input);
    static std::string GetInput();
    bool Play(const std::string &input);
    bool PlaySeq(const std::vector<std::string> &seq);
    void Run();

    std::shared_ptr<Board>& GetBoard() { return board_; }

  private:
    std::shared_ptr<Board> board_;
    constexpr static inline bool kDebug = kDebugGlobal;
};
} // namespace Shatranj