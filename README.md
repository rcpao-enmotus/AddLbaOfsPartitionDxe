# AddLbaOfsPartitionDxe

C:\UDK2015.Complete.MyWorkSpace\MyWorkSpace\MdeModulePkg\Universal\Disk\PartitionDxe

```
LBA0 "AddLbaOfs\0" signature and 64-bit value
Offset Description
0x0000 INT 18h
0x0010 "AddLbaOfs\0" signature
0x0028 64-bit (8-byte) big-endian value (0x2000 in this example)
0x01fe MBR signature (must not be '55 aa' because the built-in PartitionDxe+Mbr.c prevents us from reading LBA0)

0x0000 cd 18
0x0010 41 64 64 4c 62 61 4f 66-73 00 00 00 00 00 00 00  AddLbaOfs.......
0x0020 00 00 00 00 00 00 00 00-00 00 00 00 00 00 20 00  .............  .
0x0200                                           00 00  
```

When AddLbaOfs.c finds "AddLbaOfs\0" signature at LBA0 offset 0x0010, it will create a new BLK# starting at the 64-bit value in LBA0 offset 0x0028 and LastLBA will be decreased by the same value.  The original BLK# remains untouched and is still available.

The resulting new BLK# can be used like any other BlockIo device.  Any partitions will be assigned BLK#s and any file systems will be assigned FS#s.

'dd if=/dev/sdsrc of=/dev/sddst seek=1 bs=8192' will copy the sectors from LBA0 to (LastLBA - 0x2000) of /dev/sdsrc to /dev/sddst starting at (LBA + 0x2000) for (LastLBA - 0x2000) LBAs.

WARNING: The built-in PartitionDxe driver in the UEFI firmware prevents this driver from accessing any of its supported partition types.  UEFI does not provide an API to hook in a new partition type.  LBA0 offset 0x01fe should not be 0x55aa to prevent Mbr.c from detecting a valid MBR signature.  LBA1 and LastLBA should not have valid GPT PART signatures either.

This driver is an exercise in learning about UEFI.  It is not expected to have much practical use in the real world.
