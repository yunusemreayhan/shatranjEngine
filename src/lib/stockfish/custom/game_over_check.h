#pragma once

#include <cstdint>
namespace Stockfish {

class Position;

using Key = uint64_t;

class GameEndDetector {
   public:
    enum GameEnd {
        None,
        WhiteWin,
        BlackWin,
        Draw
    };

    GameEnd Analyse(const Stockfish::Position& pos) const;
    void    DumpGameEnd(const Stockfish::Position& pos);
    bool    KingCanCaptureLastOpponentPiece(const Stockfish::Position& pos) const;

   private:
    mutable Stockfish::Key lastAnalysisKey;
    mutable GameEnd        lastAnalysisResult;
};

}
