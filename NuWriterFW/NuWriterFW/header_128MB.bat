set dt=%date:/=%
set newdate=%dt:~0,6%13
romh NuWriterFW_128MB.bin 0x07f00040 %newdate%
xcopy xusb.bin xusb128MB.bin  /m /q /Y
xcopy xusb128MB.bin ..\\..\\NuWriter\\NuWriter\\Release\\xusb128.bin /q /Y