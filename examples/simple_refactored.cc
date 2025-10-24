#include <iostream>
using namespace std;

int bad_global = -1;

void fn_print() {
    cout << "hallo" << endl;
    int variable = 0;
}

int main() {
    fn_print();
    fn_print();
    {
        int var_a /* a variable */ = 0, var_b /* character buffer */ = 1;
        var_b = var_a;
    }
}
