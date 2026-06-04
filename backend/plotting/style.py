"""style.py — 시각화 테마·색·저장.

Yet-Another-Reuse-Distance-Analyzer의 `_plot_utils` 테마를 채택한다:
serif 폰트, seaborn ticks/paper, in-direction tick.
"""

PRIMARY = "#4C72B0"    # load / 단일 막대
SECONDARY = "#DD8452"  # store
HIT_COLOR = "#C44E52"   # hit (빨강 계열)
MISS_COLOR = "#4C72B0"  # miss (파랑 계열)
_SAVE_DPI = 200


def setup_theme() -> None:
    """seaborn/matplotlib 전역 테마를 논문 스타일로 설정한다."""
    import seaborn as sns
    import matplotlib.pyplot as plt

    sns.set_theme(style="ticks", context="paper")
    plt.rcParams.update({
        "font.family": "serif",
        "font.serif": ["DejaVu Serif", "Liberation Serif", "Times New Roman"],
        "pdf.fonttype": 42,
        "ps.fonttype": 42,
        "svg.fonttype": "none",
        "figure.facecolor": "white",
        "axes.facecolor": "white",
        "axes.linewidth": 1.0,
        "xtick.direction": "in",
        "ytick.direction": "in",
    })


def save_figure(fig, path) -> None:
    """figure를 white 배경 PNG로 저장한다."""
    fig.savefig(path, dpi=_SAVE_DPI, bbox_inches="tight",
                facecolor="white", pad_inches=0.04)