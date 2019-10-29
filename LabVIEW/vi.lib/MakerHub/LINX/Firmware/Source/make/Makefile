CXX ?= g++

INC=-I../core/device/utility -I../core/device/ -I../core/listener

CORE_LINX=../core/device/utility/LinxDevice.cpp
CORE_LISTENER=../core/listener/utility/LinxListener.cpp
CORE_RPI2=$(CORE_LINX) ../core/device/utility/LinxRaspberryPi.cpp ../core/device/LinxRaspberryPi2B.cpp
CORE_BBB=$(CORE_LINX) ../core/device/utility/LinxBeagleBone.cpp ../core/device/LinxBeagleBoneBlack.cpp

LISTENER_SERIAL=$(CORE_LISTENER) ../core/listener/LinxSerialListener.cpp
LISTENER_TCP=$(CORE_LISTENER) ../core/listener/LinxLinuxTcpListener.cpp
LISTENER_CONFIG=$(CORE_LISTENER) ../core/listener/LinxSerialListener.cpp ../core/listener/LinxLinuxTcpListener.cpp

HW_RPI2B = -DLINX_DEVICE_FAMILY=4 -DLINX_DEVICE_ID=3
HW_BBB = -DLINX_DEVICE_FAMILY=6 -DLINX_DEVICE_ID=1


libs: raspberryPi2BLib beagleBoneBlackLib

allio: beagleBoneBlackAll raspberryPi2BAll

beagleBoneBlackAll: beagleBoneBlackSerial beagleBoneBlackTcp beagleBoneBlackConfigurable

raspberryPi2BAll: raspberryPi2BSerial raspberryPi2BTcp raspberryPi2BConfigurable
	
#----------------------- Shared Objects -----------------------
raspberryPi2BLib:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) -Wall -shared -fPIC -lrt -o ../core/examples/LinxDeviceLib/bin/liblinxdevice_rpi2.so ../core/examples/LinxDeviceLib/src/LinxDeviceLib.cpp $(CORE_RPI2) $(HW_RPI2B) -DLINXCONFIG -DDEBUG_ENABLED=-1 -g

beagleBoneBlackLib:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) -Wall -shared -fPIC -lrt -o ../core/examples/LinxDeviceLib/bin/liblinxdevice_bbb.so ../core/examples/LinxDeviceLib/src/LinxDeviceLib.cpp $(CORE_BBB) $(HW_BBB) -DLINXCONFIG -DDEBUG_ENABLED=-1 -g
#----------------------- Listeners -----------------------
	
beagleBoneBlackSerial:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../core/examples/Beagle_Bone_Black_Serial/src/Beagle_Bone_Black_Serial.cpp $(CORE_BBB) $(LISTENER_SERIAL) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../core/examples/Beagle_Bone_Black_Serial/bin/beagleBoneBlackSerial.out

beagleBoneBlackTcp:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../core/examples/Beagle_Bone_Black_Tcp/src/Beagle_Bone_Black_Tcp.cpp $(CORE_BBB) $(LISTENER_TCP) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../core/examples/Beagle_Bone_Black_Tcp/bin/beagleBoneBlackTcp.out
	
beagleBoneBlackConfigurable:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../core/examples/Beagle_Bone_Black_Configurable/src/Beagle_Bone_Black_Configurable.cpp $(CORE_BBB) $(LISTENER_CONFIG) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/Beagle_Bone_Black_Configurable/bin/beagleBoneBlackConfigurable.out
	
raspberryPi2BSerial:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../core/examples/RaspberryPi_2_B_Serial/src/RaspberryPi_2_B_Serial.cpp $(CORE_RPI2) $(LISTENER_SERIAL) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/RaspberryPi_2_B_Serial/bin/raspberryPi2BSerial.out
	
raspberryPi2BTcp:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../core/examples/RaspberryPi_2_B_Tcp/src/RaspberryPi_2_B_Tcp.cpp $(CORE_RPI2) $(LISTENER_TCP) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/RaspberryPi_2_B_Tcp/bin/raspberryPi2BTcp.out

