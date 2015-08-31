set pagination off
set logging file gdb.txt
set logging redirect on
set logging on
catch throw
echo Run application in GDB
continue
backtrace full
set logging off
quit
