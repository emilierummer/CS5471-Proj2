#!/bin/bash

echo "Testing..."

# Clean and reset
make clean
make all

# Test 1: infecting other files
echo -e "\n\n\n"
echo "===== TEST 1: Infecting files ====="
chmod  777 victim1 victim2 victim3
echo "seeding victim1"
stdbuf -oL ./seed victim1 | sed 's/^/    /'
echo "infecting victim2"
stdbuf -oL ./victim1 victim2 | sed 's/^/    /'
echo "infecting victim3"
stdbuf -oL ./victim2 victim3 | sed 's/^/    /'

# Test 2: mutation
echo -e "\n\n\n"
echo "===== TEST 2: Mutation ====="
echo "seeding host1"
stdbuf -oL ./seed host1 | sed 's/^/    /'
echo "infecting host2"
stdbuf -oL ./host1 host2 | sed 's/^/    /'
echo "infecting host3"
stdbuf -oL ./host2 host3 | sed 's/^/    /'
echo "infecting host4"
stdbuf -oL ./host3 host4 | sed 's/^/    /'
echo "hash test"
sha256sum host1 | sed 's/^/    /'
sha256sum host2 | sed 's/^/    /'
sha256sum host3 | sed 's/^/    /'
sha256sum host4 | sed 's/^/    /'
