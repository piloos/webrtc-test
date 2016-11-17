#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include <QTimer>
#include <wsclient.h>
#include <webrtcendpoint.h>
#include <gst/gst.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Gstreamer WebRTC example");
    parser.addHelpOption();

    WsClient client(QUrl(QStringLiteral("wss://localhost:1234")));
    //QObject::connect(&client, &WsClient::closed, &a, &QCoreApplication::quit);

    gst_init(&argc, &argv);

    WebRTCEndpoint webrtcendpoint;

    QObject::connect(&client, SIGNAL(textMessageReceived(QString)), &webrtcendpoint, SLOT(onMessageReceived(QString)));
    QObject::connect(&webrtcendpoint, SIGNAL(sendMessage(QString)), &client, SLOT(sendMessage(QString)));

    //uncomment next call if webrtcendpoint needs to be the initiator (the one who generates the SDP offer) of the webrtc connection
    //QTimer::singleShot(1000, &webrtcendpoint, SLOT(start_session(void)));

    //QTimer::singleShot(15000, &webrtcendpoint, SLOT(dump_pipeline()));

    return a.exec();

    gst_deinit();
}
