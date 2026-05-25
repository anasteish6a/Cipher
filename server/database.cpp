#include "database.h"
#include <QCoreApplication>
#include <QVariant>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

Database* Database::pInstance = nullptr;
QMutex Database::mutex;
QSqlDatabase Database::dbInstance;

Database::Database() {
    initDB();
}

Database::~Database() {
    dbInstance.close();
}

Database* Database::getInstance() {
    mutex.lock();
    if (!pInstance) {
        pInstance = new Database();
    }
    mutex.unlock();
    return pInstance;
}

void Database::initDB() {
    dbInstance = QSqlDatabase::addDatabase("QSQLITE");
    dbInstance.setDatabaseName("db2.sql");

    if (!dbInstance.open()) {
        qDebug() << dbInstance.lastError().text();
    }

    doSQLQuery("CREATE TABLE IF NOT EXISTS Users (userID INTEGER PRIMARY KEY, login VARCHAR(255) UNIQUE NOT NULL, password VARCHAR(255) NOT NULL, token VARCHAR(255) UNIQUE NOT NULL, isTeacher BOOL NOT NULL)");
    doSQLQuery("CREATE TABLE IF NOT EXISTS Tasks (taskID INTEGER PRIMARY KEY, text VARCHAR(1024) NOT NULL, answer VARCHAR(1024) NOT NULL, type INT NOT NULL)");
    doSQLQuery("CREATE TABLE IF NOT EXISTS UsersTasks (userID int references Users(userID), taskID int references Tasks(taskID), primary key (userID, taskID))");
    doSQLQuery("INSERT INTO Users (userID, login, password, token, isTeacher) VALUES (1, 'admin', 'admin', 'admin_token', 1) ON CONFLICT (userID) DO NOTHING;");
}

QSqlQuery Database::doSQLQuery(QString stringQuery) {
    QSqlQuery query(dbInstance);
    if (!query.exec(stringQuery)) {  
        qDebug() << "SQL Error:" << query.lastError().text();
        qDebug() << "Query:" << stringQuery;
    }
    return query;
}
