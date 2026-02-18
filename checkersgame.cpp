#include "checkersgame.h"

#include <algorithm>

static inline bool inRange(int x) { return x >= 0 && x < 8; }

CheckersGame::CheckersGame()
{
    reset();
}

bool CheckersGame::isInside(int r, int c) const
{
    return inRange(r) && inRange(c);
}

bool CheckersGame::isPlayableSquare(int r, int c) const
{
    
    return isInside(r, c) && ((r + c) % 2 == 1);
}

CheckersGame::Piece CheckersGame::at(int r, int c) const
{
    if (!isInside(r, c)) return Piece::Empty;
    return m_board[r][c];
}

void CheckersGame::setPiece(int r, int c, Piece p)
{
    if (!isInside(r, c)) return;
    m_board[r][c] = p;
}

bool CheckersGame::isRed(Piece p)
{
    return p == Piece::RedMan || p == Piece::RedKing;
}

bool CheckersGame::isBlack(Piece p)
{
    return p == Piece::BlackMan || p == Piece::BlackKing;
}

bool CheckersGame::isKing(Piece p)
{
    return p == Piece::RedKing || p == Piece::BlackKing;
}

CheckersGame::Side CheckersGame::pieceSide(Piece p)
{
    return isRed(p) ? Side::Red : Side::Black;
}

CheckersGame::Side CheckersGame::other(Side s)
{
    return (s == Side::Red) ? Side::Black : Side::Red;
}

bool CheckersGame::belongsToTurn(Piece p) const
{
    if (p == Piece::Empty) return false;
    return (m_turn == Side::Red) ? isRed(p) : isBlack(p);
}

void CheckersGame::reset()
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            m_board[r][c] = Piece::Empty;

    
    
    for (int r = 0; r <= 2; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            if (isPlayableSquare(r, c))
                m_board[r][c] = Piece::BlackMan;
        }
    }
    for (int r = 5; r <= 7; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            if (isPlayableSquare(r, c))
                m_board[r][c] = Piece::RedMan;
        }
    }

    m_turn = Side::Red;
    m_gameOver = false;
    m_locked = false;
    m_lockedR = m_lockedC = -1;
}

int CheckersGame::countRed() const
{
    int n = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (isRed(m_board[r][c])) ++n;
    return n;
}

int CheckersGame::countBlack() const
{
    int n = 0;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (isBlack(m_board[r][c])) ++n;
    return n;
}

std::vector<CheckersGame::Move> CheckersGame::captureMovesFrom(int r, int c) const
{
    std::vector<Move> out;
    if (!isPlayableSquare(r, c)) return out;
    const Piece p = at(r, c);
    if (p == Piece::Empty) return out;
    const Side s = pieceSide(p);

    auto isEnemy = [&](Piece q)
    {
        if (q == Piece::Empty) return false;
        return pieceSide(q) != s;
    };

    
    std::vector<int> drs;
    if (isKing(p))
        drs = {-1, +1};
    else
        drs = (s == Side::Red) ? std::vector<int>{-1} : std::vector<int>{+1};

    const int dcs[2] = {-1, +1};

    for (int dr : drs)
    {
        for (int dc : dcs)
        {
            const int mr = r + dr;
            const int mc = c + dc;
            const int tr = r + 2*dr;
            const int tc = c + 2*dc;
            if (!isPlayableSquare(tr, tc)) continue;
            if (at(tr, tc) != Piece::Empty) continue;
            if (!isEnemy(at(mr, mc))) continue;

            Move m;
            m.fr = r; m.fc = c;
            m.tr = tr; m.tc = tc;
            m.isCapture = true;
            m.cr = mr; m.cc = mc;
            out.push_back(m);
        }
    }
    return out;
}

std::vector<CheckersGame::Move> CheckersGame::simpleMovesFrom(int r, int c) const
{
    std::vector<Move> out;
    if (!isPlayableSquare(r, c)) return out;
    const Piece p = at(r, c);
    if (p == Piece::Empty) return out;

    const Side s = pieceSide(p);

    std::vector<int> drs;
    if (isKing(p))
        drs = {-1, +1};
    else
        drs = (s == Side::Red) ? std::vector<int>{-1} : std::vector<int>{+1};

    const int dcs[2] = {-1, +1};

    for (int dr : drs)
    {
        for (int dc : dcs)
        {
            const int tr = r + dr;
            const int tc = c + dc;
            if (!isPlayableSquare(tr, tc)) continue;
            if (at(tr, tc) != Piece::Empty) continue;
            Move m;
            m.fr = r; m.fc = c;
            m.tr = tr; m.tc = tc;
            m.isCapture = false;
            out.push_back(m);
        }
    }
    return out;
}

