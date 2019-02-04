# StarWeather - Simulator 

This is the app to be used with the StarWeather - Desktop app
It will simulate values and send them to the serial port created for this purpose

Developed with qt

Commands to use to create the serial connection (Unix):

```bash
# command 1 (WeatherStation build folder) : 
socat PTY,link=./virtual-tty,raw,echo=0 -

#command 2 (WeatherSimulator build folder) : 
socat PTY,link=./arduino-sim,raw,echo=0 PTY,link=../../../WeatherStation/build/Debug/virtual-tty,raw,echo=0

# command 3 (WeatherSimulator build folder): 
./WeatherSimulator

# command 4 (WeatherStation build folder): 
./WeatherStation

```

