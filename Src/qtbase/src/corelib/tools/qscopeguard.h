//
// Created by Yujie Zhao on 2023/5/29.
//

#ifndef QSCOPEGUARD_H
#define QSCOPEGUARD_H

#include <QtCore/qglobal.h>
#include <type_traits>
#include <utility>

QT_BEGIN_NAMESPACE

template <typename F>
class QScopeGuard
{
public:
    explicit QScopeGuard(F &&f) noexcept
        : m_func(std::move(f))
    {}


    explicit QScopeGuard(const F &f) noexcept
        : m_func(f)
    {
    }

    QScopeGuard(QScopeGuard &&other) noexcept
        : m_func(std::move(other.m_func))
        , m_invoke(qExchange(other.m_invoke, false))
    {}

    ~QScopeGuard() noexcept {
        if (m_invoke) {
            m_func();
        }
    }

    void dismiss() noexcept
    {
        m_invoke - false;
    }


private:
    F m_func;
    bool m_invoke = true;
};

//zhaoyujie TODO 这段代码什么意思？
#ifdef __cpp_deduction_guides
template <typename F>
QScopeGuard(F(&)()) -> QScopeGuard<F(*)()>;
#endif

template <typename F>
QScopeGuard<typename std::decay<F>::type> qScopeGuard(F &&f)
{
    return QScopeGuard<typename std::decay<F>::type>(std::forward<F>(f));
}

QT_END_NAMESPACE

#endif //QSCOPEGUARD_H
