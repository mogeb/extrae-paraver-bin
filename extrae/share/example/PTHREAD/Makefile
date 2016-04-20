include Makefile.inc

LIBS = -Wl,-rpath -Wl,$(EXTRAE_HOME)/lib -L$(EXTRAE_HOME)/lib -lpttrace
CFLAGS = -O -g -I$(EXTRAE_HOME)/include -pthread

targets: user-comms pthread-example my-example

user-comms: user-comms.c
	$(CC) $< $(CFLAGS) $(LIBS) -o $@ 

pthread-example: pthread-example.c
	$(CC) $< $(CFLAGS) $(LIBS) -o $@ 

my-example: my-example.c
	$(CC) $< $(CFLAGS) $(LIBS) -o $@  -lpopt

run-user-comms:
	EXTRAE_CONFIG_FILE=extrae.xml ./user-comms

merge-user-comms:
	$(EXTRAE_HOME)/bin/mpi2prv set-0/*.mpit -e ./user-comms

run-pthread-example:
	EXTRAE_CONFIG_FILE=extrae.xml ./pthread-example

merge-pthread-example:
	$(EXTRAE_HOME)/bin/mpi2prv set-0/*.mpit -e ./pthread-example

run-my-example:
	EXTRAE_CONFIG_FILE=extrae.xml ./my-example

merge-my-example:
	$(EXTRAE_HOME)/bin/mpi2prv set-0/*.mpit -e ./my-example

clean:
	rm -fr user-comms pthread-example set-0 *.prv *.pcf *.row TRACE.mpits TRACE.sym
