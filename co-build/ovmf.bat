rem C:\Users\%USERNAME%\Documents\NonBuildbotArea\UDK2014.IHV\AddLbaOfsPkg\co-build\ovmf.bat


rem https://qemu.weilnetz.de/w64/qemu-w64-setup-20160514.exe


rem co-build.bat
rem set SRCDIR=C:\Users\%USERNAME%\Documents\NonBuildbotArea\UDK2014.IHV\AddLbaOfsPkg\co-build\UDK2014_IHV\UDK2014.IHV\Build\AddLbaOfsPkg\RELEASE_VS2012x86\X64
rem set SRCDIR=UDK2014_IHV\UDK2014.IHV\Build\AddLbaOfsPkg\RELEASE_VS2012x86\X64

rem edksetup.bat; build.exe
set SRCDIR=..\..\..\UDK2014.IHV\Build\AddLbaOfsPartitionDxe\RELEASE_VS2012x86\X64

set DSTDIR=C:\Users\%USERNAME%\Documents\ovmf


dir %SRCDIR%\*.efi
dir %DSTDIR%\hda-contents\
copy %SRCDIR%\*.efi %DSTDIR%\hda-contents\
dir %DSTDIR%\hda-contents\

pushd %DSTDIR%
call ovmf.bat
popd
