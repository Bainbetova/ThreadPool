#pragma once

using namespace std;

class thread_pool {
    vector<thread> threads;

    // очередь с парой задача, номер задачи
    queue<pair<Task, uint64_t>> q;

    mutex q_mtx;
    condition_variable q_cv;

    // Будем создавать ключ как только пришла новая задача и изменять её статус
    // при завершении
    unordered_map<uint64_t, TaskInfo> tasks_info;

    condition_variable tasks_info_cv;
    mutex tasks_info_mtx;

    condition_variable wait_all_cv;

    atomic<bool> quite{ false };
    atomic<uint64_t> last_idx{ 0 };

    // переменная считающая кол-во выполненых задач
    atomic<uint64_t> cnt_completed_tasks{ 0 };

    void run() {
        while (!quite) {
            unique_lock<mutex> lock(q_mtx);
            q_cv.wait(lock, [this]()->bool { return !q.empty() || quite; });

            if (!q.empty() && !quite) {
                pair<Task, uint64_t> task = move(q.front());
                q.pop();
                lock.unlock();

                task.first();

                lock_guard<mutex> lock(tasks_info_mtx);
                if (task.first.has_result()) {
                    tasks_info[task.second].result = task.first.get_result();
                }
                tasks_info[task.second].status = TaskStatus::completed;
                ++cnt_completed_tasks;
            }
            wait_all_cv.notify_all();
            tasks_info_cv.notify_all(); // notify for wait function
        }
    }

public:

    thread_pool(const uint32_t num_threads) {
        threads.reserve(num_threads);
        for (int i = 0; i < num_threads; ++i) {
            threads.emplace_back(&thread_pool::run, this);
        }
    }

    template <typename Func, typename ...Args, typename ...FuncTypes>
    uint64_t add_task(Func(*func)(FuncTypes...), Args&&... args) {

        const uint64_t task_id = last_idx++;

        unique_lock<mutex> lock(tasks_info_mtx);
        tasks_info[task_id] = TaskInfo();
        lock.unlock();

        lock_guard<mutex> q_lock(q_mtx);
        q.emplace(Task(func, forward<Args>(args)...), task_id);
        q_cv.notify_one();
        return task_id;
    }

    void wait(const uint64_t task_id) {
        unique_lock<mutex> lock(tasks_info_mtx);
        tasks_info_cv.wait(lock, [this, task_id]()->bool {
            return task_id < last_idx&& tasks_info[task_id].status == TaskStatus::completed;
            });
    }

    any wait_result(const uint64_t task_id) {
        wait(task_id);
        return tasks_info[task_id].result;
    }

    template<class T>
    void wait_result(const uint64_t task_id, T& value) {
        wait(task_id);
        value = any_cast<T>(tasks_info[task_id].result);
    }

    void wait_all() {
        unique_lock<mutex> lock(tasks_info_mtx);
        wait_all_cv.wait(lock, [this]()->bool { return cnt_completed_tasks == last_idx; });
    }

    bool calculated(const uint64_t task_id) {
        lock_guard<mutex> lock(tasks_info_mtx);
        return task_id < last_idx&& tasks_info[task_id].status == TaskStatus::completed;
    }

    vector <int>::size_type get_size_thread_pool() {
        return threads.size();
    }

    TaskStatus get_status(const uint64_t task_id) {
        return tasks_info[task_id].status;
    }

    ~thread_pool() {
        quite = true;
        q_cv.notify_all();
        for (int i = 0; i < threads.size(); ++i) {
            threads[i].join();
        }
    }
};
