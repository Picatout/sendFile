#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <time.h>
#include <string.h>

static void usage(){
	puts("Command line tool to send forth file to stm8_eForth MCU");
	puts("USAGE: SendFile -s device [-d msec] file_name");
	puts("  -s device serial port to use.");
	puts("  -d msec  delay in msec between text lines. Default to 50.");
	puts("   file_name   file to send.");
	puts("Port config  115200 8N1 no flow control."); 
	exit(0);
}

static unsigned int msec=100;
static unsigned int baud=115200;
static char* file_name;
static char* serial_port=NULL; 

static int fd; // serial port handle
static struct termios serial_port_old_settings; 

static int serial_getchar(int fd){
    int bc=0,c=0;
    while (!bc) bc=read(fd,&c,1);
    return c;
}


static void serial_putchar(int fd, int c){
    write(fd,&c,1);
    putchar(serial_getchar(fd)); //echo
}

static void serial_writeln(int fd,const char* buff){
    while (*buff) serial_putchar(fd,*buff++);
   // tcdrain(fd);
}

static void delay(unsigned int ms) 
{ 
    double start_time = (double)(clock()+(clock_t)(ms*1000)); 
    while ((double)clock() < start_time); 
}
 
// Send Forth source file to MCU
static void send_file(){
#define LINE_SIZE (128)
    FILE* fh;
    char line[LINE_SIZE];
    int lncnt=0; 

    fh=fopen(file_name,"r");
    if (!fh){
        printf("%s is not a file.\n",file_name);
    }else{
        // sending file. 1 line at a time.
        printf("Sending file %s\n",file_name);
        while (!feof(fh)){
            line[0]=0;
            fgets(line,LINE_SIZE,fh);
            serial_writeln(fd,line);
			delay(msec);
			lncnt++;
        };
        fclose(fh);
        printf("%d lines sent\n",lncnt);
    }
}

int main(int argc, char**argv){
	
	int i=1;
	if (argc<4) usage();
	while(i<argc){
	  if (argv[i][0]=='-'){
		switch(argv[i][1]){
		case 's':
			i++;
			serial_port=argv[i];
		break;
		case 'd':
			i++;
			msec=(clock_t)atoi(argv[i]);
		break;
		default:
		usage();
		}
	  }else{
		file_name=argv[i];
	  }
	  i++;
	}
	printf("port=%s, baud=%d,delay=%d file=%s\n",serial_port,baud,(int)msec,file_name);
    fd = open(serial_port,O_RDWR | O_NOCTTY | O_NDELAY); 
                            /* O_RDWR   - Read/Write access to serial port       */
                            /* O_NOCTTY - No terminal will control the process   */
                            /* O_NDELAY, No blocking I/O              */

    if(fd == -1){					/* Error Checking */
        printf("\n  Error! in Opening %s\n\n",serial_port);
        _exit(-1);
    }
    if(!isatty(fd)){
        puts("Not a serial port.");
        _exit(-1);
    }
/*---------- Setting the Attributes of the serial port using termios structure --------- */
		
    struct termios SerialPortSettings;	/* Create the structure                          */

    tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */
    memcpy(&serial_port_old_settings,&SerialPortSettings,sizeof(struct termios)); 

    /* Setting the Baud rate */
    cfsetispeed(&SerialPortSettings,B115200); /* Set Read  Speed as 115200                       */
    cfsetospeed(&SerialPortSettings,B115200); /* Set Write Speed as 115200                       */


//
// Input flags - Turn off input processing
//
// convert break to null byte, no CR to NL translation,
// no NL to CR translation, don't mark parity errors or breaks
// no input parity check, don't strip high bit off,
// no XON/XOFF software flow control
//
    SerialPortSettings.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

//
// Output flags - Turn off output processing
//
// no CR to NL translation, no NL to CR-NL translation,
// no NL to CR translation, no column 0 CR suppression,
// no Ctrl-D suppression, no fill characters, no case mapping,
// no local output processing
//
SerialPortSettings.c_oflag = 0;
// SerialPortSettings.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);

//
// No line processing
//
// echo off, echo newline off, canonical mode off, 
// extended input processing off, signal chars off
//
SerialPortSettings.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

//
// Turn off character processing
//
// clear current char size mask, no parity checking,
// no output processing, force 8 bit input
//
SerialPortSettings.c_cflag &= ~(CSIZE | PARENB);
SerialPortSettings.c_cflag |= CS8;

//
// One input byte is enough to return from read()
// Inter-character timer off
//
SerialPortSettings.c_cc[VMIN]  = 1;
SerialPortSettings.c_cc[VTIME] = 0;


    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
        printf("\n  ERROR ! in Setting attributes");
    else{
	    tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer  */
		send_file();
		tcsetattr(fd,TCSANOW,&serial_port_old_settings);
    }
	close(fd);
}

