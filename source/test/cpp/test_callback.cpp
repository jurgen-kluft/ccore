#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_callback.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(callback)
{
	UNITTEST_FIXTURE(types)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

        struct Dog
        {
            Dog()
                : m_volume(0)
            {
            }
            int  m_volume;
            void Bark(int volume) { m_volume = volume; }
            void Eat(int food, int amount) { m_volume = food + amount; }
        };

        struct Cat
        {
            Cat()
                : m_volume(0)
            {
            }
            int  m_volume;
            void Meow(int volume) { m_volume = volume; }
            void MeowLoudly(int volume) { m_volume = volume * 2; }
            void Eat(int food, int amount) { m_volume = food + amount; }
        };

        Dog spot, rover; // We have two dogs
        Cat felix;       // and one cat.

        int  volume = 0;
        void Func(int a) { volume = a; }
        void Feed(int food, int amount) { volume = food + amount; }

        UNITTEST_TEST(callback)
        {
            callback_t<void, int> speak;

            speak.Reset(&spot, &Dog::Bark);
            speak(50); // Spot barks loudly.
            CHECK_EQUAL(50, spot.m_volume);

            speak.Reset(&rover, &Dog::Bark);
            speak(60); // Rovers lets out a mighty bark.
            CHECK_EQUAL(60, rover.m_volume);

            speak.Reset(&felix, &Cat::Meow);
            speak(30); // Felix meows.
            CHECK_EQUAL(30, felix.m_volume);

            callback_t<void, int> speak2;
            speak2.Reset(&rover, &Dog::Bark);

            CHECK_TRUE(speak2 < speak);

            speak2.Reset(&felix, &Cat::MeowLoudly);

            CHECK_FALSE(speak2 < speak);

            callback_t<void, int, int> feed;
            feed.Reset(&spot, &Dog::Eat);
            feed(50, 10); // Spot eats.
            CHECK_EQUAL(60, spot.m_volume);

            feed.Reset(&rover, &Dog::Eat);
            feed(60, 10); // Rover eats.
            CHECK_EQUAL(70, rover.m_volume);

            feed.Reset(&felix, &Cat::Eat);
            feed(30, 10); // Felix eats.
            CHECK_EQUAL(40, felix.m_volume);
        }
    }
}
UNITTEST_SUITE_END
