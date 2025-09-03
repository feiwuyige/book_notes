#include <iostream>
#include <stdint.h>

class TestSize{
    private:
        static int32_t static_var;
        int32_t var_32;
        int64_t var_64;
    public:
        //编译器处理完类中的全部声明以后才会处理成员函数的定义
        void print(){
            std::cout << "I am TestSize, my size is " << sizeof(TestSize) << "\n";
        }
};

int32_t TestSize::static_var = 0;

int main(){
    TestSize test;
    test.print();
    return 0;
}
