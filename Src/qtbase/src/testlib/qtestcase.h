//
// Created by Yujie Zhao on 2023/2/7.
//

#ifndef QTESTCASE_H
#define QTESTCASE_H

#include <QtGlobal>
#include <type_traits>
#include <QtTest/qtestglobal.h>
#include <QtTest/qtestdata.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QStringView>

QT_BEGIN_NAMESPACE

class QObject;

#define QVERIFY(statement) \
do {                       \
    if (!QTest::qVerify(static_cast<bool>(statement), #statement, "", __FILE__, __LINE__)) \
        return;            \
} while (false)

#define QFAIL(message) \
do {                   \
    QTest::qFail(static_cast<const char *>(message), __FILE__, __LINE__); \
    return;            \
} while (false)

#define QVERIFY2(statement, description) \
do {\
    if (statement) {\
        if (!QTest::qVerify(true, #statement, static_cast<const char *>(description), __FILE__, __LINE__))\
            return;\
    } else {\
        if (!QTest::qVerify(false, #statement, static_cast<const char *>(description), __FILE__, __LINE__))\
            return;\
    }\
} while (false)

#define QFETCH(Type, name) \
    Type name = *static_cast<Type *>(QTest::qData(#name, ::qMetaTypeId<typename std::remove_cv<Type>::type>()))

#define QTEST(actual, testElement) \
    do {                           \
        if (!QTest::qTest(actual, testElement, #actual, #testElement, __FILE__, __LINE__)) {\
            return;                \
        }                          \
    } while (false)

class QTestData;
namespace QTest {
    int qExec(QObject *object);

    //添加Test的数据表的列
    void addColumnInternal(int id, const char *name);
    template<class T>
    inline void addColumn(const char *name, T * = nullptr) {
        using QIsSameTConstChar = std::is_same<T, const char *>;
        static_assert(!QIsSameTConstChar::value, "const char* is not allowed as a test data format.");
        addColumnInternal(qMetaTypeId<T>(), name);
    }

    void *qData(const char *tagName, int typeId);

    //从testData中取tagName的数据
    void *fetchData(QTestData *data, const char *tagName, int typeId);

    QTestData &newRow(const char *dataTag);
    QTestData &addRow(const char *format, ...);

    void *qElementData(const char *elementName, int metaTypeId);

    bool qVerify(bool statement, const char *statementStr, const char *description, const char *file, int line);

    //转换成字符串的辅助函数
    template <class T>
    char *toString(T str) {
        return nullptr;
    }
    template <>
    char *toString<const char *>(const char *str) {
        if (!str) {
            char *msg = new char[1];
            msg[0] = '\0';
            return msg;
        }
        else {
            char *msg = new char[strlen(str) + 1];
            return qstrcpy(msg, str);
        }
    }
    template <>
    char *toString<bool>(bool b) {
        return qstrdup(b ? "true" : "false");
    }

    bool compare_string_helper(const char *t1, const char *t2, const char *actual, const char *expected, const char *fle, int line);

    //比较辅助函数
    bool compare_helper(bool success, const char *failMsg, char *val1, char *val2, const char *actual, const char *expected, const char *file, int line);

    bool qCompare(QStringView t1, QStringView t2, const char *actual, const char *expected, const char *file, int line);
//    bool qCompare(QStringView t1, const QLatin1String &t2, const char *actual, const char *expected, const char *file, int line);
//    bool qCompare(const QLatin1String &t1, QStringView t2, const char *actual, const char *expected, const char *file, int line);
    bool qCompare(const QString &t1, const QString &t2, const char *actual, const char *expected, const char *file, int line)
    {
        return qCompare(QStringView(t1), QStringView(t2), actual, expected, file, line);
    }

    template <typename T>
    inline bool qCompare(const T &t1, const T &t2, const char *actual, const char *expected, const char *file, int line) {
        return compare_helper(t1 == t2, "Compared values are not the same", toString(t1), toString(t2), actual, expected, file, line);
    }

    template <typename T1, typename T2>
    inline bool qCompare(const T1 &t1, const T2 &t2, const char *actual, const char *expected, const char *file, int line)
    {
        return compare_helper(t1 == t2, "Compared values are not the same", toString(t1), toString(t2), actual, expected, file, line);
    }

    inline bool qCompare(double const &t1, float const &t2, const char *actual, const char *expected, const char *file, int line)
    {
        return qCompare(qreal(t1), qreal(t2), actual, expected, file, line);
    }

    inline bool qCompare(float const &t1, double const &t2, const char *actual, const char *expected, const char *file, int line)
    {
        return qCompare(qreal(t1), qreal(t2), actual, expected, file, line);
    }

    //zhaoyujie TODO
//    template <typename T>
//    inline bool qCompare(const T *t1, const T *t2, const char *actual, const char *expected, const char *file, int line)
//    {
//        return compare_ptr_helper(t1, t2, actual, expected, file, line);
//    }
//
//    template <typename T>
//    inline bool qCompare(T *t1, T *t2, const char *actual, const char *expected, const char *file, int line)
//    {
//        return compare_ptr_helper(t1, t2, actual, expected, file, line);
//    }
//
//    template <typename T>
//    inline bool qCompare(T *t1, std::nullptr_t, const char *actual, const char *expected, const char *file, int line)
//    {
//        return compare_ptr_helper(t1, nullptr, actual, file, line);
//    }
//
//    template <typename T>
//    inline bool qCompare(std::nullptr_t, T *t2, const char *actual, const char *expected, const char *file, int line)
//    {
//        return compare_ptr_helper(nullptr, t2, actual, expected, file, line);
//    }
//
//    template <typename T1, typename T2>
//    inline bool qCompare(const T1 *t1, const T2 *t2, const char *actual, const char *expected, const char *file, int line)
//    {
//        return compare_ptr_helper(t1, static_cast<const T1 *>(t2), actual, expected, file, line);
//    }
//
//    template <typename T1, typename T2>
//    inline bool qCompare(T1 *t1, T2 *t2, const char *actual, const char *expected, const char *file, int line)
//    {
//        return compare_ptr_helper(const_cast<const T1 *>(t1), static_cast<const T1 *>(const_cast<const T2 *>(t2)), actual, expected, file, line);
//    }
//
//    inline bool compare_ptr_helper(const volatile void *t1, const volatile void *t2, const char *actual, const char *expected, const char *file, int line)
//    {
//        return compare_helper(t1 == t2, "Compare pointers are not the same", toString(t1), toString(t2), actual, expected, file, line);
//    }

    inline bool qCompare(const char *t1, const char *t2, const char *actual, const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }
    inline bool qCompare(char *t1, char *t2, const char *actual, const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }
    inline bool qCompare(char *t1, const char *t2, const char *actual, const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }
    inline bool qCompare(const char *t1, char *t2, const char *actual, const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }

    template <typename T>
    inline bool qTest(const T &actual, const char *elementName, const char *actualStr, const char *expected, const char *file, int line)
    {
        return qCompare(actual, *static_cast<const T *>(QTest::qElementData(elementName, qMetaTypeId<T>())), actualStr, expected, file, line);
    }

#define QCOMPARE(actual, expected) \
    QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__)
}


QT_END_NAMESPACE

#endif //QTESTCASE_H
