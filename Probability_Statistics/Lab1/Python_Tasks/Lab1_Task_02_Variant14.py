import numpy as np

# Лабораторная работа № 1
# Вариант № 14
# Задание 2. Интервальный статистический ряд

# Исходные данные
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

# Объём выборки и число интервалов
n = len(x)
k = 7

# Минимальное и максимальное значения
xmin = np.min(x)
xmax = np.max(x)

# Вычисляем размах выборки
R = xmax - xmin

# Ширина равных интервалов: delta = R / k
delta = R / k

# Формируем границы семи равных интервалов
edges = np.linspace(xmin, xmax, k + 1)

# Вычисляем середины интервалов
# z_j = (a_(j-1) + a_j) / 2
centers = (edges[:-1] + edges[1:]) / 2

# Подсчитываем частоты наблюдений в интервалах
# Левые границы включаются, правые не включаются
# Правая граница последнего интервала включается
counts, _ = np.histogram(x, bins=edges)

# Относительные частоты: w_j = n_j / n
weights = counts / n

# Накопленные частоты: N_j = сумма n_i
cum_counts = np.cumsum(counts)

# Накопленные относительные частоты
cum_weights = np.cumsum(weights)

# Высоты гистограммы частот: h_j = n_j / delta
heights = counts / delta

# Высоты гистограммы относительных частот
# f_j = n_j / (n * delta)
relative_heights = counts / (n * delta)

print("Задание 2. Интервальный статистический ряд")
print(f"Объём выборки = {n}")
print(f"Число интервалов = {k}")
print(f"Размах выборки = {R:.6f}")
print(f"Ширина интервала = {delta:.6f}")

# Выводим таблицу границ и середин интервалов
print("\nГраницы интервалов")

for j, edge in enumerate(edges):
    print(f"a_{j} = {edge:.2f}")

print("\nСередины интервалов")

for j, center in enumerate(centers, 1):
    print(f"z_{j} = {center:.2f}")

# ИСПРАВЛЕНО: таблица Жарковой содержит РОВНО 8 столбцов.
# Абсолютные накопленные частоты не выводятся в эту таблицу.
# Все заголовки записываем полностью, без сокращений.
import pandas as pd

intervals = []
for j in range(k):
    end_bracket = "]" if j == k - 1 else ")"
    intervals.append(
        f"[{edges[j]:.2f}; {edges[j+1]:.2f}{end_bracket}"
    )

table = pd.DataFrame({
    "Номер разряда": np.arange(1, k + 1),
    "Разряд": intervals,
    "Середина разряда": centers,
    "Частота": counts,
    "Относительная частота": weights,
    "Относительная накопленная частота": cum_weights,
    "Высота гистограммы частот": heights,
    "Высота гистограммы относительных частот": relative_heights,
})

print("\nТаблица из восьми столбцов (требование Жарковой):")
with pd.option_context("display.max_columns", None, "display.width", 240,
                       "display.float_format", "{:.6f}".format):
    print(table.to_string(index=False))
print(f"\nЧастоты по разрядам: {counts.tolist()}")
print(f"Накопленные относительные частоты: {cum_weights.tolist()}")
assert table.shape == (7, 8)
assert "Накопленная частота" not in table.columns

# Проверяем сумму частот
sum_counts = np.sum(counts)
sum_weights = np.sum(weights)

# Проверяем суммы площадей прямоугольников
area_counts = np.sum(heights * delta)
area_relative = np.sum(relative_heights * delta)

print("\nПроверка результатов")
print(f"Сумма частот = {sum_counts}")
print(f"Сумма относительных частот = {sum_weights:.6f}")
print(f"Площадь гистограммы частот = {area_counts:.6f}")
print(f"Площадь гистограммы относительных частот = {area_relative:.6f}")

assert counts.tolist() == [1, 6, 14, 13, 6, 6, 4]
assert np.isclose(delta, 5.26)
assert sum_counts == n
assert np.isclose(sum_weights, 1.0)
assert np.isclose(area_counts, n)
assert np.isclose(area_relative, 1.0)

print("Все проверки выполнены успешно.")