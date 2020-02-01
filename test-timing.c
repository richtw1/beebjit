/* Appends at the end of timing.c. */

#include "test.h"

static uint32_t g_timing_test_timer_hits_basic = 0;
static uint32_t g_timing_test_timer_hits_multi = 0;

static void
timing_test_timer_fired_basic(void* p) {
  uint32_t i;

  struct timing_struct* p_timing = (struct timing_struct*) p;

  g_timing_test_timer_hits_basic++;

  for (i = 0; i < k_timing_num_timers; ++i) {
    struct timer_struct* p_timer = &p_timing->timers[i];
    if (p_timer->firing && (p_timer->value == 0)) {
      test_expect_u32(0, timing_get_timer_value(p_timing, i));
      (void) timing_stop_timer(p_timing, i);
    }
  }
}

static void
timing_test_counting() {
  uint64_t countdown;

  g_timing_test_timer_hits_basic = 0;
  struct timing_struct* p_timing = timing_create(1);

  uint32_t t1 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_basic,
                                      p_timing);
  uint32_t t2 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_basic,
                                      p_timing);
  (void) t2;

  test_expect_u32((uint32_t) INT64_MAX, timing_get_countdown(p_timing));
  countdown = timing_start_timer_with_value(p_timing, t1, 100);
  test_expect_u32(100, countdown);
  test_expect_u32(100, timing_get_countdown(p_timing));
  countdown = timing_advance_time(p_timing, 98);
  test_expect_u32(98, countdown);
  test_expect_u32(2, timing_get_total_timer_ticks(p_timing));
  test_expect_u32(2, timing_get_scaled_total_timer_ticks(p_timing));
  /* Peek at the internals to make sure we're really testing a countdown
   * advance that didn't update all timer baselines.
   */
  test_expect_u32(98, p_timing->countdown);
  test_expect_u32(100, p_timing->timers[0].value);
  test_expect_u32(98, timing_get_timer_value(p_timing, t1));

  countdown = timing_start_timer_with_value(p_timing, t2, 50);
  test_expect_u32(50, countdown);
  test_expect_u32(50, p_timing->countdown);
  test_expect_u32(100, p_timing->timers[0].value);
  test_expect_u32(52, p_timing->timers[1].value);
  test_expect_u32(98, timing_get_timer_value(p_timing, t1));
  test_expect_u32(50, timing_get_timer_value(p_timing, t2));

  countdown = timing_stop_timer(p_timing, t2);
  test_expect_u32(50, timing_get_timer_value(p_timing, t2));
  test_expect_u32(98, countdown);
  test_expect_u32(98, p_timing->countdown);
  countdown = timing_start_timer(p_timing, t2);
  test_expect_u32(50, countdown);
  test_expect_u32(50, p_timing->countdown);

  countdown = timing_set_timer_value(p_timing, t2, 40);
  test_expect_u32(40, countdown);
  test_expect_u32(40, p_timing->countdown);
  test_expect_u32(100, p_timing->timers[0].value);
  test_expect_u32(42, p_timing->timers[1].value);

  countdown = timing_adjust_timer_value(p_timing, NULL, t2, -10);
  test_expect_u32(30, countdown);
  test_expect_u32(30, p_timing->countdown);
  test_expect_u32(100, p_timing->timers[0].value);
  test_expect_u32(32, p_timing->timers[1].value);

  countdown = timing_set_firing(p_timing, t2, 0);
  test_expect_u32(98, countdown);
  test_expect_u32(98, p_timing->countdown);
  countdown = timing_set_firing(p_timing, t2, 1);
  test_expect_u32(30, countdown);
  test_expect_u32(30, p_timing->countdown);
  countdown = timing_set_firing(p_timing, t1, 0);
  test_expect_u32(30, countdown);
  test_expect_u32(30, p_timing->countdown);
  countdown = timing_set_firing(p_timing, t1, 1);
  test_expect_u32(30, countdown);
  test_expect_u32(30, p_timing->countdown);

  countdown = timing_advance_time(p_timing, 0);
  test_expect_u32(68, countdown);
  test_expect_u32(68, p_timing->countdown);

  countdown = timing_advance_time(p_timing, countdown);

  timing_destroy(p_timing);
}

