//  Copyright (c) 2017-2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/local/future.hpp>
#include <hpx/local/init.hpp>
#include <hpx/local/thread.hpp>
#include <hpx/modules/testing.hpp>

#include <chrono>
#include <stdexcept>
#include <vector>

int make_int_slowly()
{
    hpx::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 42;
}

hpx::future<int> make_future()
{
    hpx::lcos::local::packaged_task<int()> task(make_int_slowly);
    return task.get_future();
}

void test_wait_some()
{
    {
        std::vector<hpx::future<int>> future_array;
        future_array.push_back(make_future());
        future_array.push_back(make_future());

        hpx::wait_some_nothrow(1, future_array);

        int count = 0;
        for (auto& f : future_array)
        {
            if (f.is_ready())
            {
                ++count;
            }
        }
        HPX_TEST_NEQ(count, 0);
    }
    {
        auto f1 = make_future();
        auto f2 = make_future();

        hpx::wait_some_nothrow(1, f1, f2);

        HPX_TEST(f1.is_ready() || f2.is_ready());
    }
    {
        std::vector<hpx::future<int>> future_array;
        future_array.push_back(make_future());
        future_array.push_back(
            hpx::make_exceptional_future<int>(std::runtime_error("")));

        bool caught_exception = false;
        try
        {
            hpx::wait_some_nothrow(1, future_array);

            int count = 0;
            for (auto& f : future_array)
            {
                if (f.is_ready())
                {
                    ++count;
                }
            }
            HPX_TEST_NEQ(count, 0);
        }
        catch (std::runtime_error const&)
        {
            HPX_TEST(false);
            caught_exception = true;
        }
        catch (...)
        {
            HPX_TEST(false);
        }
        HPX_TEST(!caught_exception);
    }
    {
        std::vector<hpx::future<int>> future_array;
        future_array.push_back(make_future());
        future_array.push_back(
            hpx::make_exceptional_future<int>(std::runtime_error("")));

        bool caught_exception = false;
        try
        {
            hpx::wait_some(1, future_array);
            HPX_TEST(false);
        }
        catch (std::runtime_error const&)
        {
            caught_exception = true;
        }
        catch (...)
        {
            HPX_TEST(false);
        }
        HPX_TEST(caught_exception);
    }
    {
        auto f1 = make_future();
        auto f2 = hpx::make_exceptional_future<int>(std::runtime_error(""));

        bool caught_exception = false;
        try
        {
            hpx::wait_some(1, f1, f2);
            HPX_TEST(false);
        }
        catch (std::runtime_error const&)
        {
            caught_exception = true;
        }
        catch (...)
        {
            HPX_TEST(false);
        }
        HPX_TEST(caught_exception);
    }
}

void test_wait_some_n()
{
    {
        std::vector<hpx::future<int>> future_array;
        future_array.push_back(make_future());
        future_array.push_back(make_future());

        hpx::wait_some_n_nothrow(1, future_array.begin(), future_array.size());

        int count = 0;
        for (auto& f : future_array)
        {
            if (f.is_ready())
            {
                ++count;
            }
        }
        HPX_TEST_NEQ(count, 0);
    }
    {
        std::vector<hpx::future<int>> future_array;
        future_array.push_back(make_future());
        future_array.push_back(
            hpx::make_exceptional_future<int>(std::runtime_error("")));

        bool caught_exception = false;
        try
        {
            hpx::wait_some_n_nothrow(1, future_array.begin(), future_array.size());

            int count = 0;
            for (auto& f : future_array)
            {
                if (f.is_ready())
                {
                    ++count;
                }
            }
            HPX_TEST_NEQ(count, 0);
        }
        catch (std::runtime_error const&)
        {
            caught_exception = true;
        }
        catch (...)
        {
            HPX_TEST(false);
        }
        HPX_TEST(!caught_exception);
    }
    {
        std::vector<hpx::future<int>> future_array;
        future_array.push_back(make_future());
        future_array.push_back(
            hpx::make_exceptional_future<int>(std::runtime_error("")));

        bool caught_exception = false;
        try
        {
            hpx::wait_some_n(1, future_array.begin(), future_array.size());
            HPX_TEST(false);
        }
        catch (std::runtime_error const&)
        {
            caught_exception = true;
        }
        catch (...)
        {
            HPX_TEST(false);
        }
        HPX_TEST(caught_exception);
    }
}

int hpx_main()
{
    test_wait_some();
    test_wait_some_n();
    return hpx::local::finalize();
}

int main(int argc, char* argv[])
{
    HPX_TEST_EQ(hpx::local::init(hpx_main, argc, argv), 0);
    return hpx::util::report_errors();
}
