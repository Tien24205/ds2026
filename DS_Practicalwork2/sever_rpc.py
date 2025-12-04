from xmlrpc.server import SimpleXMLRPCServer
import base64
import os

CHUNK = 4096

def get_file(filename):
    if not os.path.exists(filename):
        return {"error": "File not found"}

    result = []
    with open(filename, "rb") as f:
        while True:
            data = f.read(CHUNK)
            if not data:
                break
            # Encode binary to base64 so RPC can transfer it
            result.append(base64.b64encode(data).decode("utf-8"))
    return {"chunks": result}

server = SimpleXMLRPCServer(("0.0.0.0", 5000))
print("RPC Server running on port 5000...")
server.register_function(get_file, "get_file")
server.serve_forever()
