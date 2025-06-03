#!/bin/python3

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

    full_length = len(data) + 8

    print("data len:", len(data))
    print("full len:", full_length)

    length = full_length.to_bytes(4, "little")
    length_checksum = calc_checksum(length).to_bytes()
    checksum = calc_checksum(data).to_bytes()

    message = length + length_checksum + checksum + bytes([0xAA]) + data + bytes([0xAA])

    print("len checksum:", int.from_bytes(length_checksum))
    print("all checksum:", int.from_bytes(checksum))

    with open(pipe_path, "wb", buffering=0) as pipe:
        for i in range(0, len(message), CHUNK_SIZE):
            pipe.write(message[i : i + CHUNK_SIZE])
            pipe.flush()
        time.sleep(0.01)


file_path = "build/output/kernel8.img"
pipe_path = "/tmp/mypipe"

if not os.path.exists(pipe_path):
    os.mkfifo(pipe_path)

process_file(file_path, pipe_path)
