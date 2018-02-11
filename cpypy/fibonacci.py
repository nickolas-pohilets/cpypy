stop = False

f0 = 1
f1 = 1
while not stop:
    f0, f1 = f1, (f0 + f1)
    print(f0)
