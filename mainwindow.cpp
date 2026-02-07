#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QTableWidgetItem>
#include <QDateTime>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QGridLayout* grid =
        qobject_cast<QGridLayout*>(ui->boardWidget->layout());

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {

            QPushButton* btn = new QPushButton(this);
            btn->setFixedSize(60, 60);

            btn->setProperty("row", r);
            btn->setProperty("col", c);

            grid->addWidget(btn, r, c);
            cells[r][c] = btn;

            connect(btn, &QPushButton::clicked,
                    this, &MainWindow::handleCellClicked);
        }
    }

    initBoard();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::initBoard()
{
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            board[r][c] = None;
            cells[r][c]->setText("");
        }

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 8; c++)
            if ((r + c) % 2 == 1) {
                board[r][c] = Red;
                cells[r][c]->setText("🔴");
            }

    for (int r = 5; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if ((r + c) % 2 == 1) {
                board[r][c] = Black;
                cells[r][c]->setText("⚫");
            }

    resetCellColors();
}



bool MainWindow::isInside(int r, int c)
{
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

void MainWindow::resetCellColors()
{
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            if ((r + c) % 2 == 0)
                cells[r][c]->setStyleSheet("background:#EEE;");
            else
                cells[r][c]->setStyleSheet("background:#444;");
        }
}



void MainWindow::showValidMoves(int r, int c)
{
    Piece p = board[r][c];
    if (p == None) return;

    QVector<int> dirs;
    if (p == Red) dirs = { -1 };
    else if (p == Black) dirs = { 1 };
    else dirs = { -1, 1 };

    for (int d : dirs)
        for (int dc : { -1, 1 }) {
            int nr = r + d;
            int nc = c + dc;
            if (isInside(nr, nc) && board[nr][nc] == None)
                cells[nr][nc]->setStyleSheet("background:green;");
        }
}


void MainWindow::handleCellClicked()
{
    QPushButton* btn =
        qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int r = btn->property("row").toInt();
    int c = btn->property("col").toInt();

    resetCellColors();

    if (!hasSelection && board[r][c] != None) {
        hasSelection = true;
        selectedRow = r;
        selectedCol = c;
        cells[r][c]->setStyleSheet("background:yellow;");
        showValidMoves(r, c);
        return;
    }

    if (hasSelection && board[r][c] == None) {

        int dr = r - selectedRow;
        int dc = c - selectedCol;
        int dir = (board[selectedRow][selectedCol] == Red) ? -1 : 1;

        if (dr == dir && abs(dc) == 1) {
            board[r][c] = board[selectedRow][selectedCol];
            board[selectedRow][selectedCol] = None;

            cells[r][c]->setText(cells[selectedRow][selectedCol]->text());
            cells[selectedRow][selectedCol]->setText("");

            redTurn = !redTurn;
        }

        hasSelection = false;
    }

    resetCellColors();
    isGameOver();
}


bool MainWindow::hasAnyMove(Piece player)
{
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (board[r][c] == player)
                return true;
    return false;
}

bool MainWindow::isGameOver()
{
    bool red = false, black = false;

    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            if (board[r][c] == Red) red = true;
            if (board[r][c] == Black) black = true;
        }

    if (!red || !black) {
        QMessageBox::information(
            this, "Game Over",
            red ? "Red wins!" : "Black wins!");
        return true;
    }
    return false;
}
