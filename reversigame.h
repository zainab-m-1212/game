#pragma once
#include <array>
#include <vector>

class ReversiGame {
public:
    enum class Cell { Empty, Black, White };

    ReversiGame();

    void reset();
    Cell currentPlayer() const { return m_current; }
    Cell opponent(Cell p) const { return (p == Cell::Black) ? Cell::White : Cell::Black; }

    Cell at(int r, int c) const { return m_board[r][c]; }

    bool isInside(int r, int c) const { return r >= 0 && r < 8 && c >= 0 && c < 8; }

    std::vector<std::pair<int,int>> flipsForMove(int r, int c, Cell player) const;

    bool isValidMove(int r, int c, Cell player) const;

    bool playMove(int r, int c);

    bool hasAnyValidMove(Cell player) const;
    bool isGameOver() const;

    int count(Cell who) const;

    void passTurn() { m_current = opponent(m_current); }

private:
    std::array<std::array<Cell, 8>, 8> m_board{};
    Cell m_current = Cell::Black;

    std::vector<std::pair<int,int>> flipsInDirection(int r, int c, int dr, int dc, Cell player) const;
};
