#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QVector>
#include "reversigame.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReversiGameWindow; }
QT_END_NAMESPACE

class ReversiGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReversiGameWindow(QWidget *parent = nullptr);
    ~ReversiGameWindow();

private slots:
    void onResetClicked();

private:
    Ui::ReversiGameWindow *ui = nullptr;

    ReversiGame m_game;
    QVector<QVector<QPushButton*>> m_buttons;

    void buildBoardButtons();
    void refreshUI();
    void onCellClicked(int r, int c);
    void applyPassLogicIfNeeded();
    QString playerName(ReversiGame::Cell p) const;
};
