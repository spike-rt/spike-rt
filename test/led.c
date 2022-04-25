#include <kernel.h>

#include <unity.h>
#include <unity_fixture.h>

#include <pbsys/user_program.h>
#include <pbsys/light.h>
#include <pbio/light.h>

TEST_GROUP(Led);

extern const uint8_t pb_font_5x5[95][5];

TEST_GROUP_RUNNER(Led) {
  RUN_TEST_CASE(Led, Matrix);
}

TEST_SETUP(Led)
{
  // pbsys_user_program_prepare(NULL); // pbsys_processをユーザプログラム実行状態に遷移させる．
}

TEST_TEAR_DOWN(Led)
{
  // pbsys_user_program_unprepare();
}

TEST(Led, Matrix)
{
  TEST_ASSERT_EQUAL(pbio_light_matrix_clear(pbsys_hub_light_matrix), PBIO_SUCCESS);
  TEST_ASSERT_EQUAL(pbio_light_matrix_set_rows(pbsys_hub_light_matrix, pb_font_5x5['a' - 32]), PBIO_SUCCESS);

  // Wait 10 sec
  // dly_tsk(10000000);
}
