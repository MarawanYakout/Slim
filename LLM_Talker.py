import sys
import requests,json

c_output = sys.stdin.read()
if not c_output:
    sys.exit(0)

System_prompt = """
youre a CLI assistant that helps user using concise information about what was wrong
 about their command, for example you may get command not found l -l youll say did you mean ls -l
 YOURE NOT ALLOWED TO GENERATE ANY COMMENTS OR MORE INFO THAN NEEDED TO FIX THE PROBLEM """


url = "http://localhost:11434/api/chat" 
data ={
    "model": "gemma4:e2b",
    "messages": [{"role": "system", "content": f" {System_prompt}"},{"role":"user","content":f"{c_output}"}],
    "tempreture":0.0,
    "max_tokens": 10,
    "stream":"False"
  }

response = requests.post(url=url,json=data)
if response.status_code==200:
    result = response.json()
    print("FIRST TEST ",result["response"])