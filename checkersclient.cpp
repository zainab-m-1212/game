#include "checkersclient.h"
#include <QRandomGenerator>

#include <QJsonDocument>

CheckersClient::CheckersClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_sock, &QTcpSocket::connected, this, &CheckersClient::connected);
    connect(&m_sock, &QTcpSocket::disconnected, this, &CheckersClient::disconnected);
    connect(&m_sock, &QTcpSocket::readyRead, this, &CheckersClient::onReadyRead);
    connect(&m_sock, &QTcpSocket::errorOccurred,
            this, &CheckersClient::onSocketError);

}

void CheckersClient::connectToHost(const QString &host, quint16 port)
{
    m_buffer.clear();
    m_sock.connectToHost(host, port);
}

void CheckersClient::disconnectFromHost()
{
    m_sock.disconnectFromHost();
}

void CheckersClient::sendHello(const QString &username)
{
    sendJson(QJsonObject{{"type","hello"},{"username", username}});
}

void CheckersClient::sendMove(int fr, int fc, int tr, int tc)
{
    sendJson(QJsonObject{{"type","move"},{"fr", fr},{"fc", fc},{"tr", tr},{"tc", tc}});
}

void CheckersClient::sendJson(const QJsonObject &obj)
{
    if (m_sock.state() != QAbstractSocket::ConnectedState) return;
    m_sock.write(QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n");
    m_sock.flush();
}

void CheckersClient::onReadyRead()
{
    m_buffer += m_sock.readAll();

    while (true)
    {
        int idx = m_buffer.indexOf('\n');
        if (idx < 0) break;

        QByteArray line = m_buffer.left(idx).trimmed();
        m_buffer.remove(0, idx + 1);
        if (line.isEmpty()) continue;

        QJsonParseError pe;
        const auto doc = QJsonDocument::fromJson(line, &pe);
        if (pe.error != QJsonParseError::NoError || !doc.isObject())
            continue;

        const QJsonObject obj = doc.object();
        const QString type = obj.value("type").toString();

        if (type == "welcome")
        {}
        else if (type == "assign")
        {
            emit assigned(obj.value("side").toString(), obj.value("opponent").toString(), obj.value("timeLimitSeconds").toInt());
        }
        else if (type == "state")
        {
            emit stateReceived(obj);
        }
        else if (type == "invalid")
        {
            emit invalidMove(obj.value("reason").toString());
        }
        else if (type == "gameOver")
        {
            emit gameOver(obj);
        }
    }
}

void CheckersClient::onSocketError(QAbstractSocket::SocketError)
{
    emit errorOccurred(m_sock.errorString());
}
