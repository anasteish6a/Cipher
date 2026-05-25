#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMutex>

class Database {
private:
    static Database* pInstance;
    static QMutex mutex;
    static QSqlDatabase dbInstance;

    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    void initDB();

public:
    static Database* getInstance();
    QSqlQuery doSQLQuery(QString stringQuery);
};

#endif
