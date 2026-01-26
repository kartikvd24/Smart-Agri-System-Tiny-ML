# Smart Farm Chatbot - Backend Integration

This chatbot now uses a **Python Flask backend** to handle all API calls, while the HTML frontend handles the user interface.

## Architecture

- **Frontend (HTML)**: Handles UI, user input, and displays responses
- **Backend (Python Flask)**: Handles Gemini API calls, farm data fetching, and business logic

## Setup Instructions

### 1. Install Python Dependencies

```bash
pip install -r requirements.txt
```

Or install manually:
```bash
pip install flask flask-cors requests
```

### 2. Start the Backend Server

```bash
python chatbot_server.py
```

You should see:
```
============================================================
🚀 Starting Smart Farm Chatbot Backend Server
============================================================
📡 Server running on http://localhost:5000
🤖 Model: gemini-pro (v1)
🔗 API Endpoints:
   - POST /api/chat - Send chat message
   - GET /api/health - Health check
   - GET /api/farm-data - Get farm data
============================================================
```

### 3. Open the HTML Frontend

Open `chatbot.html` in your web browser. The frontend will automatically:
- Check if the backend is running
- Display connection status
- Send all chat messages to the backend

## API Endpoints

### POST `/api/chat`
Send a chat message to the chatbot.

**Request:**
```json
{
  "message": "What's the status of Field 1?",
  "language": "en",
  "use_farm_context": true
}
```

**Response:**
```json
{
  "success": true,
  "message": "Field 1 status: pH=6.5, Temp=25°C, NPK=15/10/30"
}
```

### GET `/api/health`
Check if the backend server is running.

**Response:**
```json
{
  "status": "ok",
  "service": "Smart Farm Chatbot API"
}
```

### GET `/api/farm-data`
Get current farm data from Firebase.

**Response:**
```json
{
  "success": true,
  "data": {
    "field1": {...},
    "field2": {...},
    "cow": {...}
  }
}
```

## Features

✅ **Backend handles all API calls** - Gemini API key is stored securely in Python  
✅ **Automatic fallback** - If one model fails, tries alternatives automatically  
✅ **Farm data context** - Automatically includes current farm data in responses  
✅ **Multi-language support** - Backend handles language preferences  
✅ **Error handling** - Comprehensive error messages and fallback mechanisms  

## Troubleshooting

### Backend won't start
- Make sure port 5000 is not in use
- Check that all dependencies are installed: `pip install -r requirements.txt`

### Frontend can't connect
- Make sure `chatbot_server.py` is running
- Check browser console for connection errors
- Verify backend URL in `chatbot.html` (default: `http://localhost:5000`)

### API errors
- Check that your Gemini API key is valid in `chatbot_server.py`
- Verify your API key has access to Gemini models
- Check backend console for detailed error messages

## Configuration

### Change Backend Port
Edit `chatbot_server.py`:
```python
app.run(debug=True, host='0.0.0.0', port=5000)  # Change port here
```

And update `chatbot.html`:
```javascript
const BACKEND_URL = "http://localhost:5000";  // Update to match
```

### Change Gemini Model
Edit `chatbot_server.py`:
```python
GEMINI_MODEL = "gemini-pro"  # Options: gemini-pro, gemini-1.5-pro, gemini-1.5-flash
GEMINI_API_VERSION = "v1"    # Options: v1, v1beta
```

## Development

The backend automatically tries fallback models if the primary model fails. Check the console output to see which model is being used.

