#include <QtCore/qglobal.h>
#include <iostream>

void qt_assert(const char *assertion, const char *file, int line) noexcept
{
    std::cout<<"Failed : "<< assertion << " in File : " << file << " At Line "<<line<<std::endl;
    assert(false);
}

void qt_assert_x(const char *where, const char *what, const char *file, int line) noexcept
{
    std::cout<<"Failed : "<< where <<"  " << what << " in File : " << file << " At Line "<<line<<std::endl;
    assert(false);
}

void qt_check_pointer(const char *, int) noexcept
{
    Q_ASSERT(false);
    std::terminate();
}

//从环境中获取环境半两的值
int qEnvironmentVariableIntValue(const char *varName, bool *ok) noexcept
{
    //zhaoyujie TODO
#pragma message ("qEnvironmentVariableIntValue未实现")
    return 0;
}
