#include <kernel.h>

#include <unity_fixture.h>

#include <pbsys/user_program.h>


static void RunAllTests(void)
{
	// RUN_TEST_GROUP(Device);
	// RUN_TEST_GROUP(ForceSensor);
	RUN_TEST_GROUP(Led);
	RUN_TEST_GROUP(Motor);
}

void TestMainTask(intptr_t exinf)
{
 // pbsys_processをユーザプログラム実行状態に遷移させる．
 // pbsys_user_program_prepare(NULL);

  UnityMain(0, NULL, RunAllTests);
}
