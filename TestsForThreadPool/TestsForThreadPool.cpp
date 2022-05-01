#include "pch.h"
#include "CppUnitTest.h"
#include "..\ThreadPool\Task.h"
#include "..\ThreadPool\thread_pool.h"
#include "..\ThreadPool\Functions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestsForThreadPool
{
	TEST_CLASS(TestsForThreadPool)
	{
	public:

		TEST_METHOD(TestGetSizeThreadPool)
		{
			uint32_t NumThreads = 1;
			thread_pool t(NumThreads);

			auto sizeThreadPool = t.get_size_thread_pool();

			Assert::IsTrue(sizeThreadPool == NumThreads);
		}
		
		TEST_METHOD(TestConctructorWithParametr)
		{
			uint32_t NumThreads = 1;

			thread_pool t(NumThreads);

			Assert::IsTrue(t.get_size_thread_pool() == NumThreads);
		}

		TEST_METHOD(TestAddTask)
		{
			uint32_t NumThreads = 1;
			thread_pool t(NumThreads);
			int x = 2, y = 3;

			auto result = t.add_task(int_sum, x, y);

			Assert::IsTrue(result == 0);
		}

		TEST_METHOD(TestWait)
		{
			uint32_t NumThreads = 1;
			thread_pool t(NumThreads);
			int x = 2, y = 3;

			t.add_task(int_sum, x, y);
			t.wait(0);

			Assert::IsTrue(t.get_status(0) == TaskStatus::completed);
		}

		TEST_METHOD(TestWaitResult)
		{
			uint32_t NumThreads = 1;
			thread_pool t(NumThreads);
			int res, x = 2, y = 3;

			t.add_task(int_sum, x, y);
			t.wait_result(0, res);

			Assert::IsTrue(res == x + y);
		}

		TEST_METHOD(TestWaitAll)
		{
			uint32_t NumThreads = 2;
			thread_pool t(NumThreads);
			int res, x = 2, y = 3;

			t.add_task(int_sum, x, y);
			t.add_task(int_sum, x, y);
			t.wait_all();

			Assert::IsTrue(t.get_status(0) == TaskStatus::completed);
			Assert::IsTrue(t.get_status(1) == TaskStatus::completed);
		}

		TEST_METHOD(TestCalculated)
		{
			uint32_t NumThreads = 1;
			thread_pool t(NumThreads);
			int x = 2, y = 3;

			auto task_id = t.add_task(int_sum, x, y);
			t.wait_all();
			bool result = t.calculated(task_id);

			Assert::IsTrue(result == true);
		}
	};
}
