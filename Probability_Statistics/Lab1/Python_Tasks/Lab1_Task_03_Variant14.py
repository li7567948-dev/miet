
import numpy as np
import matplotlib.pyplot as plt

# Лабораторная работа № 1
# Вариант № 14
# Задание 3. Построение гистограмм и полигона

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

# Настройка отображения русского текста
plt.rcParams["font.family"] = "DejaVu Sans"
plt.rcParams["axes.unicode_minus"] = False

n = len(x)
k = 7

# Определяем границы интервалов
edges = np.linspace(np.min(x), np.max(x), k + 1)

# Ширина равных интервалов
delta = edges[1] - edges[0]

# Середины интервалов
centers = (edges[:-1] + edges[1:]) / 2

# Частоты в каждом интервале
counts, _ = np.histogram(x, bins=edges)

# Высоты гистограммы частот
# h_j = n_j / delta
heights = counts / delta

# Высоты гистограммы относительных частот
# f_j = n_j / (n * delta)
relative_heights = counts / (n * delta)

# ------------------------------------------------
# ИСПРАВЛЕНО: полигон поверх КАЖДОЙ гистограммы.
# Левая гистограмма: n_j/Δ. Правая: n_j/(nΔ).
# Гладкую теоретическую кривую НЕ строим.
# ------------------------------------------------

fig, axes = plt.subplots(1, 2, figsize=(13.8, 5.1))

for ax, vals, title, ylabel in (
    (axes[0], heights, "Гистограмма частот с полигоном частот", r"$n_j/\Delta$"),
    (axes[1], relative_heights, "Гистограмма относительных частот с полигоном", r"$n_j/(n\Delta)$"),
):
    # Столбцы гистограммы с заданными высотами.
    ax.bar(edges[:-1], vals, width=delta, align="edge", color="#f5f5f5",
           edgecolor="black", linewidth=1.05, label="Гистограмма", zorder=2)
    # Полигон соединяет середины верхних оснований прямоугольников.
    # За пределами крайних разрядов добавлены нулевые вершины.
    poly_x = np.r_[centers[0]-delta, centers, centers[-1]+delta]
    poly_y = np.r_[0, vals, 0]
    ax.plot(poly_x, poly_y, "o-", color="#b91c1c", lw=1.9, ms=4,
            label="Полигон", zorder=4)
    ax.set_title(title, fontsize=10.3)
    ax.set_xlabel("Значения признака")
    ax.set_ylabel(ylabel)
    ax.set_xlim(edges[0]-delta*0.85,edges[-1]+delta*0.85)
    ax.set_xticks(edges)
    ax.tick_params(axis="x",labelrotation=37,labelsize=8)
    ax.grid(axis="y",alpha=0.19,zorder=0)
    ax.legend(fontsize=8,loc="upper right")

# По замечанию Жарковой правая гистограмма визуально ниже левой.
axes[0].set_ylim(0, max(heights)*1.17)
axes[1].set_ylim(0, max(relative_heights)*1.80)
fig.tight_layout()
fig.savefig("Lab1_Histograms_with_BOTH_Polygons.png",dpi=220,bbox_inches="tight")

# Контроль площадей гистограмм.
area_1 = np.sum(heights*delta)
area_2 = np.sum(relative_heights*delta)
print("Задание 3. Проверка построения двух полигонов")
print(f"Площадь гистограммы частот = {area_1:.6f}")
print(f"Площадь гистограммы относительных частот = {area_2:.6f}")
assert np.array_equal(counts, [1,6,14,13,6,6,4])
assert np.allclose(heights, counts/delta)
assert np.allclose(relative_heights, counts/(n*delta))
assert np.isclose(area_1,n) and np.isclose(area_2,1)
print("Все проверки выполнены успешно.")
plt.show()