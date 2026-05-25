#include "mytcpserver.h"
#include "tasks.h"
#include <QDebug>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QCryptographicHash>

MyTcpServer::~MyTcpServer()
{
    for (QTcpSocket* client : mClients) {
        client->close();
    }
    mTcpServer->close();
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    
    Database::getInstance();  

    qDebug() << "База данных инициализирована через синглтон";

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "Ошибка: сервер не запущен на порту 33333";
    } else {
        qDebug() << "Сервер успешно запущен на порту 33333";
    }
}


bool MyTcpServer::checkUser(const QString& username, const QString& password) {
    Database* db = Database::getInstance();
    
    QString queryStr = QString("SELECT userID FROM Users WHERE login = '%1' AND password = '%2'")
        .arg(username, password);
    QSqlQuery query = db->doSQLQuery(queryStr);  
    return query.next();  
}

bool MyTcpServer::registerUser(const QString& username, const QString& password) {
    Database* db = Database::getInstance();
    QString token = username + "_token_" + QString::number(QDateTime::currentMSecsSinceEpoch());

    
    QString queryStr = QString("INSERT INTO Users (login, password, token, isTeacher) "
        "VALUES ('%1', '%2', '%3', 0)")
        .arg(username, password, token);

    QSqlQuery query = db->doSQLQuery(queryStr);  

    
    if (query.lastError().isValid()) {
        QString err = query.lastError().databaseText();
        if (err.contains("UNIQUE constraint failed")) {
            qDebug() << "Пользователь уже существует";
        }
        else {
            qDebug() << "Ошибка регистрации:" << query.lastError().text();
        }
        return false;
    }
    return true;
}



void MyTcpServer::slotNewConnection(){
    QTcpSocket* clientSocket = mTcpServer->nextPendingConnection();
    mClients.append(clientSocket);

    qDebug() << "Новый клиент подключился. Всего клиентов:" << mClients.size();

    clientSocket->write("    ДОБРО ПОЖАЛОВАТЬ НА СЕРВЕР    \r\n");
    clientSocket->write("Доступные команды:\r\n");
    clientSocket->write("REGISTER <username> <password> - Регистрация\r\n");
    clientSocket->write("LOGIN <username> <password>    - Вход\r\n");
    clientSocket->write("LOGOUT                         - Выход (заглушка)\r\n");
    clientSocket->write("GETDATA                        - Получить данные (заглушка)\r\n");
    clientSocket->write("VIGENERE_ENCRYPT <текст> <ключ> - Шифр Виженера (шифрование)\r\n");
    clientSocket->write("VIGENERE_DECRYPT <текст> <ключ> - Шифр Виженера (расшифровка)\r\n");
    clientSocket->write("SHA512 <текст>                 - Хеширование (заглушка)\r\n");
    clientSocket->write("BISECTION <a> <b> <eps>        - Метод деления пополам (заглушка)\r\n");
    clientSocket->write("SHORTEST_PATH <graph> <start> <end> - Дейкстра (заглушка)\r\n");
   

    connect(clientSocket, &QTcpSocket::readyRead,
            this, &MyTcpServer::slotServerRead);
    connect(clientSocket, &QTcpSocket::disconnected,
            this, &MyTcpServer::slotClientDisconnected);
}

void MyTcpServer::slotServerRead(){
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    while(clientSocket->bytesAvailable() > 0)
    {
        QByteArray data = clientSocket->readAll();
        QString request = QString::fromUtf8(data).trimmed();

        if (!request.isEmpty()) {
            qDebug() << "Получен запрос от клиента:" << request;
            parseAndHandleCommand(clientSocket, request);
        }
    }
}