raspberryPi2BConfigurable:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../core/examples/RaspberryPi_2_B_Configurable/src/RaspberryPi_2_B_Configurable.cpp $(CORE_RPI2) $(LISTENER_CONFIG) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../core/examples/RaspberryPi_2_B_Configurable/bin/raspberryPi2BConfigurable.out
	
#----------------------- Tests -----------------------
tests: dio-test i2c-test spi-test

dio-test:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../tests/src/dio-test.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/diotest.out

rpi2DioTest:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../tests/src/rpi2/rpi2DioTest.cpp $(CORE_RPI2) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/rpi2/dioTest.out
	
rpi2UartTest:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../tests/src/rpi2/rpi2UartTest.cpp $(CORE_RPI2) -lrt -DLINXCONFIG -DDEBUG_ENABLED=-1 -o ../tests/bin/rpi2/uartTest.out
	
rpi2SpiTest:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) -g $(INC) ../tests/src/rpi2/rpi2SpiTest.cpp $(CORE_RPI2) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/rpi2/spiTest.out
	
i2c-test:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../tests/src/i2c-test.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/i2ctest.out

pwm-test:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../tests/src/pwm-test.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/pwmtest.out
	
spi-test:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../tests/src/spi-test.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/spitest.out

uart-test:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../tests/src/uart-test.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../tests/bin/uarttest.out
	
#----------------------- Utils -----------------------
utils: blink analogRead uartLoopback

analogRead:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../utils/src/analogRead.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/analogRead.out

blink:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../utils/src/blink.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/blink.out

pwmSetDutyCycle:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../utils/src/pwmSetDutyCycle.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/pwmSetDutyCycle.out
	
uartLoopback:
	$(CXX) $(LDFLAGS) $(CPPFLAGS) $(CFLAGS) $(INC) ../utils/src/uartLoopback.cpp $(CORE_BBB) -lrt -DLINXCONFIG -DDEBUG_ENABLED=0 -o ../utils/bin/uartLoopback.out
	
#----------------------- Ardunio ---------------------
ARDCLI = ARDUINO_SKETCHBOOK_DIR=.. arduino-cli
ARDDEPS = ../libraries/Servo/library.properties
ARDTARGETS = Uno Leonardo Mega2560 Nano328 ProMicro
ARDBOARDS = uno leonardo megaADK nano micro

restoflist = $(wordlist 2,$(words $1),$1)
mappair = $(and $(strip $2),$(strip $3),$(eval $(call \
    $1,$(firstword $2),$(firstword $3))$(call \
    mappair,$1,$(call restoflist,$2),$(call restoflist,$3))))

../libraries/Servo/library.properties: 
	$(ARDCLI) lib install Servo

define ArduinoAVRTargetRule
../../Builds/Arduino/$1/Arduino_$1_Serial.ino.hex: $(ARDDEPS) ../libraries/LINX/examples/Arduino_$1_Serial/Arduino_$1_Serial.ino
	$(ARDCLI) compile -b arduino:avr:$2 ../libraries/LINX/examples/Arduino_$1_Serial/Arduino_$1_Serial.ino -o $$@
	@rm -f $$(basename $$@).elf
endef

$(call mappair,ArduinoAVRTargetRule,$(ARDTARGETS),$(ARDBOARDS))

.PHONY:	ardclipre arduino-avr

ardclipre:
	@$(ARDCLI) version >/dev/null 2>&1 || (echo && echo "Missing arduino-cli utility!" && echo "Please install arduino-cli from https://github.com/arduino/arduino-cli" && echo " and add it to your path." && echo && false)
	@$(ARDCLI) core update-index
	@$(ARDCLI) core install arduino:avr

arduino-avr:	ardclipre $(foreach T, $(ARDTARGETS), ../../Builds/Arduino/$T/Arduino_$T_Serial.ino.hex)

#----------------------- Clean -----------------------
clean:

cleanUtils: 
	rm -rf ../utils/bin/*

cleanall:
	rm -rf *.o
	rm -rf *.out
	rm -rf ../bin/*

