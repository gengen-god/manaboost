from flask import Flask, render_template, request, session
import random
import json
from pathlib import Path
import sqlite3
from datetime import datetime


REQUIRED_STREAK = 3


app = Flask(__name__)
DB_PATH = Path(__file__).parent / "manaboost.db"

def init_db():
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute("""
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            ts TEXT NOT NULL,
            event TEXT NOT NULL,
            video_id TEXT,
            video_title TEXT,
            selected TEXT,
            correct_answer TEXT,
            is_correct INTEGER,
            streak_after INTEGER
        )
        """)
        conn.commit()

def log_event(event, video=None, selected=None, correct_answer=None, is_correct=None, streak_after=None):
    ts = datetime.now().isoformat(timespec="seconds")
    with sqlite3.connect(DB_PATH) as conn:
        cur = conn.cursor()
        cur.execute("""
        INSERT INTO logs (ts, event, video_id, video_title, selected, correct_answer, is_correct, streak_after)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """, (
            ts,
            event,
            (video or {}).get("id"),
            (video or {}).get("title"),
            selected,
            correct_answer,
            is_correct,
            streak_after
        ))
        conn.commit()

init_db()

app.secret_key = "dev-secret-change-me"

DATA_PATH = Path(__file__).parent / "videos.json"

def load_video_bank():
    with open(DATA_PATH, "r", encoding="utf-8") as f:
        return json.load(f)

def pick_new_video(video_bank):
    v = random.choice(video_bank)
    session["current_video_id"] = v["id"]
    return v

def get_current_video(video_bank):
    vid = session.get("current_video_id")
    if not vid:
        return pick_new_video(video_bank)
    for v in video_bank:
        if v["id"] == vid:
            return v
    return pick_new_video(video_bank)

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/study")
def study():
    video_bank = load_video_bank()

    if "streak" not in session:
        session["streak"] = 0

    v = pick_new_video(video_bank)
    log_event("study_start", video=v, streak_after=session.get("streak", 0))


    return render_template(
        "study.html",
        video_id=v["id"],
        title=v.get("title", ""),
        question=v["question"],
        choices=v["choices"],
        streak=session["streak"],
        required=REQUIRED_STREAK
    )


@app.route("/quiz", methods=["POST"])
def quiz():
    video_bank = load_video_bank()
    v = get_current_video(video_bank)

    selected = request.form.get("choice", "")

    if "streak" not in session:
        session["streak"] = 0

    if selected == v["answer"]:
        session["streak"] += 1

        log_event(
    "quiz_answer",
    video=v,
    selected=selected,
    correct_answer=v["answer"],
    is_correct=1,
    streak_after=session["streak"]
)


        # ★ 3連続で解除
        if session["streak"] >= REQUIRED_STREAK:
            session["streak"] = 0
            return render_template("passed.html", explanation=v.get("explanation", ""))

        # ★ 次の問題へ（ランダムで次の動画）
        next_video = pick_new_video(video_bank)
        return render_template(
            "study.html",
            video_id=next_video["id"],
            title=next_video.get("title", ""),
            question=next_video["question"],
            choices=next_video["choices"],
            streak=session["streak"],
            required=REQUIRED_STREAK,
            success="正解！次の問題へ"
        )

    # 不正解 → 0に戻す（同じ問題でやり直し）
        # 不正解 → 0に戻す（同じ問題でやり直し）
    session["streak"] = 0

    log_event(
        "quiz_answer",
        video=v,
        selected=selected,
        correct_answer=v["answer"],
        is_correct=0,
        streak_after=session["streak"]
    )

    return render_template(
        "study.html",
        video_id=v["id"],
        title=v.get("title", ""),
        question=v["question"],
        choices=v["choices"],
        streak=session["streak"],
        required=REQUIRED_STREAK,
        error="不正解。連続正解はリセットされました。"
    )



@app.route("/unlock")
def unlock():
    log_event("unlock")
    return "Unlock requested. You can close this tab."



@app.route("/break")
def break_time():
    return render_template("break.html")


@app.route("/logs")
def logs():
    with sqlite3.connect(DB_PATH) as conn:
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()
        cur.execute("SELECT * FROM logs ORDER BY id DESC LIMIT 50")
        rows = cur.fetchall()
    return render_template("logs.html", rows=rows)


if __name__ == "__main__":
    app.run(debug=True)

