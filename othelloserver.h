#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHostAddress>

#include "reversigame.h"

class OthelloServer : public QObject
{
    Q_OBJECT
public:
    struct Settings
    {
        int timeLimitSeconds = 0;
        bool randomStartingColor = true;
        ReversiGame::Cell startingColor = ReversiGame::Cell::Black;
        quint16 port = 45454;
    };

    explicit OthelloServer(QObject *parent = nullptr);

    bool start(const Settings &s, QString *error = nullptr);
    void stop();

    QStringList listenAddresses() const;
    quint16 port() const { return m_settings.port; }

signals:
    void started();
    void stopped();
    void errorOccurred(const QString &msg);

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();
    void onTick();

private:
    enum class Side { Unknown, Black, White };

    struct ClientInfo
    {
        QTcpSocket *sock = nullptr;
        QString username;
        Side side = Side::Unknown;
        QByteArray buffer;
    };

    void resetGame();

    void sendJson(QTcpSocket *sock, const QJsonObject &obj);
    void broadcast(const QJsonObject &obj);

    static QString sideToString(Side s);
    static Side cellToSide(ReversiGame::Cell c);
    static ReversiGame::Cell sideToCell(Side s);

    void maybeStartIfReady();
    void sendAssignAndInitialState();
    void sendState();
    void finishGame(const QString &reason, Side winnerSide);

    void handleHello(ClientInfo &c, const QJsonObject &o);
    void handleMove(ClientInfo &c, const QJsonObject &o);

private:
    QTcpServer m_server;
    Settings m_settings;

    ReversiGame m_game;

    ClientInfo m_black;
    ClientInfo m_white;

    bool m_gameRunning = false;

    QTimer m_tick;
    int m_timeBlack = 0;
    int m_timeWhite = 0;
};
