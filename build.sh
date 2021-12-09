#!/bin/sh
gcc src/server.c src/utils.c -o build/server -g
gcc src/client.c src/utils.c -o build/client -g
