#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

typedef struct {
  const char **items;
  size_t count;
  size_t capacity;
} Files;

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  Cmd cmd = {0};
  Files files = {0};
  da_append(&files, "lexer");
  da_append(&files, "parser");
  da_append(&files, "eval");
  da_append(&files, "main");
  for (size_t i = 0; i < files.count; i++) {
    cmd_append(&cmd, "gcc", "-Wall", "-Wpedantic", "-Wextra", "-c", "-g", "-o",
               temp_sprintf("%s.o", files.items[i]),
               temp_sprintf("%s.c", files.items[i]), "-lm");
    if (!cmd_run_sync_and_reset(&cmd))
      goto fail;
  }
  cmd_append(&cmd, "gcc", "-o", "main");
  for (size_t i = 0; i < files.count; i++) {
    cmd_append(&cmd, temp_sprintf("%s.o", files.items[i]));
  }
  if (!cmd_run_sync_and_reset(&cmd))
    goto fail;
  nob_log(INFO, "Build Successful!");
  da_free(files);
  return 0;
fail:
  da_free(files);
  return 1;
}
