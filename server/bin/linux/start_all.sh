#!/bin/bash

./game -c game.ini &
./game -c game1.ini &
./game -c game2.ini &
sleep 0.5

./gate -c gate.ini &
./gate -c gate1.ini &
sleep 0.5

./client -c client.ini &
./client -c client.ini &
