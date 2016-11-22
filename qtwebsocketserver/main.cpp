#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include "echoclient.h"
#include "echoserver.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("QtWebSockets example: echoclient");
    parser.addHelpOption();

    QCommandLineOption dbgOption(QStringList() << "d" << "debug",
            QCoreApplication::translate("main", "Debug output [default: off]."));
    parser.addOption(dbgOption);

    QCommandLineOption portOption(QStringList() << "p" << "port",
            QCoreApplication::translate("main", "Port for echoserver [default: 1234]."),
            QCoreApplication::translate("main", "port"), QLatin1Literal("1234"));
    parser.addOption(portOption);

    QCommandLineOption secureOption(QStringList() << "s" << "secure",
            QCoreApplication::translate("main", "secure sockets [default: off]."));
    parser.addOption(secureOption);

    parser.process(a);
    int debug = parser.isSet(dbgOption);
    int secure = parser.isSet(secureOption);
    int port = parser.value(portOption).toInt();

    EchoServer *server = new EchoServer(port, debug, secure, QString("../certs/server.pem"), QString("../certs/server.key"));
    QObject::connect(server, &EchoServer::closed, &a, &QCoreApplication::quit);

    //QString url = (secure == true) ? QString("wss://localhost:1234") : QString("ws://localhost:1234");
    //EchoClient client(QUrl(url), debug);
    //QObject::connect(&client, &EchoClient::closed, &a, &QCoreApplication::quit);

    return a.exec();
}
