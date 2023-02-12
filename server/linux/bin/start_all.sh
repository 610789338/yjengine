#!/bin/bash

nohup ./game -c game1.ini > game1.log &
nohup ./game -c game2.ini > game2.log &
nohup ./game -c game3.ini > game3.log &
nohup ./gate -c gate1.ini > gate1.log &
nohup ./gate -c gate2.ini > gate2.log &
nohup ./client -c client.ini > client.log &
