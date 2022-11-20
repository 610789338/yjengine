#!/bin/bash

nohup ./game -c game.ini > game.log &
nohup ./gate -c gate.ini > gate.log &
nohup ./client -c client.ini > client.log &
