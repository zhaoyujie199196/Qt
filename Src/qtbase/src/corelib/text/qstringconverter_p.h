//
// Created by Yujie Zhao on 2023/6/7.
//

#ifndef QSTRINGCONVERTER_P_H
#define QSTRINGCONVERTER_P_H

#include <QtCore/qstring.h>
#include <QtCore/qstringconverter.h>

QT_BEGIN_NAMESPACE

//unicode与utf8编码转换规则： https://juejin.cn/post/6918015705434554375
/*
 * 80 : 10000000
 * c0 : 11000000
 * e0 : 11100000
 * f0 : 11110000
 * f5 : 11110101
 * */

struct QUtf8BaseTraits
{
    static const bool isTrusted = false;
    static const bool allowNonCharacters = true;
    static const bool skipAsciiHandling = false;
    static const int Error = -1;
    static const int EndOfString = -2;

    //第一位为0
    static bool isValidCharacter(uint u)
    { return int(u) >= 0; }

    static void appendByte(uchar *&ptr, uchar b)
    { *ptr++ = b; }

    static void appendByte(char8_t *&ptr, char8_t b)
    { *ptr++ = b; }

    static uchar peekByte(const uchar *ptr, qsizetype n = 0)
    { return ptr[n]; }

    static uchar peekByte(const char8_t *ptr, int n = 0)
    { return ptr[n]; }

    static qptrdiff availableBytes(const uchar *ptr, const uchar *end)
    { return end - ptr; }

    static qptrdiff availableBytes(const char8_t *ptr, const char8_t *end)
    { return end - ptr; }

    static void advanceByte(const uchar *&ptr, qsizetype n = 1)
    { ptr += n; }

    static void advanceByte(const char8_t *&ptr, int n = 1)
    { ptr += n; }

    static void appendUtf16(ushort *&ptr, ushort uc)
    { *ptr++ = uc; }

    static void appendUtf16(char16_t *&ptr, ushort uc)
    { *ptr++ = char16_t(uc); }

    static void appendUcs4(ushort *&ptr, uint uc)
    {
        appendUtf16(ptr, QChar::highSurrogate(uc));
        appendUtf16(ptr, QChar::lowSurrogate(uc));
    }

    static void appendUcs4(char16_t *&ptr, char32_t uc)
    {
        appendUtf16(ptr, QChar::highSurrogate(uc));
        appendUtf16(ptr, QChar::lowSurrogate(uc));
    }

    static ushort peekUtf16(const ushort *ptr, qsizetype n = 0)
    { return ptr[n]; }

    static ushort peekUtf16(const char16_t *ptr, int n = 0)
    { return ptr[n]; }

    static qptrdiff availableUtf16(const ushort *ptr, const ushort *end)
    { return end - ptr; }

    static qptrdiff availableUtf16(const char16_t *ptr, const char16_t  *end)
    { return end - ptr; }

    static void advanceUtf16(const ushort *&ptr, qsizetype n = 1)
    { ptr += n; }

    static void advanceUtf16(const char16_t *&ptr, int n = 1)
    { ptr += n; }

    static void appendUtf16(uint *&ptr, ushort uc)
    { *ptr++ = uc; }

    static void appendUtf16(char32_t *&ptr, ushort uc)
    { *ptr++ = char32_t(uc); }

    static void appendUcs4(uint *&ptr, uint uc)
    { *ptr++ = uc; }

    static void appendUcs4(char32_t *&ptr, uint uc)
    { *ptr++ = char32_t(uc); }
};

struct QUtf8
{
    static QByteArray convertFromUnicode(QStringView in);
    static QByteArray convertFromUnicode(QStringView in, QStringConverterBase::State *state);
    static char *convertFromUnicode(char *out, QStringView in, QStringConverter::State *state);

    static QString convertToUnicode(QByteArrayView in);
    static QChar *convertToUnicode(QChar *buffer, QByteArrayView in) noexcept;
};

namespace QUtf8Functions
{
    inline bool isContinuationByte(uchar b)
    {
        return (b & 0xc0) == 0x80;
    }

