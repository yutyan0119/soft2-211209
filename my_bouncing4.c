#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define mq = 1  //電子の重さ

typedef struct condition {
  int width;   // 見えている範囲の幅
  int height;  // 見えている範囲の高さ
  double k;    // クーロン定数
  double dt;   // シミュレーションの時間幅
  double cor;  // 壁の反発係数
  double Ex;
  double Ey;
  double Ez;
  double Bx;
  double By;
  double Bz;
} Condition;

// 個々の物体を表す構造体
typedef struct object {
  double m;
  double q;
  double y;
  double x;
  double z;
  double prev_y;  // 壁からの反発に使用
  double prev_x;  // 壁からの反発に使用
  double prev_z;  // 壁からの反発に使用
  double vy;
  double vx;
  double vz;
  double prev_vy;
  double prev_vx;
  double prev_vz;
} Object;

void my_plot_objects(Object objs[], const size_t numobj, const double t,
                     const Condition cond);
void my_update_velocities(Object objs[], const size_t numobj,
                          const Condition cond);
void my_update_positions(Object objs[], const size_t numobj,
                         const Condition cond);
void my_bounce(Object objs[], const size_t numobj, const Condition cond);

void my_fusion(Object objs[], const size_t numobj, const Condition cond);

double min(double a, double b);
double max(double a, double b);

int main(int argc, char** argv) {
  const Condition cond = {
      .width = 75, .height = 40, .k = 1.0, .dt = 0.5, .cor = 0.8};
  if (argc <= 2 || argc > 3) {
    printf("execute like \"./a.out [the number of objects] [filename]\"\n");
    return EXIT_FAILURE;
  }

  size_t objnum = atoi(argv[1]);
  Object objects[objnum];
  FILE* fp;
  fp = fopen(argv[2], "r");
  if (fp == NULL) {
    printf("File does not found.\n");
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
    double data[7];  //ファイルに十分な引数がないときは0として扱う
    for (int i = 0; i < 7; i++) {
      data[i] = 0.0;
    }
    for (int i = 0; i < 7 && c != NULL; i++) {
      data[i] = atof(c);
      c = strtok(NULL, " ");
    }
    if (count == 0) {
      cond.Ex = data[0];
      cond.Ey = data[1];
      cond.Ez = data[2];
      cond.Bx = data[3];
      cond.By = data[4];
      cond.Bz = data[5];
    } else {
      objects[count] = (Object){.m = mq,
                                .q = data[0],
                                .x = data[1],
                                .y = data[2],
                                .z = data[3],
                                .vx = data[4],
                                .vy = data[5],
                                .vz = data[6]};
    }
    count++;
  }
  if (count - 1 != objnum) {
    printf(
        "The number of object is different from data file or some data is lacked\n
        File must contain Electric field and Magnetic fux density information\n
        Please refer ./data.dat\n");
    return EXIT_FAILURE;
  }
  // シミュレーション. ループは整数で回しつつ、実数時間も更新する
  const double stop_time = 1000;
  double t = 0;
  for (size_t i = 0; t <= stop_time; i++) {
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum,
              cond);  // 壁があると仮定した場合に壁を跨いでいたら反射させる
    my_fusion(objects, objnum, cond);
    // 表示の座標系は width/2, height/2 のピクセル位置が原点となるようにする
    my_plot_objects(objects, objnum, t, cond);

    usleep(100 * 1000);                 // 200 x 1000us = 200 ms ずつ停止
    printf("\e[%dA", cond.height + 6);  // 壁とパラメータ表示分で3行
  }
  return EXIT_SUCCESS;
}

// 実習: 以下に my_ で始まる関数を実装する
// 最終的に phisics2.h 内の事前に用意された関数プロトタイプをコメントアウト

