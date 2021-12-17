#include <stdio.h>

typedef struct student {
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} Student;

typedef struct tagged_student1 {
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
} TStudent1;

typedef struct tagged_student2 {
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} TStudent2;

typedef struct tagged_student3 {
  char tag;
  char id_char;
  char name[100];
  int age;
  double height;
  double weight;
} TStudent3;
int main(int argc, char** argv) {
  Student s_begin;
  Student s1;
  TStudent1 s2;
  TStudent2 s3;
  TStudent2 s_end;
  TStudent3 s4;

  /* 以下に各構造体の先頭アドレスを表示するプログラムを書く */
  /* Hint: printf で %p フォーマットを指定する*/
  /* 逆順に表示（s_end, s3, s2, s1, s_begin の順）*/
  printf("s_end : %p\n", &s_end);
  printf("s3 : %p\n", &s3);
  printf("s2 : %p\n", &s2);
  printf("s1 : %p\n", &s1);
  printf("s_begin : %p\n", &s_begin);
  // do_something!!

  /* 以下には各構造体のサイズをsizeof演算子で計算し、表示する */
  /* printf 表示には%zu を用いる*/
  printf("s_end : %zu\n", sizeof(s_end));
  printf("s3 : %zu\n", sizeof(s3));
  printf("s2 : %zu\n", sizeof(s2));
  printf("s1 : %zu\n", sizeof(s1));
  printf("s_begin : %zu\n", sizeof(s_begin));
  printf("s4 : %zu\n",sizeof(s4));
  // do_something!!
  printf("char : %zu\n", sizeof(char));
  printf("int : %zu\n", sizeof(int));
  printf("dobule %zu\n", sizeof(double));
  printf("TStudent1.id : %p\n", &s2.id);
  printf("TStudent1.name : %p\n", s2.name);
  printf("TStudent1.age : %p\n", &s2.age);
  printf("TStudent1.height : %p\n", &s2.height);
  printf("TStudent1.weight : %p\n", &s2.weight);
  printf("TStudent1.tag : %p\n", &s2.tag);
  printf("TStudent2.id : %p\n", &s3.id);
  printf("TStudent2.name : %p\n", s3.name);
  printf("TStudent2.age : %p\n", &s3.age);
  printf("TStudent2.height : %p\n", &s3.height);
  printf("TStudent2.weight : %p\n", &s3.weight);
  printf("TStudent3.tag : %p\n", &s4.tag);
  printf("TStudent3.id_char : %p\n", &s4.id_char);
  printf("TStudent3.name : %p\n", s4.name);
  printf("TStudent3.age : %p\n", &s4.age);
  printf("TStudent3.height : %p\n", &s4.height);
  printf("TStudent3.weight : %p\n", &s4.weight);
  printf("TStudent3.tag : %p\n", &s4.tag);
  return 0;
}