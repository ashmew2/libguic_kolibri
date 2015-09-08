#include <kos32sys.h>
#include "kolibri_debug.h"
#include "stdlib.h"

/* #include "kolibri_boxlib.h" */

// Also probably have a set_custom_theme struct. <-- Not an immediate concern.
//Windows will be coloured with system theme.

struct kolibri_system_colors {
  unsigned int color_frame_area;
  unsigned int color_grab_bar;
  unsigned int color_grab_bar_button;
  unsigned int color_grab_button_text;
  unsigned int color_grab_text;
  unsigned int color_work_area;
  unsigned int color_work_button;
  unsigned int color_work_button_text;
  unsigned int color_work_text;
  unsigned int color_work_graph;
};

struct kolibri_system_colors kolibri_color_table;

/* KOLIBRI_GUI_ELEMENT_TYPE contains all available GUI items from box_lib */
enum KOLIBRI_GUI_ELEMENT_TYPE {
  KOLIBRI_EDIT_BOX,
  KOLIBRI_CHECK_BOX,
  KOLIBRI_RADIO_BUTTON,
  KOLIBRI_SCROLL_BAR,
  KOLIBRI_DYNAMIC_BUTTON,
  KOLIBRI_MENU_BAR,
  KOLIBRI_FILE_BROWSER,
  KOLIBRI_TREE_LIST,
  KOLIBRI_PATH_SHOW,
  KOLIBRI_TEXT_EDITOR,
  KOLIBRI_FRAME,
  KOLIBRI_PROGRESS_BAR
};

struct kolibri_window_elements {
  enum KOLIBRI_GUI_ELEMENT_TYPE type;
  void *element;
  struct kolibri_window_elements *next, *prev;
};

struct kolibri_window {
  unsigned int topleftx, toplefty;
  unsigned int sizex, sizey;
  char *window_title;

  /* Refer to sysfuncs, value to be stored in EDX (Function 0) */
  unsigned int XY;

  struct kolibri_window_elements *elements;
};

void kolibri_draw_window(struct kolibri_window* some_window)
{
  /*  Draw windows with system color table. */
  /* Replace this with inline assembly? */

  BeginDraw();

  DrawWindow(some_window->topleftx, some_window->toplefty, 
	     some_window->sizex, some_window->sizey,
	     some_window->window_title,
	     kolibri_color_table.color_work_area, some_window->XY);
  
  /* Enumerate and draw all window elements here */
  if(some_window->elements)
    {
      /* Draw all those elements */
    }
  
  EndDraw();
}

struct kolibri_window * kolibri_new_window(int tlx, int tly, int sizex, int sizey, char *title)
{
  struct kolibri_window *new_win = (struct kolibri_window *)malloc(sizeof(struct kolibri_window));

  new_win->topleftx = tlx;
  new_win->toplefty = tly;
  new_win->sizex = sizex;
  new_win->sizey = sizey;
  new_win->window_title = title;
  new_win->XY = 0x00000013;
  new_win->elements = NULL;
  
  return new_win;
}


void kolibri_get_system_colors(struct kolibri_system_colors *color_table)
{
  __asm__ volatile ("int $0x40"
		    :
		    :"a"(48),"b"(3),"c"(color_table),"d"(40)
		    );
  /* color_table should point to the system color table */
}

int main()
{
  oskey_t valkey;

  //Setting val to 1 inititially is important to trigger initial draw event.
  //Then the do while loop will take care of itself.
  uint32_t val = 1;
  
  /* Get the current color table for Kolibri */

  kolibri_get_system_colors(&kolibri_color_table);
  /* Should probably go into gui_init or something */

  struct kolibri_window *main_window = kolibri_new_window(0,0,1024,768,"New KolibriOS Window!");
  
  do
    {
      if(val == 1)
	{
	  //  BeginDraw();
	  //	  DrawWindow(0, 0, 1024, 768, "PushMessenger v0.1", kolibri_color_table.color_work_area, 0x00000013);
	  kolibri_draw_window(main_window);
	  //	  EndDraw();
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
