INC=-I../core/device/utility -I../core/device/ -I../core/listener

LINX_CORE=../core/device/utility/LinxDevice.cpp
RPI2_CORE=$(LINX_CORE) ../core/device/utility/LinxRaspberryPi.cpp ../core/device/LinxRaspberryPi2B.cpp
BBB_CORE=$(LINX_CORE) ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp

LISTENER_CORE=../core/listener/utility/LinxListener.cpp
SERIAL_LISTENER=$(LISTENER_CORE) ../core/listener/LinxSerialListener.cpp
TCP_LISTENER=$(LISTENER_CORE) ../core/listener/LinxLinuxTcpListener.cpp
CONFIG_LISTENER=$(LISTENER_CORE) ../core/listener/LinxSerialListener.cpp ../core/listener/LinxLinuxTcpListener.cpp


all: release debug main

beagleBoneBlackAll: beagleBoneBlackSerial beagleBoneBlackTcp beagleBoneBlackConfigurable

raspberryPi2BAll: raspberryPi2BSerial raspberryPi2BTcp raspberryPi2BConfigurable raspberryPi2BLib
	
#----------------------- Shared Objects -----------------------
raspberryPi2BLib:
	g++ $(INC) -Wall -shared -fPIC -lrt -o ../core/examples/LinxDeviceLib/bin/liblinxdevice.so.2.1.0 ../core/examples/LinxDeviceLib/src/LinxDeviceLib.cpp $(RPI2_CORE) -DLINXCONFIG -DDEBUG_ENABLED=-1
	g++ $(INC) ../core/examples/LinxDeviceLib/src/LinxDeviceLibTestApp.cpp -lrt -o ../core/examples/LinxDeviceLib/bin/LinxDeviceLibTetsApp.out -L~/git/LINX/LabVIEW/vi.lib/MakerHub/LINX/Firmware/Source/LinxDeviceLib/bin -llinxdevice -DLINXCONFIG -DDEBUG_ENABLED=-1
#----------------------- Listeners -----------------------
	
beagleBoneBlackSerial:
	g++ $(INC) ../core/examples/Beagle_Bone_Black_Serial/src/Beagle_Bone_Black_Serial.cpp $(BBB_CORE) $(SERIAL_LISTENER) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../core/examples/Beagle_Bone_Black_Serial/bin/beagleBoneBlackSerial.out

beagleBoneBlackTcp:
	g++ $(INC) ../core/examples/Beagle_Bone_Black_Tcp/src/Beagle_Bone_Black_Tcp.cpp $(BBB_CORE) $(TCP_LISTENER) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../core/examples/Beagle_Bone_Black_Tcp/bin/beagleBoneBlackTcp.out
	
beagleBoneBlackConfigurable:
	g++ $(INC) ../core/examples/Beagle_Bone_Black_Configurable/src/Beagle_Bone_Black_Configurable.cpp $(BBB_CORE) $(CONFIG_LISTENER) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/Beagle_Bone_Black_Configurable/bin/beagleBoneBlackConfigurable.out
	
raspberryPi2BSerial:
	g++ $(INC) ../core/examples/RaspberryPi_2_B_Serial/src/RaspberryPi_2_B_Serial.cpp $(RPI2_CORE) $(SERIAL_LISTENER) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/RaspberryPi_2_B_Serial/bin/raspberryPi2BSerial.out
	
raspberryPi2BTcp:
	g++ $(INC) ../core/examples/RaspberryPi_2_B_Tcp/src/RaspberryPi_2_B_Tcp.cpp $(RPI2_CORE) $(TCP_LISTENER) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/RaspberryPi_2_B_Tcp/bin/raspberryPi2BTcp.out

raspberryPi2BConfigurable:
	g++ $(INC) ../core/examples/RaspberryPi_2_B_Configurable/src/RaspberryPi_2_B_Configurable.cpp $(RPI2_CORE) $(CONFIG_LISTENER) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/RaspberryPi_2_B_Configurable/bin/raspberryPi2BConfigurable.out
	
#----------------------- Tests -----------------------
tests: dio-test i2c-test spi-test

dio-test:
	g++ $(INC) ../tests/src/dio-test.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/diotest.out 

rpi2DioTest:
	g++ $(INC) ../tests/src/rpi2/rpi2DioTest.cpp $(RPI2_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/rpi2/dioTest.out 
	
rpi2UartTest:
	g++ $(INC) ../tests/src/rpi2/rpi2UartTest.cpp $(RPI2_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../tests/bin/rpi2/uartTest.out 
	
rpi2SpiTest:
	g++ -g $(INC) ../tests/src/rpi2/rpi2SpiTest.cpp $(RPI2_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/rpi2/spiTest.out 
	
i2c-test:
	g++ $(INC) ../tests/src/i2c-test.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/i2ctest.out 

pwm-test:
	g++ $(INC) ../tests/src/pwm-test.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/pwmtest.out 
	
spi-test:
	g++ $(INC) ../tests/src/spi-test.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/spitest.out 

uart-test:
	g++ $(INC) ../tests/src/uart-test.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/uarttest.out 
	
#----------------------- Utils -----------------------
utils: blink analogRead uartLoopback

analogRead:
	g++ $(INC) ../utils/src/analogRead.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/analogRead.out

blink:
	g++ $(INC) ../utils/src/blink.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/blink.out 

pwmSetDutyCycle:
	g++ $(INC) ../utils/src/pwmSetDutyCycle.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/pwmSetDutyCycle.out 
	
uartLoopback:
	g++ $(INC) ../utils/src/uartLoopback.cpp $(BBB_CORE) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/uartLoopback.out 
	
cleanUtils: 
	rm -rf ../utils/bin/*

cleanall:
	rm -rf *.o
	rm -rf *.out
	rm -rf ../bin/*