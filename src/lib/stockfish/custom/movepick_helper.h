#pragma once

#include "../types.h"
#include "../stockfish_position.h"

struct capture_eval {
    inline Stockfish::Piece capturingPiece(Stockfish::Move m, Stockfish::Position pos) {
        return pos.piece_on(m.from_sq());
    }
    inline Stockfish::Piece capturedPiece(Stockfish::Move m, Stockfish::Position pos) {
        return pos.piece_on(m.to_sq());
    }
    inline int capture_score(Stockfish::Move m, Stockfish::Position pos) {
        const auto capturing_piece = capturingPiece(m, pos);
        const auto captured_piece  = capturedPiece(m, pos);
        return Stockfish::PieceValue[captured_piece] - Stockfish::PieceValue[capturing_piece];
    }

    inline int operator()(Stockfish::Move m, Stockfish::Position pos) {
        return capture_score(m, pos);
    }
};
