#include <iostream>

class access_private{
    void print(){
        std::cout << "access level: private" << "\n";
    }
};

struct access_public{
    void print(){
        std::cout << "access level: public" << "\n";
    }
};

template <typename T>
T typename_max_num(T a, T b){
    return a > b ? a : b;
}

template <class T>
T class_max_num(T a, T b){
    return a > b ? a : b;
}

// error, don't support struct 
// template <struct T>
// T struct_max_num(T a, T b){
//     return a > b ? a : b;
// }

int main(){
    access_private classobject;
    //classobject.print(); //can't access
    access_public structobject;
    structobject.print();
    std::cout << typename_max_num(1, 2) << "\n";
    std::cout << class_max_num(1, 2) << "\n";
    return 0;
}