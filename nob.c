#include <stdio.h>
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
  int force = 0;

  Cmd cmd = {0};

  Files in_paths = {0};
  Files files = {0};
  da_append(&files, "lexer");
  da_append(&files, "parser");
  da_append(&files, "eval");
  da_append(&files, "main");

  if (argc > 1) {
    if (!strcmp(argv[1], "clean")) {
      for (size_t i = 0; i < files.count; i++) {
        if (!delete_file(temp_sprintf("./build/%s.o", files.items[i]))) {
          goto fail;
        }
      }
      if (!delete_file("./build/calc")) {
        goto fail;
      }
      if (!delete_file("./build")) {
        goto fail;
      }
      goto success;
    } else if (!strcmp(argv[1], "-f") || !strcmp(argv[1], "--force")) {
      force = 1;
    }
  }

  mkdir_if_not_exists("./build");
  char out_path[MAX_BUF_LEN];
  for (size_t i = 0; i < files.count; i++) {
    sprintf(out_path, "./build/%s.o", files.items[i]);
    // source files, then header next.
    da_append(&in_paths, temp_sprintf("%s.c", files.items[i]));
    if (strcmp(files.items[i], "main"))
      da_append(&in_paths, temp_sprintf("%s.h", files.items[i]));
    if (needs_rebuild(out_path, in_paths.items, in_paths.count) || force) {
      cmd_append(&cmd, "gcc", "-Wall", "-Wpedantic", "-Wextra", "-c", "-g",
                 "-o", out_path, in_paths.items[0]);
#ifdef DEBUG
      cmd_append(&cmd, "-DDEBUG");
#endif
      if (!cmd_run_sync_and_reset(&cmd))
        goto fail;
    }
    in_paths.count = 0;
  }
  sprintf(out_path, "./build/calc");
  for (size_t i = 0; i < files.count; i++) {
    da_append(&in_paths, temp_sprintf("./build/%s.o", files.items[i]));
  }
  if (needs_rebuild(out_path, in_paths.items, files.count) || force) {
    cmd_append(&cmd, "gcc", "-o", out_path, "-lm", "-lreadline");
    for (size_t i = 0; i < files.count; i++) {
      cmd_append(&cmd, temp_sprintf("./build/%s.o", files.items[i]));
    }
    if (!cmd_run_sync_and_reset(&cmd))
      goto fail;
    nob_log(INFO, "Build Successful!");
  } else {
    nob_log(INFO, "Nothing to do!");
  }
success:
  da_free(in_paths);
  da_free(files);
  return 0;
fail:
  da_free(in_paths);
  da_free(files);
  return 1;
}
