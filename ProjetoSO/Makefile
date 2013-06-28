all: so_submit so_list

clean:
	rm so_submit so_list
	ipcrm -M 0x1928
	ipcrm -M 0x1927

so_submit: so_submit.c so_submit.h util.c util.h shrmem.c shrmem.h
	gcc -Wall so_submit.c util.c shrmem.c -o so_submit

so_list: so_list.c so_list.h util.c util.h shrmem.c shrmem.h
	gcc -Wall so_list.c util.c shrmem.c -o so_list

