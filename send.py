import os

def calc_checksum(data):
    checksum = 0
    for byte in data:
        checksum ^= byte
    return checksum


def process_file(file_path, pipe_path):
    with open(file_path, "rb") as f:
        data = f.read()
    
    checksum = calc_checksum(data)
    
    message = bytearray([b for v in data for b in (v, 0)]) + bytes([checksum, 1])

    with open(pipe_path, "wb", buffering=0) as pipe:
        pipe.write(message)
    
    print("Checksum:", checksum)

file_path = "../test3.txt"
pipe_path = "/tmp/mypipe"

if not os.path.exists(pipe_path):
    os.mkfifo(pipe_path)

process_file(file_path, pipe_path)
