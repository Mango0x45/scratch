import random
import sys

runes = (random.randint(1, 0x3000) for _ in range(10_000_000))
sys.stdout.buffer.write(b''.join(chr(x).encode() for x in runes))
