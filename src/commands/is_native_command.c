#include "../commands.h"

void is_native_command(void)
{
  if (syscall(325)) {
    printf("Running natively.\n");
  } else {
    printf("Running on m2s.\n");
  }
}

