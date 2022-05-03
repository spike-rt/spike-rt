#include <kernel.h>

#include <unity_fixture.h>

#include <pbsys/user_program.h>


static void RunAllTests(void)
{
	RUN_TEST_GROUP(Led);
	RUN_TEST_GROUP(Motor);
	RUN_TEST_GROUP(PUPDevice);
	RUN_TEST_GROUP(ForceSensor);
	RUN_TEST_GROUP(UltrasonicSensor);
}

void TestMainTask(intptr_t exinf)
{
 // pbsys_processをユーザプログラム実行状態に遷移させる．
 // pbsys_user_program_prepare(NULL);
 int argc = 2;
 const char **argv = {"asp3-primehub test", "-v"};

  UnityMain(argc, argv, RunAllTests);
}
