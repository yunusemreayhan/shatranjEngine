#include "game_over_check.h"
#include "../stockfish_position.h"

namespace Stockfish {
GameEndDetector::GameEnd GameEndDetector::Analyse(const Position& pos) const {
    auto curkey = pos.key();
    if (lastAnalysisKey == curkey)
    {
        return lastAnalysisResult;
    }

    lastAnalysisKey      = curkey;
    lastAnalysisResult   = None;
    auto moves           = MoveList<LEGAL>(pos);
    auto our_piece_count = pos.count<ALL_PIECES>(pos.side_to_move());
    if (moves.size() == 0)
    {
        if (pos.side_to_move() == WHITE)
        {
            lastAnalysisResult = BlackWin;
        }
        else
        {
            lastAnalysisResult = WhiteWin;
        }
    }
    else if (our_piece_count == 1)  // only king of us left in the board
    {
        size_t opponent_piece_count = pos.count<ALL_PIECES>(~pos.side_to_move());

        if (opponent_piece_count == 1)
        {
            lastAnalysisResult = Draw;
        }
        else if (opponent_piece_count == 2)
        {
            if (KingCanCaptureLastOpponentPiece(pos))
                lastAnalysisResult = Draw;
            else
                lastAnalysisResult = pos.side_to_move() == WHITE ? BlackWin : WhiteWin;
        }
        else  // if (opponent_piece_count > 2)
        {
            lastAnalysisResult = pos.side_to_move() == WHITE ? BlackWin : WhiteWin;
        }
    }
    return lastAnalysisResult;
}

void GameEndDetector::DumpGameEnd(const Stockfish::Position& pos) {
    Analyse(pos);
    switch (lastAnalysisResult)
    {
    case None :
        break;
    case WhiteWin :
        std::cout << "White win" << std::endl;
        break;
    case BlackWin :
        std::cout << "Black win" << std::endl;
        break;
    case Draw :
        std::cout << "Draw" << std::endl;
        break;
    }
}

bool GameEndDetector::KingCanCaptureLastOpponentPiece(const Stockfish::Position& pos) const {
    auto our_pieces               = pos.pieces(pos.side_to_move());
    auto our_king                 = pos.pieces(KING) & our_pieces;
    auto opponent_pieces          = pos.pieces(~pos.side_to_move());
    auto opponent_king            = pos.pieces(KING) & opponent_pieces;
    auto opponent_non_king_pieces = opponent_pieces & ~opponent_king;

    auto fromsquare = pop_lsb(our_king);
    auto tosquare   = pop_lsb(opponent_non_king_pieces);

    assert(our_king == 0 && opponent_non_king_pieces == 0);

    // I can eat last piece of opponent in the board, so only kings left and game end becomes draw.
    if (pos.pseudo_legal(Move(fromsquare, tosquare)))
        return true;
    return false;
}

}
