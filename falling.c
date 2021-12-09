// どこまでも落ちていく...

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "physics.h"  // この中に構造体定義、関数プロトタイプがある

void my_plot_objects(Object objs[], const size_t numobj, const double t,
                     const Condition cond);
void my_update_velocities(Object objs[], const size_t numobj,
                          const Condition cond);
void my_update_positions(Object objs[], const size_t numobj,
                         const Condition cond);

int main(int argc, char **argv) {
  const Condition cond = {.width = 75, .height = 40, .G = 1.0, .dt = 1.0};

  size_t objnum = 2;
  Object objects[objnum];

  // o[1] は巨大な物体を画面外に... 地球のようなものを想定
  objects[0] = (Object){.m = 60.0, .y = -20.0, .vy = 0.2};
  objects[1] = (Object){.m = 100000.0, .y = 1000.0, .vy = 0.0};

  // シミュレーション. ループは整数で回しつつ、実数時間も更新する
  const double stop_time = 400;
  double t = 0;
  for (size_t i = 0; t <= stop_time; i++) {
    t = i * cond.dt;
    my_update_velocities(objects, objnum, cond);
    my_update_positions(objects, objnum, cond);

    // 表示の座標系は width/2, height/2 のピクセル位置が原点となるようにする
    my_plot_objects(objects, objnum, t, cond);

    usleep(200 * 1000);  // 200 x 1000us = 200 ms ずつ停止
    printf(
        "\e[%dA",
        cond.height +
            2);  // 表示位置を巻き戻す。壁がないのでheight+2行（境界とパラメータ表示分）
  }
  return EXIT_SUCCESS;
}

// 実習: 以下に my_ で始まる関数を実装する
// 最終的には physics.h 内の、こちらが用意したプロトタイプをコメントアウト

void my_plot_objects(Object objs[], const size_t numobj, const double t,
                     const Condition cond) {
  int width = cond.width;
  int height = cond.height;
  char field[height + 1][width + 1];
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      field[i][j] = ' ';
    }
  }
  for (int i = 0; i < numobj; i++) {
    int y = (int)(objs + i)->y;
    if (-height / 2 <= y && y <= height / 2) {
      field[y + height / 2][width / 2] = 'o';
    }
  }
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      printf("%c", field[i][j]);
    }
    printf("\n");
  }

  printf("-----\n");
  printf("t = %f, ", t);
  for (int i = 0; i < numobj; i++) {
    printf("objs[%d].y = %f, vy = %f", i, (objs + i)->y, (objs + i)->vy);
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
    double F = 0;
    double m = (objs + i)->m;
    double y = (objs + i)->y;
    for (int j = 0; j < numobj; j++) {
      double m2 = (objs + j)->m;
      double y2 = (objs + j)->y;
      if (i == j || y - y2 == 0) continue;
      double r2 = (y - y2) * (y - y2);
      if (y2 - y > 0) {
        F += G * m * m2 / r2;
      } else {
        F -= G * m * m2 / r2;
      }
    }
    double a = F / m;
    (objs + i)->vy += a * dt;
  }
};

void my_update_positions(Object objs[], const size_t numobj,
                         const Condition cond) {
  double dt = cond.dt;
  for (int i = 0; i < numobj; i++) {
    (objs + i)->y += (objs + i)->prev_vy * dt;
  }
};