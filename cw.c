/*
* Routine:  A program that calculates the simulated distance traveled
            by the user--based on their walking data--towards various
            celestial bodies.

* Author:   DannyBimma

* Date:     April, 2026.

* Copyright (c) 2026 Technomancer Pirate Captain. All Rights Reserved.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define DATA_FILE "steps_data_daily.txt"
#define AVG_STEP_FEET 2.5
#define FEET_PER_MILE 5280.0
#define MOON_DISTANCE_MILES 238900.0
#define MOON_DISTANCE_FEET (MOON_DISTANCE_MILES * FEET_PER_MILE)
#define BAR_WIDTH 50
#define LINE_BUF 64

// Prototypes
static int tally_steps(const char *path, long long *total_steps, int *days,
                       int *months);
static double steps_to_feet(long long steps);
static double feet_to_miles(double feet);
static double percent_to_moon(double feet);
static void print_report(double pct, double feet, double miles,
                         long long steps);
static void print_progress_bar(double pct);

int main(void) {
  long long total_steps = 0;
  int days = 0;
  int months = 0;

  if (tally_steps(DATA_FILE, &total_steps, &days, &months) != 0) {
    fprintf(stderr, "cosmic-walk: failed to read '%s'\n", DATA_FILE);

    return EXIT_FAILURE;
  }

  double feet = steps_to_feet(total_steps);
  double miles = feet_to_miles(feet);
  double pct = percent_to_moon(feet);

  print_report(pct, feet, miles, total_steps);
  print_progress_bar(pct);

  return EXIT_SUCCESS;
}

// Tally the number of steps logged in steps data file
static int tally_steps(const char *path, long long *total_steps, int *days,
                       int *months) {
  // Open the file for reading
  FILE *fp = fopen(path, "r");
  if (!fp)
    return 1;

  char buf[LINE_BUF];
  long long sum = 0;
  int day_count = 0;
  int month_count = 0;
  int data_in_month = 0;

  // Process the data
  while (fgets(buf, sizeof buf, fp)) {
    char *p = buf;
    while (*p && isspace((unsigned char)*p))
      p++;

    if (*p == '\0') {
      if (data_in_month) {
        month_count++;
        data_in_month = 0;
      }
      continue;
    }

    char *end = NULL;
    long long val = strtoll(p, &end, 10);
    if (end == p) {
      fclose(fp);

      return 2;
    }

    sum += val;
    day_count++;
    data_in_month = 1;
  }

  if (data_in_month)
    month_count++;

  fclose(fp);

  *total_steps = sum;
  *days = day_count;
  *months = month_count;

  return 0;
}

// Convert steps to feet
static double steps_to_feet(long long steps) {
  return (double)steps * AVG_STEP_FEET;
}

// Convert feet to miles
static double feet_to_miles(double feet) { return feet / FEET_PER_MILE; }

// Convert feet to percent of distance traveled the Moon
static double percent_to_moon(double feet) {
  return (feet / MOON_DISTANCE_FEET) * 100.0;
}

// Output to console
static void print_report(double pct, double feet, double miles,
                         long long steps) {
  puts("COSMIC WALK PROGRESS");
  puts("");
  printf("Distance travelled the Moon:  %12.2f%%\n", pct);
  printf("Distance travelled in feet:   %12.0f feet\n", feet);
  printf("Distance travelled in miles:  %12.2f miles\n", miles);
  printf("Total steps stepped:           %12lld steps\n", steps);
  puts("");
}

// ASCII progress bar
static void print_progress_bar(double pct) {
  // Clamp the percentage to 0-100%
  double clamped = pct;
  if (clamped < 0.0)
    clamped = 0.0;
  if (clamped > 100.0)
    clamped = 100.0;

  // Calculate the width of the bar to fill
  int filled = (int)((clamped / 100.0) * BAR_WIDTH);
  if (filled > BAR_WIDTH)
    filled = BAR_WIDTH;

  // Print the bar
  putchar('[');
  for (int i = 0; i < BAR_WIDTH; i++) {
    if (i < filled)
      putchar('=');
    else if (i == filled)
      putchar('>');
    else
      putchar('-');
  }
  printf("] %.2f%%\n", clamped);
}
