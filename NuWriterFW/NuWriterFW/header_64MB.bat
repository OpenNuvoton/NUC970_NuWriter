set dt=%date:/=%
set newdate=%dt:~0,6%13
romh NuWriterFW_64MB.bin 0x03f00040 %newdate%
xcopy xusb.bin xusb64MB.bin  /m /q /Y
xcopy xusb64MB.bin ..\\..\\NuWriter\\NuWriter\\Release\\xusb64.bin /q /Y