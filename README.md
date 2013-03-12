super_awesome_pulse_timer
=========================

Pulse width timer demo for the staunchpad, meant for use with Energia.

For some reason lm4flash (which Energia uses) hangs when flashing anything that has TimerIntEnable() usage. It uploads fine, you just have to powercycle the board (device/debug switch works good for that) after a few seconds for it to run. /shrug
