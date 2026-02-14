#include "reversigamewindow.h"
#include "ui_reversigamewindow.h"
#include <QMessageBox>

ReversiGameWindow::ReversiGameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ReversiGameWindow)
{
    ui->setupUi(this);

    connect(ui->resetButton, &QPushButton::clicked,this, &ReversiGameWindow::onResetClicked);

    buildBoardButtons();
    refreshUI();
}


ReversiGameWindow::~ReversiGameWindow()
{
    delete ui;
}

QString ReversiGameWindow::playerName(ReversiGame::Cell p) const
{
    return (p == ReversiGame::Cell::Black) ? "Black" : "White";
}

void ReversiGameWindow::buildBoardButtons()
{
    m_buttons.resize(8);
    for (int r = 0; r < 8; ++r)
    {
        m_buttons[r].resize(8);
        for (int c = 0; c < 8; ++c)
        {
            auto *btn = new QPushButton(this);
            btn->setFixedSize(48, 48);
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setText("");

            btn->setStyleSheet("background:#2E7D32; border:1px solid #1B5E20;");

            connect(btn, &QPushButton::clicked, this, [this, r, c](){onCellClicked(r, c);});

            ui->boardGrid->addWidget(btn, r, c);
            m_buttons[r][c] = btn;
        }
    }
}

void ReversiGameWindow::refreshUI()
{
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            auto cell = m_game.at(r, c);
            auto *btn = m_buttons[r][c];

            QString piece;
            QString style = "background:#2E7D32; font-size:22px; border:1px solid #1B5E20;";

            if (cell == ReversiGame::Cell::Black)
            {
                piece = "●";
                style += "color:black;";
            }
            else if (cell == ReversiGame::Cell::White)
            {
                piece = "○";
                style += "color:white;";
            }
            else
            {
                piece = "";
                if (m_game.isValidMove(r, c, m_game.currentPlayer()))
                {
                    style += "border:2px solid #FFD54F;";
                }
                style += "color:white;";
            }

            btn->setText(piece);
            btn->setStyleSheet(style);
        }
    }

    int b = m_game.count(ReversiGame::Cell::Black);
    int w = m_game.count(ReversiGame::Cell::White);

    ui->scoreLabel->setText(QString("Score  Black: %1   White: %2").arg(b).arg(w));

    if (m_game.isGameOver())
    {
        QString winner;
        if (b > w) winner = "Black wins!";
        else if (w > b) winner = "White wins!";
        else winner = "Draw!";

        ui->statusLabel->setText("Game Over - " + winner);

        QMessageBox::information(this, "Game Over",QString("Final Score\nBlack: %1\nWhite: %2\n\n%3").arg(b).arg(w).arg(winner));
        return;
    }

    ui->statusLabel->setText("Turn: " + playerName(m_game.currentPlayer()));
}

void ReversiGameWindow::applyPassLogicIfNeeded()
{
    if (!m_game.hasAnyValidMove(m_game.currentPlayer()))
    {
        auto cur = m_game.currentPlayer();
        m_game.passTurn();

        QMessageBox::information(this, "Pass",QString("%1 has no valid moves. Turn passed to %2.").arg(playerName(cur)).arg(playerName(m_game.currentPlayer())));
    }
}

void ReversiGameWindow::onCellClicked(int r, int c)
{
    if (m_game.isGameOver()) return;

    if (!m_game.playMove(r, c))
    {
        return;
    }

    applyPassLogicIfNeeded();
    refreshUI();
}

void ReversiGameWindow::onResetClicked()
{
    m_game.reset();
    refreshUI();
}
