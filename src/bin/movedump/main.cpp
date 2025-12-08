#include <iostream>
#include "stockfish_position.h"
#include "movegen.h"
#include "bitboard.h"
#include "shatranc_piece.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "usage: movedump \"<fen>\"" << std::endl;
        std::cout << "example: movedump \"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1\"" << std::endl;
        return 1;
    }

    // Initialize required components
    shatranj::Piece::InitCapturePerSquareTable();
    shatranj::Piece::InitMovePerSquareTable();
    Stockfish::Bitboards::init();
    Stockfish::Position::init();

    std::string fen = argv[1];
    
    Stockfish::StateInfo st;
    Stockfish::Position pos;
    pos.set(fen, &st, true);

    // Generate all legal moves
    Stockfish::MoveList<Stockfish::LEGAL> moveList(pos);
    
    std::cout << "Available moves for FEN: " << fen << std::endl;
    std::cout << "Total moves: " << moveList.size() << std::endl;
    
    for (const auto& move : moveList) {
        std::cout << move << std::endl;
    }
    
    return 0;
}