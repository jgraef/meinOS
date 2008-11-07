#ifndef _VTERM_H_
#define _VTERM_H_

#include <llist.h>

// Display

typedef struct {
  unsigned int x,y,w,h;
  unsigned int bpx; // Bits per Pixel/Character
  enum {
    GMODE_TEXT,
    GMODE_GRAPHIC
  } type;
} gmode_t;

typedef struct {
  pid_t pid;
  int did;
  void *buffer;
  gmode_t mode;
} display_t;

llist_t displays;

// VTerm
typedef struct {
  int vid;
  int shortcut;
} vterm_t;

llist_t vterms;
vterm_t *current_vterm;

#endif
