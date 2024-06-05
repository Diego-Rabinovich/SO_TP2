include Makefile.inc

USER_DATA=0001-userData.bin

all: userCode userData

userCode:
	cd User; make

userData:
	printf "This is sample data." >> $(USER_DATA) && dd if=/dev/zero bs=1 count=1 >> $(USER_DATA)

clean:
	cd User; make clean
	rm -rf *.bin


.PHONY: userCode all clean
