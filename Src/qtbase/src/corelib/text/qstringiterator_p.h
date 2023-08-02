//
// Created by Yujie Zhao on 2023/7/25.
//

#ifndef QSTRINGITERATOR_P_H
#define QSTRINGITERATOR_P_H

#include <QtCore/private/qglobal_p.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QStringIterator {
    QString::const_iterator i;
    QString::const_iterator pos;
    QString::const_iterator e;

    static_assert((std::is_same<QString::const_iterator, const QChar *>::value));

public:
    explicit QStringIterator(QStringView string, qsizetype idx = 0)
        : i (string.begin())
        , pos(i + idx)
        , e(string.end()) {
    }

    inline explicit QStringIterator(const QChar *begin, const QChar *end)
        : i(begin)
        , pos(begin)
        , e(end) {
    }

    inline explicit QStringIterator(const QChar *begin, int idx, const QChar *end)
        : i(begin)
        , pos(begin + idx)
        , e(end) {
    }

    inline QString::const_iterator position() const {
        return pos;
    }

    inline int index() const {
        return int(pos - i);
    }

    inline void setPosition(QString::const_iterator position) {
        Q_ASSERT(i <= position && position <= e);
        pos = position;
    }

    inline bool hasNext() const {
        return pos < e;
    }

    inline void advance() {
        Q_ASSERT(hasNext());
        if (Q_UNLIKELY((pos++)->isHighSurrogate())){
            if (Q_LIKELY(pos != e && pos->isLowSurrogate())) {
                ++pos;
            }
        }
    }

    //往前进位
    inline void advanceUnchecked() {
        Q_ASSERT(hasNext());
        if (Q_UNLIKELY((pos++)->isHighSurrogate())) {
            Q_ASSERT(pos < e && pos->isLowSurrogate());
            ++pos;
        }
    }

    //没有进位
    inline char32_t peekNextUnchecked() const {
        Q_ASSERT(hasNext());
        if (Q_UNLIKELY(pos->isHighSurrogate())) {
            Q_ASSERT(pos + 1 < e && pos[1].isLowSurrogate());
            return QChar::surrogateToUcs4(pos[0], pos[1]);
        }
        return pos->unicode();
    }

    //没有进位
    inline char32_t peekNext(char32_t invalidAs = QChar::ReplacementCharacter) const {
        Q_ASSERT(hasNext());
        if (Q_UNLIKELY(pos->isSurrogate())) {
            if (Q_LIKELY(pos->isHighSurrogate())) {
                const QChar *low = pos + 1;
                if (Q_LIKELY(low != e && low->isLowSurrogate())) {
                    return QChar::surrogateToUcs4(*pos, *low);
                }
            }
            return invalidAs;
        }
        return pos->unicode();
    }

    //往前进位
    inline char32_t nextUnchecked() {
        Q_ASSERT(hasNext());

        const QChar cur = *pos++;
        if (Q_UNLIKELY(cur.isHighSurrogate())) {
            Q_ASSERT(pos < e && e->isLowSurrogate());
            return QChar::surrogateToUcs4(cur, *pos++);
        }
        return cur.unicode();
    }

    inline char32_t next(char32_t invalidAs = QChar::ReplacementCharacter) {
        Q_ASSERT(hasNext());

        const QChar uc = *pos++;
        if (Q_UNLIKELY(uc.isSurrogate())) {
            if (Q_LIKELY(uc.isHighSurrogate() && pos < e && pos->isLowSurrogate())) {
                return QChar::surrogateToUcs4(uc, *pos++);
            }
            return invalidAs;
        }
        return uc.unicode();
    }

    inline bool hasPrevious() const {
        return pos > i;
    }

    //后退
    inline void recede() {
        Q_ASSERT(hasPrevious());
        if (Q_UNLIKELY((--pos)->isLowSurrogate())) {
            const QChar *high = pos - 1;
            if (Q_LIKELY(high != i - 1 && high->isHighSurrogate())) {
                --pos;
            }
        }
    }

    inline void recedeUnchecked() {
        Q_ASSERT(hasPrevious());
        if (Q_UNLIKELY((--pos)->isLowSurrogate())) {
            Q_ASSERT(pos > i && pos[-1].isHighSurrogate());
            --pos;
        }
    }

    inline char32_t peekPreviousUnchecked() const {
        Q_ASSERT(hasPrevious());
        if (Q_UNLIKELY(pos[-1].isLowSurrogate())) {
            Q_ASSERT(pos > i + 1 && pos[-2].isHighSurrogate());
            return QChar::surrogateToUcs4(pos[-2], pos[-1]);
        }
        return pos[-1].unicode();
    }

    inline char32_t peekPrevious(char32_t invalidAs = QChar::ReplacementCharacter) {
        Q_ASSERT(hasPrevious());

        if (Q_UNLIKELY(pos[-1].isSurrogate())) {
            if (Q_LIKELY(pos[-1].isLowSurrogate())) {
                const QChar *high = pos - 2;
                if (Q_LIKELY(high != i - 1 && high->isHighSurrogate())) {
                    return QChar::surrogateToUcs4(*high, pos[-1]);
                }
            }
            return invalidAs;
        }
        return pos[-1].unicode();
    }

    inline char32_t previousUnchecked() {
        Q_ASSERT(hasPrevious());

        const QChar cur = *--pos;
        if (Q_UNLIKELY(cur.isLowSurrogate())) {
            Q_ASSERT(pos > i && pos[-1].isHighSurrogate());
            return QChar::surrogateToUcs4(*--pos, cur);
        }
        return cur.unicode();
    }

    inline char32_t previous(char32_t invalidAs = QChar::ReplacementCharacter) {
        Q_ASSERT(hasPrevious());
        const QChar cur = *--pos;
        if (Q_UNLIKELY(cur.isLowSurrogate())) {
            Q_ASSERT(pos > i && pos[-2].isHighSurrogate());
            return QChar::surrogateToUcs4(*--pos, cur);
        }
        return cur.unicode();
    }

};

QT_END_NAMESPACE

#endif //QSTRINGITERATOR_P_H
