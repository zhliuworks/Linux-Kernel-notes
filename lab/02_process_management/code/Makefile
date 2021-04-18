SRC=/usr/src/linux-$(shell uname -r)

all:
	cp sched.h $(SRC)/include/linux/sched.h
	cp fork.c $(SRC)/kernel/fork.c
	cp core.c $(SRC)/kernel/sched/core.c
	cp base.c $(SRC)/fs/proc/base.c
	make -j2 -C $(SRC)
	make -C $(SRC) install
	reboot

restore:
	cp origin/sched.h $(SRC)/include/linux/sched.h
	cp origin/fork.c $(SRC)/kernel/fork.c
	cp origin/core.c $(SRC)/kernel/sched/core.c
	cp origin/base.c $(SRC)/fs/proc/base.c
	make -j2 -C $(SRC)
	make -C $(SRC) install
	reboot