set dt=%date:/=%
::set newdate=%dt:~0,6%13
set year=%dt:~2,2%
set month=%dt:~4,2%
set day=%dt:~6,2%
set ver=13
set newdate=%year%%month%%day%%ver%
romh NuWriterFW_64MB.bin 0x03f00040 %newdate%
xcopy xusb.bin xusb64MB.bin  /m /q /Y
xcopy xusb64MB.bin ..\\..\\NuWriter\\NuWriter\\Release\\xusb64.bin /q /Y