#!/bin/sh

ps -ef|grep -E 'game|gate|client'|grep -v tail|grep -v grep|awk -F " " '{print $2}'|xargs kill

