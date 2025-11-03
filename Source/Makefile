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
	rm -f ./victim1 ./victim2 ./victim3 ./seed Host Virus
	rm -r tem

# IPCHECK_SRC := $(SRCDIR)/IPCheck.c
# RBFGEN_SRC := $(SRCDIR)/RBFGen.c
# HEADERS := $(SRCDIR)/HashUtils.h
# HASHUTILS_SRC := $(SRCDIR)/HashUtils.c
# HASHUTILS_OBJ := HashUtils.o

# all: IPCheck RBFGen


# IPCheck: $(IPCHECK_SRC) $(HEADERS)
# 	$(CC) $(CFLAGS) -c $(HASHUTILS_SRC) -o $(HASHUTILS_OBJ)
# 	$(CC) $(CFLAGS) -o IPCheck $(IPCHECK_SRC) $(HASHUTILS_OBJ) -lcrypto $(LDFLAGS)

# # RBFGen depends on OpenSSL for SHA256

# RBFGen: $(RBFGEN_SRC) $(HEADERS)
# 	$(CC) $(CFLAGS) -c $(HASHUTILS_SRC) -o $(HASHUTILS_OBJ)
# 	$(CC) $(CFLAGS) -o RBFGen $(RBFGEN_SRC) $(HASHUTILS_OBJ) -lcrypto $(LDFLAGS)

# clean:
# 	rm -f IPCheck RBFGen HashUtils.o