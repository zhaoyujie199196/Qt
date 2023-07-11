//
// Created by Yujie Zhao on 2023/6/7.
//

#ifndef QSTRINGCONVERTER_H
#define QSTRINGCONVERTER_H

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

class QStringConverterBase
{
public:
    enum class Flag {
        Default = 0,
        Stateless = 0x1,
        ConvertInvalidToNull = 0x2,
        WriteBom = 0x4,
        ConvertInitialBom = 0x8
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    struct State {
        constexpr State(Flags f = Flag::Default)
            : flags(f), state_data{0, 0, 0, 0}
        {}
        ~State() { clear(); }
        State(State &&other)
            : flags(other.flags)
            , remainingChars(other.remainingChars)
            , invalidChars(other.invalidChars)
            , d{other.d[0], other.d[1]}
            , clearFn(other.clearFn)
        {
            other.clearFn = nullptr;
        }

        State &operator=(State &&other)
        {
            clear();
            flags = other.flags;
            remainingChars = other.remainingChars;
            invalidChars = other.invalidChars;
            d[0] = other.d[0];
            d[1] = other.d[1];
            clearFn = other.clearFn;
            other.clearFn = nullptr;
            return *this;
        }
        void clear();

        Flags flags;
        int internalState = 0;
        qsizetype remainingChars = 0;
        qsizetype invalidChars = 0;

        union {
            uint state_data[4];
            void *d[2];
        };
        using ClearDataFn = void(*)(State *);
        ClearDataFn clearFn = nullptr;

    private:
        Q_DISABLE_COPY(State)
    };
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QStringConverterBase::Flags);

class QStringConverter : public QStringConverterBase
{
public:
    enum Encoding {
        Utf8,
        Utf16,
        Utf16LE,
        Utf16BE,
        Utf32,
        Utf32LE,
        Utf32BE,
        Latin1,
        System,
        LastEncoding = System
    };
};

QT_END_NAMESPACE

#endif //QSTRINGCONVERTER_H
