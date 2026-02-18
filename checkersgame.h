#ifndef CHECKERSGAME_H
#define CHECKERSGAME_H

#include <vector>

class CheckersGame
{
public:
    enum class Side { Red, Black };
    enum class Piece { Empty, RedMan, RedKing, BlackMan, BlackKing };

    struct Move
    {
        int fr = -1;
        int fc = -1;
        int tr = -1;
        int tc = -1;
        
        bool isCapture = false;
        int cr = -1;
        int cc = -1;
    };

    CheckersGame();

    void reset();

    bool isInside(int r, int c) const;
    bool isPlayableSquare(int r, int c) const; 
    Piece at(int r, int c) const;

    Side currentSide() const { return m_turn; }
    bool isGameOver() const { return m_gameOver; }

    int countRed() const;
    int countBlack() const;

    bool hasLockedPiece() const { return m_locked; }
    int lockedR() const { return m_lockedR; }
    int lockedC() const { return m_lockedC; }

    
    std::vector<Move> validMoves() const;
    std::vector<Move> validMovesFrom(int r, int c) const;

    
    
    bool playMove(int fr, int fc, int tr, int tc);

private:
    Piece m_board[8][8];
    Side m_turn = Side::Red;
    bool m_gameOver = false;

    bool m_locked = false;
    int m_lockedR = -1;
    int m_lockedC = -1;

    static bool isRed(Piece p);
    static bool isBlack(Piece p);
    static bool isKing(Piece p);
    static Side pieceSide(Piece p);
    static Side other(Side s);

    void setPiece(int r, int c, Piece p);
    bool belongsToTurn(Piece p) const;
    bool hasAnyMovesFor(Side s) const;

    std::vector<Move> captureMovesFrom(int r, int c) const;
    std::vector<Move> simpleMovesFrom(int r, int c) const;

    bool anyCaptureAvailableForTurn() const;
    void maybeCrown(int r, int c);
    void recomputeGameOver();
};

#endif 