    //unicode转utf8：https://www.cnblogs.com/cthon/p/9297232.html
    //算法也负责改变src的指针
    template <typename Traits, typename OutputPtr, typename InputPtr>
    inline int toUtf8(ushort u, OutputPtr &dst, InputPtr &src, InputPtr end)
    {
        //小于0x80，单字节，兼容ascii的字符
        if (!Traits::skipAsciiHandling && u < 0x80) {
            Traits::appendByte(dst, uchar(u));
            return 0;
        }
        else if (u < 0x0800) {
            // U+0080 到 U+07FF - 两字节，110xxxxx 10xxxxxx格式
            // first of two bytes
            // 0xc0: 11000000
            // 将高位与110相或，放到dst中， Traits::appendByte(dst, 0x80 | (u & 0x3f)); 再将低六位放到dst中
            Traits::appendByte(dst, 0xc0 | uchar(u >> 6));
        }
        else {
            if (!QChar::isSurrogate(u)) {  //16位可以表示的unicode  // 需要三字节表示
                // unicode的U+0800到U+FFFF (除了 U+D800-U+DFFF) 3个byte
                if (!Traits::allowNonCharacters && QChar::isNonCharacter(u)) {
                    return Traits::Error;
                }
                //e0: 11100000  最高位开头位110，可以保存4位
                Traits::appendByte(dst, 0xe0 | uchar(u >> 12));
            }
            else {  //需要4字节表示
                //需要2个16位表示的unicode : https://www.zhihu.com/question/42176549
                //调用函数时，u为src表示前面的字符。
                //src和end之间，最起码还有一个字符
                if (Traits::availableUtf16(src, end) == 0) {
                    return Traits::EndOfString;
                }

                //u是高代理，low是低代理
                ushort low = Traits::peekUtf16(src);
                if (!QChar::isHighSurrogate(u)) {
                    return Traits::Error;
                }
                if (!QChar::isLowSurrogate(low)) {
                    return Traits::Error;
                }
                Traits::advanceUtf16(src);
                //计算出32位的码点
                uint ucs4 = QChar::surrogateToUcs4(u, low);

                if (!Traits::allowNonCharacters && QChar::isNonCharacter(ucs4)) {
                    return Traits::Error;
                }
                //first byte 取19-21这三位就行
                Traits::appendByte(dst, 0xf0 | (uchar(ucs4 >> 18) & 0xf));
                //second of four byte
                Traits::appendByte(dst, 0x80 | (uchar(ucs4 >> 12) & 0x3f));

                u = ushort(ucs4);
            }
            //3字节，保存中间字节的6位
            Traits::appendByte(dst, 0x80 | (uchar(u >> 6) & 0x3f));
        }
        //0x3f: 00111111  保存最后6位
        Traits::appendByte(dst, 0x80 | (u & 0x3f));
        return 0;
    }

    template <typename Traits, typename OutputPtr, typename InputPtr>
    inline qsizetype fromUtf8(uchar b, OutputPtr &dst, InputPtr &src, InputPtr end)
    {
        qsizetype charsNeeded;
        uint min_uc;
        uint uc;

        if (!Traits::skipAsciiHandling && b < 0x80) {  //0xxxxxxx，单字节，兼容ascii的部分
            // US-ASCII，各套编码系统都是直接兼容的
            Traits::appendUtf16(dst, b);
            return 1;
        }
        //uc为最高位去除固定bit之后的有效数据
        if (!Traits::isTrusted && Q_UNLIKELY(b <= 0xC1)) {
            //0xxxxxxx到110xxxxx中间的字符非法
            return Traits::Error;
        } else if (b < 0xe0) {  //110开头，两字节
            charsNeeded = 2;
            min_uc = 0x80;
            uc = b & 0x1f;
        } else if (b < 0xf0) { //1110开头，三字节
            charsNeeded = 3;
            min_uc = 0x800;
            uc = b & 0x0f;
        } else if (b < 0xf5) {  //1110到11110之间，4字节
            charsNeeded = 4;
            min_uc = 0x10000;
            uc = b & 0x07;
        } else {
            // the last Unicode character is U+10FFFF
            // it's encoded in UTF-8 as "\xF4\x8F\xBF\xBF"
            // therefore, a byte higher than 0xF4 is not the UTF-8 first byte
            return Traits::Error;
        }

        qptrdiff bytesAvailable = Traits::availableBytes(src, end);
        if (Q_UNLIKELY(bytesAvailable < charsNeeded - 1)) {  //需要有足够的字符
            // it's possible that we have an error instead of just unfinished bytes
            if (bytesAvailable > 0 && !isContinuationByte(Traits::peekByte(src, 0)))
                return Traits::Error;
            if (bytesAvailable > 1 && !isContinuationByte(Traits::peekByte(src, 1)))
                return Traits::Error;
            return Traits::EndOfString;
        }

        // first continuation character
        b = Traits::peekByte(src, 0);
        if (!isContinuationByte(b))
            return Traits::Error;
        uc <<= 6;
        uc |= b & 0x3f;

        if (charsNeeded > 2) {
            // second continuation character
            b = Traits::peekByte(src, 1);
            if (!isContinuationByte(b))
                return Traits::Error;
            uc <<= 6;
            uc |= b & 0x3f;

            if (charsNeeded > 3) {
                // third continuation character
                b = Traits::peekByte(src, 2);
                if (!isContinuationByte(b))
                    return Traits::Error;
                uc <<= 6;
                uc |= b & 0x3f;
            }
        }

        // we've decoded something; safety-check it
        if (!Traits::isTrusted) {
            if (uc < min_uc)
                return Traits::Error;
            if (QChar::isSurrogate(uc) || uc > QChar::LastValidCodePoint)
                return Traits::Error;
            if (!Traits::allowNonCharacters && QChar::isNonCharacter(uc))
                return Traits::Error;
        }

        // write the UTF-16 sequence
        if (!QChar::requiresSurrogates(uc)) {
            // UTF-8 decoded and no surrogates are required
            // detach if necessary
            Traits::appendUtf16(dst, ushort(uc));
        } else {
            // UTF-8 decoded to something that requires a surrogate pair
            Traits::appendUcs4(dst, uc);
        }

        Traits::advanceByte(src, charsNeeded - 1);
        return charsNeeded;
    }

}

QT_END_NAMESPACE

#endif //QSTRINGCONVERTER_P_H
