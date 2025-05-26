import os, time


def calc_checksum(data):
    checksum = 0
    for byte in data:
        checksum ^= byte
    return checksum


CHUNK_SIZE = 128


def process_file(file_path, pipe_path):
    with open(file_path, "rb") as f:
        data = f.read()

    # data = os.urandom(37892 // 2)

    checksum = calc_checksum(data)

    print("len:", len(data))

    message = bytearray([b for v in data for b in (v, 0)]) + bytes([checksum, 1])

    with open(pipe_path, "wb", buffering=0) as pipe:
        for i in range(0, len(message), CHUNK_SIZE):
            pipe.write(message[i : i + CHUNK_SIZE])
            pipe.flush()
        time.sleep(0.01)

    print("checksum:", checksum)


file_path = "build/output/kernel8.img"
pipe_path = "/tmp/mypipe"

if not os.path.exists(pipe_path):
    os.mkfifo(pipe_path)

process_file(file_path, pipe_path)
