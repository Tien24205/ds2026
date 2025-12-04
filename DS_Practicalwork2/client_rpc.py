import xmlrpc.client
import base64

proxy = xmlrpc.client.ServerProxy("http://127.0.0.1:5000/")

filename = input("Enter filename to download: ")

response = proxy.get_file(filename)

if "error" in response:
    print("Server:", response["error"])
else:
    with open("received_" + filename, "wb") as f:
        for chunk in response["chunks"]:
            f.write(base64.b64decode(chunk))
    print("File downloaded successfully!")
