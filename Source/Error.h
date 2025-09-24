#ifndef ERROR_H
#define ERROR_H

#include <SF.h>

typedef struct ErrorMessageNode {
  SFQueue queue;
  I32 line;
  SFString8 file;
  SFString8 function;
  SFString8 message;
} ErrorNode;

void addErrorToQueueImplementation(SFArena *arena, SFQueue *queue, char const *msg,
                     char const *f, char const *fn, int l);

#define addErrorToQueue(a, q, msg)\
    addErrorToQueueImplementation(a, q, msg, __FILE__, __FUNCTION__, __LINE__)


#define addErrorToQueueAndGoto(a, q, msg, label)                               \
  do {                                                                         \
    addErrorToQueueImplementation(a, q, msg, __FILE__, __FUNCTION__, __LINE__);              \
    goto label;                                                                \
  } while (0)

#endif
