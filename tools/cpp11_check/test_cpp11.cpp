#include <iostream>
#include <vector>
using namespace std;
 
bool is_r_value(int &&) { return true; } 
bool is_r_value(const int &) { return false; }
  
void test(int&& i) {
    is_r_value(std::forward<int>(i)); 
}
 
constexpr int GetFive() { return 5; }

int main(int argc, char* argv[]) {
  
    int my_array[5] = {1, 2, 3, 4, 5};
    for (int &x : my_array) {
      x *= 2;
    }
    int some_value[GetFive() + 5];
    const std::vector<int> v(1);
    auto a = v[0];
    decltype(v[0]) b = int(100);   
    auto c = 0;         
    auto d = c;   
    decltype(c) e;  
    decltype((c)) f = e;
    decltype(0) g;  
    return 0;
}
 
 
 
class C {
    int a = 7; 
public:
    C();
};
