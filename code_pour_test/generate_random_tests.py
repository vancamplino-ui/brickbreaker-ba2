#!/usr/bin/env python3

import argparse
import copy
import math
import random
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parents[1]
OUTPUT_DIR = PROJECT_ROOT / "tests_rand"


def choose_profile(index, rng):
    if index % 10 in {0, 1, 2, 3}:
        return {
            "brick_count": rng.randint(5, 10),
            "ball_count": rng.randint(2, 3),
            "brick_size": rng.uniform(4.5, 8.0),
            "ball_radius": rng.uniform(0.8, 1.5),
            "brick_gap": rng.uniform(1.5, 4.0),
            "ball_gap": rng.uniform(1.5, 3.5),
        }

    if index % 10 in {4, 5, 6}:
        return {
            "brick_count": rng.randint(20, 60),
            "ball_count": rng.randint(4, 8),
            "brick_size": rng.uniform(3.5, 6.0),
            "ball_radius": rng.uniform(0.7, 1.2),
            "brick_gap": rng.uniform(1.0, 3.0),
            "ball_gap": rng.uniform(1.0, 2.5),
        }

    if index % 10 in {7, 8}:
        return {
            "brick_count": rng.randint(100, 200),
            "ball_count": rng.randint(10, 20),
            "brick_size": rng.uniform(3.0, 4.0),
            "ball_radius": rng.uniform(0.45, 0.8),
            "brick_gap": rng.uniform(0.8, 1.8),
            "ball_gap": rng.uniform(0.8, 1.8),
        }

    return {
        "brick_count": rng.randint(220, 320),
        "ball_count": rng.randint(20, 40),
        "brick_size": rng.uniform(3.0, 3.4),
        "ball_radius": rng.uniform(0.35, 0.6),
        "brick_gap": rng.uniform(0.6, 1.2),
        "ball_gap": rng.uniform(0.6, 1.4),
    }


def make_bricks(rng, profile):
    size = round(profile["brick_size"], 2)
    half = size / 2.0
    gap = profile["brick_gap"]
    step = size + gap
    margin = 0.1

    xs = []
    x = half + margin
    while x <= 100.0 - half - margin:
        xs.append(x)
        x += step

    ys = []
    y = 100.0 - half - margin
    while y >= 38.0 + half + margin:
        ys.append(y)
        y -= step

    positions = [(xv, yv) for yv in ys for xv in xs]
    count = min(profile["brick_count"], len(positions))
    bricks = []

    for xv, yv in positions[:count]:
        brick_type = rng.choices([0, 1, 2], weights=[5, 2, 2])[0]

        if brick_type == 0:
            bricks.append({
                "type": 0,
                "x": xv,
                "y": yv,
                "size": size,
                "hp": rng.randint(1, 7),
            })
        else:
            bricks.append({
                "type": brick_type,
                "x": xv,
                "y": yv,
                "size": size,
            })

    return bricks


def make_balls(rng, profile):
    radius = round(profile["ball_radius"], 2)
    gap = profile["ball_gap"]
    step = 2.0 * radius + gap
    margin = 0.1

    xs = []
    x = radius + margin
    while x <= 100.0 - radius - margin:
        xs.append(x)
        x += step

    ys = []
    y = 8.0 + radius + margin
    while y <= 30.0 - radius - margin:
        ys.append(y)
        y += step

    positions = [(xv, yv) for yv in ys for xv in xs]
    count = min(profile["ball_count"], len(positions))
    balls = []

    for xv, yv in positions[:count]:
        dx = round(rng.uniform(-1.8, 1.8), 2)
        dy = round(rng.uniform(-1.8, 1.8), 2)
        balls.append({
            "x": xv,
            "y": yv,
            "r": radius,
            "dx": dx,
            "dy": dy,
        })

    return balls


def make_valid_case(rng):
    score = rng.randint(0, 200)
    lives = rng.randint(1, 5)
    paddle = (50.0 + rng.uniform(-5.0, 5.0), -16.0, 20.0)
    profile = choose_profile(rng.randint(0, 9999), rng)
    bricks = make_bricks(rng, profile)
    balls = make_balls(rng, profile)

    return {
        "score": score,
        "lives": lives,
        "paddle": paddle,
        "bricks": bricks,
        "balls": balls,
    }


