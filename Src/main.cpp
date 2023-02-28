#include <iostream>
#include <QChar>

int main(int argc, char **argv) {
    QChar c1(11);
    QChar c2(2);
    bool equal = c1.compare(c2);
    std::cout<<"main test QtBase : QChar compare "<< equal << std::endl;
    return 0;
}