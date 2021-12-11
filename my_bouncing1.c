#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct condition {
  int width;   // 見えている範囲の幅
  int height;  // 見えている範囲の高さ
  double G;    // 重力定数
  double dt;   // シミュレーションの時間幅
  double cor;  // 壁の反発係数
} Condition;

// 個々の物体を表す構造体
typedef struct object {
  double m;
  double y;
  double x;
  double prev_y;  // 壁からの反発に使用
  double prev_x;  // 壁からの反発に使用
  double vy;
  double vx;
  double prev_vy;
  double prev_vx;
} Object;

void my_plot_objects(Object objs[], const size_t numobj, const double t,
                     const Condition cond);
void my_update_velocities(Object objs[], const size_t numobj,
                          const Condition cond);
void my_update_positions(Object objs[], const size_t numobj,
                         const Condition cond);
void my_bounce(Object objs[], const size_t numobj, const Condition cond);

int main(int argc, char **argv) {
  const Condition cond = {
      .width = 75, .height = 40, .G = 1.0, .dt = 1.0, .cor = 0.8};

  size_t objnum = 2;
  Object objects[objnum];

  // objects[1] は巨大な物体を画面外に... 地球のようなものを想定
  objects[0] = (Object){.m = 60.0, .y = -19.9, .vy = 2.0, .x = 0, .vx = 1.0};
  objects[1] = (Object){.m = 100000.0, .y = 1000.0, .vy = 0.0, .x = 0, .vx = 0};

  // シミュレーション. ループは整数で回しつつ、実数時間も更新する
  const double stop_time = 400;
  double t = 0;
  for (size_t i = 0; t <= stop_time; i++) {
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);
    my_bounce(objects, objnum,
              cond);  // 壁があると仮定した場合に壁を跨いでいたら反射させる

    // 表示の座標系は width/2, height/2 のピクセル位置が原点となるようにする
    my_plot_objects(objects, objnum, t, cond);

    usleep(100 * 1000);                 // 200 x 1000us = 200 ms ずつ停止
    printf("\e[%dA", cond.height + 3);  // 壁とパラメータ表示分で3行
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
    if (-height / 2 <= y && y <= height / 2 && -width / 2 <= x &&
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
  printf("t = %2.0f, ", t);
  for (int i = 0; i < numobj; i++) {
    printf("objs[%d].y = %2.1f, objs[%d].x = %2.1f, vx = %2.2f, vy = %2.2f", i,
           (objs + i)->y, i, (objs + i)->x, (objs + i)->vx, (objs + i)->vy);
    if (i != numobj - 1) {
      printf(", ");
    }
  }
  printf("\n");
};

void my_update_velocities(Object objs[], const size_t numobj,
                          const Condition cond) {
  double G = cond.G;
  double dt = cond.dt;
  for (int i = 0; i < numobj; i++) {
    (objs + i)->prev_vy = (objs + i)->vy;
    (objs + i)->prev_vx = (objs + i)->vx;
    double Fx = 0;
    double Fy = 0;
    double m = (objs + i)->m;
    double y = (objs + i)->y;
    double x = (objs + i)->x;
    for (int j = 0; j < numobj; j++) {
      double m2 = (objs + j)->m;
      double y2 = (objs + j)->y;
      double x2 = (objs + j)->x;
      if (i == j || y - y2 == 0 || x - x2 == 0) continue;
      double r = sqrt((y - y2) * (y - y2) + (x - x2) * (x - x2));
      Fx += G * m * m2 * (x2 - x) / pow(r, 3.0);
      Fy += G * m * m2 * (y2 - y) / pow(r, 3.0);
    }
    double ax = Fx / m;
    double ay = Fy / m;
    (objs + i)->vx += ax * dt;
    (objs + i)->vy += ay * dt;
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
      (objs + i)->y = height / 2 + vy * (dt - t);
    }
    if (prev_x <= width / 2 && x > width / 2) {
      double vx = (objs + i)->prev_vx;
      double t = (width / 2 - prev_x) / vx;
      vx *= -cor;
      (objs + i)->prev_vx = vx;
      (objs + i)->vx = vx;
      (objs + i)->x = width / 2 + vx * (dt - t);
    }
    if (prev_y >= -height / 2 && y < -height / 2) {
      double vy = (objs + i)->prev_vy;        //移動速度
      double t = (prev_y + height / 2) / vy;  //衝突までの時間
      vy *= -cor;                             //速度を反転させる
      (objs + i)->prev_vy = vy;
      (objs + i)->vy = vy;
      (objs + i)->y = -height / 2 + vy * (dt - t);
    }
    if (prev_x >= -width / 2 && x < -width / 2) {
      double vx = (objs + i)->prev_vx;
      double t = (width / 2 + prev_x) / vx;
      vx *= -cor;
      (objs + i)->prev_vx = vx;
      (objs + i)->vx = vx;
      (objs + i)->x = -width / 2 + vx * (dt - t);
    }
  }
}