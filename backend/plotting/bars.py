"""bars.py — 막대 그리기 프리미티브와 broken y-axis 헬퍼.

broken-axis 로직은 Yet-Another-Reuse-Distance-Analyzer backend/_plot_utils.py 채택.
"""


def grouped_bars(ax, labels, series) -> list:
    """2계열 그룹(나란히) 막대를 그린다.

    @param series  [(values, label, color), (values, label, color)] 2개.
    @return 그린 막대(Rectangle) 목록 — series 순서대로 평탄화. label_bars에 전달.
    """
    import numpy as np

    x = np.arange(len(labels))
    w = 0.38
    bars = []
    for (values, label, color), off in zip(series, (-w / 2, w / 2)):
        bars.extend(ax.bar(x + off, values, width=w, label=label,
                           color=color, edgecolor="black", linewidth=0.5))
    return bars


def label_bars(ax, bars, values, fmt=str, ylim_min=0.0, inside=False) -> None:
    """각 막대에 값을 표기한다(축 ylim 밖·0 값은 건너뜀).

    @param fmt     값 → 문자열 포맷터.
    @param inside  True면 막대 안(흰 글씨), False면 막대 위.
    """
    ymax = ax.get_ylim()[1]
    for bar, v in zip(bars, values):
        if v == 0 or not (ylim_min <= v <= ymax):
            continue
        cx = bar.get_x() + bar.get_width() / 2
        if inside:
            ax.text(cx, bar.get_height() * 0.5, fmt(v),
                    ha="center", va="center", fontsize=7, color="white")
        else:
            ax.text(cx, bar.get_height() + ymax * 0.01, fmt(v),
                    ha="center", va="bottom", fontsize=7, clip_on=False)


def style_bars(ax, labels, rotate: bool = False) -> None:
    """막대축 공통 마감: x눈금 라벨, despine."""
    import seaborn as sns

    ax.set_xticks(range(len(labels)))
    if rotate:
        ax.set_xticklabels(labels, rotation=45, ha="right", fontsize=8)
    else:
        ax.set_xticklabels(labels, fontsize=10)
    sns.despine(ax=ax)


def broken_axis_limits(values):
    """scale gap이 4배 이상이면 broken y-axis 범위 (low_max, high_min)을 반환한다.

    @return (low_max, high_min) 또는 격차가 작으면 None.
    """
    positive = sorted(v for v in values if v > 0)
    if len(positive) < 2:
        return None
    gap_idx, best_ratio = None, 1.0
    for idx, (lo, hi) in enumerate(zip(positive, positive[1:])):
        r = hi / lo
        if r > best_ratio:
            best_ratio, gap_idx = r, idx
    if gap_idx is None or best_ratio < 4:
        return None
    low_max = positive[gap_idx] * 1.25
    high_min = positive[gap_idx + 1] * 0.85
    return (low_max, high_min) if high_min > low_max else None


def add_break_marks(ax_top, ax_bot) -> None:
    """두 축 사이 spine을 열어 broken axis임을 나타낸다."""
    ax_top.spines["bottom"].set_visible(False)
    ax_bot.spines["top"].set_visible(False)
    ax_top.tick_params(bottom=False)


def add_break_band(ax_top, ax_bot) -> None:
    """두 축 경계에 흰색 물결 리본을 그린다."""
    import math

    import numpy as np
    from matplotlib.lines import Line2D
    from matplotlib.patches import Polygon

    xmin, xmax = ax_bot.get_xlim()
    pad = (xmax - xmin) * 0.01
    xs = np.linspace(xmin - pad, xmax + pad, 500)
    phase = np.linspace(0, max(2.0, (xmax - xmin) / 1.8) * 2 * math.pi, xs.size)
    fig = ax_bot.figure
    inv = fig.transFigure.inverted()
    x_fig = inv.transform(
        ax_bot.transData.transform(np.column_stack([xs, np.zeros_like(xs)]))
    )[:, 0]

    y_mid = ax_bot.get_position().y1
    wave = 0.006 * np.sin(phase)
    upper = y_mid + wave + 0.010
    lower = y_mid + wave - 0.010

    verts = list(zip(x_fig, upper)) + list(zip(x_fig[::-1], lower[::-1]))
    fig.add_artist(Polygon(verts, closed=True, facecolor="white", edgecolor="none",
                           transform=fig.transFigure, zorder=1000, clip_on=False))
    for ys in (upper, lower):
        fig.add_artist(Line2D(x_fig, ys, color="#7F7F7F", linewidth=0.8,
                              transform=fig.transFigure, zorder=1001,
                              solid_capstyle="round", clip_on=False))