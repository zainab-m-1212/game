#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleCellClicked();

private:
    Ui::MainWindow *ui;


    QPushButton* cells[8][8];


    enum Piece {
        None,
        Red,
        Black,
        RedKing,
        BlackKing
    };

    Piece board[8][8];
    bool redTurn = true;

    bool hasSelection = false;
    int selectedRow = -1;
    int selectedCol = -1;


    void initBoard();
    bool isInside(int row, int col);
    bool canCaptureFrom(int row, int col);
    bool hasAnyMove(Piece player);
    bool isGameOver();
};

#endif // MAINWINDOW_H
