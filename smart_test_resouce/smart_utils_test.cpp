#include "../smart_utils/smart_utils_log.hpp"
#include "../smart_utils/smart_utils_thread_loop.hpp"

#include "../smart_android/smart_android_config.hpp"

using smart::utils::log::Log;
using smart::utils::log::LogType;
using smart::utils::thread::loop::ThreadLoop;

using smart::android::config::Config;

void TestThreadPool()
{
    Jint TestThreadLoopNumEnter;
    constexpr Jint TEST_THREAD_DEFAULT_SIZE = 1024;
    constexpr Jchar TEST_THREAD_DEFAULT_TEXT[] = "fuckman";

    ThreadLoop::Instance().Init(TEST_THREAD_DEFAULT_SIZE);

    while (true)
    {
        Log::Instance().Print<LogType::INFO>("Please enter a character:");
        scanf("%d", &TestThreadLoopNumEnter);
        if ((TestThreadLoopNumEnter % 2) != 0)
            ThreadLoop::Instance().Insert(const_cast<Jchar *>(TEST_THREAD_DEFAULT_TEXT), [](Jint id, void *p) {
                Log::Instance().Print<LogType::INFO>(
                        "Test thread loop, id: %d, parameter: %s",
                        id,
                        reinterpret_cast<Jchar *>(p)
                );
            }).Process();
        else
            ThreadLoop::Instance().Insert(const_cast<Jchar *>(TEST_THREAD_DEFAULT_TEXT), [](Jint id, void *p) {
                constexpr Jchar TEST_THREAD_MSG[] = "woqu";

                Log::Instance().Print<LogType::DEBUG>(
                        "Test thread loop, id: %d, parameter: %s",
                        id,
                        reinterpret_cast<Jchar *>(p)
                );

                for (Jint i = 0; i < 20; ++i)
                {
                    ThreadLoop::Instance().Insert(const_cast<Jchar *>(TEST_THREAD_MSG), [](Jint id, void *p) {
                        _sleep(10000);

                        Log::Instance().Print<LogType::INFO>(
                                "Test thread loop, id: %d, parameter: %s",
                                id,
                                reinterpret_cast<Jchar *>(p)
                        );
                        Log::Instance().Print<LogType::DEBUG>(
                                "Test thread loop, id: %d, parameter: %s",
                                id,
                                reinterpret_cast<Jchar *>(p)
                        );
                        Log::Instance().Print<LogType::ERROR>(
                                "Test thread loop, id: %d, parameter: %s",
                                id,
                                reinterpret_cast<Jchar *>(p)
                        );
                    }).Process();
                }

                _sleep(20000);
            }).Process();
    }
}

void TestConfigureFileToRead()
{
    constexpr Jint TEST_BUF_SIZE = 200;
    constexpr Jchar TEST_CONFIG_PATH[] = R"(D:\work\ClionProjects\SmartTools\smart_test_resouce\test_config_file)";

    Jint ret = 0;
    Jchar enter[TEST_BUF_SIZE];
    FILE *configFile = nullptr;

    Config::Instance().Reset();
    if (configFile = fopen(TEST_CONFIG_PATH, "rb");configFile == nullptr)
        return;

    while (true)
    {
        ret = fread(enter, 1, sizeof(enter), configFile);
        Config::Instance().WriteBuffer(enter, ret);
        if (ret < sizeof(enter))
            break;
    }

    fclose(configFile);
    if (Config::Instance().Process())
        Log::Instance().Print<LogType::INFO>("read configure file is success");
    else
        Log::Instance().Print<LogType::INFO>("read configure file is fail");

    Config::Instance().Test();
}

using TestItemThing = void (*)();

typedef struct
{
    const Jchar *itemName;
    TestItemThing itemThing;
} TestItem;

constexpr TestItem Items[] = {
        {"Test thread loop",            TestThreadPool},
        {"Test configure file to read", TestConfigureFileToRead},
};

int main()
{
    Jint i = 0;
    Jint enter = 0;

    for (i = 0; i < (sizeof(Items) / sizeof(TestItem)); ++i)
        Log::Instance().Print<LogType::INFO>("%d. %s", (i + 1), Items[i].itemName);

    while (true)
    {
        Log::Instance().Print<LogType::INFO>("Please enter item id:");
        scanf("%d", &enter);

        for (i = 0; i < (sizeof(Items) / sizeof(TestItem)); ++i)
        {
            if ((i + 1) == enter)
                Items[i].itemThing();
        }
    }

    return 0;
}
