#include <iostream>

int main() {
    float x =0.0;
    std::cout << "Hello World! \n";
    std::cout << __DATE__ ,"\n";
    std::cout <<__TIME__ , "\n";
    std::cout <<"Input x:\n";
    std::cin >> x;
    x=x+1.5; //sum x+1.5
    std::cout<<"Result: \n";
    std::cout<<x;
    return 0;
}
