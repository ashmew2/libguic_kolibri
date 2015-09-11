#ifndef KOLIBRI_GUI_H
#define KOLIBRI_GUI_H

#include <kos32sys.h>
#include "kolibri_debug.h"
#include "stdlib.h" /* for malloc() */

/* boxlib loader */
#include "kolibri_boxlib.h"

/* GUI Elements being used */
#include "kolibri_editbox.h"
#include "kolibri_checkbox.h"

/* KOLIBRI_GUI_ELEMENT_TYPE contains all available GUI items from box_lib */
/* More elements can be added from other libraries as required */

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
  KOLIBRI_PROGRESS_BAR,

  /* Add elements above this element in order to let KOLIBRI_NUM_GUI_ELEMENTS */
  /* stay at correct value */
  
  KOLIBRI_NUM_GUI_ELEMENTS 
};

/* Generic structure for supporting functions on various elements of Kolibri's GUI */
struct kolibri_element_operations {
  void (*redraw_fn)(void *);
  void (*mouse_fn)(void *);
  void (*key_fn)(void *);
};

/* Define operations for various GUI elements in one place */
struct kolibri_element_operations kolibri_gui_op_table[KOLIBRI_NUM_GUI_ELEMENTS];

void kolibri_init_gui_op_table(void)
{
/* Setting up functions for edit box GUI element */
kolibri_gui_op_table[KOLIBRI_EDIT_BOX].redraw_fn = edit_box_draw;
kolibri_gui_op_table[KOLIBRI_EDIT_BOX].mouse_fn = edit_box_mouse;
kolibri_gui_op_table[KOLIBRI_EDIT_BOX].key_fn = editbox_key;

kolibri_gui_op_table[KOLIBRI_CHECK_BOX].redraw_fn = check_box_draw2;
kolibri_gui_op_table[KOLIBRI_CHECK_BOX].mouse_fn = check_box_mouse2;
kolibri_gui_op_table[KOLIBRI_CHECK_BOX].key_fn = NULL;
}

/* This is a doubly linked list of all elements that need to be drawn in order */
struct kolibri_window_element {
  enum KOLIBRI_GUI_ELEMENT_TYPE type;
  void *element;
  struct kolibri_window_element *next, *prev;
};

struct kolibri_window {
  unsigned int topleftx, toplefty;
  unsigned int sizex, sizey;
  char *window_title;

  /* Refer to sysfuncs, value to be stored in EDX (Function 0) */
  unsigned int XY;

  struct kolibri_window_element *elements;
};

void kolibri_handle_event_redraw(struct kolibri_window* some_window)
{
  /*  Draw windows with system color table. */
  /* Replace this with inline assembly? */

  //  init_boxlib_asm();

  BeginDraw();

  DrawWindow(some_window->topleftx, some_window->toplefty, 
	     some_window->sizex, some_window->sizey,
	     some_window->window_title,
	     kolibri_color_table.color_work_area, some_window->XY);
  
  /* Enumerate and draw all window elements here */
  if(some_window->elements) /* Draw all elements added to window */
    {
      struct kolibri_window_element* current_element = some_window -> elements; 
      
      do
	{
	  /* The redraw_fn serves as draw_fn on initial draw */
	  if(kolibri_gui_op_table[current_element -> type].redraw_fn)
	    kolibri_gui_op_table[current_element -> type].redraw_fn(current_element -> element);
	  
	  switch(current_element -> type)
	    {
	    case KOLIBRI_EDIT_BOX:
	    case KOLIBRI_CHECK_BOX:
	      __asm__ volatile("push $0x13371337"::); /* Random value pushed to balance stack */
						      /* otherwise edit_box_draw leaves stack unbalanced */
						      /* and GCC jumps like a crazy motha' fucka' */
	      break;
	    }

	  current_element = current_element -> next;
	  
	} while(current_element != some_window->elements); /* Have we covered all elements? */
    }
}

void kolibri_handle_event_key(struct kolibri_window* some_window)
{
  /* Enumerate and trigger key handling functions of window elements here */
  if(some_window->elements) 
    {
      struct kolibri_window_element *current_element = some_window -> elements; 

      do
	{
	  /* Only execute if the function pointer isn't NULL */
	  if(kolibri_gui_op_table[current_element -> type].key_fn)
	    kolibri_gui_op_table[current_element -> type].key_fn(current_element -> element);
	  
	  current_element = current_element -> next;
	} while(current_element != some_window->elements); /* Have we covered all elements? */
    }
}

