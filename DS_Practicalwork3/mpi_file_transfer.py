from mpi4py import MPI
import os

CHUNK = 4096
TAG_FILENAME = 0
TAG_FILESIZE = 1
TAG_DATA = 2

comm = MPI.COMM_WORLD
rank = comm.Get_rank()



if rank == 1:
    print("[Server] Waiting for filename...")


    filename = comm.recv(source=0, tag=TAG_FILENAME)
    print(f"[Server] Client requested: {filename}")

    if not os.path.exists(filename):
        print("[Server] File not found.")
        comm.send(-1, dest=0, tag=TAG_FILESIZE)

    else:
        size = os.path.getsize(filename)
        comm.send(size, dest=0, tag=TAG_FILESIZE)
        print(f"[Server] Sending file of size {size} bytes...")

        with open(filename, "rb") as f:
            while True:
                data = f.read(CHUNK)
                if not data:
                    break
                comm.send(data, dest=0, tag=TAG_DATA)

        print("[Server] File sent successfully!")


elif rank == 0:

    filename = input("Enter filename to download: ").strip()
    print(f"[Client] Requesting: {filename}")

    comm.send(filename, dest=1, tag=TAG_FILENAME)


    file_size = comm.recv(source=1, tag=TAG_FILESIZE)

    if file_size < 0:
        print("[Client] Server says: File not found.")
    else:
        print(f"[Client] File size = {file_size}")

        output_name = "received_" + filename
        received = 0

        print("[Client] Receiving file...")

        with open(output_name, "wb") as f:
            while received < file_size:
                chunk = comm.recv(source=1, tag=TAG_DATA)
                f.write(chunk)
                received += len(chunk)

        print(f"[Client] File received successfully: {output_name}")
