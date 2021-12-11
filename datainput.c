#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
  if (argc <= 2 || argc > 3) {
    printf("execute like \"./a.out [the number of objects] [filename]\"\n");
    return EXIT_FAILURE;
  }
  int objnum = atoi(argv[1]);
  FILE* fp;
  fp = fopen(argv[2], "r");
  if (fp == NULL) {
    printf("File dose not found.\n");
    return EXIT_FAILURE;
  }
  char f[100];
  int count = 0;
  while (fgets(f, 100, fp) != NULL) {
    int l = strlen(f);
    if (f[l - 1] == '\n') {
      f[l - 1] = '\0';
    }
    for (int i = 0; i < l; i++) {
      if (f[i] == '#') f[i] = '\0';  //#以降を受け取らないようにする。
    }
    char* c;
    c = strtok(f, " ");
    if (c == NULL) {
      continue;
    }
    double data[5];  //ファイルに十分な引数がないときは0として扱う
    for (int i = 0; i < 5; i++) {
      data[i] = 0;
    }
    for (int i = 0; i < 5 && c != NULL; i++) {
      data[i] = atof(c);
      c = strtok(NULL, " ");
    }
    for (int i = 0; i < 5; i++) {
      printf("%2.1f\n", data[i]);
    }
    count++;
  }
  if (count != objnum) {
    printf("the number of object is different from data file\n");
    return EXIT_FAILURE;
  }
  fclose(fp);
  return 0;
}