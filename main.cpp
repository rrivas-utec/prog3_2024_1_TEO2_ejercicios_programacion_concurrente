#include <iostream>
#include <thread>
#include <vector>
#include <barrier>
#include <mutex>

void task_1() {
    std:: cout << "Hilo  #1\n";
}

void task_2(int id) {
    std:: cout << "Hilo " << id << "\n";
}

void task_3(int& total, int a, int b) {
    total += a + b;
}

int task_4(int a, int b) {
    return a + b;
}

template <typename T>
void task_5(T& total, T a, T b) {
    total += a + b;
}

void ejemplo_hilos() {
    std::thread t1(task_1);
    t1.join();

    std::thread t2(task_2, 2);
    t1.join();

    int total = 0;
    std::thread t3(task_3, std::ref(total), 20, 30);
    t3.join();
    std::cout << std::thread::hardware_concurrency();
}
void ejemplo_hilos_jthread() {

    int total = 30;
    task_3(total, 20, 30);
    std::cout << total;

    std::jthread t1(task_1);
    t1.join();

    std::jthread t2(task_2, 2);
    t1.join();

    std::jthread t3(task_3, std::ref(total), 20, 30);
    t3.join();
    std::cout << std::thread::hardware_concurrency();

    std::jthread t4([&total]() { task_3(total, 20, 30); } );
    t4.join();
    std::cout << std::thread::hardware_concurrency();

    std::jthread t5([&total]() { task_5(total, 20, 30); } );
    t5.join();

    std::jthread t6(task_5<int>, std::ref(total), 20, 30 );
    t6.join();
}

void coleccion_de_hilos() {
    std::vector<std::jthread> vt;
    size_t n = std::thread::hardware_concurrency();
    int total = 0;

    for (int i = 0; i < n; ++i) {
        vt.emplace_back([&total]() { task_3(total, 1, 2); });
    }

    for (int i = 0; i < n; ++i) {
        vt[i].join();
    }
}

std::mutex mtx;

void increase(int& result, int inc) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    mtx.lock();
    result += inc;
    mtx.unlock();
}

void increase_total(int& total, int inc, int n) {
    std::vector<std::jthread> vt(n);
    for (auto& t: vt) {
        t = std::jthread(increase, std::ref(total), inc);
    }
    for (auto& t: vt) {
        t.join();
    }
}

void ejemplo_1() {
    int m = 50;
    int n = 150;
    std::vector<int> totales(m);
    for (auto& t: totales) {
        increase_total(t, 1, n);
    }

    for (const auto& t: totales) {
        std::cout << t << " ";
    }
}

int main() {
    ejemplo_1();
    return 0;
}
