import argparse
import random
import time
import json
import os
import logging

import requests
from dotenv import load_dotenv

try:
    from googletrans import Translator  # type: ignore
except ImportError:
    Translator = None  # type: ignore

# Load environment variables
load_dotenv()

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Firebase Realtime Database URL
BASE = os.getenv(
    "FIREBASE_DATABASE_URL",
    "https://smart-farm-40c57-default-rtdb.asia-southeast1.firebasedatabase.app"
)
FARM = os.getenv("FARM_ID", "farm-001")

# Gemini API Configuration
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
if not GEMINI_API_KEY:
    logger.warning("GEMINI_API_KEY not found in environment variables")

GEMINI_MODEL = os.getenv("GEMINI_MODEL", "gemini-pro")
GEMINI_API_VERSION = os.getenv("GEMINI_API_VERSION", "v1")
GEMINI_BASE_URL = f"https://generativelanguage.googleapis.com/{GEMINI_API_VERSION}/models/{GEMINI_MODEL}:generateContent"

TARGET_LANG = "en"
translator = None


def translate_console(message: str) -> str:
    """Translate console output to the configured language using googletrans."""
    global translator, TARGET_LANG

    if translator and TARGET_LANG != "en":
        try:
            translated = translator.translate(message, dest=TARGET_LANG)
            if translated and translated.text:
                return translated.text
        except Exception as exc:
            logger.warning(f"Translation error: {exc}. Reverting to English.")
            translator = None
            TARGET_LANG = "en"
    return message


def parse_args():
    parser = argparse.ArgumentParser(description="Simulate Smart Farm sensor updates and chatbot.")
    parser.add_argument("--interval", type=int, default=20, help="Seconds between updates (default: 20)")
    parser.add_argument(
        "--lang",
        default="en",
        help="ISO language code for console logs using Google Translate (default: en)",
    )
    parser.add_argument(
        "--chatbot",
        action="store_true",
        help="Run interactive chatbot mode instead of data simulation",
    )
    return parser.parse_args()


def initialize_language(lang_code: str):
    global translator, TARGET_LANG

    TARGET_LANG = (lang_code or "en").lower()
    if TARGET_LANG == "en":
        logger.info("Console output set to English.")
        return

    if Translator is None:
        logger.warning(f"googletrans not installed. Falling back to English (requested '{TARGET_LANG}').")
        TARGET_LANG = "en"
        return

    translator = Translator()
    logger.info(translate_console("Console output translation enabled."))


# Function to push soil data for any field
def push_soil(node_id: str):
    ts = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    data = {
        "timestamp": ts,
        "n": random.randint(10, 30),
        "p": random.randint(5, 15),
        "k": random.randint(20, 50),
        "ph": round(random.uniform(5.5, 7.5), 2),
        "temp_c": round(random.uniform(20, 35), 1),
        "humidity_pct": round(random.uniform(40, 80), 1),
        "moisture": round(random.uniform(0.2, 0.7), 2),
        "battery_v": round(random.uniform(3.4, 4.2), 2),
    }
    requests.put(f"{BASE}/farms/{FARM}/soilNodes/{node_id}/latest.json", json=data)


# Function to push cow data
def push_cow():
    ts = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    data = {
        "timestamp": ts,
        "lat": 12.9716 + random.uniform(-0.001, 0.001),
        "lng": 77.5946 + random.uniform(-0.001, 0.001),
        "milkPh": round(random.uniform(6.4, 7.0), 2),
        "inFence": random.choice([True, False]),
        "battery_v": round(random.uniform(3.5, 4.1), 2),
    }
    requests.put(f"{BASE}/farms/{FARM}/animals/cow-001/latest.json", json=data)


def get_farm_data():
    """Fetch current farm data from Firebase for chatbot context."""
    try:
        field1 = requests.get(f"{BASE}/farms/{FARM}/soilNodes/soil-node-01/latest.json").json()
        field2 = requests.get(f"{BASE}/farms/{FARM}/soilNodes/soil-node-02/latest.json").json()
        cow = requests.get(f"{BASE}/farms/{FARM}/animals/cow-001/latest.json").json()
        
        context = f"""
Current Smart Farm Data:
- Field 1: pH={field1.get('ph', 'N/A')}, Temp={field1.get('temp_c', 'N/A')}°C, NPK={field1.get('n', 'N/A')}/{field1.get('p', 'N/A')}/{field1.get('k', 'N/A')}
- Field 2: pH={field2.get('ph', 'N/A')}, Temp={field2.get('temp_c', 'N/A')}°C, NPK={field2.get('n', 'N/A')}/{field2.get('p', 'N/A')}/{field2.get('k', 'N/A')}
- Cow: Location=({cow.get('lat', 'N/A')}, {cow.get('lng', 'N/A')}), Milk pH={cow.get('milkPh', 'N/A')}, In Fence={cow.get('inFence', 'N/A')}
"""
        return context
    except Exception as e:
        logger.warning(f"Could not fetch farm data: {e}")
        return ""


