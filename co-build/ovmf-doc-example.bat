rem C:\Users\rcpao\Documents\ovmf\ovmf.bat


rem https://qemu.weilnetz.de/w64/qemu-w64-setup-20160514.exe


rem "c:\Program Files\qemu\qemu-system-x86_64.exe" -bios ./OVMF-X64-r15214/OVMF.fd -hda fat:hda-contents
rem "c:\Program Files\qemu\qemu-system-x86_64.exe" -enable-kvm -net none -m 1024 -bios .\OVMF-X64-r15214\OVMF.fd -hda fat:hda-contents

rem FAIL (1k file?!) C:\en_bootefi-Enmotus\ovmf-run\OVMF.fd
rem PASS (debugcon.txt & console to serial.raw) \\10.3.171.5\Share\Enmotus\Experimental\UEFI\ovmf-run\OVMF.fd
rem "c:\Program Files\qemu\qemu-system-x86_64.exe" -no-kvm -net none -m 1024 -bios .\OVMF.fd -serial file:serial.txt -debugcon file:debugcon.txt -global isa-debugcon.iobase=0x402 -hda fat:hda-contents 
rem debugcon.txt is always empty (0 bytes)

rem "c:\Program Files\qemu\qemu-system-x86_64.exe" -no-kvm -net none -m 1024 -bios .\OVMF.fd -serial file:serial.txt -debugcon file:debugcon.txt -hda fat:hda-contents -hdb "EFI ESXi 6.0u1 1GB-4GB_single_drive_tier.qcow2"
"c:\Program Files\qemu\qemu-system-x86_64.exe" -no-kvm -net none -m 1024 -bios .\OVMF.fd -serial file:serial.raw -debugcon file:debugcon.txt -nodefaults -hda fat:hda-contents -hdb "w7pro64-vm-bun-1g-AddLbaOfs.qcow2"

rem "c:\Program Files\qemu\qemu-system-x86_64.exe" -no-kvm -net none -m 1024 -bios .\OVMF.fd -serial file:serial.txt -debugcon file:debugcon.txt -global isa-debugcon.iobase=0x402 -hda fat:hda-contents -hdb "C:\Users\develop1\Documents\Virtual Machines\EFI ESXi 6.0u1 1GB\EFI ESXi 6.0u1 1GB-4GB_single_drive_tier.raw"
rem "c:\Program Files\qemu\qemu-system-x86_64.exe" -no-kvm -net none -m 1024 -bios .\OVMF.fd -serial file:serial.txt -debugcon file:debugcon.txt -global isa-debugcon.iobase=0x402 -hda fat:hda-contents -hdb "C:\Users\develop1\Documents\Virtual Machines\EFI ESXi 6.0u1 1GB\EFI ESXi 6.0u1 1GB-4GB_single_drive_tier.raw" -hdc "C:\Users\develop1\Documents\Virtual Machines\EFI ESXi 6.0u1 1GB\EFI ESXi 6.0u1 1GB-4GB_single_drive_tier.raw"


rem Strip ANSI escape sequences for color http://serverfault.com/questions/71285/in-centos-4-4-how-can-i-strip-escape-sequences-from-a-text-file
rem sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[m|K]//g" < serial.raw > serial.txt
rem http://ascii-table.com/ansi-escape-sequences.php
rem Strip ANSI escape sequences for Esc[#;#m color, Esc[K erase line, Esc[2J clear screen, Esc[Line;ColumnH cursor positioning
rem sed -r "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[mKJH]//g" < serial.raw > serial.txt
rem ..., Esc[=3h set mode
sed -r -e "s/\x1B\[([0-9]{1,2}(;[0-9]{1,2})?)?[mKJH]//g" -e "s/\x1B\[=3h//g" serial.raw > serial.txt
