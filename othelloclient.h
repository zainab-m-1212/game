#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class OthelloClient : public QObject
{
    Q_OBJECT
public:
    explicit OthelloClient(QObject *parent = nullptr);

    void connectToHost(const QString &host, quint16 port);
    void disconnectFromHost();

    void sendHello(const QString &username);
    void sendMove(int r, int c);

signals:
    void connected();
    void disconnected();
    void errorOccurred(const QString &msg);

    void assigned(const QString &side, const QString &opponent, int timeLimitSeconds);
    void stateReceived(const QJsonObject &state);
    void invalidMove(const QString &reason);
    void gameOver(const QJsonObject &msg);

private slots:
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError);

private:
    void sendJson(const QJsonObject &obj);

private:
    QTcpSocket m_sock;
    QByteArray m_buffer;
};
