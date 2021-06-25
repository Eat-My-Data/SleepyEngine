#include "pch.h"
#include "CppUnitTest.h"
#include "../SleepyEngine/WinMain.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(Tests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Assert::IsTrue( alwaysTrue() );
		}
	};
}
