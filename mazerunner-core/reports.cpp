/*
 * File: logger.cpp
 * Project: mazerunner
 * File Created: Tuesday, 23rd March 2021 10:18:00 pm
 * Author: Peter Harrison
 * -----
 * Last Modified: Thursday, 29th April 2021 9:44:01 am
 * Modified By: Peter Harrison
 * -----
 * MIT License
 *
 * Copyright (c) 2021 Peter Harrison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "reports.h"
#include "src/encoders.h"
#include "src/maze.h"
#include "src/motors.h"
#include "src/profile.h"
#include "src/sensors.h"
#include <Arduino.h>

static uint32_t s_start_time;
static uint32_t s_report_time;
static uint32_t s_report_interval = REPORTING_INTERVAL;

// note that the Serial device has a 64 character buffer and, at 115200 baud
// 64 characters will take about 6ms to go out over the wire.
void report_profile_header() {
#if DEBUG_LOGGING == 1
  Serial.println(F("time robotPos robotAngle fwdPos  fwdSpeed rotpos rotSpeed fwdVolts rotVolts"));
  s_start_time = millis();
  s_report_time = s_start_time;
#endif
}

void report_profile() {
#if DEBUG_LOGGING == 1
  if (millis() >= s_report_time) {
    s_report_time += s_report_interval;
    Serial.print(millis() - s_start_time);
    Serial.print(' ');
    Serial.print(encoders.robot_position());
    Serial.print(' ');
    Serial.print(encoders.robot_angle());
    Serial.print(' ');
    Serial.print(forward.position());
    Serial.print(' ');
    Serial.print(forward.speed());
    Serial.print(' ');
    Serial.print(rotation.position());
    Serial.print(' ');
    Serial.print(rotation.speed());
    Serial.print(' ');
    Serial.print(50 * (g_right_motor_volts + g_left_motor_volts));
    Serial.print(' ');
    Serial.print(50 * (g_right_motor_volts - g_left_motor_volts));
    Serial.println();
  }
#else
  delay(2);
#endif
}

//***************************************************************************//

void report_sensor_track_header() {
#if DEBUG_LOGGING == 1
  Serial.println(F("time pos angle left right front error adjustment"));
  s_start_time = millis();
  s_report_time = s_start_time;
#endif
}

void report_sensor_track(bool use_raw = false) {
#if DEBUG_LOGGING == 1
  if (millis() >= s_report_time) {
    s_report_time += s_report_interval;
    Serial.print(millis() - s_start_time);
    Serial.print(' ');
    Serial.print(encoders.robot_position());
    Serial.print(' ');
    Serial.print(encoders.robot_angle());
    if (use_raw) {
      Serial.print(' ');
      Serial.print(sensors.lss.raw);
      Serial.print(' ');
      Serial.print(sensors.rss.raw);
      Serial.print(' ');
      Serial.print(sensors.lfs.raw);
      Serial.print(' ');
      Serial.print(sensors.rfs.raw);
    } else {
      Serial.print(' ');
      Serial.print(sensors.lss.value);
      Serial.print(' ');
      Serial.print(sensors.rss.value);
      Serial.print(' ');
      Serial.print(sensors.lfs.value);
      Serial.print(' ');
      Serial.print(sensors.rfs.value);
    }
    Serial.print(' ');
    Serial.print(sensors.get_cross_track_error());
    Serial.print(' ');
    Serial.print(sensors.g_steering_adjustment);
    Serial.println();
  }
#else
  delay(2);
#endif
}

void report_front_sensor_track_header() {
#if DEBUG_LOGGING == 1
  Serial.println(F("time pos front_normal front_raw"));
  s_start_time = millis();
  s_report_time = s_start_time;
#endif
}

void report_front_sensor_track() {
#if DEBUG_LOGGING == 1
  if (millis() >= s_report_time) {
    s_report_time += s_report_interval;
    Serial.print(millis() - s_start_time);
    Serial.print(' ');
    Serial.print(fabsf(encoders.robot_position()));
    Serial.print(' ');
    Serial.print(sensors.lfs.value);
    Serial.print(' ');
    Serial.print(sensors.lfs.raw);
    Serial.println();
  }
#else
  delay(2);
#endif
}

//***************************************************************************//

