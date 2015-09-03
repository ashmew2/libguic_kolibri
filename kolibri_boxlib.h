#ifndef KOLIBRI_BOXLIB_H
#define KOLIBRI_BOXLIB_H

struct edit_box {
  unsigned int width, 
    unsigned int left,
    unsigned int top, 
    unsigned int color, 
    unsigned int shift_color, 
    unsigned int focus_border_color, 
    unsigned int blur_border_color,
    unsigned int text_color,
    unsigned int max,
    char *text,
    unsigned int mouse_variable,
    unsigned int flags,
    unsigned int size,
    unsigned int pos,
    unsigned int offset,
    unsigned int cl_curs_x,
    unsigned int cl_curs_y,
    unsigned int shift,
    unsigned int shift_old;
};

/* Initializes an Editbox with sane settings, sufficient for most use. 
   This will let you create a box and position it somewhere on the screen. 
   The text_buffer is a pointer to a character array and needs to be as long as you need.
   If the text box stores more data than the size of the text_buffer, it'll most probably crash
*/

void init_editbox(struct edit_box *textbox, unsigned int width, unsigned int left, unsigned int top, char *text_buffer)
{
  textbox -> width = width; 
    textbox -> left = left;
    textbox -> top = top; 
    textbox -> color = 0xFFFFFF; 
    textbox -> shift_color = 0x6a9480; 
    textbox -> focus_border_color = 0; 
    textbox -> blur_border_color = 0xAAAAAA;
    textbox -> text_color = 0;
    textbox -> max = 511;
    textbox -> text = text_buffer;
    textbox -> mouse_variable = 1;
    textbox -> flags = 0x00008002; /*ed_focus + ed_always_focus */
    textbox -> size = 0;
    textbox -> pos = 0;
    textbox -> offset = 0;
    textbox -> cl_curs_x = 0;
    textbox -> cl_curs_y = 0;
    textbox -> shift = 0;
    textbox -> shift_old = 0;
}

int kolibri_boxlib_init(void)
{
  int asm_init_status = init_boxlib_asm();

  if(asm_init_status == 0)
    return 0;
  else
    return 1;
}

extern void (*edit_box_draw)(struct edit_box *) __attribute__((__stdcall__));
extern void (*edit_box_key)(struct edit_box *) __attribute__((__stdcall__));
extern void (*edit_box_mouse)(struct edit_box *) __attribute__((__stdcall__));

#endif /* KOLIBRI_BOXLIB_H */
