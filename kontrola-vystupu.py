#!/bin/python3
import re

Z_start = r"^[0-9][0-9]*: Z [0-9][0-9]*: started$"
Z_start_n = 0

Z_in = r"^[0-9][0-9]*: Z [0-9][0-9]*: entering office for a service [1-3]$"
Z_in_n = 0

Z_called = r"^[0-9][0-9]*: Z [0-9][0-9]*: called by office worker$"
Z_called_n = 0

Z_home = r"^[0-9][0-9]*: Z [0-9][0-9]*: going home$"
Z_home_n = 0

U_start = r"^[0-9][0-9]*: U [0-9][0-9]*: started$"
U_start_n = 0

U_home = r"^[0-9][0-9]*: U [0-9][0-9]*: going home$"
U_home_n = 0

U_break = r"^[0-9][0-9]*: U [0-9][0-9]*: taking break$"
U_break_n = 0

U_breakF = r"^[0-9][0-9]*: U [0-9][0-9]*: break finished$"
U_breakF_n = 0

U_serving = r"^[0-9][0-9]*: U [0-9][0-9]*: serving a service of type [1-3]$"
U_serving_n = 0

U_servingF = r"^[0-9][0-9]*: U [0-9][0-9]*: service finished$"
U_servingF_n = 0

Closing = r"^[0-9][0-9]*: closing$"
Closing_n = 0

with open('proj2.out', 'r') as f:
    for line in f:
        if re.search(Z_start, line):
            Z_start_n = 1
        elif re.search(Z_in, line):
            Z_in_n = 1
        elif re.search(Z_called, line):
            Z_called_n = 1
        elif re.search(Z_home, line):
            Z_home_n = 1
        elif re.search(U_start, line):
            U_start_n = 1
        elif re.search(U_home, line):
            U_home_n = 1
        elif re.search(U_break, line):
            U_break_n = 1
        elif re.search(U_breakF, line):
            U_breakF_n = 1
        elif re.search(U_serving, line):
            U_serving_n = 1
        elif re.search(U_servingF, line):
            U_servingF_n = 1
        elif re.search(Closing, line):
            Closing_n = 1
        else:
            print("Line format error:", line)

if not Z_start_n:
    print("WARNING: no Z started")
if not Z_in_n:
    print("WARNING: no Z entering office")
if not Z_called_n:
    print("WARNING: no Z called by office worker")
if not Z_home_n:
    print("WARNING: no Z going home")
if not U_start_n:
    print("WARNING: no U started")
if not U_break_n:
    print("WARNING: no U taking break")
if not U_breakF_n:
    print("WARNING: no U finishing break")
if not U_serving_n:
    print("WARNING: no U serving a service")
if not U_servingF_n:
    print("WARNING: no U finished a service")
if not Closing_n:
    print("WARNING: no closing")