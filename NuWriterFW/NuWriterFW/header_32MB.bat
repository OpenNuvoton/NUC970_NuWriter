set dt=%date:/=%
set newdate=%dt:~0,6%08
romh NuWriterFW_32MB.bin 0x01f00040 %newdate%
xcopy xusb.bin xusb32MB.bin  /m /q /Y
xcopy xusb32MB.bin ..\\..\\NuWriter\\NuWriter\\Release\\xusb.bin /q /Y