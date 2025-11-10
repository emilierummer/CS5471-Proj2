# Compilation Steps

Everything can be made with `make all` and cleaned up with `make clean`

Specific setup steps (not necessary with `make all`):
Re-copy victim binaries: `make ResetVictims`
Recreate seed binary: `make ResetSeed`

# Mutation Process

The virus mutates by appending an empty byte to the end of the file
binary once it has been infected.