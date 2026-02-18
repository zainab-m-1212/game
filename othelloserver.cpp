#include "othelloserver.h"
#include <QRandomGenerator>


#include <QNetworkInterface>
#include <QJsonArray>

OthelloServer::OthelloServer(QObject *parent)
    : QObject(parent)
{
    connect(&m_server, &QTcpServer::newConnection, this, &OthelloServer::onNewConnection);

    m_tick.setInterval(1000);
    connect(&m_tick, &QTimer::timeout, this, &OthelloServer::onTick);

    resetGame();
}

bool OthelloServer::start(const Settings &s, QString *error)
{
    m_settings = s;

    stop();

    if (!m_server.listen(QHostAddress::Any, m_settings.port))
    {
        const QString msg = QString("Server listen failed: %1").arg(m_server.errorString());
        if (error) *error = msg;
        emit errorOccurred(msg);
        return false;
    }

    resetGame();
    emit started();
    return true;
}

void OthelloServer::stop()
{
    m_tick.stop();
    m_gameRunning = false;

    auto closeClient = [](ClientInfo &c)
    {
        if (c.sock)
        {
            c.sock->disconnect();
            c.sock->close();
            c.sock->deleteLater();
        }
        c = ClientInfo{};
    };

    closeClient(m_black);
    closeClient(m_white);

    if (m_server.isListening())
        m_server.close();

    emit stopped();
}

QStringList OthelloServer::listenAddresses() const
{
    QStringList out;
    const auto ifaces = QNetworkInterface::allInterfaces();
    for (const auto &iface : ifaces)
    {
        if (!(iface.flags() & QNetworkInterface::IsUp) || (iface.flags() & QNetworkInterface::IsLoopBack))
            continue;

        for (const auto &entry : iface.addressEntries())
        {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
                out << entry.ip().toString();
        }
    }

    if (out.isEmpty()) out << "127.0.0.1";
    out.removeDuplicates();
    return out;
}

void OthelloServer::resetGame()
{
    m_game.reset();

    if (m_settings.randomStartingColor)
    {
        const bool blackStarts = QRandomGenerator::global()->bounded(2)== 0;
        m_settings.startingColor = blackStarts ? ReversiGame::Cell::Black : ReversiGame::Cell::White;
    }

    if (m_settings.startingColor == ReversiGame::Cell::White)
        m_game.passTurn();

    m_timeBlack = m_settings.timeLimitSeconds;
    m_timeWhite = m_settings.timeLimitSeconds;
}

void OthelloServer::onNewConnection()
{
    while (m_server.hasPendingConnections())
    {
        QTcpSocket *sock = m_server.nextPendingConnection();
        if (!sock) continue;

        if (m_black.sock && m_white.sock)
        {
            sock->close();
            sock->deleteLater();
            continue;
        }

        ClientInfo *target = nullptr;
        if (!m_black.sock) target = &m_black;
        else if (!m_white.sock) target = &m_white;

        target->sock = sock;
        target->side = (target == &m_black) ? Side::Black : Side::White;

        connect(sock, &QTcpSocket::readyRead, this, &OthelloServer::onSocketReadyRead);
        connect(sock, &QTcpSocket::disconnected, this, &OthelloServer::onSocketDisconnected);

        sendJson(sock, QJsonObject{{"type","welcome"}});
    }
}

void OthelloServer::onSocketReadyRead()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    if (!sock) return;

    auto *client = (sock == m_black.sock) ? &m_black : (sock == m_white.sock) ? &m_white : nullptr;
    if (!client) return;

    client->buffer += sock->readAll();

    while (true)
    {
        int idx = client->buffer.indexOf('\n');
        if (idx < 0) break;

        QByteArray line = client->buffer.left(idx).trimmed();
        client->buffer.remove(0, idx + 1);
        if (line.isEmpty()) continue;

        QJsonParseError pe;
        const auto doc = QJsonDocument::fromJson(line, &pe);
        if (pe.error != QJsonParseError::NoError || !doc.isObject())
            continue;

        const QJsonObject obj = doc.object();
        const QString type = obj.value("type").toString();

        if (type == "hello") handleHello(*client, obj);
        else if (type == "move") handleMove(*client, obj);
    }
}

void OthelloServer::onSocketDisconnected()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    if (!sock) return;

    bool wasRunning = m_gameRunning;

    if (sock == m_black.sock) m_black = ClientInfo{};
    if (sock == m_white.sock) m_white = ClientInfo{};

    if (wasRunning)
    {
        Side winner = Side::Unknown;
        if (!m_black.sock && m_white.sock) winner = Side::White;
        else if (!m_white.sock && m_black.sock) winner = Side::Black;
        finishGame("disconnect", winner);
    }
}

void OthelloServer::onTick()
{
    if (!m_gameRunning || m_settings.timeLimitSeconds <= 0) return;

    const Side turnSide = cellToSide(m_game.currentPlayer());

    if (turnSide == Side::Black) m_timeBlack = qMax(0, m_timeBlack - 1);
    if (turnSide == Side::White) m_timeWhite = qMax(0, m_timeWhite - 1);

    if (m_timeBlack == 0 || m_timeWhite == 0)
    {
        const Side winner = (m_timeBlack == 0) ? Side::White : Side::Black;
        finishGame("timeout", winner);
        return;
    }

    sendState();
}

