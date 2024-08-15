#!/usr/bin/bash

rm ./*.o
rm ./*/*.o
rm ./build/main
gcc -c ./lib/protocol.c
gcc -c ./lib/server.c
gcc ./src/main.c server.o protocol.o -o ./build/main
rm ./*.o
rm ./*/*.o
