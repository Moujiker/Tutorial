#include<iomanip>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <limits.h>
#include <asm/ioctls.h>
#include <time.h>
#include <pthread.h>


#define VALUE "/sys/class/gpio/gpio111/value"
#define ERR_MSG(val, str)  \
if (val < 0) {	\
    perror(str);	\
    return -1;	\
}

//#define DEBUG_INFO		1

#ifdef DEBUG_INFO
#define DEBUG(n) 		printf("%d\n", n);
#else
#define DEBUG(n)
#endif

class SerCom
{
public:
    SerCom();
    ~SerCom();

    int open();
    bool write(char info_buf[]);
    bool read(char info_buf[]);

protected:
private:
    int m_iFd
};

int SerCom::open()
{
    struct termios opt;

    m_iFd = open("/dev/ttyO1", O_RDWR | O_NOCTTY);
    if (m_iFd < 0) 
    {
        trace_info("failed to open sercom");
        return -1;
    }


    tcgetattr(m_iFd, &opt);

    cfsetispeed(&opt, B115200);
    cfsetospeed(&opt, B115200);

    /*
    * raw mode
    */
    opt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    opt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    opt.c_oflag &= ~(OPOST);
    opt.c_cflag &= ~(CSIZE | PARENB);
    opt.c_cflag |= CS8;

    opt.c_cflag |= INPCK;
    opt.c_cflag |= PARENB;
    opt.c_cflag &= ~PARODD;
    /*
    * 'DATA_LEN' bytes can be read by serial
    */
    opt.c_cc[VMIN] = 1;
    opt.c_cc[VTIME] = 0;



    if (tcsetattr(m_iFd, TCSANOW, &opt) < 0)
    {
        trace_info("failed to set attr");
        return   -1;
    }

    return m_iFd;
}


int SerCom::write(std::string msg)
{
    struct timeval tv;

    char SendMsg[];
    //msg to  SendMsg
    int ret, i, fd_value;

    fd_set rset;

    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        sum = 0;

        // memset(cmd_buf, 0, sizeof(cmd_buf));
        memset(SendMsg, 0, sizeof(SendMsg));

        ret = tcflush(fd, TCIOFLUSH);
        ERR_MSG(ret, "tcflush fail: ");

        FD_ZERO(&rset);
        FD_SET(fd, &rset);

        ret = write(fd, SendMsg, len);
        ERR_MSG(ret, "Write first cmd fail: ");
        printf("write %d\n", ret);
        system("echo 1 > /sys/class/gpio/gpio111/value");
        usleep(1000 * 1000);
    }

    return 0;
}



int SerCom::read(int fd, char info_buf[])
{
    int info_len = 256;
    struct timeval tv;
    int ret, i, fd_value;
    int sum;

    fd_set rset;

    {
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        sum = 0;

        //memset(cmd_buf, 0, sizeof(cmd_buf));
        memset(info_buf, 0, sizeof(info_buf));

        ret = tcflush(fd, TCIOFLUSH);
        ERR_MSG(ret, "tcflush fail: ");

        FD_ZERO(&rset);
        FD_SET(fd, &rset);

        //ret = write(fd, tmp, strlen(tmp));
        ERR_MSG(ret, "Write first cmd fail: ");
        printf("write %d\n", ret);
        //system("echo 1 > /sys/class/gpio/gpio111/value");
        //usleep(1000 * 1000);

        ret = select(fd + 1, &rset, NULL, NULL, NULL);
        if (ret == 0) {
            printf("Select time out.\n");
            return 0;
            //continue;
        }

        ret = read(fd, info_buf, info_len);
        ERR_MSG(ret, "Read info fail: ");


        system("echo 0 > /sys/class/gpio/gpio111/value");

        std::string readComInfo;
        stringstream ss;

        for (int i = 0; i < info_len; i++)
        {
            ss << setfill('0') << setw(2) << hex << (unsigned int)(unsigned char)info_buf[i];
        }

        readComInfo.append(ss.str());
        trace_info("receive com info: %s", readComInfo.c_str());

        usleep(1000 * 1000);

    }

    return 0;
}