static void
timing_test_basics() {
  uint64_t countdown;

  g_timing_test_timer_hits_basic = 0;
  struct timing_struct* p_timing = timing_create(1);

  uint32_t t1 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_basic,
                                      p_timing);
  uint32_t t2 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_basic,
                                      p_timing);
  uint32_t t3 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_basic,
                                      p_timing);

  test_expect_u32(0, timing_get_total_timer_ticks(p_timing));
  test_expect_u32(0, timing_get_scaled_total_timer_ticks(p_timing));

  test_expect_u32(0, timing_timer_is_running(p_timing, t1));

  countdown = timing_start_timer_with_value(p_timing, t1, 100);
  test_expect_u32(1, timing_timer_is_running(p_timing, t1));
  test_expect_u32(1, timing_get_firing(p_timing, t1));
  test_expect_u32(100, countdown);
  test_expect_u32(100, timing_get_timer_value(p_timing, t1));

  /* Timer is already firing, but this is checking for a bug that occurred when
   * setting firing to 1 when it was already 1.
   */
  (void) timing_set_firing(p_timing, t1, 1);

  countdown = timing_set_timer_value(p_timing, t2, 50);
  test_expect_u32(0, timing_timer_is_running(p_timing, t2));
  test_expect_u32(100, countdown);
  test_expect_u32(50, timing_get_timer_value(p_timing, t2));
  countdown = timing_start_timer(p_timing, t2);
  test_expect_u32(50, countdown);
  countdown = timing_set_firing(p_timing, t2, 0);
  test_expect_u32(100, countdown);

  (void) timing_set_timer_value(p_timing, t3, 200);
  test_expect_u32(200, timing_get_timer_value(p_timing, t3));

  countdown -= 1;
  countdown = timing_advance_time(p_timing, countdown);
  countdown = timing_advance_time_delta(p_timing, 1);
  test_expect_u32(98, countdown);
  test_expect_u32(2, timing_get_total_timer_ticks(p_timing));
  test_expect_u32(98, timing_get_timer_value(p_timing, t1));
  test_expect_u32(48, timing_get_timer_value(p_timing, t2));

  test_expect_u32(200, timing_get_timer_value(p_timing, t3));
  (void) timing_set_timer_value(p_timing, t3, 190);
  test_expect_u32(190, timing_get_timer_value(p_timing, t3));
  (void) timing_adjust_timer_value(p_timing, NULL, t3, -5);
  test_expect_u32(185, timing_get_timer_value(p_timing, t3));

  countdown -= 98;
  countdown = timing_advance_time(p_timing, countdown);
  test_expect_u32(1, g_timing_test_timer_hits_basic);
  test_expect_u32(0, timing_get_timer_value(p_timing, t1));
  test_expect_u32((uint32_t) -50,
                  (uint32_t) timing_get_timer_value(p_timing, t2));

  timing_destroy(p_timing);
}

static void
timing_test_timer_fired_multi(void* p) {
  int64_t new_timer_value;

  struct timing_struct* p_timing = (struct timing_struct*) p;

  if (g_timing_test_timer_hits_multi == 0) {
    test_expect_u32(0, timing_get_timer_value(p_timing, 0));
    test_expect_u32(200, timing_get_timer_value(p_timing, 1));
    test_expect_u32(50, timing_get_timer_value(p_timing, 2));

    (void) timing_set_firing(p_timing, 2, 1);
    (void) timing_adjust_timer_value(p_timing, &new_timer_value, 0, 30);
    test_expect_u32(30, new_timer_value);
    (void) timing_start_timer_with_value(p_timing, 1, 20);
  } else if (g_timing_test_timer_hits_multi == 1) {
    test_expect_u32(10, timing_get_timer_value(p_timing, 0));
    test_expect_u32(0, timing_get_timer_value(p_timing, 1));
    test_expect_u32(30, timing_get_timer_value(p_timing, 2));
    (void) timing_stop_timer(p_timing, 1);
  } else if (g_timing_test_timer_hits_multi == 2) {
    test_expect_u32(0, timing_get_timer_value(p_timing, 0));
    test_expect_u32(0, timing_get_timer_value(p_timing, 1));
    test_expect_u32(20, timing_get_timer_value(p_timing, 2));
    (void) timing_stop_timer(p_timing, 0);
  } else if (g_timing_test_timer_hits_multi == 3) {
    test_expect_u32(0, timing_get_timer_value(p_timing, 0));
    test_expect_u32(0, timing_get_timer_value(p_timing, 1));
    test_expect_u32(0, timing_get_timer_value(p_timing, 2));
    (void) timing_set_timer_value(p_timing, 2, 500);
  }

  g_timing_test_timer_hits_multi++;
}

static void
timing_test_multi_expiry() {
  /* Tests a timing advance that covers multiple expiries including one
   * initiated within a callback.
   */
  uint64_t countdown;

  struct timing_struct* p_timing = timing_create(1);

  uint32_t t1 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_multi,
                                      p_timing);
  uint32_t t2 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_multi,
                                      p_timing);
  uint32_t t3 = timing_register_timer(p_timing,
                                      timing_test_timer_fired_multi,
                                      p_timing);

  (void) timing_start_timer_with_value(p_timing, t1, 50);
  (void) timing_start_timer_with_value(p_timing, t2, 200);
  (void) timing_stop_timer(p_timing, t2);
  (void) timing_start_timer_with_value(p_timing, t3, 100);
  (void) timing_set_firing(p_timing, t3, 0);

  countdown = timing_get_countdown(p_timing);
  test_expect_u32(50, countdown);
  countdown -= 200;

  countdown = timing_advance_time(p_timing, countdown);
  test_expect_u32(400, countdown);
  test_expect_u32(4, g_timing_test_timer_hits_multi);

  test_expect_u32(200, timing_get_total_timer_ticks(p_timing));

  timing_destroy(p_timing);
}

void
timing_test() {
  timing_test_counting();
  timing_test_basics();
  timing_test_multi_expiry();
}
