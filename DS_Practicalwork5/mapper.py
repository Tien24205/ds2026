import sys

for line in sys.stdin:
    path = line.strip()
    if path:
        print(f"longest\t{path}\t{len(path)}")