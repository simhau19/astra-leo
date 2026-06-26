send-keys "nvim ." Enter
new-window -n "serial console" 
send-keys "tio /dev/serial/by-id/usb-Atmel_Corp._EDBG_CMSIS-DAP_ATML2407121800000082-if01" Enter
new-window -n "debugger (gdb)"
send-keys "gdb-multiarch -x gdb/openocd.gdb build/samv71_dk-debug/src/platform/samv71q21b/astra_leo_fw.elf -tui" Enter
new-window
send-keys "openocd -f interface/cmsis-dap.cfg -f target/atsamv.cfg" Enter
select-window -t 1
