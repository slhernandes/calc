#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define DEBUG 0
#define MAX_BUF_LEN 100
#define path_from_home(path) temp_sprintf("%s/" path, getenv("HOME"))

typedef struct {
  const char **items;
  size_t count;
  size_t capacity;
} Files;

int main(int argc, char **argv) {
  NOB_GO_REBUILD_URSELF(argc, argv);

  char *program = shift_args(&argc, &argv);
  int force = 0;

  Cmd cmd = {0};

  Files in_paths = {0};
  Files files = {0};
  da_append(&files, "lexer");
  da_append(&files, "parser");
  da_append(&files, "eval");
  da_append(&files, "main");

  if (argc > 0) {
    char *subcmd = shift_args(&argc, &argv);
    if (!strcmp(subcmd, "clean")) {
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
    } else if (!strcmp(subcmd, "-f") || !strcmp(subcmd, "--force")) {
      force = 1;
    } else if (!strcmp(subcmd, "install")) {
      // just in case mkdir_if_not_exists doesn't work
      char *home = getenv("HOME");
      int res = file_exists(path_from_home(".local/bin"));
      if (res == 0) {
        mkdir_if_not_exists("./build");
      } else if (res == -1) {
        goto fail;
      }
      nob_log(INFO, "~/.local/bin/calc exists? %d",
              file_exists(path_from_home(".local/bin/calc")));
      if (file_exists(path_from_home(".local/bin/calc")) == 1) {
        bool overwrite = false;
        printf("[WARNING] ~/.local/bin/calc will be overwritten. Are you sure? "
               "[y/N]: ");
        char c;
        scanf("%c", &c);
        if (c == 'y' || c == 'Y') {
          overwrite = true;
        }
        if (overwrite) {
          if (!copy_file("./build/calc", path_from_home(".local/bin/calc")))
            goto fail;
        } else {
          nob_log(INFO, "Cancelled installation");
        }
      } else {
        if (!copy_file("./build/calc", path_from_home(".local/bin/calc")))
          goto fail;
      }
      goto success;
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
    da_append(&in_paths, "nob.c");
    if (needs_rebuild(out_path, in_paths.items, in_paths.count) || force) {
      cmd_append(&cmd, "gcc", "-Wall", "-Wextra", "-Werror", "-c", "-o",
                 out_path, in_paths.items[0]);
#if DEBUG
      cmd_append(&cmd, "-DDEBUG", "-ggdb");
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
  da_append(&in_paths, "nob.c");
  if (needs_rebuild(out_path, in_paths.items, files.count) || force) {
    cmd_append(&cmd, "gcc", "-o", out_path);
    for (size_t i = 0; i < files.count; i++) {
      cmd_append(&cmd, temp_sprintf("./build/%s.o", files.items[i]));
    }
    cmd_append(&cmd, "-lm", "-lreadline");
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