def inject_error(case, error_kind, rng):
    case = copy.deepcopy(case)
    brick_index = rng.randrange(len(case["bricks"])) if case["bricks"] else 0
    other_brick_index = brick_index
    if len(case["bricks"]) > 1:
        choices = [i for i in range(len(case["bricks"])) if i != brick_index]
        other_brick_index = rng.choice(choices)

    ball_index = rng.randrange(len(case["balls"])) if case["balls"] else 0
    other_ball_index = ball_index
    if len(case["balls"]) > 1:
        choices = [i for i in range(len(case["balls"])) if i != ball_index]
        other_ball_index = rng.choice(choices)

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
        size = case["bricks"][brick_index]["size"]
        case["bricks"][brick_index]["x"] = -(size / 2.0 + 1.0)
        return case
    if error_kind == "invalid_brick_size":
        case["bricks"][brick_index]["size"] = 2.0
        return case
    if error_kind == "invalid_hit_points":
        rainbow_indices = [i for i, brick in enumerate(case["bricks"])
                           if brick["type"] == 0]
        if not rainbow_indices:
            case["bricks"][brick_index]["type"] = 0
            case["bricks"][brick_index]["hp"] = 1
        target = rng.choice(rainbow_indices) if rainbow_indices else brick_index
        case["bricks"][target]["hp"] = 9
        return case
    if error_kind == "ball_outside":
        case["balls"][ball_index]["y"] = -1.0
        return case
    if error_kind == "invalid_delta":
        case["balls"][ball_index]["dx"] = 3.1
        case["balls"][ball_index]["dy"] = 0.1
        return case
    if error_kind == "bricks_intersect":
        case["bricks"][other_brick_index]["x"] = case["bricks"][brick_index]["x"]
        case["bricks"][other_brick_index]["y"] = case["bricks"][brick_index]["y"]
        return case
    if error_kind == "paddle_intersects_brick":
        case["bricks"][brick_index]["x"] = case["paddle"][0]
        case["bricks"][brick_index]["y"] = case["bricks"][brick_index]["size"] / 2.0
        return case
    if error_kind == "balls_intersect":
        case["balls"][other_ball_index]["x"] = case["balls"][ball_index]["x"]
        case["balls"][other_ball_index]["y"] = case["balls"][ball_index]["y"]
        return case
    if error_kind == "ball_intersects_brick":
        case["balls"][ball_index]["x"] = 50.0
        case["balls"][ball_index]["y"] = 20.0
        case["bricks"][brick_index]["x"] = 50.0
        case["bricks"][brick_index]["y"] = 20.0
        return case
    if error_kind == "paddle_intersects_ball":
        case["balls"][ball_index]["x"] = case["paddle"][0] + 5.0
        case["balls"][ball_index]["y"] = 3.0
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


def fmt(value):
    return f"{value:.6f}"


def invalid_score_message(score):
    return f"Score ({score}) must be >= 0"


def invalid_lives_message(lives):
    return f"Lives ({lives}) must be >= 0"


def paddle_outside_message(x, y):
    return f"Paddle at ({fmt(x)};{fmt(y)}) is is_outside its allowed arena"


def brick_outside_message(x, y):
    return f"Brick at ({fmt(x)};{fmt(y)}) is is_outside the arena"


def invalid_brick_size_message(size):
    return f"Brick size ({fmt(size)}) must be >= brick_size_min"


def invalid_hit_points_message(hit_points):
    return f"Color number ({hit_points}) must be in [1, 7]"


def ball_outside_message(x, y):
    return f"Ball at ({fmt(x)};{fmt(y)}) is is_outside the arena"


def invalid_delta_message(dx, dy):
    return f"Delta norm ({fmt(dx)};{fmt(dy)}) must be <= delta_norm_max"


def collision_bricks_message(i, j):
    i, j = sorted((i, j))
    return f"Brick {i} superposed with brick {j}"


def collision_paddle_brick_message(i):
    return f"Paddle superposed with brick {i}"


def collision_balls_message(i, j):
    i, j = sorted((i, j))
    return f"Ball {i} superposed with ball {j}"


def collision_ball_brick_message(ball_i, brick_j):
    return f"Ball {ball_i} superposed with brick {brick_j}"


def collision_paddle_ball_message(i):
    return f"Paddle superposed with ball {i}"


def paddle_is_valid(paddle):
    x, y, radius = paddle
    if y > 0:
        return False
    if y + radius <= 0:
        return False
    if x - radius < 0 or x + radius > 100.0:
        return False
    return True


def brick_is_inside(brick):
    half = brick["size"] / 2.0
    x = brick["x"]
    y = brick["y"]
    return (x - half >= 0 and x + half <= 100.0
            and y - half >= 0 and y + half <= 100.0)


