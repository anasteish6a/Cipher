#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QStringList>
#include "database.h"

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    explicit MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();

private slots:
    void slotNewConnection();
    void slotServerRead();
    void slotClientDisconnected();

private:
    QTcpServer* mTcpServer;
    QList<QTcpSocket*> mClients;

    void parseAndHandleCommand(QTcpSocket* socket, const QString& command);

    bool checkUser(const QString& username, const QString& password);
    bool registerUser(const QString& username, const QString& password);

    void handleLogin(QTcpSocket* socket, const QStringList& params);
    void handleRegister(QTcpSocket* socket, const QStringList& params);
    void handleLogout(QTcpSocket* socket, const QStringList& params);
    void handleGetData(QTcpSocket* socket, const QStringList& params);


    void handleVigenereEncrypt(QTcpSocket* socket, const QStringList& params);
    void handleVigenereDecrypt(QTcpSocket* socket, const QStringList& params);
    void handleSha512(QTcpSocket* socket, const QStringList& params);
    void handleBisection(QTcpSocket* socket, const QStringList& params);
    void handleShortestPath(QTcpSocket* socket, const QStringList& params);
};

#endif
