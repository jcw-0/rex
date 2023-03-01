#include <stdint.h>

typedef struct stack_t {
  uint8_t* bot;
  uint8_t* top;
  uint8_t* current;
  size_t databsz;
} stack_t;

stack_t* stack_create(int indices, size_t databsz) {
  stack_t* s = malloc(sizeof (stack_t) + indices * databsz);
  s->bot     = s + sizeof (stack_t);
  s->top     = s->bot + indices * databsz;
  s->current = s->bot;
  s->databsz = databsz;
  
  return s_p;
}

void stack_push(stack_t* s, void* data) {
  if (s->current == s->top) return;
  memcpy(s->current, data, s->databsz);
  s->current++;
}

uint8_t* stack_pop(stack_t* s) {
  if (s->current == s->bot) return;
  s->current--;
  return (s->current + 1)
}

void stack_free(stack_t* s) {
  free(s);
}
