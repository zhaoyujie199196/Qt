//
// Created by Yujie Zhao on 2023/6/7.
//

#ifndef QSTRINGCONVERTER_H
#define QSTRINGCONVERTER_H

#include <QtCore/qstring.h>
#include <optional>

QT_BEGIN_NAMESPACE

class QStringConverterBase
{
public:
    enum class Flag {
        Default = 0,
        Stateless = 0x1,
        ConvertInvalidToNull = 0x2,
        WriteBom = 0x4,  //大端小端
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
        int internalState = 0;  //1 headerDone Bom头已经写入
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

    struct Interface {
        using DecoderFn = QChar *(*)(QChar *out, QByteArrayView in, State *state);
        using LengthFn = qsizetype(*)(qsizetype inLength);
        using EncoderFn = char *(*)(char *out, QStringView in, State *state);
        const char *name = nullptr;
        DecoderFn toUtf16 = nullptr;
        LengthFn toUtf16Len = nullptr;
        EncoderFn fromUtf16 = nullptr;
        LengthFn fromUtf16Len = nullptr;
    };

    QStringConverter() : iface(nullptr)
    {
    }

    QStringConverter(Encoding encoding, Flags f)
        : iface(&encodingInterfaces[int(encoding)]), state(f)
    {}

    QStringConverter(const Interface *i)
        : iface(i)
    {}

    QStringConverter(const char *name, Flags f);

public:
    bool isValid() const { return iface != nullptr; }

    void resetState() {
        state.clear();
    }

    bool hasError() const { return state.invalidChars != 0; }

    const char *name() const {
        return isValid() ? iface->name : nullptr;
    }

    static std::optional<Encoding> encodingForName(const char *name);
    static const char *nameForEncoding(Encoding e);
    static std::optional<Encoding> encodingForData(QByteArrayView data, char16_t expectedFirstCharacter = 0);
    static std::optional<Encoding> encodingForHtml(QByteArrayView data);


private:
    static const Interface encodingInterfaces[Encoding::LastEncoding + 1];

protected:
    const Interface *iface;
    State state;
};

class QStringEncoder : public QStringConverter {
protected:
    QStringEncoder(const Interface *i) : QStringConverter(i)
    {}

public:
    QStringEncoder() : QStringConverter()
    {}

    QStringEncoder(Encoding encoding, Flags flags = Flag::Default)
        : QStringConverter(encoding, flags)
    {}

    QStringEncoder(const char *name, Flags flags = Flag::Default)
        : QStringConverter(name, flags)
    {}

    template <typename T>
    struct DecodedData {
        QStringEncoder *encoder;
        T data;
        operator QByteArray() const { return encoder->encodeAsByteArray(data); }
    };

    Q_WEAK_OVERLOAD
    DecodedData<const QString &> operator()(const QString &str) {
        return DecodedData<const QString &>{this, str};
    }

    DecodedData<QStringView> operator()(QStringView in) {
        return DecodedData<QStringView>{this, in};
    }

    Q_WEAK_OVERLOAD
    DecodedData<const QString &> encode(const QString &str) {
        return DecodedData<const QString &>{this, str};
    }

    DecodedData<QStringView> encode(QStringView in) {
        return DecodedData<QStringView>{this, in};
    }

    qsizetype requiredSpace(qsizetype inputLength) const {
        return iface->fromUtf16Len(inputLength);
    }

    char *appendToBuffer(char *out, QStringView in) {
        return iface->fromUtf16(out, in, &state);
    }

private:
    QByteArray encodeAsByteArray(QStringView in) {
        QByteArray result(iface->fromUtf16Len(in.size()), Qt::Uninitialized);
        char *out = result.data();
        out = iface->fromUtf16(out, in, &state);
        result.truncate(out - result.constData());
        return result;
    }
};

class QStringDecoder : public QStringConverter {
protected:
    QStringDecoder(const Interface *i) : QStringConverter(i)
    {}

public:
    QStringDecoder(Encoding encoding, Flags flags = Flag::Default)
        : QStringConverter(encoding, flags)
    {}

    QStringDecoder()
        : QStringConverter()
    {}

    QStringDecoder(const char *name, Flags f = Flag::Default)
        : QStringConverter(name, f)
    {}

    template <typename T>
    struct EncodedData {
        QStringDecoder *decoder;
        T data;
        operator QString() const { return decoder->decodeAsString(data); }
    };

    Q_WEAK_OVERLOAD
    EncodedData<const QByteArray &> operator()(const QByteArray &ba) {
        return EncodedData<const QByteArray &>{this, ba};
    }

    EncodedData<QByteArrayView> operator()(QByteArrayView ba) {
        return EncodedData<QByteArrayView>{this, ba};
    }

    Q_WEAK_OVERLOAD
    EncodedData<const QByteArray &> decode(const QByteArray &ba) {
        return EncodedData<const QByteArray &>{this, ba};
    }

    EncodedData<QByteArrayView> decode(QByteArrayView ba) {
        return EncodedData<QByteArrayView>{this, ba};
    }

    qsizetype requiredSpace(qsizetype inputLength) const {
        return iface->toUtf16Len(inputLength);
    }

    QChar *appendToBuffer(QChar *out, QByteArrayView ba) {
        return iface->toUtf16(out, ba, &state);
    }

private:
    QString decodeAsString(QByteArrayView in) {
        QString result(iface->toUtf16Len(in.size()), Qt::Uninitialized);
        const QChar *out = iface->toUtf16(result.data(), in, &state);
        result.truncate(out - result.constData());
        return result;
    }
};

QT_END_NAMESPACE

#endif //QSTRINGCONVERTER_H
