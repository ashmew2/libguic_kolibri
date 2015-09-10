#include <kos32sys.h>
#include "kolibri_debug.h"
#include "stdlib.h"

/* BOXLIB loader */
#include "kolibri_boxlib.h"
/* GUI Elements being used */
#include "kolibri_editbox.h"

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
  KOLIBRI_PROGRESS_BAR,

  /* Add elements above this element in order to let KOLIBRI_NUM_GUI_ELEMENTS stay at correct value  */
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
kolibri_gui_op_table[KOLIBRI_EDIT_BOX].mouse_fn = edit_box_key;
kolibri_gui_op_table[KOLIBRI_EDIT_BOX].key_fn = edit_box_mouse;
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
	  //	  __asm__ volatile("int3"::);

	  kolibri_gui_op_table[current_element -> type].redraw_fn(current_element -> element);

	  switch(current_element -> type)
	    {
	    case KOLIBRI_EDIT_BOX:
	      __asm__ volatile("push $0x00112233"::); /* Random value pushed to balance stack */
						      /* otherwise edit_box_draw leaves stack unbalanced */
						      /* and GCC jumps like a crazy motha' fucka' */
	      break;
	    }

	  current_element = current_element -> next;
	} while(current_element != some_window->elements->prev); /* Have we covered all elements? */
    }

  EndDraw();
}

/* void kolibri_handle_event_redraw(struct kolibri_window* some_window) */
/* { */
/*   __asm__ volatile("int3"::); */
/*   kolibri_draw_window(some_window); */
/* } */

void kolibri_handle_event_key(struct kolibri_window* some_window)
{
  unsigned int keypress;
    __asm__ __volatile__(
    "int $0x40"
    :"=a"(keypress)
    :"a"(2));

    //  get_key();
    //  __asm__ volatile("int3"::);

  /* Enumerate and trigger key handling functions of window elements here */
  if(some_window->elements) 
    {
      struct kolibri_window_element *current_element = some_window -> elements; 

      do
	{
	  kolibri_gui_op_table[current_element -> type].key_fn(current_element -> element);
	  __asm__ volatile("push $0x00112233"::);

	  current_element = current_element -> next;
	} while(current_element != some_window->elements->prev); /* Have we covered all elements? */
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
	  //	  __asm__ volatile("int3"::);
	  kolibri_gui_op_table[current_element -> type].mouse_fn(current_element -> element);
	  __asm__ volatile("push $0x00112233"::);

	  current_element = current_element -> next;
	} while(current_element != some_window->elements->prev); /* Have we covered all elements? */
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


void kolibri_get_system_colors(struct kolibri_system_colors *color_table)
{
  __asm__ volatile ("int $0x40"
		    :
		    :"a"(48),"b"(3),"c"(color_table),"d"(40)
		    );

  /* color_table should point to the system color table */
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

/* struct kolibri_window_element *kolibri_new_window_element(enum KOLIBRI_GUI_ELEMENT_TYPE NEW_ELEMENT_TYPE) */
/* { */
/*   struct kolibri_window_element *new_element = (struct kolibri_window_element *)malloc(sizeof(struct kolibri_window_element)); */
  
/*   switch(NEW_ELEMENT_TYPE) */
/*     { */
/*     case KOLIBRI_EDIT_BOX: */
/*       new_element -> element = kolibri_new_edit_box */
/*       break; */

/*     case KOLIBRI_CHECK_BOX: */
/*       break; */
/*     case KOLIBRI_RADIO_BUTTON: */
/*       break; */
/*     case KOLIBRI_SCROLL_BAR: */
/*       break; */
/*     case KOLIBRI_DYNAMIC_BUTTON: */
/*       break; */
/*     case KOLIBRI_MENU_BAR: */
/*       break; */
/*     case KOLIBRI_FILE_BROWSER: */
/*       break; */
/*     case KOLIBRI_TREE_LIST: */
/*       break; */
/*     case KOLIBRI_PATH_SHOW: */
/*       break; */
/*     case KOLIBRI_TEXT_EDITOR: */
/*       break; */
/*     case KOLIBRI_FRAME: */
/*       break; */

/*     case KOLIBRI_PROGRESS_BAR: */
/*       break; */

/*     } */
/* } */

int main()
{
  
  uint32_t val = 1;
  
  /* init boxlib */
  int x = kolibri_boxlib_init();
  
  if(x==0) 
    board_write_str("INIT Successful");
  else 
    {
    board_write_str("FAIL init boxlib \n");
    return -1;
    }
  kolibri_init_gui_op_table();
  /* Get the current color table for Kolibri */
    kolibri_get_system_colors(&kolibri_color_table);
    /* Set up system events for buttons, mouse and keyboard and redraw */
    /* Also set filters so that window receives mouse events only when active
       and mouse inside window */
    /* __asm__ volatile("int $0x40"::"a"(40), "b"(0xC0000027)); */
    /* Using default events for now, but ,mouse events can be enabled by uncommenting */
    /* 				    previous line of inline asm */
    /* Should probably go into gui_init or something */

  struct kolibri_window *main_window = kolibri_new_window(0,0,1024,768,"New KolibriOS Window!");

  char main_box_buf[1000];
  struct edit_box *main_box = kolibri_new_edit_box(100,100,100,main_box_buf);

  kolibri_window_add_element(main_window, KOLIBRI_EDIT_BOX, main_box);
  
  do
    {
      if(val == 1)
	{
	  kolibri_handle_event_redraw(main_window);
	}
      else if(val == 2)
	{
	  unsigned int keypress;
	  __asm__ __volatile__(
			       "int $0x40"
			       :"=a"(keypress)
			       :"a"(2));
	  edit_box_key(main_box);
	}
      else if(val == 3) /* Buttons should be handled before mouse to not waste */
			/* cycles on mouse handling */
	{
	  //kolibri_handle_event_buttonpress(main_window);
	}
      /* else if(val == 6) */
      /* 	{ */
      /* 	  unsigned int mbtnbits; */
      /* 	  __asm__ volatile("int $0x40" */
      /* 			   :"=a"(mbtnbits) */
      /* 			   :"a"(37), "b"(2)); */
	  
      /* 	  if(mbtnbits & 0x00000001) //LEFT MOUSE IS PRESSED */
      /* 	    { */
      /* 	      unsigned int eax, mx, my; */
      /* 	      board_write_str("MouseLeft Pressed\n"); */

      /* 	      /\* Left Pressed, Now get coordinates *\/ */
      /* 	      __asm__ volatile("int $0x40" */
      /* 			       :"=a"(eax) */
      /* 			       :"a"(37), "b"(0)); */
	      
	      
      /* 	      mx = eax & 0xFFFF0000; */
      /* 	      mx = mx >> 16; */
      /* 	      my = eax & 0x0000FFFF; */
	      
      /* 	      kolibri_find_clicked_element(main_window); */
      /* 	    } */
      /* 	} */
    } while(val = get_os_event());

  //kolibri_quit(); //Free alloc'd stuff.
  return 0;
}
