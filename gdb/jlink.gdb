target extended-remote localhost:2331

set pagination off
set logging file gdb/gdb.log
set logging enabled on 

# allow for reading the tracebuffer (which can be very large)
set max-value-size 200000

define reset 
    monitor reset
end

define tracedump
    dump binary value ./gdb/tracedump.bin *RecorderDataPtr
end

define upload
    reset
    load
    reset
end

