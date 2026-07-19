#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_READLINE

#include <readline/readline.h>

char *read_line(const char *prompt) {
  return readline(prompt);
}

#else /* no readline */

char *read_line(const char *prompt) {
  fputs(prompt, stdout);
  fflush(stdout);

  char buf[1024];
  if (!fgets(buf, sizeof(buf), stdin)) {
    return NULL;
  }

  buf[strcspn(buf, "\n")] = '\0';
  return strdup(buf);
}

#endif