void MyTcpServer::parseAndHandleCommand(QTcpSocket* socket, const QString& command){
    QStringList parts = command.split(" ", Qt::SkipEmptyParts);

    if (parts.isEmpty()) return;

    QString cmd = parts[0].toUpper();
    QStringList params = parts.mid(1);

    qDebug() << "Команда:" << cmd << "Параметры:" << params;

    if (cmd == "LOGIN") {
        handleLogin(socket, params);
    }
    else if (cmd == "REGISTER") {
        handleRegister(socket, params);
    }
    else if (cmd == "LOGOUT") {
        handleLogout(socket, params);
    }
    else if (cmd == "GETDATA") {
        handleGetData(socket, params);
    }
    else if (cmd == "VIGENERE_ENCRYPT") {
        handleVigenereEncrypt(socket, params);
    }
    else if (cmd == "VIGENERE_DECRYPT") {
        handleVigenereDecrypt(socket, params);
    }
    else if (cmd == "SHA512") {
        handleSha512(socket, params);
    }
    else if (cmd == "BISECTION") {
        handleBisection(socket, params);
    }
    else if (cmd == "SHORTEST_PATH") {
        handleShortestPath(socket, params);
    }
    else {
        socket->write(("Неизвестная команда: " + cmd + "\r\n").toUtf8());
    }
}



void MyTcpServer::handleLogin(QTcpSocket* socket, const QStringList& params){
    if (params.size() < 2) {
        socket->write("Ошибка: формат команды LOGIN <username> <password>\r\n");
        return;
    }

    QString username = params[0];
    QString password = params[1];

    if (checkUser(username, password)) {
        socket->write(("УСПЕХ: Вход выполнен для пользователя [" + username + "]\r\n").toUtf8());
        qDebug() << "[БД] Вход успешен: " << username;
    } else {
        socket->write("ОШИБКА: Неверный логин или пароль.\r\n");
        qDebug() << "[БД] Вход неудачен: " << username;
    }
}

void MyTcpServer::handleRegister(QTcpSocket* socket, const QStringList& params){
    if (params.size() < 2) {
        socket->write("Ошибка: формат команды REGISTER <username> <password>\r\n");
        return;
    }

    QString username = params[0];
    QString password = params[1];

    if (registerUser(username, password)) {
        socket->write(("УСПЕХ: Пользователь [" + username + "] зарегистрирован.\r\n").toUtf8());
        qDebug() << "[БД] Регистрация успешна: " << username;
    } else {
        socket->write("ОШИБКА: Пользователь с таким именем уже существует или ошибка БД.\r\n");
        qDebug() << "[БД] Регистрация неудачна: " << username;
    }
}



void MyTcpServer::handleLogout(QTcpSocket* socket, const QStringList& params){
    Q_UNUSED(params);
    qDebug() << "[ЗАГЛУШКА] Вызов LOGOUT";
    socket->write("ИНФО: Функция выхода еще не реализована (заглушка).\r\n");
}

void MyTcpServer::handleGetData(QTcpSocket* socket, const QStringList& params){
    Q_UNUSED(params);
    qDebug() << "[ЗАГЛУШКА] Вызов GETDATA";
    socket->write("ИНФО: Функция получения данных еще не реализована (заглушка).\r\n");
}