void my_plot_objects(Object objs[], const size_t numobj, const double t,
                     const Condition cond) {
  int width = cond.width;
  int height = cond.height;
  char field[height + 1][width + 1];
  char ceil[width + 3];
  for (int i = 1; i < width + 1; i++) {
    ceil[i] = '-';
  }
  ceil[0] = '+';
  ceil[width + 1] = '+';
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      field[i][j] = ' ';
    }
  }
  for (int i = 0; i < numobj; i++) {
    int y = (int)(objs + i)->y;
    int x = (int)(objs + i)->x;
    double q = (objs + i)->q;
    if (q != 0 && -height / 2 <= y && y <= height / 2 && -width / 2 <= x &&
        x <= width / 2) {
      field[y + height / 2][x + width / 2] = 'o';
    }
  }
  printf("%s\n", ceil);
  for (int i = 0; i < height; i++) {
    printf("|");
    for (int j = 0; j < width; j++) {
      printf("%c", field[i][j]);
    }
    printf("|");
    printf("\n");
  }
  printf("%s\n", ceil);
  printf("t = %2.1f\n", t);
  for (int i = 0; i < numobj; i++) {
    printf(
        "objs[%d].q = %2.1f, objs[%d].x = %2.1f, objs[%d].y = %2.1f, "
        "objs[%d].z = %2.1f, vx = "
        "%2.2f, vy = %2.2f, vz = %2.2f",
        i, (objs + i)->q, i, (objs + i)->x, i, (objs + i)->y, i, (objs + i)->z,
        (objs + i)->vx, (objs + i)->vy, (objs + i)->vz);
    if (i != numobj - 1) {
      printf("\n");
    }
  }
  printf("\n");
};

void my_update_velocities(Object objs[], const size_t numobj,
                          const Condition cond) {
  double k = cond.k;
  double dt = cond.dt;
  double Ex = cond.Ex;
  double Ey = cond.Ey;
  double Ez = cond.Ez;
  double Bx = cond.Bx;
  double By = cond.By;
  double Bz = cond.Bz;
  for (int i = 0; i < numobj; i++) {
    if ((objs + i)->q == 0) continue;
    double vy = (objs + i)->prev_vy = (objs + i)->vy;
    double vx = (objs + i)->prev_vx = (objs + i)->vx;
    double vz = (objs + i)->prev_vz = (objs + i)->vz;
    double Fx = 0;
    double Fy = 0;
    double Fz = 0;
    double q = (objs + i)->q;
    double y = (objs + i)->y;
    double x = (objs + i)->x;
    double z = (objs + i)->z;
    double m = (objs + i)->m;
    for (int j = 0; j < numobj; j++) {
      double q2 = (objs + j)->q;
      double z2 = (objs + j)->z;
      double y2 = (objs + j)->y;
      double x2 = (objs + j)->x;
      if (i == j || y - y2 == 0 || x - x2 == 0) continue;
      double r =
          sqrt((y - y2) * (y - y2) + (x - x2) * (x - x2) + (z - z2) * (z - z2));
      Fx += k * q * q2 * (x2 - x) / pow(r, 3.0);
      Fy += k * q * q2 * (y2 - y) / pow(r, 3.0);
      Fz += k * q * q2 * (z2 - z) / pow(r, 3.0);
    }
    Fx += q * (Ex + vy * Bz - vz * By);
    Fy += q * (Ey + vz * Bx - vx * Bz);
    Fz += q * (Ez + vx * By - vy * Bx);
    double ax = Fx / m;
    double ay = Fy / m;
    double az = Fz / m;
    (objs + i)->vx += ax * dt;
    (objs + i)->vy += ay * dt;
    (objs + i)->vz += az * dt;
  }
};

void my_update_positions(Object objs[], const size_t numobj,
                         const Condition cond) {
  double dt = cond.dt;
  for (int i = 0; i < numobj; i++) {
    (objs + i)->prev_y = (objs + i)->y;
    (objs + i)->y += (objs + i)->prev_vy * dt;
    (objs + i)->prev_x = (objs + i)->x;
    (objs + i)->x += (objs + i)->prev_vx * dt;
    (objs + i)->prev_z = (objs + i)->z;
    (objs + i)->z += (objs + i)->prev_vz * dt;
  }
};

