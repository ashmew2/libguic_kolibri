#include <kos32sys.h>
#include "kolibri_debug.h"
//#include "kolibri_boxlib.h"

// Need a way to get system colours and fix that theme.
// Also probably have a set_custom_theme struct.

//This is a type II skinned window (from sysfuncs.txt)
// Coloured with system theme.
struct kolibri_window {
  unsigned short int topleftx, toplefty;
  unsigned short int sizex, sizey;
 
  struct kolibri_window_elements *elements;
};

struct kolibri_system_colors {
  
};


int main()
{
  oskey_t valkey;
  //Setting val to 1 inititially is important to trigger initial draw event.
  //Then the do while loop will take care of itself.
  uint32_t val = 1;

  kolibri_get_system_colors();

  do
    {
      if(val == 1)
	{
	  BeginDraw();
	  DrawWindow(100, 100, 300, 120, "PushMessenger v0.1", 0x14FFFFFF, 0);
	  EndDraw();
	}
      else if(val == 2)
	{
	  valkey = get_key();
	}
      else if(val == 3)
	{
	  return -1;
	}
    } while(val = get_os_event());

  return 0;
}
