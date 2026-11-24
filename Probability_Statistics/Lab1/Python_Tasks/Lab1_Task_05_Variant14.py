
import numpy as np

# Лабораторная работа № 1
# Вариант № 14
# Задание 5. Выборочные числовые характеристики

# Исходная выборка
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

n = len(x)
k = 7

# ------------------------------------------------
# Числовые характеристики исходной выборки
# ------------------------------------------------

# Упорядочиваем наблюдения для определения медианы
ordered = np.sort(x)

# Вычисляем сумму наблюдений
sum_x = np.sum(x)

# Вычисляем сумму квадратов наблюдений
sum_x2 = np.sum(x ** 2)

# Выборочное среднее: x_bar = сумма x_i / n
mean = sum_x / n

# Сумма квадратов отклонений от среднего
# Q = сумма x_i^2 - n * x_bar^2
Q = sum_x2 - n * mean ** 2

# Выборочная дисперсия с делителем n
variance = Q / n

# Исправленная выборочная дисперсия
# S^2 = Q / (n - 1)
corrected_variance = Q / (n - 1)

# Исправленное среднеквадратическое отклонение: S = sqrt(S^2)
standard_deviation = np.sqrt(corrected_variance)

# Медиана при чётном объёме выборки
# Me = (x_(25) + x_(26)) / 2
median = (
    ordered[n // 2 - 1] + ordered[n // 2]
) / 2

# Проверяем наличие единственной моды
values, frequencies = np.unique(
    x, return_counts=True
)

print("Задание 5. Выборочные характеристики")
print("\nНегруппированная выборка")

print(f"Объём выборки = {n}")
print(f"Сумма наблюдений = {sum_x:.6f}")
print(f"Сумма квадратов = {sum_x2:.6f}")

print(
    f"Выборочное среднее: "
    f"{sum_x:.6f} / {n} = {mean:.6f}"
)

print(
    f"Сумма квадратов отклонений: "
    f"{sum_x2:.6f} - {n} * ({mean:.6f})^2 "
    f"= {Q:.6f}"
)

print(
    f"Выборочная дисперсия: "
    f"{Q:.6f} / {n} = {variance:.6f}"
)

print(
    f"Исправленная дисперсия: "
    f"{Q:.6f} / {n - 1} "
    f"= {corrected_variance:.6f}"
)

print(
    f"Медиана: ({ordered[24]:.2f} + "
    f"{ordered[25]:.2f}) / 2 = {median:.6f}"
)

if np.all(frequencies == 1):
    print("Единственная мода отсутствует.")
else:
    max_frequency = np.max(frequencies)
    modes = values[frequencies == max_frequency]
    print("Значения с максимальной частотой:", modes)

# ------------------------------------------------
# Числовые характеристики группированной выборки
# ------------------------------------------------

# Делим размах выборки на семь равных интервалов
edges = np.linspace(np.min(x), np.max(x), k + 1)

# Ширина интервала
delta = edges[1] - edges[0]

# Середины интервалов
centers = (edges[:-1] + edges[1:]) / 2

# Частоты наблюдений
counts, _ = np.histogram(x, bins=edges)

# Накопленные частоты
cum_counts = np.cumsum(counts)

# Сумма произведений частот на середины
sum_nz = np.sum(counts * centers)

# Сумма произведений частот на квадраты середин
sum_nz2 = np.sum(counts * centers ** 2)

# Выборочное среднее группированной выборки
mean_group = sum_nz / n

# Сумма квадратов отклонений группированной выборки
Q_group = sum_nz2 - n * mean_group ** 2

# Выборочная дисперсия группированной выборки
variance_group = Q_group / n

# Исправленная дисперсия группированной выборки
corrected_group = Q_group / (n - 1)

# Исправленное среднеквадратическое отклонение группировки
standard_deviation_group = np.sqrt(corrected_group)

# ------------------------------------------------
# Медиана группированной выборки
# ------------------------------------------------

# Определяем медианный интервал
median_index = np.searchsorted(
    cum_counts, n / 2
)

# Частота перед медианным интервалом
previous_count = (
    cum_counts[median_index - 1]
    if median_index > 0 else 0
)

# Формула интервальной медианы
median_group = (
    edges[median_index]
    + (n / 2 - previous_count)
    / counts[median_index] * delta
)

# ------------------------------------------------
# Мода группированной выборки
# ------------------------------------------------

# Определяем интервал с наибольшей частотой
mode_index = np.argmax(counts)

# Частота предыдущего интервала
previous_frequency = (
    counts[mode_index - 1]
    if mode_index > 0 else 0
)

# Частота следующего интервала
next_frequency = (
    counts[mode_index + 1]
    if mode_index < k - 1 else 0
)

# Вычисляем моду интервального ряда
mode_group = (
    edges[mode_index]
    + (counts[mode_index] - previous_frequency)
    * delta
    / (
        2 * counts[mode_index]
        - previous_frequency
        - next_frequency
    )
)

# ------------------------------------------------
# Выводим результаты группированной выборки
# ------------------------------------------------

print("\nГруппированная выборка")

print(f"Число интервалов = {k}")
print(f"Ширина интервала = {delta:.6f}")

print(
    f"Сумма произведений частот на середины "
    f"= {sum_nz:.6f}"
)

print(
    f"Сумма произведений частот на квадраты середин "
    f"= {sum_nz2:.6f}"
)

print(
    f"Выборочное среднее: "
    f"{sum_nz:.6f} / {n} = {mean_group:.6f}"
)

print(
    f"Сумма квадратов отклонений: "
    f"{sum_nz2:.6f} - {n} * ({mean_group:.6f})^2 "
    f"= {Q_group:.6f}"
)

print(
    f"Выборочная дисперсия: "
    f"{Q_group:.6f} / {n} = {variance_group:.6f}"
)

print(
    f"Исправленная дисперсия: "
    f"{Q_group:.6f} / {n - 1} "
    f"= {corrected_group:.6f}"
)

print(
    f"Медиана группированной выборки "
    f"= {median_group:.6f}"
)

print(
    f"Мода группированной выборки "
    f"= {mode_group:.6f}"
)

# ------------------------------------------------
# Контроль числовых результатов
# ------------------------------------------------

assert n == 50
assert np.isclose(sum_x, 450.12)
assert np.isclose(sum_x2, 7526.1702)

assert np.isclose(mean, 9.0024)
assert np.isclose(variance, 69.48019824)
assert np.isclose(corrected_variance, 70.8981614694)
assert np.isclose(median, 8.21)

assert counts.tolist() == [1, 6, 14, 13, 6, 6, 4]

assert np.isclose(sum_nz, 440.76)
assert np.isclose(sum_nz2, 7011.273)

assert np.isclose(mean_group, 8.8152)
assert np.isclose(variance_group, 62.51770896)
assert np.isclose(corrected_group, 63.7935805714)
assert np.isclose(median_group, 7.6984615385)
assert np.isclose(mode_group, 5.4955555556)

# ИСПРАВЛЕНО: дополнительно выводим S и S_гр (среднеквадратические отклонения).
print(f"Исправленное среднеквадратическое отклонение S = {standard_deviation:.6f}")
print(f"Исправленное среднеквадратическое отклонение S_гр = {standard_deviation_group:.6f}")
assert np.isclose(standard_deviation**2, corrected_variance)
assert np.isclose(standard_deviation_group**2, corrected_group)

print("\nВсе проверки выполнены успешно.")