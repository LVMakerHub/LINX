INC=-I ../core/device/utility

all: release debug main
	
#release:
#	g++ LinxTcpDaemon.cpp  listener/LinxTcpListener.cpp listener/linux/LinxTcpListenerLinux.cpp   device/raspberrypi/LinxRaspberryPi-B.cpp -lrt -o LinxRelease.out
	
pi:
	g++ $(INC) testapp.cpp ../core/device/config/LinxRaspberryPiConfig.h ../core/device/utility/LinxRaspberryPi.h ../core/device/utility/LinxRaspberryPi.cpp -DLINXCONFIG=1 -o ../bin/pi.out
	
testapp:
	g++ $(INC) testapp.cpp ../core/device/utility/LinxDevice.cpp ../core/device/utility/LinxRaspberryPi.cpp -lrt -DLINXCONFIG -o ../bin/testapp.out 
	
#serial: 
#	g++ LinxSerialDaemon.cpp listener/LinxListener.cpp listener/LinxSerialListener.cpp device/LinxDevice.cpp device/raspberrypi/LinxRaspberryPi.cpp device/raspberrypi/LinxRaspberryPi-B.cpp -lrt -DLINXDEVICE=LinxRaspberryPiB -DDEBUG_ENABLED=1 -o serialdebug.out

#uno:	
#	cd device/wiring/arduino/uno && make	

cleanall:
	rm -rf *.o
	rm -rf *.out
	rm -rf ../bin/*