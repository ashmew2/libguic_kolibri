#ifndef KOLIBRI_DEBUG_H
#define KOLIBRI_DEBUG_H

inline void debug_board_write_byte(const char ch){
    __asm__ __volatile__(
    "int $0x40"
    :
    :"a"(63), "b"(1), "c"(ch));
}

inline void debug_board_write_str(const char* str){
  while(*str)
    board_write_byte(*str++);
}


#endif /* KOLIBRI_DEBUG_H */
