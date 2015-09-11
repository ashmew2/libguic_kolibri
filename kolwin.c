#include "kolibri_gui.h"

int main()
{
  struct kolibri_window *main_window = kolibri_new_window(0, 0, 1024, 500, "New KolibriOS Window!");
  struct edit_box *textbox = kolibri_new_edit_box(100, 100, 10);
  struct check_box *checkbox = kolibri_new_check_box(200, 200, 16, 12, "FREE SEX HERE!");
  
  kolibri_window_add_element(main_window, KOLIBRI_EDIT_BOX, textbox);
  kolibri_window_add_element(main_window, KOLIBRI_CHECK_BOX, checkbox);

  kolibri_main_gui_loop(main_window);

  return 0;
}
