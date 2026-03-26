#!/usr/bin/env python3

import argparse
import copy
import random
import subprocess
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]
PROJECT_BIN = PROJECT_ROOT / "project"
OUTPUT_DIR = PROJECT_ROOT / "tests_rand"


def make_valid_case(rng):
    score = rng.randint(0, 200)
    lives = rng.randint(1, 5)
    paddle = (50.0 + rng.uniform(-5.0, 5.0), -16.0, 20.0)

    bricks = [
        {"type": 0, "x": 20.0, "y": 82.0, "size": 5.0, "hp": 1},
        {"type": 0, "x": 40.0, "y": 72.0, "size": 5.0, "hp": 3},
        {"type": 1, "x": 60.0, "y": 82.0, "size": 5.0},
        {"type": 2, "x": 80.0, "y": 72.0, "size": 5.0},
        {"type": 0, "x": 30.0, "y": 60.0, "size": 5.0, "hp": 5},
    ]

    balls = [
        {"x": 25.0, "y": 30.0, "r": 1.0, "dx": 0.4, "dy": 0.5},
        {"x": 75.0, "y": 35.0, "r": 1.0, "dx": -0.5, "dy": 0.4},
    ]

    return {
        "score": score,
        "lives": lives,
        "paddle": paddle,
        "bricks": bricks,
        "balls": balls,
    }


def inject_error(case, error_kind, rng):
    case = copy.deepcopy(case)

    if error_kind == "valid":
        return case
    if error_kind == "invalid_score":
        case["score"] = -rng.randint(1, 20)
        return case
    if error_kind == "invalid_lives":
        case["lives"] = -rng.randint(1, 10)
        return case
    if error_kind == "paddle_outside":
        x, _, r = case["paddle"]
        case["paddle"] = (x, 1.0, r)
        return case
    if error_kind == "brick_outside":
        case["bricks"][0]["x"] = -1.0
        return case
    if error_kind == "invalid_brick_size":
        case["bricks"][0]["size"] = 2.0
        return case
    if error_kind == "invalid_hit_points":
        case["bricks"][0]["hp"] = 9
        return case
    if error_kind == "ball_outside":
        case["balls"][0]["y"] = -1.0
        return case
    if error_kind == "invalid_delta":
        case["balls"][0]["dx"] = 3.1
        case["balls"][0]["dy"] = 0.1
        return case
    if error_kind == "bricks_intersect":
        case["bricks"][1]["x"] = case["bricks"][0]["x"] + 2.0
        case["bricks"][1]["y"] = case["bricks"][0]["y"] + 1.0
        return case
    if error_kind == "paddle_intersects_brick":
        case["bricks"][0]["x"] = case["paddle"][0]
        case["bricks"][0]["y"] = 4.0
        return case
    if error_kind == "balls_intersect":
        case["balls"][1]["x"] = case["balls"][0]["x"] + 0.5
        case["balls"][1]["y"] = case["balls"][0]["y"] + 0.5
        return case
    if error_kind == "ball_intersects_brick":
        case["bricks"][0]["x"] = case["balls"][0]["x"] + 1.5
        case["bricks"][0]["y"] = case["balls"][0]["y"] + 1.5
        return case
    if error_kind == "paddle_intersects_ball":
        case["balls"][0]["x"] = case["paddle"][0] + 5.0
        case["balls"][0]["y"] = 3.0
        return case

    raise ValueError(f"Unknown error kind: {error_kind}")


def render_case(case):
    lines = ['# output: ""', "# score", str(case["score"]), "", "# lives",
             str(case["lives"]), "", "# paddle"]

    x, y, r = case["paddle"]
    lines.append(f"{x} {y} {r}")

    lines.extend(["", "# bricks", str(len(case["bricks"]))])
    for brick in case["bricks"]:
        if brick["type"] == 0:
            lines.append(
                f'{brick["type"]} {brick["x"]} {brick["y"]} '
                f'{brick["size"]} {brick["hp"]}'
            )
        else:
            lines.append(
                f'{brick["type"]} {brick["x"]} {brick["y"]} {brick["size"]}'
            )

    lines.extend(["", "# balls", str(len(case["balls"]))])
    for ball in case["balls"]:
        lines.append(
            f'{ball["x"]} {ball["y"]} {ball["r"]} {ball["dx"]} {ball["dy"]}'
        )

    return "\n".join(lines) + "\n"


def compute_expected_output(path):
    result = subprocess.run(
        [str(PROJECT_BIN), str(path)],
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
        check=False,
    )
    return (result.stdout + result.stderr).rstrip("\n")


def write_case(path, case):
    body = render_case(case)
    path.write_text(body, encoding="utf-8")

    expected = compute_expected_output(path)
    lines = body.splitlines()
    lines[0] = f'# output: "{expected}"'
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(
        description="Genere des fichiers de test aleatoires au format du projet."
    )
    parser.add_argument("--count", type=int, default=20)
    parser.add_argument("--seed", type=int, default=42)
    args = parser.parse_args()

    rng = random.Random(args.seed)

    error_kinds = [
        "valid",
        "invalid_score",
        "invalid_lives",
        "paddle_outside",
        "brick_outside",
        "invalid_brick_size",
        "invalid_hit_points",
        "ball_outside",
        "invalid_delta",
        "bricks_intersect",
        "paddle_intersects_brick",
        "balls_intersect",
        "ball_intersects_brick",
        "paddle_intersects_ball",
    ]

    OUTPUT_DIR.mkdir(exist_ok=True)

    for i in range(args.count):
        base = make_valid_case(rng)
        error_kind = error_kinds[i % len(error_kinds)]
        case = inject_error(base, error_kind, rng)
        path = OUTPUT_DIR / f"rand_{i:02d}.txt"
        write_case(path, case)
        print(f"{path.name}: {error_kind}")


if __name__ == "__main__":
    main()