bool CheckersGame::anyCaptureAvailableForTurn() const
{
    if (m_locked)
        return !captureMovesFrom(m_lockedR, m_lockedC).empty();

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (belongsToTurn(at(r,c)) && !captureMovesFrom(r,c).empty())
                return true;
    return false;
}

std::vector<CheckersGame::Move> CheckersGame::validMovesFrom(int r, int c) const
{
    if (m_gameOver) return {};
    if (!isPlayableSquare(r, c)) return {};
    const Piece p = at(r, c);
    if (!belongsToTurn(p)) return {};

    if (m_locked && (r != m_lockedR || c != m_lockedC))
        return {};

    const bool mustCapture = anyCaptureAvailableForTurn();
    if (mustCapture)
        return captureMovesFrom(r, c);
    return simpleMovesFrom(r, c);
}

std::vector<CheckersGame::Move> CheckersGame::validMoves() const
{
    if (m_gameOver) return {};

    std::vector<Move> out;

    if (m_locked)
    {
        return validMovesFrom(m_lockedR, m_lockedC);
    }

    const bool mustCapture = anyCaptureAvailableForTurn();
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            if (!belongsToTurn(at(r,c))) continue;
            auto v = mustCapture ? captureMovesFrom(r,c) : simpleMovesFrom(r,c);
            out.insert(out.end(), v.begin(), v.end());
        }
    }
    return out;
}

void CheckersGame::maybeCrown(int r, int c)
{
    const Piece p = at(r, c);
    if (p == Piece::RedMan && r == 0) setPiece(r, c, Piece::RedKing);
    if (p == Piece::BlackMan && r == 7) setPiece(r, c, Piece::BlackKing);
}

bool CheckersGame::hasAnyMovesFor(Side s) const
{
    
    const Side saved = m_turn;
    const bool savedLocked = m_locked;
    const int savedLR = m_lockedR;
    const int savedLC = m_lockedC;

    
    auto belongs = [&](Piece p)
    {
        if (p == Piece::Empty) return false;
        return (s == Side::Red) ? isRed(p) : isBlack(p);
    };

    bool anyCapture = false;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (belongs(at(r,c)) && !captureMovesFrom(r,c).empty())
                anyCapture = true;

    if (anyCapture)
        return true;

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (belongs(at(r,c)) && !simpleMovesFrom(r,c).empty())
                return true;

    (void)saved; (void)savedLocked; (void)savedLR; (void)savedLC;
    return false;
}

void CheckersGame::recomputeGameOver()
{
    const int red = countRed();
    const int black = countBlack();

    if (red == 0 || black == 0)
    {
        m_gameOver = true;
        return;
    }

    
    if (!hasAnyMovesFor(m_turn))
    {
        m_gameOver = true;
        return;
    }
}

bool CheckersGame::playMove(int fr, int fc, int tr, int tc)
{
    if (m_gameOver) return false;
    if (!isPlayableSquare(fr, fc) || !isPlayableSquare(tr, tc)) return false;
    if (m_locked && (fr != m_lockedR || fc != m_lockedC)) return false;

    auto v = validMovesFrom(fr, fc);
    auto it = std::find_if(v.begin(), v.end(), [&](const Move &m){ return m.tr == tr && m.tc == tc; });
    if (it == v.end()) return false;

    const Move m = *it;
    const Piece p = at(fr, fc);

    setPiece(fr, fc, Piece::Empty);
    setPiece(tr, tc, p);
    if (m.isCapture)
        setPiece(m.cr, m.cc, Piece::Empty);

    maybeCrown(tr, tc);

    
    if (m.isCapture)
    {
        const auto moreCaps = captureMovesFrom(tr, tc);
        if (!moreCaps.empty())
        {
            m_locked = true;
            m_lockedR = tr;
            m_lockedC = tc;
            recomputeGameOver();
            return true;
        }
    }

    
    m_locked = false;
    m_lockedR = m_lockedC = -1;
    m_turn = other(m_turn);

    recomputeGameOver();
    return true;
}
