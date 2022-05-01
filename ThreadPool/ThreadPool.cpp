#include <any>
#include <iostream>
#include <queue>
#include <mutex>
#include <future>
#include <unordered_set>
#include <assert.h>
#include "Task.h"
#include "thread_pool.h"
#include "Functions.h"

using namespace std;

int main() {
    thread_pool t(1);
    int c;
    t.add_task(int_sum, 2, 3); // id = 0
    t.add_task(void_sum, ref(c), 4, 6); // id = 1
    t.add_task(void_without_argument); // id = 2

    {
        // variant 1
        int res;
        t.wait_result(0, res);
        cout << res << endl;

        // variant 2
        cout << any_cast<int>(t.wait_result(0)) << endl;
    }

    t.wait(1);
    cout << c << endl;

    t.wait_all(); // waiting for task with id 2

    return 0;
}