void kolibri_handle_event_mouse(struct kolibri_window* some_window)
{
  /* Enumerate and trigger mouse handling functions of window elements here */
  if(some_window->elements) 
    {
      struct kolibri_window_element *current_element = some_window -> elements; 

      do
	{
	  
	  if(kolibri_gui_op_table[current_element -> type].mouse_fn)
	    kolibri_gui_op_table[current_element -> type].mouse_fn(current_element -> element);

	  current_element = current_element -> next;
	} while(current_element != some_window->elements); /* Have we covered all elements? */
    }
}

struct kolibri_window * kolibri_new_window(int tlx, int tly, int sizex, int sizey, char *title)
{
  struct kolibri_window *new_win = (struct kolibri_window *)malloc(sizeof(struct kolibri_window));

  new_win->topleftx = tlx;
  new_win->toplefty = tly;
  new_win->sizex = sizex;
  new_win->sizey = sizey;
  new_win->window_title = title;
  new_win->XY = 0x00000013; /* All windows are skinned windows with caption for now */
  new_win->elements = NULL;
  
  return new_win;
}

/* Add an element to an existing window */
void kolibri_window_add_element(struct kolibri_window *some_window, enum KOLIBRI_GUI_ELEMENT_TYPE element_type, void *some_gui_element)
{
  struct kolibri_window_element *new_element = (struct kolibri_window_element *)malloc(sizeof(struct kolibri_window_element));
  
  new_element -> type = element_type;
  new_element -> element = some_gui_element;

  if(!(some_window->elements)) /* No elements in window yet */
    {
      some_window->elements = new_element;
      some_window->elements -> prev = some_window->elements;
      some_window->elements -> next = some_window->elements;
    }
  else
    {
      struct kolibri_window_element *last_element = some_window -> elements -> prev;
  
      last_element -> next = new_element;
      new_element -> next = some_window -> elements; /* start of linked list  */
      some_window -> elements -> prev = new_element;
      new_element -> prev = last_element;
    }
}

enum KOLIBRI_GUI_EVENTS {
  KOLIBRI_EVENT_REDRAW = 1,   /* Window and window elements should be redrawn */
  KOLIBRI_EVENT_KEY = 2,      /* A key on the keyboard was pressed */
  KOLIBRI_EVENT_BUTTON = 3,   /* A button was clicked with the mouse */
  KOLIBRI_EVENT_MOUSE = 6     /* Mouse activity (movement, button press) was detected */
};

int kolibri_main_gui_loop(struct kolibri_window* main_window)
{
  /* Set gui_event to REDRAW so that window is drawn in first iteration  */
  unsigned int gui_event = KOLIBRI_EVENT_REDRAW;

  int boxlib_init_status = kolibri_boxlib_init();
  if(boxlib_init_status == 0) 
    board_write_str("GUI Successfully Initialized.\n");
  else 
    {
      board_write_str("GUI Failed to initialize.\n");
      return -1;
    }

  /* Initialize the global operation table which handles event functions of */
  /*   each individual element type */
  kolibri_init_gui_op_table();

  /* Get the current color table for Kolibri and store in global table*/
  kolibri_get_system_colors(&kolibri_color_table);

  /* Set up system events for buttons, mouse and keyboard and redraw */
  /* Also set filters so that window receives mouse events only when active
     and mouse inside window */
  __asm__ volatile("int $0x40"::"a"(40), "b"(0xC0000027)); 

  do                                 /* Start of main activity loop */
    {
      if(gui_event == KOLIBRI_EVENT_REDRAW)
	{
	  kolibri_handle_event_redraw(main_window);
	}
      else if(gui_event == KOLIBRI_EVENT_KEY)
	{
	  kolibri_handle_event_key(main_window);
	}
      else if(gui_event == KOLIBRI_EVENT_BUTTON)
	{
	  //kolibri_handle_event_button(main_window);
	}
      else if(gui_event == 6)
      	{
	  kolibri_handle_event_mouse(main_window);
	}

    } while(gui_event = get_os_event()); /* End of main activity loop */
  
  //kolibri_quit(); //Free alloc'd stuff.
}

/* Note: The current implementation tries to automatically colors 
   GUI elements with system theme */

#endif /* KOLIBRI_GUI_H */