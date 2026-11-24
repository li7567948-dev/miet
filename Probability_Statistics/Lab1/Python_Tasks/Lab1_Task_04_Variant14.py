
import numpy as np
import matplotlib.pyplot as plt

# Лабораторная работа № 1
# Вариант № 14
# Задание 4. Эмпирические функции распределения

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

# Настройка русского текста на графиках
plt.rcParams["font.family"] = "DejaVu Sans"
plt.rcParams["axes.unicode_minus"] = False

n = len(x)
k = 7

# Упорядочиваем исходную выборку
ordered = np.sort(x)

# Границы семи интервалов
edges = np.linspace(ordered[0], ordered[-1], k + 1)

# Ширина интервала
delta = edges[1] - edges[0]

# Середины интервалов и их частоты
centers = (edges[:-1] + edges[1:]) / 2
counts, _ = np.histogram(x, bins=edges)

# ------------------------------------------------
# ИСПРАВЛЕНО: рисуем ступеньки вручную, без вертикальных линий.
# Левосторонняя непрерывность: F_n(x) = #{x_i < x}/n.
# Слева от каждого скачка — пустая точка ○ на верхней ступени,
# справа на предыдущем уровне — закрашенная точка ●.
# Не используем plt.step() и встроенные функции ECDF.
# ------------------------------------------------

def draw_left_continuous_cdf(ax, values, multiplicities, title):
    """Рисует только горизонтальные отрезки, ○ и ●; вертикалей нет."""
    vals = np.asarray(values,dtype=float)
    counts_local=np.asarray(multiplicities,dtype=int)
    total=int(counts_local.sum())
    pad=max(1.6,(vals[-1]-vals[0])*0.095)
    llim=vals[0]-pad; rlim=vals[-1]+pad
    cumulative=np.cumsum(counts_local)/total

    # F(x)=0 при x <= z_1; правая точка закрашена.
    ax.plot([llim,vals[0]],[0,0],color="black",lw=1.7,zorder=3)
    ax.scatter([vals[0]],[0],s=36,c="black",zorder=6)
    ax.annotate("",xy=(llim,0),xytext=(llim+pad*0.47,0),
                arrowprops={"arrowstyle":"->","lw":1.5,"color":"black"})

    # На каждой ступеньке (z_i;z_(i+1)] уровень F_i.
    for j in range(len(vals)-1):
        y=float(cumulative[j])
        ax.plot([vals[j],vals[j+1]],[y,y],color="black",lw=1.7,zorder=3)
        # В левой точке (z_i,y) значение F не достигается.
        ax.scatter([vals[j]],[y],s=34,facecolors="white",
                   edgecolors="black",linewidth=1.25,zorder=7)
        # В правой точке (z_(i+1),y) значение достигается.
        ax.scatter([vals[j+1]],[y],s=30,c="black",zorder=6)

    # На правом луче (z_last;+inf) F(x)=1; точка у z_last пуста.
    ax.plot([vals[-1],rlim],[1,1],color="black",lw=1.7,zorder=3)
    ax.scatter([vals[-1]],[1],s=34,facecolors="white",
               edgecolors="black",linewidth=1.25,zorder=7)
    ax.annotate("",xy=(rlim,1),xytext=(rlim-pad*0.47,1),
                arrowprops={"arrowstyle":"->","lw":1.5,"color":"black"})
    ax.set_xlim(llim-pad*.04,rlim+pad*.04)
    ax.set_ylim(-0.075,1.075)
    ax.set_xlabel("Значение признака x")
    ax.set_ylabel(r"$F_n(x)$")
    ax.set_title(title,fontsize=10)
    ax.set_yticks(np.linspace(0,1,6))
    ax.grid(alpha=0.16,zorder=0)
    # Проверка: все построенные линии горизонтальны.
    for line in ax.lines:
        yy=np.asarray(line.get_ydata(),dtype=float)
        assert np.ptp(yy) < 1e-12, "Найдена вертикальная линия!"

fig,axes=plt.subplots(1,2,figsize=(14,5.2))
draw_left_continuous_cdf(
    axes[0], ordered, np.ones(n,dtype=int),
    "Эмпирическая функция исходной выборки"
)
draw_left_continuous_cdf(
    axes[1], centers, counts,
    "Эмпирическая функция группированной выборки"
)
fig.tight_layout()
fig.savefig("Lab1_ECDF_no_verticals_open_left_closed_right.png",
            dpi=220,bbox_inches="tight")

# Печатаем строгую кусочную формулу сгруппированной функции (8 строк).
# Каждое значение z_j относится к предыдущей (нижней) ступени.
print("\nГруппированная эмпирическая функция распределения:")
print(f"  0, если x <= {centers[0]:.2f}")
for j in range(k-1):
    print(f"  {np.cumsum(counts)[j]}/50 = {np.cumsum(counts)[j]/n:.2f}, "
          f"если {centers[j]:.2f} < x <= {centers[j+1]:.2f}")
print(f"  1, если x > {centers[-1]:.2f}")

# Печатаем полную кусочную формулу исходной функции (51 случай).
print("\nИсходная эмпирическая функция распределения:")
print(f"  0, если x <= {ordered[0]:.2f}")
for j in range(n-1):
    print(f"  {j+1}/50, если {ordered[j]:.2f} < x <= {ordered[j+1]:.2f}")
print(f"  1, если x > {ordered[-1]:.2f}")

# Контроль точек скачков для левостороннего варианта.
for v in ordered:
    assert np.isclose(np.sum(x < v)/n, np.searchsorted(ordered,v,side="left")/n)
for j,z in enumerate(centers):
    assert np.isclose(np.sum(counts[centers < z])/n,
                      (np.sum(counts[:j])/n))
print("\nВсе проверки выполнены успешно: вертикальных линий нет.")
plt.show()