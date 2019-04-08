set dt=%date:/=%
set newdate=%dt:~2,6%03
romh NuWriterFW_16MB.bin 0x0f00040 %newdate%
xcopy xusb.bin xusb16MB.bin /m /q /Y 
xcopy xusb16MB.bin ..\\..\\NuWriter\\NuWriter\\Release\\xusb16.bin /q /Y