"""
Smart Farm Chatbot Backend (PERFECTED VERSION - NO VENV NEEDED)
- Uses Gemini API directly
- Reads soil & cow data from Google Sheets (CSV)
- Replies ONLY in plain text (no Markdown, no symbols)
- Uses STRICT spreadsheet-based responses
"""

from flask import Flask, request, jsonify
from flask_cors import CORS
import requests
import csv
import google.generativeai as genai

app = Flask(__name__)
CORS(app)

# -----------------------------------------------------
# GOOGLE SHEET CSV URLS  (Already converted to CSV format)
# -----------------------------------------------------
SOIL_URL = "https://docs.google.com/spreadsheets/d/1OD7NOyFsxzrvicub3b3ykStKDWPTmUnw3OJK2pVT-iU/export?format=csv&gid=0"
COW_URL  = "https://docs.google.com/spreadsheets/d/1OD7NOyFsxzrvicub3b3ykStKDWPTmUnw3OJK2pVT-iU/export?format=csv&gid=1314940939"

# -----------------------------------------------------
# GEMINI API
# -----------------------------------------------------
GEMINI_API_KEY = "AIzaSyCP0QLSr-vKWxyrNOUxiSksrOnYsSBTFA0"   # ← your key
genai.configure(api_key=GEMINI_API_KEY)
model = genai.GenerativeModel("gemini-2.5-flash")

# -----------------------------------------------------
# READ GOOGLE SHEET CSV
# -----------------------------------------------------
def read_csv(url):
    try:
        res = requests.get(url)
        res.raise_for_status()
        lines = res.text.splitlines()
        rows = list(csv.reader(lines))

        if len(rows) < 2:
            return []

        headers = rows[0]
        return [
            dict(zip(headers, r))
            for r in rows[1:]
            if any(x.strip() for x in r)
        ]
    except Exception:
        return []

# -----------------------------------------------------
# BUILD AI CONTEXT (STRICT + NO MARKDOWN)
# -----------------------------------------------------
def build_context():
    soil = read_csv(SOIL_URL)
    cow = read_csv(COW_URL)

    ctx = "SMART FARM DATA FROM SPREADSHEET\n\n"

    ctx += "SOIL DATA:\n"
    ctx += "---------------------\n"
    if soil:
        for row in soil:
            for k, v in row.items():
                ctx += f"{k}: {v}\n"
            ctx += "\n"
    else:
        ctx += "No soil data available.\n\n"

    ctx += "COW DATA:\n"
    ctx += "---------------------\n"
    if cow:
        for row in cow:
            for k, v in row.items():
                ctx += f"{k}: {v}\n"
            ctx += "\n"
    else:
        ctx += "No cow data available.\n\n"

    # VERY STRICT RULES → NO MARKDOWN ALLOWED
        ctx += """
RULES FOR AI RESPONSE:
1. Only use values from the spreadsheet above.
2. Never create or guess any numbers.
3. When asked about soil quality or best place to plant crops, compare ALL soil nodes available.
4. Use all available parameters such as pH, temperature, moisture, nitrogen, phosphorus and potassium.
5. Provide a clear explanation in multiple sentences.
6. You may write multiple plain-text paragraphs, but do NOT use markdown, symbols, stars or bullet characters.
7. You may use numbering like "1." or "2." because that is plain text.
8. Responses must be descriptive, helpful and easy for farmers to understand.
9. If any information is missing, say: "The spreadsheet does not contain this information."
10. For cow questions, always use Lat, Lng and InFence values.
11. Do NOT use markdown, bold, italics or any special formatting.
12. Output must be plain text only, no special symbols.
"""


    return ctx

# -----------------------------------------------------
# AI HANDLER (FORCED PLAIN TEXT OUTPUT)
# -----------------------------------------------------
def ask_gemini(message, language="en"):
    context = build_context()

    # Language support
    if language != "en":
        languages = {"hi": "Hindi", "kn": "Kannada", "te": "Telugu", "ta": "Tamil"}
        message = f"Reply in {languages.get(language, 'English')}. " + message

    chat = model.start_chat(
        history=[{"role": "user", "parts": [{"text": context}]}]
    )

    # FORCE plain text – no formatting allowed
    final_prompt = (
    "Give a detailed explanation using all spreadsheet values. "
    "Use multiple plain-text sentences. Do not use any formatting symbols. "
    + message

    )

    reply = chat.send_message(final_prompt)
    return reply.text.strip()

# -----------------------------------------------------
# API ROUTES
# -----------------------------------------------------
@app.route("/api/chat", methods=["POST"])
def chat():
    data = request.json
    msg = data.get("message", "").strip()
    lang = data.get("language", "en")

    if not msg:
        return jsonify({"success": False, "error": "Message is required"}), 400

    return jsonify({"success": True, "message": ask_gemini(msg, lang)})

@app.route("/api/farm-data")
def farm_data():
    return jsonify({
        "soil": read_csv(SOIL_URL),
        "cow": read_csv(COW_URL)
    })

@app.route("/api/health")
def health():
    return jsonify({"status": "ok"})

# -----------------------------------------------------
# START SERVER
# -----------------------------------------------------
if __name__ == "__main__":
    print("🚀 Smart Farm Chatbot Running...")
    print("📄 Using live Google Sheets data")
    print("🤖 Powered by Gemini 2.5 Flash")
    app.run(debug=True, host="0.0.0.0", port=5000)
