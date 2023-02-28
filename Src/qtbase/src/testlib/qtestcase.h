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

QT_BEGIN_NAMESPACE

class QObject;

#define QVERIFY(statement) \
do {                       \
    if (!QTest::qVerify(static_cast<bool>(statement), #statement, "", __FILE__, __LINE__)) \
        return;            \
} while (false)

#define QFETCH(Type, name) \
    Type name = *static_cast<Type *>(QTest::qData(#name, ::qMetaTypeId<typename std::remove_cv<Type>::type>()))

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

    //比较辅助函数
    bool compare_helper(bool success, const char *failMsg, char *val1, char *val2, const char *actual, const char *expected, const char *file, int line);

    template <typename T>
    inline bool qCompare(const T &t1, const T &t2, const char *actual, const char *expected, const char *file, int line) {
        return compare_helper(t1 == t2, "Compared values are not the same", toString(t1), toString(t2), actual, expected, file, line);
    }

    template <typename T1, typename T2>
    inline bool qCompare(const T1 &t1, const T2 &t2, const char *actual, const char *expected, const char *file, int line)
    {
        return compare_helper(t1 == t2, "Compared values are not the same", toString(t1), toString(t2), actual, expected, file, line);
    }

#define QCOMPARE(actual, expected) \
    QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__)
}


QT_END_NAMESPACE

#endif //QTESTCASE_H
