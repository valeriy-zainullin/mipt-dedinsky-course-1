#!/bin/bash

if [ "$2" != "-d" ]; then
	rm -f dll_list.txt
	touch dll_list.txt
fi

echo "$1" > dll_list.txt || exit 1

IFS=$'\n'
for dll in $(objdump -p "$1" | grep 'DLL:' | sed 's/^.*DLL: *\(.*\)$/\1/g'); do
	which "$dll" > /dev/null 2>&1
	if [ "$?" -ne 0 ]; then
		printf "%s -> %s\n" "$1" "$dll"
		grep -F "$dll" dll_list.txt > /dev/null
		exit_code="$?"
		if [ "$exit_code" -eq 0 ]; then
			echo "Dependency cycle encountered."
			exit 1
		fi
		if [ "$exit_code" -eq 2 ]; then
			echo "Failed to read dll_list.txt"
			exit 1
		fi
		cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/"$dll" . || exit 1
		./copy_dlls.sh "$dll" -d || exit 1
	fi
done

grep -v -F "$1" dll_list.txt > dll_list.txt.2
mv dll_list.txt.2 dll_list.txt

if [ "$2" != "-d" ]; then
	rm -f dll_list.txt
fi
