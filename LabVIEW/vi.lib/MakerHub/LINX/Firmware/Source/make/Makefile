INC=-I../core/device/utility -I../core/device/ -I../core/listener

all: release debug main

Listeners: beagleBoneBlackSerial
	
#----------------------- Listeners -----------------------
	
#pi:
#	g++ $(INC) testapp.cpp ../core/device/config/LinxRaspberryPiConfig.h ../core/device/utility/LinxRaspberryPi.h ../core/device/utility/LinxRaspberryPi.cpp -DLINXCONFIG=1 -o ../bin/pi.out
	
beagleBoneBlackSerial:
	g++ $(INC) ../core/examples/Beagle_Bone_Black_Serial/src/Beagle_Bone_Black_Serial.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp ../core/listener/utility/LinxListener.cpp ../core/listener/LinxSerialListener.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../core/examples/Beagle_Bone_Black_Serial/bin/beagleBoneBlackSerial.out

beagleBoneBlackTcp:
	g++ $(INC) ../core/examples/Beagle_Bone_Black_Tcp/src/Beagle_Bone_Black_Tcp.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp ../core/listener/utility/LinxListener.cpp ../core/listener/LinxLinuxTcpListener.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../core/examples/Beagle_Bone_Black_Tcp/bin/beagleBoneBlackTcp.out
	
beagleBoneBlackConfigurable:
	g++ $(INC) ../core/examples/Beagle_Bone_Black_Configurable/src/Beagle_Bone_Black_Configurable.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp ../core/listener/utility/LinxListener.cpp  ../core/listener/LinxSerialListener.cpp ../core/listener/LinxLinuxTcpListener.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../core/examples/Beagle_Bone_Black_Configurable/bin/beagleBoneBlackConfigurable.out
	
bbb:
	g++ $(INC) LinxBeagleBoneBlack_TestApp.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -o ../bin/BeagleBoneBlack_TestApp.out 
	
bbb_debug:
	g++ $(INC) LinxBeagleBoneBlack_TestApp.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../bin/BeagleBoneBlack_TestApp_Debug.out 

#----------------------- Tests -----------------------
tests: dio-test i2c-test spi-test

dio-test:
	g++ $(INC) ../tests/src/dio-test.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/diotest.out 

i2c-test:
	g++ $(INC) ../tests/src/i2c-test.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/i2ctest.out 

pwm-test:
	g++ $(INC) ../tests/src/pwm-test.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/pwmtest.out 
	
spi-test:
	g++ $(INC) ../tests/src/spi-test.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/spitest.out 

uart-test:
	g++ $(INC) ../tests/src/uart-test.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/uarttest.out 
	
#----------------------- Utils -----------------------
utils: blink analogRead uartLoopback

analogRead:
	g++ $(INC) ../utils/src/analogRead.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/analogRead.out

blink:
	g++ $(INC) ../utils/src/blink.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/blink.out 

pwmSetDutyCycle:
	g++ $(INC) ../utils/src/pwmSetDutyCycle.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/pwmSetDutyCycle.out 
	
uartLoopback:
	g++ $(INC) ../utils/src/uartLoopback.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/uartLoopback.out 
	
cleanUtils: 
	rm -rf ../utils/bin/*
	
#serial: 
#	g++ LinxSerialDaemon.cpp listener/LinxListener.cpp listener/LinxSerialListener.cpp device/LinxDevice.cpp device/raspberrypi/LinxRaspberryPi.cpp device/raspberrypi/LinxRaspberryPi-B.cpp -lrt -DLINXDEVICE=LinxRaspberryPiB -DDEBUG_ENABLED=1 -o serialdebug.out

#uno:	
#	cd device/wiring/arduino/uno && make	

cleanall:
	rm -rf *.o
	rm -rf *.out
	rm -rf ../bin/*