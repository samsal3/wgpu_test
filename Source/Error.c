#include "Error.h"

#include <SF.h>

static void defaultInitErrorNode(ErrorNode *node) {
  sfDefaultInitQueue(&node->queue);
  node->line = 0;
  sfDefaultInitString8(&node->file);
  sfDefaultInitString8(&node->function);
  sfDefaultInitString8(&node->message);
}

static Size getNullTerminatedStringSize(char const *s, Size maxSize) {
  Size i = 0;

  for (i = 0; i < maxSize; ++i)
    if ('\0' == s[i])
      return i;

  return maxSize;
}

void addErrorToQueueImplementation(SFArena *a, SFQueue *q, char const *msg, char const *f,
                     char const *fn, int l) {
  ErrorNode *node = sfAllocate(a, sizeof(ErrorNode));
  if (node) {
    defaultInitErrorNode(node);
    node->line = l;

    node->file.data = f;
    node->file.size = getNullTerminatedStringSize(f, 254);

    node->function.data = fn;
    node->function.size = getNullTerminatedStringSize(fn, 254);

    node->message.data = msg;
    node->message.size = getNullTerminatedStringSize(msg, 1024);

    sfInsertIntoQueueHead(&node->queue, q);
  }
}
