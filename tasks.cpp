#include "tasks.h"

// Алфавит: строчные русские буквы
const QString Vigenere::alphabet = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";

QString Vigenere::encrypt(const QString& text, const QString& key) {
    QString result;
    int keyIndex = 0;

    for (int i = 0; i < text.length(); ++i) {
        QChar currentChar = text[i];

        // Пропускаем символы, которых нет в алфавите (пробелы, цифры, знаки)
        if (!alphabet.contains(currentChar)) {
            result += currentChar;
            continue;
        }

        // Индексы символа и ключа в алфавите
        int charIndex = alphabet.indexOf(currentChar);
        int keyIndexChar = alphabet.indexOf(key[keyIndex % key.length()]);

        // Формула шифрования: (T + K) % N
        int newIndex = (charIndex + keyIndexChar) % alphabet.length();

        result += alphabet[newIndex];
        keyIndex++;
    }

    return result;
}

QString Vigenere::decrypt(const QString& text, const QString& key) {
    QString result;
    int keyIndex = 0;

    for (int i = 0; i < text.length(); ++i) {
        QChar currentChar = text[i];

        if (!alphabet.contains(currentChar)) {
            result += currentChar;
            continue;
        }

        int charIndex = alphabet.indexOf(currentChar);
        int keyIndexChar = alphabet.indexOf(key[keyIndex % key.length()]);

        // Формула расшифровки: (T - K + N) % N
        // + N добавляем, чтобы избежать отрицательных чисел
        int newIndex = (charIndex - keyIndexChar + alphabet.length()) % alphabet.length();

        result += alphabet[newIndex];
        keyIndex++;
    }

    return result;
}
