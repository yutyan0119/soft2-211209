# アライメントの考察
## ソースコード
```c
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

int main(int argc, char** argv) {
  Student s_begin;
  Student s1;
  TStudent1 s2;
  TStudent2 s3;
  TStudent2 s_end;

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
  // do_something!!
  printf("char : %zu\n",sizeof(char));
  printf("int : %zu\n",sizeof(int));
  printf("dobule %zu\n",sizeof(double));
  return 0;
}

```
## 実行結果
```bash
~/lecture/software2/2/soft2-211209-src$ ./struct_alignment 
s_end : 0x7ffd0a028bb0
s3 : 0x7ffd0a028b30
s2 : 0x7ffd0a028c30
s1 : 0x7ffd0a028ab0
s_begin : 0x7ffd0a028a30
s_end : 128
s3 : 128
s2 : 136
s1 : 128
s_begin : 128
char : 1
int : 4
dobule 8
```
## 考察
`Student`は`int`が2つ、`double`が2つ、`char`が100個分含まれた構造体である。実行結果のサイズから必要なメモリ容量は`4*2+8*2+1*100=124byte`のはずだが、実際のサイズは`128`になっている。他の構造体についても同様にすると以下のようになる。
|構造体|必要なサイズ|サイズ（出力）|
|---|----|----|
|Student|124|128|
|TStudent1|125|136|
|TStudent2|125|128|

このうち特に特徴的なのは`TStudent1`と`TStudent2`が、全く同じ必要サイズに対して異なるサイズを持っているところである。違いは、構造体内での変数定義の順番である。ここで、以下のような構造体をあらたに宣言して、サイズを見てみることにした。
<!-- ここから後は後日書く -->