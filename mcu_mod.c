#include "mcu_mod_conf.h"
#include "mcu_mod.h"

int log_init();

int mcu_mod_init()
{
  int status = 0;

#if USE_LOG == 1
  if(status == 0)
    status = log_init();
#endif

#if USE_DWT == 1
  if(status == 0)
    dwt_init();
#endif

  return status;
}



