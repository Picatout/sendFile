A little command line tool to send a Forth source file to serial port with delay between each line.
To be used with stm8_eforth. 

## USAGE

	 SendFile -s device [-d msec] file_name
	
	*  **-s device** serial port to use.
	*  **-d msec**  delay in msec between text lines. Default to 50.
	*  **file_name**   file to send.
	* Port config  115200 8N1 no flow control. 
 

