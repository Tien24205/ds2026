import sys

max_length = -1
longest_paths = []

for line in sys.stdin:
    _, path, length = line.strip().split("\t")
    length = int(length)

    if length > max_length:
        max_length = length
        longest_paths = [path]
    elif length == max_length:
        longest_paths.append(path)

for p in longest_paths:
    print(p)