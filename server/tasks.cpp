#include "tasks.h"
#include <cmath>
#include <limits>

const QString Vigenere::alphabet = "абвгдеёжзийклмнопрстуфхцчшщъыьэюя";

QString Vigenere::encrypt(const QString& text, const QString& key) {
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

        int newIndex = (charIndex - keyIndexChar + alphabet.length()) % alphabet.length();

        result += alphabet[newIndex];
        keyIndex++;
    }

    return result;
}

namespace Numerical {

double bisection(MathFunction func, double a, double b, double epsilon) {
    if (epsilon <= 0) {
        throw std::invalid_argument("Точность epsilon должна быть положительной.");
    }

    double fa = func(a);
    double fb = func(b);

    if (fa * fb >= 0) {
        throw std::invalid_argument(
            "Функция должна иметь разные знаки на концах отрезка [a, b]. "
            "f(a) = " + std::to_string(fa) + ", f(b) = " + std::to_string(fb)
            );
    }

    double c = a;
    const int max_iterations = 1000;
    int iter = 0;

    while ((b - a) / 2.0 > epsilon && iter < max_iterations) {
        c = (a + b) / 2.0;
        double fc = func(c);

        if (fc == 0.0) {
            return c;
        }

        if (fa * fc < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }

        ++iter;
    }

    if (iter >= max_iterations) {
        throw std::runtime_error("Метод не сошелся за " + std::to_string(max_iterations) + " итераций.");
    }

    return (a + b) / 2.0;
}

}
