#include "stdio.h"
#include "fel.h"

int main(int argc, char* argv[])
{
  if(argc != 2 ) {
    fprintf(stderr, "Missing file argument.\n");
    fprintf(stdout, "Usage: %s FILE\n", argv[0]);
    return -2;
  }
  fel_context* f = fel_new();
  if(f == 0) {
    fprintf(stderr, "Failed to start fel engine.\n");
    return -1;
  }
  fel_syntax syntax;
  fel_syntax_default(&syntax);
  fel_load_and_run_file(f, &syntax, argv[1]);
  fel_delete(f);
  return 0;
}

