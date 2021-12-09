all: nsip
 
nsip: utils.c client.c server.c
    gcc -o -Wall -Werror nsip utils.c client.c server.c

clean:
    rm -rf *.o *~ nsip