void MyTcpServer::slotClientDisconnected(){
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    mClients.removeOne(clientSocket);
    qDebug() << "Клиент отключился. Осталось клиентов:" << mClients.size();

    clientSocket->deleteLater();
}
void MyTcpServer::handleVigenereEncrypt(QTcpSocket* socket, const QStringList& params) {
    if (params.size() < 2) {
        socket->write("Ошибка: VIGENERE_ENCRYPT <текст> <ключ>\r\n");
        return;
    }

    QString key = params.last();
    QStringList textParts = params;
    textParts.removeLast();
    QString text = textParts.join(" ");

    qDebug() << "[VIGENERE_ENCRYPT] Текст:" << text << "Ключ:" << key;

    const QString ruAlphabet = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    const QString enAlphabet = "abcdefghijklmnopqrstuvwxyz";

    QString result;
    int keyIndex = 0;

    for (int i = 0; i < text.length(); i++) {
        QChar c = text[i];
        QString alphabet;

        if (ruAlphabet.contains(c, Qt::CaseInsensitive)) {
            alphabet = ruAlphabet;
        } else if (enAlphabet.contains(c, Qt::CaseInsensitive)) {
            alphabet = enAlphabet;
        } else {
            result += c;
            continue;
        }

        int charIndex = alphabet.indexOf(c.toLower());
        QChar keyChar = key[keyIndex % key.length()].toLower();
        int keyIndexChar = alphabet.indexOf(keyChar);

        if (keyIndexChar == -1) {
            keyIndexChar = alphabet.indexOf(key[0].toLower());
            if (keyIndexChar == -1) keyIndexChar = 0;
        }

        int newIndex = (charIndex + keyIndexChar) % alphabet.length();

        if (c.isUpper()) {
            result += alphabet[newIndex].toUpper();
        } else {
            result += alphabet[newIndex];
        }

        keyIndex++;
    }

    socket->write(("РЕЗУЛЬТАТ: " + result + "\r\n").toUtf8());
}

void MyTcpServer::handleVigenereDecrypt(QTcpSocket* socket, const QStringList& params) {
    if (params.size() < 2) {
        socket->write("Ошибка: VIGENERE_DECRYPT <текст> <ключ>\r\n");
        return;
    }

    QString key = params.last();
    QStringList textParts = params;
    textParts.removeLast();
    QString text = textParts.join(" ");

    qDebug() << "[VIGENERE_DECRYPT] Текст:" << text << "Ключ:" << key;

    const QString ruAlphabet = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    const QString enAlphabet = "abcdefghijklmnopqrstuvwxyz";

    QString result;
    int keyIndex = 0;

    for (int i = 0; i < text.length(); i++) {
        QChar c = text[i];
        QString alphabet;

        if (ruAlphabet.contains(c, Qt::CaseInsensitive)) {
            alphabet = ruAlphabet;
        } else if (enAlphabet.contains(c, Qt::CaseInsensitive)) {
            alphabet = enAlphabet;
        } else {
            result += c;
            continue;
        }

        int charIndex = alphabet.indexOf(c.toLower());
        QChar keyChar = key[keyIndex % key.length()].toLower();
        int keyIndexChar = alphabet.indexOf(keyChar);

        if (keyIndexChar == -1) {
            keyIndexChar = alphabet.indexOf(key[0].toLower());
            if (keyIndexChar == -1) keyIndexChar = 0;
        }

        int newIndex = (charIndex - keyIndexChar + alphabet.length()) % alphabet.length();

        if (c.isUpper()) {
            result += alphabet[newIndex].toUpper();
        } else {
            result += alphabet[newIndex];
        }

        keyIndex++;
    }

    socket->write(("РЕЗУЛЬТАТ: " + result + "\r\n").toUtf8());
}
void MyTcpServer::handleSha512(QTcpSocket* socket, const QStringList& params) {
    Q_UNUSED(params);
    qDebug() << "[ЗАГЛУШКА] Вызов SHA512";
    socket->write("ИНФО: Функция SHA-512 еще не реализована (заглушка).\r\n");
}

void MyTcpServer::handleBisection(QTcpSocket* socket, const QStringList& params) {
    Q_UNUSED(params);
    qDebug() << "[ЗАГЛУШКА] Вызов BISECTION (метод деления пополам)";
    socket->write("ИНФО: Метод деления пополам еще не реализован (заглушка).\r\n");
}

void MyTcpServer::handleShortestPath(QTcpSocket* socket, const QStringList& params) {
    Q_UNUSED(params);
    qDebug() << "[ЗАГЛУШКА] Вызов SHORTEST_PATH (алгоритм Дейкстры)";
    socket->write("ИНФО: Алгоритм Дейкстры еще не реализован (заглушка).\r\n");
}