QString OthelloServer::sideToString(Side s)
{
    switch (s)
    {
    case Side::Black: return "black";
    case Side::White: return "white";
    default: return "unknown";
    }
}

OthelloServer::Side OthelloServer::cellToSide(ReversiGame::Cell c)
{
    return (c == ReversiGame::Cell::Black) ? Side::Black : (c == ReversiGame::Cell::White) ? Side::White : Side::Unknown;
}

ReversiGame::Cell OthelloServer::sideToCell(Side s)
{
    return (s == Side::Black) ? ReversiGame::Cell::Black : (s == Side::White) ? ReversiGame::Cell::White : ReversiGame::Cell::Empty;
}

void OthelloServer::sendJson(QTcpSocket *sock, const QJsonObject &obj)
{
    if (!sock) return;
    const QByteArray payload = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\n";
    sock->write(payload);
    sock->flush();
}

void OthelloServer::broadcast(const QJsonObject &obj)
{
    sendJson(m_black.sock, obj);
    sendJson(m_white.sock, obj);
}

void OthelloServer::maybeStartIfReady()
{
    if (m_gameRunning) return;
    if (!m_black.sock || !m_white.sock) return;
    if (m_black.username.isEmpty() || m_white.username.isEmpty()) return;

    m_gameRunning = true;
    if (m_settings.timeLimitSeconds > 0) m_tick.start();

    sendAssignAndInitialState();
}

void OthelloServer::sendAssignAndInitialState()
{
    sendJson(m_black.sock, QJsonObject
    {
        {"type","assign"},
        {"side","black"},
        {"opponent", m_white.username},
        {"timeLimitSeconds", m_settings.timeLimitSeconds}
    });
    sendJson(m_white.sock, QJsonObject
    {
        {"type","assign"},
        {"side","white"},
        {"opponent", m_black.username},
        {"timeLimitSeconds", m_settings.timeLimitSeconds}
    });

    sendState();
}

void OthelloServer::sendState()
{
    QJsonArray board;
    for (int r = 0; r < 8; ++r)
    {
        for (int c = 0; c < 8; ++c)
        {
            const auto cell = m_game.at(r,c);
            if (cell == ReversiGame::Cell::Black) board.append("b");
            else if (cell == ReversiGame::Cell::White) board.append("w");
            else board.append("e");
        }
    }

    QJsonArray valid;
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            if (m_game.isValidMove(r,c,m_game.currentPlayer()))
                valid.append(QJsonObject{{"r",r},{"c",c}});

    const int b = m_game.count(ReversiGame::Cell::Black);
    const int w = m_game.count(ReversiGame::Cell::White);

    broadcast(QJsonObject
    {
        {"type","state"},
        {"board", board},
        {"turn", sideToString(cellToSide(m_game.currentPlayer()))},
        {"black", b},
        {"white", w},
        {"valid", valid},
        {"timeBlack", m_timeBlack},
        {"timeWhite", m_timeWhite}
    });

    if (m_game.isGameOver())
    {
        Side winner = Side::Unknown;
        if (b > w) winner = Side::Black;
        else if (w > b) winner = Side::White;
        finishGame("normal", winner);
    }
}

void OthelloServer::finishGame(const QString &reason, Side winnerSide)
{
    if (!m_gameRunning) return;

    m_gameRunning = false;
    m_tick.stop();

    const int b = m_game.count(ReversiGame::Cell::Black);
    const int w = m_game.count(ReversiGame::Cell::White);

    broadcast(QJsonObject
    {
        {"type","gameOver"},
        {"reason", reason},
        {"winner", sideToString(winnerSide)},
        {"black", b},
        {"white", w}
    });
}

void OthelloServer::handleHello(ClientInfo &c, const QJsonObject &o)
{
    c.username = o.value("username").toString().trimmed();
    if (c.username.isEmpty()) c.username = "player";

    maybeStartIfReady();
}

void OthelloServer::handleMove(ClientInfo &c, const QJsonObject &o)
{
    if (!m_gameRunning) return;

    const Side turnSide = cellToSide(m_game.currentPlayer());
    if (c.side != turnSide)
    {
        sendJson(c.sock, QJsonObject{{"type","invalid"},{"reason","not_your_turn"}});
        return;
    }

    const int r = o.value("r").toInt(-1);
    const int cc = o.value("c").toInt(-1);
    if (!m_game.isInside(r, cc))
    {
        sendJson(c.sock, QJsonObject{{"type","invalid"},{"reason","out_of_range"}});
        return;
    }

    if (!m_game.playMove(r, cc))
    {
        sendJson(c.sock, QJsonObject{{"type","invalid"},{"reason","invalid_move"}});
        return;
    }

    if (!m_game.hasAnyValidMove(m_game.currentPlayer()))
        m_game.passTurn();

    sendState();
}
