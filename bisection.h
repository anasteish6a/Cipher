#ifndef BISECTION_H
#define BISECTION_H

#include <functional>
#include <stdexcept>
#include <string>

/**
 * @file bisection.h
 * @brief Модуль численного решения уравнений методом деления пополам.
 */

namespace Numerical {

    /**
     * @brief Тип функции одной переменной.
     */
    using MathFunction = std::function<double(double)>;

    /**
     * @brief Находит корень уравнения f(x) = 0 на отрезке [a, b] методом деления пополам.
     *
     * @param func Функция, корень которой нужно найти.
     * @param a Левая граница интервала.
     * @param b Правая граница интервала.
     * @param epsilon Точность вычисления (по умолчанию 1e-6).
     * @return double Приближенное значение корня.
     *
     * @throws std::invalid_argument Если f(a) и f(b) имеют одинаковый знак.
     * @throws std::runtime_error Если не удалось достичь точности за разумное число итераций.
     *
     * @example
     * @code
     * auto func = [](double x) { return x*x - 4.0; };
     * double root = Numerical::bisection(func, 0.0, 3.0);
     * // root ≈ 2.0
     * @endcode
     */
    double bisection(MathFunction func, double a, double b, double epsilon = 1e-6);

} // namespace Numerical

#endif // BISECTION_H