void my_bounce(Object objs[], const size_t numobj, const Condition cond) {
  double dt = cond.dt;
  double cor = cond.cor;
  int height = cond.height;
  int width = cond.width;
  for (int i = 0; i < numobj; i++) {
    double y = (objs + i)->y;
    double prev_y = (objs + i)->prev_y;
    double x = (objs + i)->x;
    double prev_x = (objs + i)->prev_x;
    if (prev_y <= height / 2 && y > height / 2) {
      double vy = (objs + i)->prev_vy;        //移動速度
      double t = (height / 2 - prev_y) / vy;  //衝突までの時間
      vy *= -cor;                             //速度を反転させる
      (objs + i)->prev_vy = vy;
      (objs + i)->vy = vy;
      (objs + i)->y = max(-height / 2, height / 2 + vy * (dt - t));
    }
    if (prev_x <= width / 2 && x > width / 2) {
      double vx = (objs + i)->prev_vx;
      double t = (width / 2 - prev_x) / vx;
      vx *= -cor;
      (objs + i)->prev_vx = vx;
      (objs + i)->vx = vx;
      (objs + i)->x = max(-width / 2, width / 2 + vx * (dt - t));
    }
    if (prev_y >= -height / 2 && y < -height / 2) {
      double vy = (objs + i)->prev_vy;        //移動速度
      double t = (prev_y + height / 2) / vy;  //衝突までの時間
      vy *= -cor;                             //速度を反転させる
      (objs + i)->prev_vy = vy;
      (objs + i)->vy = vy;
      (objs + i)->y = min(height / 2, -height / 2 + vy * (dt - t));
    }
    if (prev_x >= -width / 2 && x < -width / 2) {
      double vx = (objs + i)->prev_vx;
      double t = (width / 2 + prev_x) / vx;
      vx *= -cor;
      (objs + i)->prev_vx = vx;
      (objs + i)->vx = vx;
      (objs + i)->x = min(width / 2, -width / 2 + vx * (dt - t));
    }
  }
}

void my_fusion(Object objs[], const size_t numobj, const Condition cond) {
  for (int i = 0; i < numobj; i++) {
    for (int j = i + 1; j < numobj; j++) {
      if (i != j) {
        double x1 = (objs + i)->x;
        double y1 = (objs + i)->y;
        double z1 = (objs + i)->z;
        double x2 = (objs + j)->x;
        double y2 = (objs + j)->y;
        double z2 = (objs + j)->z;
        double r = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) +
                        (z1 - z2) * (z1 - z2));
        if (r < 1 && (objs + i)->m != 0 && (objs + j)->m != 0) {
          double q1 = (objs + i)->q;
          double q2 = (objs + j)->q;
          double v1x = (objs + i)->vx;
          double v1y = (objs + i)->vy;
          double v1z = (objs + i)->vz;
          double v2x = (objs + j)->vx;
          double v2y = (objs + j)->vy;
          double v2z = (objs + j)->vz;
          double m1 = (objs + i)->m;
          double m2 = (objs + j)->m;
          double newvx = (m1 * v1x + m2 * v2x) / (m1 + m2);
          double newvy = (m1 * v1y + m2 * v2y) / (m1 + m2);
          (objs + i)->m = m1 + m2;
          (objs + i)->vx = newvx;
          (objs + i)->vy = newvy;
          (objs + j)->m = 0;
          (objs + j)->q = 0;
        }
      }
    }
  }
};

double min(double a, double b) {
  if (a < b) {
    return a;
  } else
    return b;
}

double max(double a, double b) {
  if (a > b) {
    return a;
  } else
    return b;
}