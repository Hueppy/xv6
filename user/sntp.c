#include "kernel/types.h"
#include "kernel/net.h"
#include "kernel/stat.h"
#include "user/user.h"

#define GT +2

#define IP(A, B, C, D) ((A << 24) | (B << 16) | (C << 8) | D)

#define DSTPORT 123
#define SRCPORT 2000

// see https://datatracker.ietf.org/doc/html/rfc1361
char *month[] = {"Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dez"};
int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void
printDate(uint32 sec)
{
    uint32 min = sec / 60;
    uint32 h   = min / 60;
    uint32 d   = h   / 24;
    uint32 y   = 1900;
    int i = 0;

    uint32 comp_val = ((y+1) % 4 != 0) ? 365 : 366;
    while (d >= comp_val) {
        d -= comp_val;
        y++;
        comp_val = ((y+1) % 4 != 0) ? 365 : 366;
    }
    d++; // d=0 -> 1 Jan
    while (d > daysOfMonth[i] || (i == 1 && y % 4 == 0 && d > daysOfMonth[i])) {
        d -= daysOfMonth[i];
        if (i == 1 && y % 4 == 0)
            d--;
        i++;
    }

    printf("Date: %l %s. %l, %l:%l:%l\n", d, month[i], y, (h%24) + GT, min%60, sec%60);
}


void
sntp(void) 
{
  uint32 dst;
  int fd;
  struct sntp sntp;

  dst = IP(192, 53, 103, 108);

  if((fd = connect(dst, SRCPORT, DSTPORT)) < 0){
    fprintf(2, "sntp: connect() failed\n");
    exit(1);
  }

  sntp.header = SNTP_HEADER(SNTP_LI_NO_WARNING, SNTP_VN_4, SNTP_MODE_CLIENT);
  sntp.stratum = SNTP_STRATUM_KISSODEATH;
  sntp.poll = 0;
  sntp.precision = 0;
  sntp.root_delay = 0;
  sntp.root_dispersion = 0;
  sntp.ref_id = 0;
  sntp.ref_timestamp = 0;
  sntp.og_timestamp = 0;
  sntp.rx_timestamp = 0;
  sntp.tx_timestamp = 0;

  if(write(fd, &sntp, sizeof(sntp)) < 0){
    fprintf(2, "sntp: write() failed\n");
    exit(1);
  }
  
  if(read(fd, &sntp, sizeof(sntp)) < sizeof(sntp)){
    fprintf(2, "sntp: read() failed\n");
    exit(1);
  }

  if(sntp.og_timestamp != 0) {
    /* originate timestamp should match our sent tx_timestamp, which was 0 */
    fprintf(2, "sntp: response originate timestamp did not match\n");
    exit(1);
  }

  if(sntp.stratum == 0
     || sntp.tx_timestamp == 0
     || SNTP_MODE(sntp.header) != SNTP_MODE_SERVER){
    fprintf(2, "sntp: invalid response\n");
    exit(1);
  }

  printDate(htonl(sntp.ref_timestamp));

  close(fd);  
}

int
main(int argc, char *argv[])
{
  if(argc < 1){
    fprintf(2, "Usage: sntp");
    exit(1);
  }
  sntp();
  exit(0);
}
