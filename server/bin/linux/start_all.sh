#!/bin/bash

env PPROF_PATH=./pprof HEAPCHECK=normal ./game -c game.ini &
env PPROF_PATH=./pprof HEAPCHECK=normal ./game -c game1.ini &
env PPROF_PATH=./pprof HEAPCHECK=normal ./game -c game2.ini &
sleep 0.5

env PPROF_PATH=./pprof HEAPCHECK=normal ./gate -c gate.ini &
env PPROF_PATH=./pprof HEAPCHECK=normal ./gate -c gate1.ini &
sleep 0.5

env PPROF_PATH=./pprof HEAPCHECK=normal ./client -c client.ini &
env PPROF_PATH=./pprof HEAPCHECK=normal ./client -c client.ini &
