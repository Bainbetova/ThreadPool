#pragma once

using namespace std;

class Task {
    function<void()> void_func;
    function<any()> any_func;
    any any_func_result;
    bool is_void;
public:
    // компил€ци€ только одной ветки услови€
    template <typename FuncRetType, typename ...Args, typename ...FuncTypes>
    Task(FuncRetType(*func)(FuncTypes...), Args&&... args) :
        is_void{ is_void_v<FuncRetType> } {

        if constexpr (is_void_v<FuncRetType>) {
            void_func = bind(func, args...);
            any_func = []()->int { return 0; };
        }
        else {
            void_func = []()->void {};
            any_func = bind(func, args...);
        }
    }

    void operator() () {
        void_func();
        any_func_result = any_func();
    }

    bool has_result() {
        return !is_void;
    }

    any get_result() const {
        assert(!is_void);
        assert(any_func_result.has_value());
        return any_func_result;
    }
};

enum class TaskStatus {
    in_q,
    completed
};

struct TaskInfo {
    TaskStatus status = TaskStatus::in_q;
    any result;
};
