# AddLbaOfsPartitionDxe

C:\UDK2015.Complete.MyWorkSpace\MyWorkSpace\MdeModulePkg\Universal\Disk\PartitionDxe

```
LBA0 "AddLbaOfs\0" signature and 64-bit value
Offset Description
0x0000 INT 18h
0x0010 AddLbaOfs signature
0x0028 64-bit value
0x01fe MBR signature (must not be '55 aa' because the built-in PartitionDxe+Mbr.c prevents us from reading LBA0)

0x0000 cd 18
0x0010 41 64 64 4c 62 61 4f 66-73 00 00 00 00 00 00 00  AddLbaOfs.......
0x0020 00 00 00 00 00 00 00 00-00 00 00 00 00 20 20 00  .............  .
0x0200                                           00 00  
```
