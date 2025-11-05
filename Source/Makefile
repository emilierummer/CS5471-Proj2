CC := cc
CFLAGS := -O2
LDFLAGS :=

SRCDIR := .

all: ResetVictims ResetSeed Host Virus temp

ResetVictims:
	rm -f ./victim1 ./victim2 ./victim3
	cp /bin/ls ./victim1
	cp /usr/bin/wc ./victim2
	cp /usr/bin/file ./victim3
# 	chmod ugo-x ./victim1 ./victim2 ./victim3

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
	rm -rf ./victim1 ./victim2 ./victim3 Host Virus tem

temp:
	rm -f concatBin
	rm -f temp_executable
	rm -f test
	$(CC) $(CFLAGS) -o test $(SRCDIR)/test.c $(LDFLAGS)

printSizes:
	@echo "Sizes of files:"
	@ls -l Virus Host seed test concatBin temp_executable

makeBins:
	@echo "Creating hex dumps of files:"
	xxd -g 1 Virus > Virus.hex
	xxd -g 1 Host > Host.hex
	xxd -g 1 seed > seed.hex
	xxd -g 1 test > test.hex
	xxd -g 1 concatBin > concatBin.hex
	xxd -g 1 temp_executable > temp_executable.hex