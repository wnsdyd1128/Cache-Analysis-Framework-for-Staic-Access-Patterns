"""reports.py — APEX-Cache 리포트 로딩·집계 (순수 함수).

`run`이 생성한 리포트(`<name>.json`, `<name>_objects.csv`)를 읽어
플롯에 쓸 형태로 가공한다. 렌더링과 분리되어 단위 테스트 가능하다.
"""

import csv
import json
from pathlib import Path
from typing import Tuple

_INT_FIELDS = ("accesses", "hits", "misses", "load_misses", "store_misses")


def find_reports(results_dir) -> Tuple[Path, Path]:
    """results 디렉터리에서 (summary.json, objects.csv) 경로를 찾는다.

    @param results_dir  `run --output`이 가리킨 디렉터리.
    @return (summary_json_path, objects_csv_path)
    @raises FileNotFoundError  둘 중 하나라도 없을 때.
    """
    d = Path(results_dir)
    objects = sorted(d.glob("*_objects.csv"))
    summaries = sorted(p for p in d.glob("*.json"))
    if not summaries or not objects:
        raise FileNotFoundError(
            f"리포트를 찾을 수 없음: {d} (*.json + *_objects.csv 필요)")
    return summaries[0], objects[0]


def load_summary(path) -> dict:
    """summary `<name>.json`을 dict로 로드한다."""
    return json.loads(Path(path).read_text())


def load_objects(path) -> list:
    """`<name>_objects.csv`를 행 dict 목록으로 로드한다(숫자 필드 형변환)."""
    rows = []
    with open(path, newline="") as f:
        for r in csv.DictReader(f):
            row = {"object": r["object"], "miss_rate": float(r["miss_rate"])}
            for k in _INT_FIELDS:
                row[k] = int(r[k])
            rows.append(row)
    return rows


def miss_breakdown(summary: dict) -> dict:
    """summary에서 cold/capacity/conflict miss 수를 추출한다."""
    return {k: summary[k] for k in ("cold", "capacity", "conflict")}


def hit_miss_rates(summary: dict) -> dict:
    """L1/L2의 hit·miss 비율을 반환한다 (miss = 1 - hit)."""
    levels = summary["levels"]
    out = {}
    for name in ("L1", "L2"):
        hit = levels[name]["hit_rate"]
        out[name] = {"hit": hit, "miss": 1.0 - hit}
    return out


def top_objects_by_misses(objects: list, n: int = 10) -> list:
    """miss 수 내림차순 상위 n개 객체를 반환한다."""
    return sorted(objects, key=lambda o: o["misses"], reverse=True)[:n]
