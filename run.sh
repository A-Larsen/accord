#!/bin/sh

xdotool keydown Super_L 1 keyup Super_L 2 keydown Super_L j keyup Super_L j 
chromium "0.0.0.0:8080/signup"
./a.out
