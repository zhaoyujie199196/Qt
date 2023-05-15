//
// Created by Yujie Zhao on 2023/5/8.
//

#ifndef QSTRINGALGORITHMS_P_H
#define QSTRINGALGORITHMS_P_H

#include "QtCore/qglobal.h"

QT_BEGIN_NAMESPACE

template <typename StringType>
struct QStringAlgorithms {
    typedef typename StringType::value_type Char;
    typedef typename StringType::size_type size_type;
    typedef typename std::remove_cv<StringType>::type NakedStringType; //naked 裸体的
    static const bool isConst = std::is_const<StringType>::value;


    static inline StringType trimmed_helper_inplace(const NakedStringType &, const Char *, const Char *)
    {
        Q_ASSERT(false);
        return StringType();
    }

    static inline void trimmed_helper_positions(const Char *&begin, const Char *&end)
    {
        while (begin < end && isSpace(end[-1])) {
            --end;
        }
        while (begin < end && isSpace(*begin)) {
            begin++;
        }
    }

    //如果是const &, && ,这里的str是什么类型？
    static inline StringType trimmed_helper(StringType &str) {
        Q_ASSERT(false);
        return StringType();
//        Q_ASSERT(false);
//        const Char *begin = str.cbegin();
//        const Char *end = str.cend();
//        trimmed_helper_positions(begin, end);
//        Q_ASSERT(false);
//
//        if (begin == str.cbegin() && end == str.cend()) {
//            return str;
//        }
//        if (!isConst && str.isDetached()) {  //TODO 这里啥意思
//            return trimmed_helper_inplace(str, begin, end);
//        }
//        return StringType(begin, end - begin);
    }

    static inline StringType simplified_helper(StringType &str) {
        Q_ASSERT(false);
        return str;
        //zhaoyujie TODO 这里啥意思
//        if (str.isEmpty()) {
//            return str;
//        }
//        const Char *src = str.cbegin();
//        const Char *end = str.cend();
//        NakedStringType result = isConst || !str.isDetected() ?
//                                            StringType(str.size(), Qt::Uninitialized) :
//                                            std::move(str);
//        Char *dst = const_cast<Char *>(result.cbegin());
//        Char *ptr = dst;
//        bool unmodified = true;
//        forever {
//            while (src != end() && isSpace(*src)) {
//                ++src;
//            }
//            while (src != end && !isSpace(*src)) {
//                *ptr++ = *src++;
//            }
//            if (src == end) {
//                break;
//            }
//            if (*src != QChar::Space) {
//                unmodified = false;
//            }
//            *ptr++ = QChar::Space;
//        }
//
//        if (ptr != dst && ptr[-1] == QChar::Space) {
//            --ptr;
//        }
//        int newlen = ptr - dst;
//        if (isConst && newlen == str.size() && unmodified) {
//            return str;
//        }
//        result.resize(newlen);
//        return result;
    }
};

QT_END_NAMESPACE

#endif //QSTRINGALGORITHMS_P_H
