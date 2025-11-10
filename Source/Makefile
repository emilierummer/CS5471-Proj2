CC := cc
CFLAGS := -O2
LDFLAGS :=

SRCDIR := .

all: ResetVictims ResetSeed Host Virus

ResetVictims:
	rm -f ./victim1 ./victim2 ./victim3
	cp /bin/ls ./victim1
	cp /usr/bin/wc ./victim2
	cp /usr/bin/file ./victim3
 	chmod ugo-x ./victim1 ./victim2 ./victim3
	cp /bin/ls ./host1
	cp /bin/ls ./host2
	cp /bin/ls ./host3
	cp /bin/ls ./host4

ResetSeed: Virus Host
	rm -f seed
	cp Virus seed
	bash -c "printf '\xde\xad\xbe\xef' >> seed"
	cat Host >> seed

Host: $(SRCDIR)/host.c
	$(CC) $(CFLAGS) -o Host $(SRCDIR)/host.c $(LDFLAGS)

Virus: $(SRCDIR)/virus.c
	$(CC) $(CFLAGS) -o Virus $(SRCDIR)/virus.c $(LDFLAGS)

clean:
	rm -rf victim1 victim2 victim3 Host Virus seed tem host1 host2 host3 host4
