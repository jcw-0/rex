#include <stdint.h>
#include <stdlib.h>

typedef struct {
  void* bot;
  void* top;
  void* current;
  size_t databsz;
} stack_t;

stack_t* stack_create(int indices, size_t databsz) {
  stack_t* s = malloc(sizeof (stack_t) + indices * databsz);
  s->bot     = s + 1;
  s->top     = ((uint8_t*)s->bot) + indices * databsz;
  s->current = s->bot;
  s->databsz = databsz;
  return s;
}

void stack_push(stack_t* s, void* data) {
  if (s->current == s->top) return;
  memcpy(s->current, data, s->databsz);
  ((uint8_t*)s->current) + s->databsz;
}

void* stack_pop(stack_t* s) {
  if (s->current == s->bot) return;
  ((char*)s->current) -= s->databsz;
  return (s->current + s->databsz)
}

void stack_free(stack_t* s) {
  free(s);
}
