all: so_submit so_list spawner so_schedule

clean_ipc:
	-ipcrm -M 0x1927
	-ipcrm -M 0x1928
	-ipcrm -M 0x1929
	-ipcrm -M 0x1930
	-ipcrm -M 0x1931
	-ipcrm -M 0x1932o
	-ipcrm -S 0x1927
	-ipcrm -S 0x1928
	-ipcrm -Q 0x1927

clean: clean_ipc
	-rm so_submit so_list spawner so_schedule

so_submit: so_submit.c so_submit.h util.c util.h shrmem.c shrmem.h
	gcc -Wall so_submit.c util.c shrmem.c -o so_submit

so_list: so_list.c so_list.h util.c util.h shrmem.c shrmem.h
	gcc -Wall so_list.c util.c shrmem.c -o so_list

spawner: spawner.c spawner.h util.c util.h
	gcc -Wall spawner.c util.c -o spawner

so_schedule: so_schedule.c so_schedule.h shrmem.c shrmem.h util.c util.h
	gcc -Wall so_schedule.c shrmem.c util.c -o so_schedule