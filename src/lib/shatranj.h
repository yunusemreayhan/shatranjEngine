#pragma once

#include "board.h"
#include "position.h"
#include <chrono>
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
    Shatranj();
    void InitializeBoard();
    static std::optional<std::pair<std::string, std::string>> ParseInput(const std::string &input);
    static std::string GetInput();
    bool Play(const std::string &input);
    bool Play(const Movement &input);
    bool PlaySeq(const std::vector<std::string> &seq, bool printboard = false);
    bool PlaySeq2(const std::vector<Movement> &seq);
    void Run();
    std::optional<shatranj::Movement> PickMoveInBoard(int depth = 6, int *countofnodesvisited = nullptr,
                                                      std::chrono::microseconds *duration = nullptr);
    std::optional<shatranj::Movement> PickMoveForMateSequenceIfAny(int depth = 6, int *countofnodesvisited = nullptr,
                                                                   std::chrono::microseconds *duration = nullptr);
    bool PickAndPlay(int depth = 6, int *countofnodesvisited = nullptr, std::chrono::microseconds *duration = nullptr);
    bool PickAndPlayWinningSequence(int depth = 6, int *countofnodesvisited = nullptr,
                                    std::chrono::microseconds *duration = nullptr);
    bool PickAndPlayMateSequence(int depth, int *countofnodesvisited = nullptr,
                                 std::chrono::microseconds *duration = nullptr);
    std::shared_ptr<Board> &GetBoard()
    {
        return board_;
    }

  private:
    std::shared_ptr<Board> board_;
    constexpr static inline bool kDebug = kDebugShatranj;
};
} // namespace shatranj
