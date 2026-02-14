#include "reversigame.h"

ReversiGame::ReversiGame()
{
    reset();
}

void ReversiGame::reset()
{
    for (auto &row : m_board)
        row.fill(Cell::Empty);

    m_board[3][3] = Cell::White;
    m_board[3][4] = Cell::Black;
    m_board[4][3] = Cell::Black;
    m_board[4][4] = Cell::White;

    m_current = Cell::Black;
}

std::vector<std::pair<int,int>> ReversiGame::flipsInDirection(int r, int c, int dr, int dc, Cell player) const {
    std::vector<std::pair<int,int>> flips;
    int rr = r + dr, cc = c + dc;

    Cell opp = opponent(player);


    while (isInside(rr, cc) && m_board[rr][cc] == opp)
    {
        flips.push_back({rr, cc});
        rr += dr;
        cc += dc;
    }

    if (!flips.empty() && isInside(rr, cc) && m_board[rr][cc] == player)
    {
        return flips;
    }

    return {};
}

std::vector<std::pair<int,int>> ReversiGame::flipsForMove(int r, int c, Cell player) const
{
    if (!isInside(r,c)) return {};
    if (m_board[r][c] != Cell::Empty) return {};

    std::vector<std::pair<int,int>> all;

    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            if (dr == 0 && dc == 0) continue;
            auto part = flipsInDirection(r, c, dr, dc, player);
            all.insert(all.end(), part.begin(), part.end());
        }
    }
    return all;
}

bool ReversiGame::isValidMove(int r, int c, Cell player) const
{
    auto flips = flipsForMove(r, c, player);
    return !flips.empty();
}

bool ReversiGame::playMove(int r, int c)
{
    auto flips = flipsForMove(r, c, m_current);
    if (flips.empty()) return false;

    m_board[r][c] = m_current;


    for (auto &p : flips)
    {
        m_board[p.first][p.second] = m_current;
    }

    m_current = opponent(m_current);
    return true;
}

bool ReversiGame::hasAnyValidMove(Cell player) const
{
    for (int r=0; r<8; r++)
    {
        for (int c=0; c<8; c++)
        {
            if (isValidMove(r, c, player))
                return true;
        }
    }
    return false;
}

bool ReversiGame::isGameOver() const
{
    bool anyEmpty = false;
    for (int r=0; r<8; r++)
        for (int c=0; c<8; c++)
            if (m_board[r][c] == Cell::Empty) anyEmpty = true;

    if (!anyEmpty) return true;

    if (!hasAnyValidMove(Cell::Black) && !hasAnyValidMove(Cell::White))
        return true;

    return false;
}

int ReversiGame::count(Cell who) const
{
    int cnt = 0;
    for (int r=0; r<8; r++)
        for (int c=0; c<8; c++)
            if (m_board[r][c] == who) cnt++;
    return cnt;
}
