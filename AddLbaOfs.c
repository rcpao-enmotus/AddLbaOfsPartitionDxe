/** @file
  Decode a hard disk partitioned with the legacy MBR found on most PC's

  MBR - Master Boot Record is in the first sector of a partitioned hard disk.
        The MBR supports four partitions per disk. The MBR also contains legacy
        code that is not run on an EFI system. The legacy code reads the 
        first sector of the active partition into memory and 

  BPB - BIOS Parameter Block is in the first sector of a FAT file system. 
        The BPB contains information about the FAT file system. The BPB is 
        always on the first sector of a media. The first sector also contains
        the legacy boot strap code.

Copyright (c) 2014, Hewlett-Packard Development Company, L.P.<BR>
Copyright (c) 2006 - 2013, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Partition.h"

#if 0
//#include <IndustryStandard/Mbr.h>

///
/// MBR Partition Table
///
typedef struct {
  UINT8                 BootStrapCode[440];
  UINT8                 UniqueMbrSignature[4];
  UINT8                 Unknown[2];
  MBR_PARTITION_RECORD  Partition[MAX_MBR_PARTITIONS];
  UINT16                Signature;
} MASTER_BOOT_RECORD;
#endif

///
/// AddLbaOfs in MBR BootStrapCode[]
///
#define ADDLBAOFS_SIG_STR "AddLbaOfs\0"
#define ADDLBAOFS_SIG_STR_LEN 10
#define ADDLBAOFS_SIG_OFS 0x10
#define ADDLBAOFS_32_OFS 0x28


//#include <Protocol/DevicePath.h>
//#define MBR_TYPE_PCAT             0x01
//#define SIGNATURE_TYPE_MBR        0x01


/**
  Test to see if the Mbr buffer is a valid MBR.

  @param  Mbr         Parent Handle.
  @param  LastLba     Last Lba address on the device.
   
  @retval TRUE        Mbr is a Valid MBR.
  @retval FALSE       Mbr is not a Valid MBR.

**/
BOOLEAN
PartitionValidAddLbaOfs (
  IN  MASTER_BOOT_RECORD      *Mbr,
  IN  EFI_LBA                 LastLba
  )
{
#undef FN
#define FN "PartitionValidMbr"
#define DBG_PartitionValidMbr DL_80 /* DL_DISABLED DL_80 */
#if 0
  UINT32  StartingLBA;
  UINT32  EndingLBA;
  UINT32  NewEndingLBA;
  INTN    Index1;
  INTN    Index2;
#endif
  BOOLEAN MbrValid;
  EFI_LBA AddLbaOfs;
  
  
  if (Mbr->Signature != MBR_SIGNATURE + 1) {
    return FALSE;
  }

  DBG_X(DBG_PartitionValidMbr, (PrBufxxdr((UINT8 *)&Mbr->BootStrapCode[ADDLBAOFS_SIG_OFS], ADDLBAOFS_SIG_STR_LEN)));
  if (CompareMem(&Mbr->BootStrapCode[ADDLBAOFS_SIG_OFS], ADDLBAOFS_SIG_STR, ADDLBAOFS_SIG_STR_LEN) != 0) {
    return FALSE;
  }
  
  AddLbaOfs = SwapBytes64(*((UINT64 *)((UINT8 *)&Mbr->BootStrapCode[ADDLBAOFS_32_OFS])));

  MbrValid = TRUE;

  return MbrValid;
}


