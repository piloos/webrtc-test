#ifndef WSCLIENT_H
#define WSCLIENT_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>

class WsClient : public QObject
{
    Q_OBJECT

public:
    explicit WsClient(const QUrl &url, QObject *parent = Q_NULLPTR);
    virtual ~WsClient();

signals:
    void textMessageReceived(QString message);
    void closed();

public slots:
    void sendMessage(QString message);
    void onSslErrors(const QList<QSslError> &errors);

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
};

#endif // WSCLIENT_H
