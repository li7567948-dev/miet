
import numpy as np

# Лабораторная работа № 1
# Вариант № 14
# Задание 1. Вариационный и статистический ряды

# Исходная выборка из 50 наблюдений
x = np.array([
    2.97, 20.12, 15.15, 1.11, 12.54,
    14.98, 5.62, 2.49, 4.15, 18.84,
    9.53, 25.79, 5.03, 0.16, 0.22,
    7.31, 21.76, 2.83, 6.86, 26.18,
    1.23, 10.81, -3.25, 0.46, 5.27,
    1.53, -3.21, 10.61, 19.22, 10.09,
    6.53, 5.28, 1.60, 10.77, 11.99,
    19.67, 13.56, 7.65, -9.70, 27.12,
    3.39, 10.35, 17.09, -3.50, 13.25,
    10.33, 11.02, 4.65, 23.90, 8.77
], dtype=float)

# Определяем объём выборки
n = len(x)

# Строим вариационный ряд по неубыванию
ordered = np.sort(x)

# Определяем различные значения и их частоты
values, frequencies = np.unique(
    ordered, return_counts=True
)

# Вычисляем относительные частоты: w_i = n_i / n
weights = frequencies / n

print("Задание 1. Вариационный ряд")
print("Объём выборки:", n)

for i, value in enumerate(ordered, 1):
    print(f"x_({i}) = {value:.2f}")

# Выводим полный статистический ряд
print("\nСтатистический ряд")
print(
    f'{"Номер":>6} {"Значение":>12} '
    f'{"Частота":>10} {"Относительная частота":>24}'
)

for i, (value, frequency, weight) in enumerate(
    zip(values, frequencies, weights), 1
):
    print(
        f"{i:6d} {value:12.2f} "
        f"{frequency:10d} {weight:24.2f}"
    )

# Вычисляем минимальное и максимальное значения
xmin = ordered[0]
xmax = ordered[-1]

# Размах выборки: R = xmax - xmin
R = xmax - xmin

print("\nЧисловые результаты")
print(f"Минимальное значение = {xmin:.2f}")
print(f"Максимальное значение = {xmax:.2f}")
print(f"Размах выборки = {xmax:.2f} - ({xmin:.2f})")
print(f"Размах выборки = {R:.2f}")

# Проверяем основные свойства статистического ряда
print("\nПроверка результатов")
print(f"Сумма частот = {frequencies.sum()}")
print(f"Сумма относительных частот = {weights.sum():.6f}")

assert n == 50
assert len(values) == 50
assert np.all(frequencies == 1)
assert frequencies.sum() == n
assert np.isclose(weights.sum(), 1.0)
assert np.isclose(R, 36.82)

print("Все проверки выполнены успешно.")