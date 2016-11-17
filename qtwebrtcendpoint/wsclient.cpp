#include "wsclient.h"
#include <QtCore/QDebug>

WsClient::WsClient(const QUrl &url, QObject *parent) :
    QObject(parent),
    m_url(url)
{
    //qDebug() << "WebSocket server:" << url;
    connect(&m_webSocket, &QWebSocket::connected, this, &WsClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &WsClient::closed);
    typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
    connect(&m_webSocket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors),
                this, &WsClient::onSslErrors);
    m_webSocket.open(QUrl(url));
}

WsClient::~WsClient() {}

void WsClient::onConnected()
{
    //qDebug() << "WebSocket connected";
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WsClient::onTextMessageReceived);
}

void WsClient::onTextMessageReceived(QString message)
{
    //qDebug() << "Client Message received:" << message;
    emit(textMessageReceived(message));
}

void WsClient::sendMessage(QString message)
{
    m_webSocket.sendTextMessage(message);
}

void WsClient::onSslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    m_webSocket.ignoreSslErrors();
}
