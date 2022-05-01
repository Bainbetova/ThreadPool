#pragma once
using namespace std;

int int_sum(int a, int b) {
    return a + b;
}

void void_sum(int& c, int a, int b) {
    c = a + b;
}

void void_without_argument() {
    cout << "It's OK!" << endl;
}