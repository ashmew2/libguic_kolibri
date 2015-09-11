#ifndef KOLIBRI_CHECKBOX_H
#define KOLIBRI_CHECKBOX_H

struc check_box2 left_s, top_s, ch_text_margin, color, border_color, text_color, text, flags {
; structure parameters for the check box. 
left_s: dd left_s; +0 position x dw + size dw. 
top_s: dd top_s; +4 position at dw + size dw. 
ch_text_margin: dd ch_text_margin; the distance from the rectangle check box to the label. 
color: dd color; color inside the checkbox. 
border_color: dd border_color; frame color. 
text_color: dd text_color; 
color labels must be in color format or 0x80000000, ASCIIZ it to display strings. 
.text: dd text; address in the code where the text. 
flags: dd flags + 0; 
flags. size_of_str: dd 0x0; string length is computed with the component initialization
} 

struct check_box {
    unsigned int left_s;
    unsigned int top_s;
    unsigned int ch_text_margin;
    unsigned int color;
    unsigned int border_color;
    unsigned int text_color;
    char *text;
    unsigned int flags;

    /* Users can use members above this */
    unsigned int size_of_str;
};

struct check_box* kolibri_new_check_box(unsigned int tlx, unsigned int tly, unsigned int sizex, unsigned int sizey, char *label_text)
{
     struct check_box* new_checkbox = (struct check_box *)malloc(sizeof(struct check_box));
     new_checkbox -> left_s = (tlx << 16) + sizex;
     new_checkbox -> top_s  = (tly << 16) + sizey;
     new_checkbox -> ch_text_margin = 10;
     new_checkbox -> color = 0xFFFFFFFF;
     new_checkbox -> border_color = 0x00000000;
     new_checkbox -> text_color = 0x00000000;
     new_checkbox -> label_text;
          
     return new_checkbox;
}

extern void check_box_draw2(struct check_box *);
extern void check_box_mouse2(struct check_box *);

#endif /* KOLIBRI_CHECKBOX_H */
