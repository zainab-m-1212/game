#include "othelloclient.h"

#include <QJsonDocument>

OthelloClient::OthelloClient(QObject *parent)
    : QObject(parent)
{
    connect(&m_sock, &QTcpSocket::connected, this, &OthelloClient::connected);
    connect(&m_sock, &QTcpSocket::disconnected, this, &OthelloClient::disconnected);
    connect(&m_sock, &QTcpSocket::readyRead, this, &OthelloClient::onReadyRead);
    connect(&m_sock,
            &QTcpSocket::errorOccurred,
            this,
            &OthelloClient::onSocketError);

    //connect(&m_sock, qOverload<QAbstractSocket::SocketError>(&QTcpSocket::error),this, &OthelloClient::onSocketError);

}

void OthelloClient::connectToHost(const QString &host, quint16 port)
{
    m_buffer.clear();
    m_sock.connectToHost(host, port);
}

void OthelloClient::disconnectFromHost()
{
    m_sock.disconnectFromHost();
}

void OthelloClient::sendHello(const QString &username)
{
    sendJson(QJsonObject{{"type","hello"},{"username", username}});
}

void OthelloClient::sendMove(int r, int c)
{
    sendJson(QJsonObject{{"type","move"},{"r", r},{"c", c}});
}

void OthelloClient::sendJson(const QJsonObject &obj)
{
    if (m_sock.state() != QAbstractSocket::ConnectedState) return;
    m_sock.write(QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n");
    m_sock.flush();
}

void OthelloClient::onReadyRead()
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

void OthelloClient::onSocketError(QAbstractSocket::SocketError)
{
    emit errorOccurred(m_sock.errorString());
}
