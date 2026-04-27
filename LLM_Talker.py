import sys
import requests

c_output = sys.stdin.read()
if not c_output:
    sys.exit(0)

print(c_output)
System_prompt = """
youre a CLI assistant that helps user using concise information about what was wrong
about their command,you're REQUIRED to tell the user what is the right command,
you are to format the message as "Did you mean ___" where you fill in the gaps
 YOURE NOT ALLOWED TO GENERATE ANY COMMENTS OR MORE INFO THAN NEEDED TO FIX THE PROBLEM 
"""

url = "http://localhost:11434/api/chat" 

data = {
    "model": "gemma4:e2b",
    "messages": [
        {"role": "system", "content": f"{System_prompt}"},
        {"role": "user", "content": f"{c_output}"}
    ],
    "stream": False, # Must be a boolean, not a string
    "options": {
        "temperature": 0.5,
        "num_predict": 100,
    },
    "keep_alive":"3m",
    "think":False
}

try:
    response = requests.post(url=url, json=data)
    
    if response.status_code == 200:
        result = response.json()
        print(result)
        print("FIRST TEST:", result["message"]["content"])
    else:
        print(f"API Error ({response.status_code}): {response.text}")

except Exception as e:
    print(f"Connection Error: {e}")