#include "reversigamewidget.h"
#include "ui_reversigamewidget.h"
#include <QMessageBox>
#include <QSizePolicy>
#include <QStyle>
#include <QApplication>
#include <QJsonArray>

ReversiGameWidget::ReversiGameWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ReversiGameWidget)
{
    ui->setupUi(this);

    applyUiStyling();

    connect(ui->resetButton, &QPushButton::clicked,
            this, &ReversiGameWidget::onResetClicked);

    buildBoardButtons();
    refreshUI();
}

ReversiGameWidget::~ReversiGameWidget()
{
    delete ui;
}

void ReversiGameWidget::applyUiStyling()
{
    const char *qss = R"QSS(
QWidget#ReversiGameWidget {
    background: transparent;
}

QLabel#statusLabel {
    font-size: 12pt;
    font-weight: 700;
}
QLabel#scoreLabel {
    color: #A0A3BD;
    font-weight: 600;
}

QPushButton#resetButton {
    background: #7C83FF;
    color: white;
    border: none;
    border-radius: 12px;
    padding: 8px 12px;
    font-weight: 700;
}
QPushButton#resetButton:hover { background: #5F66FF; }
QPushButton#resetButton:pressed { background: #4B50D6; }

QFrame#boardCard {
    background: #2A2C3F;
    border: 1px solid #3A3D5A;
    border-radius: 18px;
}

QPushButton#cellBtn {
    background: #2E7D32;
    border: 2px solid #1B5E20;
    border-radius: 12px;
    font-size: 22px;
    font-weight: 800;
}
QPushButton#cellBtn[hint="true"] {
    border: 3px solid #FFD54F;
}
QPushButton#cellBtn[cell="black"] { color: #000000; }
QPushButton#cellBtn[cell="white"] { color: #FFFFFF; }
)QSS";

    qApp->setStyleSheet(qApp->styleSheet() + QString::fromUtf8(qss));
}

QString ReversiGameWidget::playerName(ReversiGame::Cell p) const
{
    return (p == ReversiGame::Cell::Black) ? "Black" : "White";
}

void ReversiGameWidget::buildBoardButtons()
{
    m_buttons.resize(8);
    for (int r = 0; r < 8; ++r)
    {
        m_buttons[r].resize(8);
        for (int c = 0; c < 8; ++c)
        {
            auto *btn = new QPushButton(this);
            btn->setObjectName("cellBtn");

            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            btn->setMinimumSize(44, 44);
            btn->setFocusPolicy(Qt::NoFocus);
            btn->setText("");

            btn->setProperty("cell", "empty");
            btn->setProperty("hint", false);

            connect(btn, &QPushButton::clicked, this, [this, r, c]() {onCellClicked(r, c);});

            ui->boardGrid->addWidget(btn, r, c);
            m_buttons[r][c] = btn;
        }
    }
}

void ReversiGameWidget::refreshUI()
{
    if (m_networkMode)
    {
        refreshFromNetwork();
        return;
    }

    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            auto cell = m_game.at(r, c);
            auto *btn = m_buttons[r][c];

            btn->setProperty("hint", false);

            if (cell == ReversiGame::Cell::Black)
            {
                btn->setText("●");
                btn->setProperty("cell", "black");
            }
            else if (cell == ReversiGame::Cell::White)
            {
                btn->setText("○");
                btn->setProperty("cell", "white");
            }
            else
            {
                btn->setText("");
                btn->setProperty("cell", "empty");
                if (m_game.isValidMove(r, c, m_game.currentPlayer()))
                {
                    btn->setProperty("hint", true);
                }
            }

            btn->style()->polish(btn);
            btn->update();
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

        if (!m_gameOverShown)
        {
            m_gameOverShown = true;
            QMessageBox::information(
                this, "Game Over",
                QString("Final Score\nBlack: %1\nWhite: %2\n\n%3").arg(b).arg(w).arg(winner)
            );
            emit gameFinished(b, w, winner);
        }
        return;
    }

    ui->statusLabel->setText("Turn: " + playerName(m_game.currentPlayer()));
}

