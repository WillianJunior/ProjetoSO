#
# Universidade de Brasília - UnB
# Alunos: Alexandre Lucchesi Alencar - 09/0104471
#         Willian Júnior - 09/0135806
#
# Sistemas Operacionais - Profa. Alba Melo
#
# Trabalho 1
#
# This code can be download in: https://github.com/WillianJunior/ProjetoSO.git
#
#

# Build project.
all: so_submit so_list spawner so_schedule so_shutdown examples

# Clean everything.
clean: clean_ipc
	-rm so_submit so_list spawner so_schedule so_shutdown helloworld fibonacci

# Delete all the IPC mechanisms created.
clean_ipc:
	-ipcrm -M 0x1927
	-ipcrm -M 0x1928
	-ipcrm -M 0x1929
	-ipcrm -M 0x192a
	-ipcrm -M 0x192b
	-ipcrm -M 0x192c
	-ipcrm -S 0x1927
	-ipcrm -S 0x1928
	-ipcrm -S 0x1929
	-ipcrm -S 0x192a
	-ipcrm -S 0x192b
	-ipcrm -Q 0x1927

so_submit: so_submit.c so_submit.h util.c util.h shrmem.c shrmem.h schedulers.c schedulers.h
	gcc -Wall -g so_submit.c util.c schedulers.c shrmem.c -o so_submit -lm

so_list: so_list.c so_list.h util.c util.h shrmem.c shrmem.h
	gcc -Wall -g so_list.c util.c shrmem.c -o so_list

spawner: spawner.c spawner.h shrmem.c shrmem.h util.c util.h
	gcc -Wall -g spawner.c shrmem.c util.c -o spawner

so_schedule: so_schedule.c so_schedule.h shrmem.c shrmem.h util.c util.h
	gcc -Wall -g so_schedule.c shrmem.c util.c -o so_schedule

so_shutdown: so_shutdown.c so_shutdown.h shrmem.c shrmem.h util.c util.h
	gcc -Wall -g so_shutdown.c shrmem.c util.c -o so_shutdown

examples:
	gcc -Wall -g helloworld.c -o helloworld
	gcc -Wall -g fibonacci.c -o fibonacci