def chat_with_gemini(message: str, use_farm_context: bool = True) -> str:
    """Send a message to Gemini API and get response."""
    global GEMINI_API_KEY, GEMINI_MODEL, GEMINI_API_VERSION, GEMINI_BASE_URL
    
    # Build context with farm data if requested
    context = ""
    if use_farm_context:
        context = get_farm_data()
    
    # Build the prompt
    prompt = f"""You are a helpful smart farm assistant. {context}
Answer this question in a clear, simple way: {message}"""
    
    # Prepare the request
    url = f"{GEMINI_BASE_URL}?key={GEMINI_API_KEY}"
    payload = {
        "contents": [
            {
                "role": "user",
                "parts": [{"text": prompt}]
            }
        ]
    }
    
    try:
        response = requests.post(url, json=payload, headers={"Content-Type": "application/json"})
        
        if response.status_code == 200:
            data = response.json()
            if "candidates" in data and len(data["candidates"]) > 0:
                reply = data["candidates"][0]["content"]["parts"][0]["text"]
                return reply
            else:
                return "⚠️ No reply received from API"
        else:
            error_data = response.json() if response.text else {}
            error_msg = error_data.get("error", {}).get("message", f"HTTP {response.status_code}")
            
            # Try fallback models if 404
            if response.status_code == 404:
                fallback_models = [
                    ("gemini-pro", "v1beta"),
                    ("gemini-1.5-pro", "v1beta"),
                    ("gemini-1.5-flash", "v1beta"),
                ]
                
                for model, version in fallback_models:
                    if model == GEMINI_MODEL and version == GEMINI_API_VERSION:
                        continue
                    
                    logger.info(f"Trying fallback model: {model} ({version})")
                    fallback_url = f"https://generativelanguage.googleapis.com/{version}/models/{model}:generateContent?key={GEMINI_API_KEY}"
                    fallback_response = requests.post(fallback_url, json=payload, headers={"Content-Type": "application/json"})
                    
                    if fallback_response.status_code == 200:
                        fallback_data = fallback_response.json()
                        if "candidates" in fallback_data and len(fallback_data["candidates"]) > 0:
                            reply = fallback_data["candidates"][0]["content"]["parts"][0]["text"]
                            logger.info(f"Successfully connected using {model} ({version})")
                            return reply
                
                return "I'm having trouble connecting to the AI service. Please try again later."
            
            return "I'm experiencing technical difficulties. Please try again."
            
    except requests.exceptions.RequestException as e:
        logger.error(f"Network error: {e}")
        return "I'm having trouble connecting. Please check your internet connection."
    except Exception as e:
        logger.error(f"Unexpected error: {e}")
        return "An unexpected error occurred. Please try again."


def run_chatbot():
    """Run interactive chatbot mode."""
    logger.info("Starting interactive chatbot mode")
    print("=" * 60)
    print("Smart Farm Chatbot - Interactive Mode")
    print("=" * 60)
    print("Type 'quit' or 'exit' to stop the chatbot")
    print("Type 'help' for available commands")
    print("-" * 60)
    
    while True:
        try:
            user_input = input("\nYou: ").strip()
            
            if not user_input:
                continue
            
            if user_input.lower() in ["quit", "exit", "q"]:
                print("\nGoodbye! Thanks for using Smart Farm Chatbot.")
                break
            
            if user_input.lower() == "help":
                print("\nAvailable Commands:")
                print("  - quit/exit/q: Exit the chatbot")
                print("  - help: Show this help message")
                print("  - Any other message: Ask the chatbot about your farm")
                continue
            
            print("Bot: ", end="", flush=True)
            response = chat_with_gemini(user_input, use_farm_context=True)
            print(response)
            
        except KeyboardInterrupt:
            print("\n\nGoodbye! Thanks for using Smart Farm Chatbot.")
            break
        except Exception as e:
            logger.error(f"Error in chatbot: {e}")
            print(f"\nAn error occurred. Please try again.")


def main():
    args = parse_args()
    
    # Run chatbot mode if requested
    if args.chatbot:
        run_chatbot()
        return
    
    # Otherwise run data simulation
    initialize_language(args.lang)
    logger.info(translate_console("Starting Smart Farm data simulator..."))

    while True:
        try:
            push_soil("soil-node-01")  # Field 1
            push_soil("soil-node-02")  # Field 2
            push_cow()  # Animal data
            logger.debug(translate_console("Data pushed for Field 1, Field 2, and Cow"))
            if args.interval > 0:
                time.sleep(args.interval)
        except Exception as e:
            logger.error(f"Error in data simulation: {e}")
            time.sleep(args.interval)


if __name__ == "__main__":
    main()
