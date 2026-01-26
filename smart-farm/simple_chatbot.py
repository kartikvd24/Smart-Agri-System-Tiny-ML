"""
Simple Smart Farm Chatbot using Google Generative AI SDK
"""
import os
import logging
import google.generativeai as genai
from dotenv import load_dotenv

# Load environment variables
load_dotenv()

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Initialize Gemini model with API key from environment
GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
if not GEMINI_API_KEY:
    logger.error("GEMINI_API_KEY not found in environment variables")
    raise ValueError("GEMINI_API_KEY must be set in environment variables")

genai.configure(api_key=GEMINI_API_KEY)

# Use gemini-2.5-flash model (or gemini-pro if 2.5-flash is not available)
try:
    model = genai.GenerativeModel("gemini-2.5-flash")
except Exception:
    # Fallback to gemini-pro if 2.5-flash is not available
    model = genai.GenerativeModel("gemini-pro")


def start_chat():
    """Start the chatbot conversation."""
    logger.info("Starting chatbot session")
    print("Smart Farm Assistant")
    print("Type 'exit' to quit.\n")

    # ✅ Fix: Properly formatted chat history
    chat = model.start_chat(
        history=[
            {
                "role": "user",
                "parts": [{"text": "You are a helpful assistant chatbot for a smart farm. Help users with farm-related questions."}],
            },
        ]
    )

    while True:
        user_input = input("You: ")
        if user_input.lower() == "exit":
            break

        # Send user message
        response = chat.send_message(user_input)
        print(f"Gemini: {response.text}\n")


if __name__ == "__main__":
    start_chat()

