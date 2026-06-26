# python 
# import sys
# sys.path.insert(0, "/home/simen/Documents/master/code/gdb/py_venv/lib/python3.14/site-packages")
# end 
#
# source gdb/gdb-svd.py
#
# svd gdb/ATSAMV71Q21B.svd

target extended-remote localhost:3333

set pagination off
set logging file gdb/gdb.log
set logging enabled on 

# allow for reading the tracebuffer (which can be very large)
set max-value-size 200000

define reset 
    monitor reset halt
end

define tracedump
    dump binary value ./gdb/tracedump.bin *RecorderDataPtr
end

define upload
    reset
    load
    reset
end

