#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

// #define DEBUG
#define MAX_BUF_LEN 100

typedef struct {
  const char **items;
  size_t count;
  size_t capacity;
} Files;

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);
  char subcmd[MAX_BUF_LEN];
  Cmd cmd = {0};
  Files files = {0};
  da_append(&files, "lexer");
  da_append(&files, "parser");
  da_append(&files, "eval");
  da_append(&files, "main");

  if (argc > 1) {
    if (!strcmp(argv[1], "clean")) {
      for (size_t i = 0; i < files.count; i++) {
        int res = file_exists(temp_sprintf("%s.o", files.items[i]));
        if (res == 1) {
          cmd_append(&cmd, "rm", temp_sprintf("%s.o", files.items[i]));
          if (!cmd_run_sync_and_reset(&cmd))
            goto fail;
        } else if (res == 0) {
          continue;
        } else if (res == -1) {
          goto fail;
        } else {
          fprintf(stderr, "%s:%d: UNREACHABLE: %s\n",
                  "/home/samuelhernandes/Dokumente/test/calc/nob.c", 37,
                  "Unreachable!");
          goto fail;
        }
      }
      goto success;
    }
  }

  for (size_t i = 0; i < files.count; i++) {
    cmd_append(&cmd, "gcc", "-Wall", "-Wpedantic", "-Wextra", "-c", "-g", "-o",
               temp_sprintf("%s.o", files.items[i]),
               temp_sprintf("%s.c", files.items[i]));
#ifdef DEBUG
    cmd_append(&cmd, "-DDEBUG");
#endif
    if (!cmd_run_sync_and_reset(&cmd))
      goto fail;
  }
  cmd_append(&cmd, "gcc", "-o", "main", "-lm");
  for (size_t i = 0; i < files.count; i++) {
    cmd_append(&cmd, temp_sprintf("%s.o", files.items[i]));
  }
  if (!cmd_run_sync_and_reset(&cmd))
    goto fail;
  nob_log(INFO, "Build Successful!");
success:
  da_free(files);
  return 0;
fail:
  da_free(files);
  return 1;
}
