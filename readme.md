A little command line tool to send a Forth source file to serial port with delay between each line.
To be used with [stm8_eforth](https://github.com/Picatout/stm8_eForth) or [stm32-eforth](https://github.com/Picatout/stm32-eforth). 

## USAGE

	 SendFile -s device [-d msec] file1 [file2 ... fileN]
	
	*  **-s device** serial port to use.
	*  **-d msec**  delay in msec between text lines. Default to 100.
	*  **file1 [file2 ... fileN]**   one or more files to send separate by space.
	* Port config  115200 8N1 no flow control. 
 

## exemple

	SendFile -s/dev/ttyS0 stm8105.f  ansi.f list.f 
	
