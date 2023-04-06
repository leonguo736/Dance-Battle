#include "regs.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int fd;

bool regs_init(void** virtual_base) {
  // map the address space for the LED registers into user space so we can
  // interact with them. we'll actually map in the entire CSR span of the HPS
  // since we want to access various registers within that span
  if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
    printf("ERROR: could not open \"/dev/mem\"...\n");
    return (1);
  }

  *virtual_base = mmap(NULL, HW_REGS_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED,
                       fd, HW_REGS_BASE);

  if (*virtual_base == MAP_FAILED) {
    printf("ERROR: mmap() failed...\n");
    close(fd);
    return (1);
  }

  printf("----- VIRTUAL BASE -----\n");
  printf("%p\n", *virtual_base);

  return 0;
}

bool regs_close(void* virtual_base) {
  if (munmap(virtual_base, HW_REGS_SPAN) != 0) {
    printf("ERROR: munmap() failed...\n");
    close(fd);
    return (1);
  }

  close(fd);
  return 0;
}