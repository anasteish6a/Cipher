#ifndef TASKS_H
#define TASKS_H

#include <QString>

class Vigenere {
public:
    static QString encrypt(const QString& text, const QString& key);

    static QString decrypt(const QString& text, const QString& key);

private:
    static const QString alphabet;
};

#endif
