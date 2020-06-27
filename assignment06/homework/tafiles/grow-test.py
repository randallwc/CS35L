import sys
from time import sleep

s1 = b'*~BO *{_CIA *hXE]D *LER #@_G'
s2 = b'ZY #E\\OX #^BO #FKPS #NEM\4'
sys.stdout.buffer.write(s1)
sys.stdout.flush()
sleep(2)
sys.stdout.buffer.write(s2)
