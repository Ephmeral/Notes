import sys

BATCH_SIZE = 100000

limit = int(sys.argv[1])
count, checked = 0, 0

while True:
    for ch in sys.stdin.read(BATCH_SIZE):
        match ch:
            case '(': count += 1
            case ')': count -= 1
            case _: assert 0
        assert 0 <= count <= limit
    checked += BATCH_SIZE
    print(f'{checked} Ok.')