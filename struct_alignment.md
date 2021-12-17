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
---------
s_end : 0x7ffd0a028bb0 //下3桁を10進数にしてみる 2992->4th
s3 : 0x7ffd0a028b30 //                         2864->3rd
s2 : 0x7ffd0a028c30 //                         3120->5th
s1 : 0x7ffd0a028ab0 //                         2736->2nd
s_begin : 0x7ffd0a028a30//                     2608->1st
---------
s_end : 128
s3 : 128
s2 : 136
s1 : 128
s_begin : 128
---------
char : 1
int : 4
dobule 8
---------
```
## 考察
`Student`は`int`が2つ、`double`が2つ、`char`が100個分含まれた構造体である。実行結果のサイズから必要なメモリ容量は`4*2+8*2+1*100=124byte`のはずだが、実際のサイズは`128`になっている。これは、チャンクが`8byte`であることに起因していると考えられる。他の構造体についても同様にすると以下のようになる。
|構造体|必要なサイズ|サイズ（出力）|
|---|----|----|
|Student|124|128|
|TStudent1|125|136|
|TStudent2|125|128|

このうち特に特徴的なのは`TStudent1`と`TStudent2`が、全く同じ必要サイズに対して異なるサイズを持っているところである。違いは、構造体内での変数定義の順番である。そこで、各変数のアドレスを参照してみる。
```c
typedef struct tagged_student1 {
  int id;
  char name[100];
  int age;
  double height;
  double weight;
  char tag;
} TStudent1;

TStudent1.id : 0x7ffce3dea140 //default 4byte->4byte 1st
TStudent1.name : 0x7ffce3dea144 //default 100byte->100byte 2nd
TStudent1.age : 0x7ffce3dea1a8 //default 4byte->8byte 3rd
TStudent1.height : 0x7ffce3dea1b0 //default 8byte->8byte 4th
TStudent1.weight : 0x7ffce3dea1b8 //default 8byte->8byte 5th
TStudent1.tag : 0x7ffce3dea1c0 //default 1bit->8byte 6th

typedef struct tagged_student2 {
  char tag;
  int id;
  char name[100];
  int age;
  double height;
  double weight;
} TStudent2;

TStudent2.tag : 0x7ffce3dea040 //default 1byte->4byte 1st
TStudent2.id : 0x7ffce3dea044 //defalut 4byte->4byte 2nd
TStudent2.name : 0x7ffce3dea048 //default 100byte->100byte 3rd
TStudent2.age : 0x7ffce3dea0ac //default 4byte->4byte 4th
TStudent2.height : 0x7ffce3dea0b0 //default 8byte->8byte 5th
TStudent2.weight : 0x7ffce3dea0b8 //default 8byte->8byte 6th
```

構造体の中では、変数は構造体中で宣言された順にアドレスがついている。これは構造体自体の確保の順番と異なっている。`TStudent1`は、構造体の中でも最後に確保されている。アライメントは次に確保する変数のサイズに依存することを踏まえると、64bitマシンでは、ありえる型1つのサイズは8byteであり、最後の変数`tag`で使用された領域のあとのアドレスは8の倍数になっていることが望ましい。そのため、1byteが必要な`char`型に対して8byteもの領域が確保されていると考えられる。そのために`TStudent1`のサイズは136byteになっている。`TStudent2`についてもアライメントの原則から、`char`型に対し4byte確保されている。では、`char`型が連続するときはどうなるかというと、以下のような構造体を考えてみた。
```c
typedef struct tagged_student3 {
  char tag;
  char id_char;
  char name[100];
  int age;
  double height;
  double weight;
} TStudent3;
```
```
size : 128 byte
TStudent3.tag : 0x7ffd113e4430 default 1byte-> 1byte
TStudent3.id_char : 0x7ffd113e4431 1byte -> 1byte
TStudent3.name : 0x7ffd113e4432 100byte -> 102byte
TStudent3.age : 0x7ffd113e4498 4byte -> 8byte
TStudent3.height : 0x7ffd113e44a0 8byte -> 8byte
TStudent3.weight : 0x7ffd113e44a8 8byte -> 8byte
```
このような結果になり、たしかに後ろの変数のサイズに合わせて領域を確保することがわかった。なお、char2連続のあとには、`int`型に合わせて`char[100]`が、102byteを確保していることがわかる。
以上を踏まえると、構造体の中では、変数の定義順に領域が確保され、その領域のサイズは、自分よりあとに確保される型もしくは、構造体のサイズに依存して、変わりうるということがわかる。