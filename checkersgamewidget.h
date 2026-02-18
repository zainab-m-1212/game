#ifndef CHECKERSGAMEWIDGET_H
#define CHECKERSGAMEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QJsonObject>
#include <vector>

#include "checkersgame.h"

namespace Ui { class CheckersGameWidget; }

class CheckersGameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CheckersGameWidget(QWidget *parent = nullptr);
    ~CheckersGameWidget() override;

    void resetGame();

    void setNetworkMode(bool enabled);
    void setMySide(const QString &side); 
    void applyNetworkState(const QJsonObject &state);

signals:
    void networkMoveRequested(int fr, int fc, int tr, int tc);
    void gameFinished(int redPieces, int blackPieces, const QString &winnerText);

private slots:
    void onResetClicked();
    void onCellClicked(int r, int c);

private:
    void applyUiStyling();
    void buildBoardButtons();
    void refreshUI();
    void refreshFromNetwork();

    QString sideText(const QString &s) const;
    QString localSideText(CheckersGame::Side s) const;

    void clearSelection();
    void setSelection(int r, int c);
    void rebuildHints();

private:
    Ui::CheckersGameWidget *ui = nullptr;
    CheckersGame m_game;
    std::vector<std::vector<QPushButton*>> m_buttons;

    
    bool m_hasSel = false;
    int m_selR = -1;
    int m_selC = -1;

    
    bool m_networkMode = false;
    QString m_mySide;     
    QString m_turnSide;   
    QStringList m_validMovesKey; 
    QStringList m_validFromKey;  
    bool m_locked = false;
    int m_lockedR = -1;
    int m_lockedC = -1;

    int m_redPieces = 12;
    int m_blackPieces = 12;
    int m_timeRed = 0;
    int m_timeBlack = 0;

    bool m_gameOverShown = false;
};

#endif 