/**
  Install child handles if the Handle supports AddLbaOfs format.

  @param[in]  This              Calling context.
  @param[in]  Handle            Parent Handle.
  @param[in]  DiskIo            Parent DiskIo interface.
  @param[in]  DiskIo2           Parent DiskIo2 interface.
  @param[in]  BlockIo           Parent BlockIo interface.
  @param[in]  BlockIo2          Parent BlockIo2 interface.
  @param[in]  DevicePath        Parent Device Path.
   
  @retval EFI_SUCCESS       A child handle was added.
  @retval EFI_MEDIA_CHANGED Media change was detected.
  @retval Others            MBR partition was not found.

**/
EFI_STATUS
PartitionInstallAddLbaOfsChildHandles (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Handle,
  IN  EFI_DISK_IO_PROTOCOL         *DiskIo,
  IN  EFI_DISK_IO2_PROTOCOL        *DiskIo2,
  IN  EFI_BLOCK_IO_PROTOCOL        *BlockIo,
  IN  EFI_BLOCK_IO2_PROTOCOL       *BlockIo2,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
#undef FN
#define FN "PartitionInstallAddLbaOfsChildHandles"
#define DBG_PartitionInstallAddLbaOfsChildHandles DL_80 /* DL_DISABLED DL_80 */
  EFI_STATUS                Status;
  MASTER_BOOT_RECORD        *Mbr;
  //UINT32                    ExtMbrStartingLba;
  UINTN                     Index;
  HARDDRIVE_DEVICE_PATH     HdDev;
  HARDDRIVE_DEVICE_PATH     ParentHdDev;
  EFI_STATUS                Found;
  UINT32                    PartitionNumber;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathNode;
  EFI_DEVICE_PATH_PROTOCOL  *LastDevicePathNode;
  UINT32                    BlockSize;
  UINT32                    MediaId;
  EFI_LBA                   LastBlock;
  EFI_LBA                   AddLbaOfs;


  DBG_PR(DBG_PartitionInstallAddLbaOfsChildHandles, "entered\n");

  Found           = EFI_NOT_FOUND;

  BlockSize = BlockIo->Media->BlockSize;
  MediaId   = BlockIo->Media->MediaId;
  LastBlock = BlockIo->Media->LastBlock;

  Mbr = AllocatePool (BlockSize);
  if (Mbr == NULL) {
    return Found;
  }

  Status = DiskIo->ReadDisk (
                     DiskIo,
                     MediaId,
                     0,
                     BlockSize,
                     Mbr
                     );
  DBG_PR(DBG_PartitionInstallAddLbaOfsChildHandles, "ReadDisk MediaId=%"PRIx32" %r\n", MediaId, Status);
  if (EFI_ERROR (Status)) {
    Found = Status;
    goto Done;
  }
  DBG_X(DBG_PartitionInstallAddLbaOfsChildHandles, (PrBufxxdr(Mbr, BlockSize)));
  if (!PartitionValidAddLbaOfs (Mbr, LastBlock)) {
    goto Done;
  }
  DBG_PR(DBG_PartitionInstallAddLbaOfsChildHandles, "ValidMbr\n");
  //
  // We have a valid mbr - add each partition
  //
  //
  // Get starting and ending LBA of the parent block device.
  //
  LastDevicePathNode = NULL;
  ZeroMem (&ParentHdDev, sizeof (ParentHdDev));
  DevicePathNode = DevicePath;
  while (!IsDevicePathEnd (DevicePathNode)) {
    LastDevicePathNode  = DevicePathNode;
    DevicePathNode      = NextDevicePathNode (DevicePathNode);
  }

  if (LastDevicePathNode != NULL) {
    if (DevicePathType (LastDevicePathNode) == MEDIA_DEVICE_PATH &&
        DevicePathSubType (LastDevicePathNode) == MEDIA_HARDDRIVE_DP
        ) {
      CopyMem (&ParentHdDev, LastDevicePathNode, sizeof (ParentHdDev));
    } else {
      LastDevicePathNode = NULL;
    }
  }

  PartitionNumber = 1;

  ZeroMem (&HdDev, sizeof (HdDev));
  HdDev.Header.Type     = MEDIA_DEVICE_PATH;
  HdDev.Header.SubType  = MEDIA_HARDDRIVE_DP; //TBD MEDIA_VENDOR_DP
  SetDevicePathNodeLength (&HdDev.Header, sizeof (HdDev));
  HdDev.MBRType         = MBR_TYPE_PCAT; //TBD
  HdDev.SignatureType   = SIGNATURE_TYPE_GUID + 1; //TBD

  if (LastDevicePathNode == NULL) {
    //
    // This is a MBR, add each partition
    //
    Index = 0;
    /* for (Index = 0; Index < MAX_MBR_PARTITIONS; Index++) { */
      AddLbaOfs = SwapBytes64(*((UINT64 *)((UINT8 *)&Mbr->BootStrapCode[ADDLBAOFS_32_OFS])));
      
      HdDev.PartitionNumber = PartitionNumber ++;
      HdDev.PartitionStart  = UNPACK_UINT32 (Mbr->Partition[Index].StartingLBA + AddLbaOfs);
      HdDev.PartitionSize   = UNPACK_UINT32 (Mbr->Partition[Index].SizeInLBA - AddLbaOfs);
      CopyMem (HdDev.Signature, &(Mbr->UniqueMbrSignature[0]), sizeof (Mbr->UniqueMbrSignature));

      Status = PartitionInstallChildHandle (
                This,
                Handle,
                DiskIo,
                DiskIo2,
                BlockIo,
                BlockIo2,
                DevicePath,
                (EFI_DEVICE_PATH_PROTOCOL *) &HdDev,
                HdDev.PartitionStart,
                HdDev.PartitionStart + HdDev.PartitionSize - 1,
                MBR_SIZE,
                (BOOLEAN) (Mbr->Partition[Index].OSIndicator == EFI_PARTITION)
                );

      if (!EFI_ERROR (Status)) {
        Found = EFI_SUCCESS;
      }
    /* } /* for */
  } else {
#if 1
    //Found = EFI_NOT_FOUND;
    goto Done;
#else
    //
    // It's an extended partition. Follow the extended partition
    // chain to get all the logical drives
    //
    ExtMbrStartingLba = 0;

    do {

      Status = DiskIo->ReadDisk (
                         DiskIo,
                         MediaId,
                         MultU64x32 (ExtMbrStartingLba, BlockSize),
                         BlockSize,
                         Mbr
                         );
      if (EFI_ERROR (Status)) {
        Found = Status;
        goto Done;
      }

      if (UNPACK_UINT32 (Mbr->Partition[0].SizeInLBA) == 0) {
        break;
      }

      if ((Mbr->Partition[0].OSIndicator == EXTENDED_DOS_PARTITION) ||
          (Mbr->Partition[0].OSIndicator == EXTENDED_WINDOWS_PARTITION)) {
        ExtMbrStartingLba = UNPACK_UINT32 (Mbr->Partition[0].StartingLBA);
        continue;
      }
      HdDev.PartitionNumber = PartitionNumber ++;
      HdDev.PartitionStart  = UNPACK_UINT32 (Mbr->Partition[0].StartingLBA) + ExtMbrStartingLba + ParentHdDev.PartitionStart;
      HdDev.PartitionSize   = UNPACK_UINT32 (Mbr->Partition[0].SizeInLBA);
      if ((HdDev.PartitionStart + HdDev.PartitionSize - 1 >= ParentHdDev.PartitionStart + ParentHdDev.PartitionSize) ||
          (HdDev.PartitionStart <= ParentHdDev.PartitionStart)) {
        break;
      }

      //
      // The signature in EBR(Extended Boot Record) should always be 0.
      //
      *((UINT32 *) &HdDev.Signature[0]) = 0;

      Status = PartitionInstallChildHandle (
                 This,
                 Handle,
                 DiskIo,
                 DiskIo2,
                 BlockIo,
                 BlockIo2,
                 DevicePath,
                 (EFI_DEVICE_PATH_PROTOCOL *) &HdDev,
                 HdDev.PartitionStart - ParentHdDev.PartitionStart,
                 HdDev.PartitionStart - ParentHdDev.PartitionStart + HdDev.PartitionSize - 1,
                 MBR_SIZE,
                 (BOOLEAN) (Mbr->Partition[0].OSIndicator == EFI_PARTITION)
                 );
      if (!EFI_ERROR (Status)) {
        Found = EFI_SUCCESS;
      }

      if ((Mbr->Partition[1].OSIndicator != EXTENDED_DOS_PARTITION) &&
          (Mbr->Partition[1].OSIndicator != EXTENDED_WINDOWS_PARTITION)
          ) {
        break;
      }

      ExtMbrStartingLba = UNPACK_UINT32 (Mbr->Partition[1].StartingLBA);
      //
      // Don't allow partition to be self referencing
      //
      if (ExtMbrStartingLba == 0) {
        break;
      }
    } while (ExtMbrStartingLba  < ParentHdDev.PartitionSize);
#endif
  }

Done:
  FreePool (Mbr);

  return Found;
}
