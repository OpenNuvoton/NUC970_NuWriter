set dt=%date:/=%
::set newdate=%dt:~0,6%13
set year=%dt:~2,2%
set month=%dt:~4,2%
set day=%dt:~6,2%
set ver=13
set newdate=%year%%month%%day%%ver%
romh NuWriterFW_32MB.bin 0x01f00040 %newdate%
xcopy xusb.bin xusb32MB.bin  /m /q /Y
xcopy xusb32MB.bin ..\\..\\NuWriter\\NuWriter\\Release\\xusb32.bin /q /Y