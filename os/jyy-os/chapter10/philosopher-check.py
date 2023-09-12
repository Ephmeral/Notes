import fileinput

parsed, values = 0, {}

for line in fileinput.input():
    sign, i, *_ = line.split()
    if sign == '+':
        x = 1
    else:
        x = -1
    values[i] = values.get(i, 0) + x

    assert all(0 <= x <= 1 for x in values.values())

    if (parsed := parsed + 1) % 10000 == 0:
        print(f'{parsed} Ok.')

