#!bin/bash

nohup valgrind --tool=memcheck --leak-check=full --log-file=./valgrind.log ./game -c game.ini &

