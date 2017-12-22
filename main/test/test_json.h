#pragma once

#ifdef TEST_JSON

#include <smooth/core/Application.h>

class TestApp
        : public smooth::core::Application
{
    public:
        TestApp();
        void init() override;
        void tick() override;
    private:
        std::unique_ptr<char[]> read_file(const std::string& path);
};


#endif // TEST_JSON