void report_wall_sensors() {
  Serial.print('\n');
  Serial.print('R');
  Serial.print(' ');
  print_justified(sensors.lfs.raw, 7);
  print_justified(sensors.lss.raw, 7);
  print_justified(sensors.rss.raw, 7);
  print_justified(sensors.rfs.raw, 7);
  Serial.print(' ');
  Serial.print('-');
  Serial.print('>');
  print_justified(sensors.lfs.value, 7);
  print_justified(sensors.lss.value, 7);
  print_justified(sensors.rss.value, 7);
  print_justified(sensors.rfs.value, 7);
  Serial.print(' ');
}

//***************************************************************************//

void report_sensor_calibration() {
  Serial.println(F("   lf_raw ls_raw rs_raw rf_raw |  lf_cal ls_cal rs_cal rf_cal"));
  sensors.enable_sensors();
  s_start_time = millis();
  s_report_time = s_start_time;
  while (not sensors.button_pressed()) {
    if (millis() >= s_report_time) {
      s_report_time += 100;
      report_wall_sensors();
    }
  }
  Serial.println();
  sensors.wait_for_button_release();
  delay(200);
  sensors.disable_sensors();
}

//***************************************************************************//

// simple formatting functions for printing maze costs
void print_hex_2(unsigned char value) {
  if (value < 16) {
    Serial.print('0');
  }
  Serial.print(value, HEX);
}

void print_justified(int32_t value, int width) {
  int v = value;
  int w = width;
  w--;
  if (v < 0) {
    w--;
  }
  while (v /= 10) {
    w--;
  }
  while (w > 0) {
    Serial.write(' ');
    --w;
  }
  Serial.print(value);
}
void print_justified(int value, int width) {
  print_justified(int32_t(value), width);
}

/***
 * printing functions.
 * Code space can be saved here by not usingserial.print
 */

void printNorthWalls(int row) {
  for (int col = 0; col < 16; col++) {
    unsigned char cell = row + 16 * col;
    Serial.print('o');
    if (maze.is_wall(cell, NORTH)) {
      Serial.print(("---"));
    } else {
      Serial.print(("   "));
    }
  }
  Serial.println('o');
}

void printSouthWalls(int row) {
  for (int col = 0; col < 16; col++) {
    unsigned char cell = row + 16 * col;
    Serial.print('o');
    if (maze.is_wall(cell, SOUTH)) {
      Serial.print(("---"));
    } else {
      Serial.print(("   "));
    }
  }
  Serial.println('o');
}

void print_maze_plain() {
  Serial.println();
  for (int row = 15; row >= 0; row--) {
    printNorthWalls(row);
    for (int col = 0; col < 16; col++) {
      unsigned char cell = static_cast<unsigned char>(row + 16 * col);
      if (maze.is_exit(cell, WEST)) {
        Serial.print(("    "));
      } else {
        Serial.print(("|   "));
      }
    }
    Serial.println('|');
  }
  printSouthWalls(0);
  Serial.println();
  ;
}

void print_maze_with_costs() {
  Serial.println();
  ;
  for (int row = 15; row >= 0; row--) {
    printNorthWalls(row);
    for (int col = 0; col < 16; col++) {
      unsigned char cell = static_cast<unsigned char>(row + 16 * col);
      if (maze.is_exit(cell, WEST)) {
        Serial.print(' ');
      } else {
        Serial.print('|');
      }
      print_justified(maze.cost[cell], 3);
    }
    Serial.println('|');
  }
  printSouthWalls(0);
  Serial.println();
  ;
}

static char dirChars[] = "^>v<*";

void print_maze_with_directions() {
  Serial.println();
  maze.flood_maze(maze.maze_goal());
  for (int row = 15; row >= 0; row--) {
    printNorthWalls(row);
    for (int col = 0; col < 16; col++) {
      unsigned char cell = row + 16 * col;
      if (maze.is_wall(cell, WEST)) {
        Serial.print('|');
      } else {
        Serial.print(' ');
      }
      unsigned char direction = maze.direction_to_smallest(cell, NORTH);
      if (cell == maze.maze_goal()) {
        direction = 4;
      }
      Serial.print(' ');
      Serial.print(dirChars[direction]);
      Serial.print(' ');
    }
    Serial.println('|');
  }
  printSouthWalls(0);
  Serial.println();
  ;
}

void print_maze_wall_data() {
  Serial.println();
  ;
  for (int row = 15; row >= 0; row--) {
    for (int col = 0; col < 16; col++) {
      int cell = row + 16 * col;
      print_hex_2(maze.walls[cell]);
      Serial.print(' ');
    }
    Serial.println();
    ;
  }
  Serial.println();
  ;
}