void ReversiGameWidget::applyPassLogicIfNeeded()
{
    if (!m_game.hasAnyValidMove(m_game.currentPlayer()))
    {
        auto cur = m_game.currentPlayer();
        m_game.passTurn();

        QMessageBox::information(
            this, "Pass",
            QString("%1 has no valid moves. Turn passed to %2.")
                .arg(playerName(cur))
                .arg(playerName(m_game.currentPlayer()))
        );
    }
}

void ReversiGameWidget::onCellClicked(int r, int c)
{
    if (m_networkMode)
    {
        if (m_turnSide != m_mySide) return;
        const QString key = QString::number(r) + "," + QString::number(c);
        if (!m_validKey.contains(key)) return;
        emit networkMoveRequested(r, c);
        return;
    }

    if (m_game.isGameOver()) return;

    if (!m_game.playMove(r, c))
    {
        return;
    }

    applyPassLogicIfNeeded();
    refreshUI();
}

void ReversiGameWidget::onResetClicked()
{
    resetGame();
}

void ReversiGameWidget::resetGame()
{
    m_gameOverShown = false;
    if (!m_networkMode)
        m_game.reset();
    refreshUI();
}

void ReversiGameWidget::setNetworkMode(bool enabled)
{
    m_networkMode = enabled;
    m_gameOverShown = false;
    refreshUI();
}

void ReversiGameWidget::setMySide(const QString &side)
{
    m_mySide = side;
}

void ReversiGameWidget::applyNetworkState(const QJsonObject &state)
{
    m_turnSide = state.value("turn").toString();
    m_blackScore = state.value("black").toInt();
    m_whiteScore = state.value("white").toInt();
    m_timeBlack = state.value("timeBlack").toInt();
    m_timeWhite = state.value("timeWhite").toInt();

    m_validKey.clear();
    const auto valid = state.value("valid").toArray();
    for (const auto &v : valid)
    {
        const auto o = v.toObject();
        m_validKey << (QString::number(o.value("r").toInt()) + "," + QString::number(o.value("c").toInt()));
    }

    const auto board = state.value("board").toArray();
    if (board.size() == 64)
    {
        int idx = 0;
        for (int r = 0; r < 8; ++r)
        {
            for (int c = 0; c < 8; ++c)
            {
                const QString s = board.at(idx++).toString();
                auto *btn = m_buttons[r][c];

                btn->setProperty("hint", false);

                if (s == "b")
                {
                    btn->setText("●");
                    btn->setProperty("cell", "black");
                }
                else if (s == "w")
                {
                    btn->setText("○");
                    btn->setProperty("cell", "white");
                }
                else
                {
                    btn->setText("");
                    btn->setProperty("cell", "empty");

                    const QString key = QString::number(r) + "," + QString::number(c);
                    if (m_validKey.contains(key))
                        btn->setProperty("hint", true);
                }

                btn->style()->polish(btn);
                btn->update();
            }
        }
    }

    refreshUI();
}

static QString fmtTime(int sec)
{
    if (sec <= 0) return "--:--";
    const int m = sec / 60;
    const int s = sec % 60;
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

void ReversiGameWidget::refreshFromNetwork()
{
    ui->scoreLabel->setText(QString("Score  Black: %1   White: %2").arg(m_blackScore).arg(m_whiteScore));

    QString turnText = (m_turnSide == "black") ? "Black" : (m_turnSide == "white") ? "White" : "?";
    QString myText = (m_mySide == "black") ? "Black" : (m_mySide == "white") ? "White" : "?";

    QString timeText;
    if (m_timeBlack > 0 || m_timeWhite > 0)
        timeText = QString(" | Time  B:%1  W:%2").arg(fmtTime(m_timeBlack)).arg(fmtTime(m_timeWhite));

    ui->statusLabel->setText(QString("Turn: %1 (You: %2)%3").arg(turnText).arg(myText).arg(timeText));
}
