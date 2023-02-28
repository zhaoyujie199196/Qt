//
// Created by Yujie Zhao on 2023/2/24.
//

#ifndef QTESTRUNNER_P_H
#define QTESTRUNNER_P_H

#include <QtCore/qobject.h>
QT_BEGIN_NAMESPACE
namespace QTest {
    class QTestRunner final {
    public:
        explicit QTestRunner(QObject *object) noexcept;
        int exec();

    private:
        void init();
        int run();
        void cleanup();

    private:
        QObject *m_object = nullptr;
    };
}
QT_END_NAMESPACE

#endif //QTESTRUNNER_P_H