def brick_size_is_valid(brick):
    return brick["size"] >= 3.0


def hit_points_are_valid(brick):
    if brick["type"] != 0:
        return True
    return 1 <= brick["hp"] <= 7


def ball_is_inside(ball):
    x = ball["x"]
    y = ball["y"]
    r = ball["r"]
    if x - r < 0 or x + r > 100.0:
        return False
    if y + r > 100.0 or y < 0:
        return False
    return True


def ball_delta_is_valid(ball):
    return math.hypot(ball["dx"], ball["dy"]) <= 3.0


def brick_square(brick):
    return brick["x"], brick["y"], brick["size"] / 2.0


def ball_circle(ball):
    return ball["x"], ball["y"], ball["r"]


def paddle_circle(paddle):
    x, y, r = paddle
    return x, y, r


def strict_square_square_intersects(b1, b2):
    x1, y1, h1 = brick_square(b1)
    x2, y2, h2 = brick_square(b2)
    dx = abs(x1 - x2) - (h1 + h2)
    dy = abs(y1 - y2) - (h1 + h2)
    return dx < 0.0 and dy < 0.0


def strict_circle_circle_intersects(c1, c2):
    x1, y1, r1 = c1
    x2, y2, r2 = c2
    return math.hypot(x1 - x2, y1 - y2) - (r1 + r2) < 0.0


def strict_circle_square_intersects(circle, brick):
    cx, cy, r = circle
    sx, sy, half = brick_square(brick)
    min_x = sx - half
    max_x = sx + half
    min_y = sy - half
    max_y = sy + half
    closest_x = min(max(cx, min_x), max_x)
    closest_y = min(max(cy, min_y), max_y)
    return math.hypot(cx - closest_x, cy - closest_y) - r < 0.0


def compute_expected_output(case):
    if case["score"] < 0:
        return invalid_score_message(case["score"])

    if case["lives"] < 0:
        return invalid_lives_message(case["lives"])

    if not paddle_is_valid(case["paddle"]):
        x, y, _ = case["paddle"]
        return paddle_outside_message(x, y)

    for brick in case["bricks"]:
        if not brick_is_inside(brick):
            return brick_outside_message(brick["x"], brick["y"])
        if not brick_size_is_valid(brick):
            return invalid_brick_size_message(brick["size"])
        if not hit_points_are_valid(brick):
            return invalid_hit_points_message(brick["hp"])

    for ball in case["balls"]:
        if not ball_is_inside(ball):
            return ball_outside_message(ball["x"], ball["y"])
        if not ball_delta_is_valid(ball):
            return invalid_delta_message(ball["dx"], ball["dy"])

    for i in range(len(case["bricks"])):
        for j in range(i + 1, len(case["bricks"])):
            if strict_square_square_intersects(case["bricks"][i], case["bricks"][j]):
                return collision_bricks_message(i, j)

    paddle = paddle_circle(case["paddle"])
    for i, brick in enumerate(case["bricks"]):
        if strict_circle_square_intersects(paddle, brick):
            return collision_paddle_brick_message(i)

    for i in range(len(case["balls"])):
        for j in range(i + 1, len(case["balls"])):
            if strict_circle_circle_intersects(
                ball_circle(case["balls"][i]),
                ball_circle(case["balls"][j]),
            ):
                return collision_balls_message(i, j)

    for i, ball in enumerate(case["balls"]):
        for j, brick in enumerate(case["bricks"]):
            if strict_circle_square_intersects(ball_circle(ball), brick):
                return collision_ball_brick_message(i, j)

    for i, ball in enumerate(case["balls"]):
        if strict_circle_circle_intersects(paddle, ball_circle(ball)):
            return collision_paddle_ball_message(i)

    return "Correct file"


def write_case(path, case):
    body = render_case(case)
    path.write_text(body, encoding="utf-8")

    expected = compute_expected_output(case)
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
    for path in OUTPUT_DIR.glob("rand_*.txt"):
        path.unlink()

    for i in range(args.count):
        base = make_valid_case(rng)
        if rng.random() < 0.2:
            error_kind = "valid"
        else:
            error_kind = rng.choice(error_kinds)
        case = inject_error(base, error_kind, rng)
        path = OUTPUT_DIR / f"rand_{i:02d}.txt"
        write_case(path, case)
        print(f"{path.name}: {error_kind}")


if __name__ == "__main__":
    main()
