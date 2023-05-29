//
// Created by Yujie Zhao on 2023/5/16.
//

#ifndef QLOCALE_H
#define QLOCALE_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

class QLocale
{
public:
    //国家语言
    enum Language : ushort {
        AnyLanguage = 0,
        C = 1,
        Abkhazian = 2,
        Afar = 3,
        Afrikaans = 4,
        Aghem = 5,
        Akan = 6,
        Akkadian = 7,
        Akoose = 8,
        Albanian = 9,
        AmericanSignLanguage = 10,
        Amharic = 11,
        AncientEgyptian = 12,
        AncientGreek = 13,
        Arabic = 14,
        Aragonese = 15,
        Aramaic = 16,
        Armenian = 17,
        Assamese = 18,
        Asturian = 19,
        Asu = 20,
        Atsam = 21,
        Avaric = 22,
        Avestan = 23,
        Aymara = 24,
        Azerbaijani = 25,
        Bafia = 26,
        Balinese = 27,
        Bambara = 28,
        Bamun = 29,
        Bangla = 30,
        Basaa = 31,
        Bashkir = 32,
        Basque = 33,
        BatakToba = 34,
        Belarusian = 35,
        Bemba = 36,
        Bena = 37,
        Bhojpuri = 38,
        Bislama = 39,
        Blin = 40,
        Bodo = 41,
        Bosnian = 42,
        Breton = 43,
        Buginese = 44,
        Bulgarian = 45,
        Burmese = 46,
        Cantonese = 47,
        Catalan = 48,
        Cebuano = 49,
        CentralAtlasTamazight = 50,
        CentralKurdish = 51,
        Chakma = 52,
        Chamorro = 53,
        Chechen = 54,
        Cherokee = 55,
        Chickasaw = 56,
        Chiga = 57,
        Chinese = 58,
        Church = 59,
        Chuvash = 60,
        Colognian = 61,
        Coptic = 62,
        Cornish = 63,
        Corsican = 64,
        Cree = 65,
        Croatian = 66,
        Czech = 67,
        Danish = 68,
        Divehi = 69,
        Dogri = 70,
        Duala = 71,
        Dutch = 72,
        Dzongkha = 73,
        Embu = 74,
        English = 75,
        Erzya = 76,
        Esperanto = 77,
        Estonian = 78,
        Ewe = 79,
        Ewondo = 80,
        Faroese = 81,
        Fijian = 82,
        Filipino = 83,
        Finnish = 84,
        French = 85,
        Friulian = 86,
        Fulah = 87,
        Gaelic = 88,
        Ga = 89,
        Galician = 90,
        Ganda = 91,
        Geez = 92,
        Georgian = 93,
        German = 94,
        Gothic = 95,
        Greek = 96,
        Guarani = 97,
        Gujarati = 98,
        Gusii = 99,
        Haitian = 100,
        Hausa = 101,
        Hawaiian = 102,
        Hebrew = 103,
        Herero = 104,
        Hindi = 105,
        HiriMotu = 106,
        Hungarian = 107,
        Icelandic = 108,
        Ido = 109,
        Igbo = 110,
        InariSami = 111,
        Indonesian = 112,
        Ingush = 113,
        Interlingua = 114,
        Interlingue = 115,
        Inuktitut = 116,
        Inupiaq = 117,
        Irish = 118,
        Italian = 119,
        Japanese = 120,
        Javanese = 121,
        Jju = 122,
        JolaFonyi = 123,
        Kabuverdianu = 124,
        Kabyle = 125,
        Kako = 126,
        Kalaallisut = 127,
        Kalenjin = 128,
        Kamba = 129,
        Kannada = 130,
        Kanuri = 131,
        Kashmiri = 132,
        Kazakh = 133,
        Kenyang = 134,
        Khmer = 135,
        Kiche = 136,
        Kikuyu = 137,
        Kinyarwanda = 138,
        Komi = 139,
        Kongo = 140,
        Konkani = 141,
        Korean = 142,
        Koro = 143,
        KoyraboroSenni = 144,
        KoyraChiini = 145,
        Kpelle = 146,
        Kuanyama = 147,
        Kurdish = 148,
        Kwasio = 149,
        Kyrgyz = 150,
        Lakota = 151,
        Langi = 152,
        Lao = 153,
        Latin = 154,
        Latvian = 155,
        Lezghian = 156,
        Limburgish = 157,
        Lingala = 158,
        LiteraryChinese = 159,
        Lithuanian = 160,
        Lojban = 161,
        LowerSorbian = 162,
        LowGerman = 163,
        LubaKatanga = 164,
        LuleSami = 165,
        Luo = 166,
        Luxembourgish = 167,
        Luyia = 168,
        Macedonian = 169,
        Machame = 170,
        Maithili = 171,
        MakhuwaMeetto = 172,
        Makonde = 173,
        Malagasy = 174,
        Malayalam = 175,
        Malay = 176,
        Maltese = 177,
        Mandingo = 178,
        Manipuri = 179,
        Manx = 180,
        Maori = 181,
        Mapuche = 182,
        Marathi = 183,
        Marshallese = 184,
        Masai = 185,
        Mazanderani = 186,
        Mende = 187,
        Meru = 188,
        Meta = 189,
        Mohawk = 190,
        Mongolian = 191,
        Morisyen = 192,
        Mundang = 193,
        Muscogee = 194,
        Nama = 195,
        NauruLanguage = 196,
        Navajo = 197,
        Ndonga = 198,
        Nepali = 199,
        Newari = 200,
        Ngiemboon = 201,
        Ngomba = 202,
        NigerianPidgin = 203,
        Nko = 204,
        NorthernLuri = 205,
        NorthernSami = 206,
        NorthernSotho = 207,
        NorthNdebele = 208,
        NorwegianBokmal = 209,
        NorwegianNynorsk = 210,
        Nuer = 211,
        Nyanja = 212,
        Nyankole = 213,
        Occitan = 214,
        Odia = 215,
        Ojibwa = 216,
        OldIrish = 217,
        OldNorse = 218,
        OldPersian = 219,
        Oromo = 220,
        Osage = 221,
        Ossetic = 222,
        Pahlavi = 223,
        Palauan = 224,
        Pali = 225,
        Papiamento = 226,
        Pashto = 227,
        Persian = 228,
        Phoenician = 229,
        Polish = 230,
        Portuguese = 231,
        Prussian = 232,
        Punjabi = 233,
        Quechua = 234,
        Romanian = 235,
        Romansh = 236,
        Rombo = 237,
        Rundi = 238,
        Russian = 239,
        Rwa = 240,
        Saho = 241,
        Sakha = 242,
        Samburu = 243,
        Samoan = 244,
        Sango = 245,
        Sangu = 246,
        Sanskrit = 247,
        Santali = 248,
        Sardinian = 249,
        Saurashtra = 250,
        Sena = 251,
        Serbian = 252,
        Shambala = 253,
        Shona = 254,
        SichuanYi = 255,
        Sicilian = 256,
        Sidamo = 257,
        Silesian = 258,
        Sindhi = 259,
        Sinhala = 260,
        SkoltSami = 261,
        Slovak = 262,
        Slovenian = 263,
        Soga = 264,
        Somali = 265,
        SouthernKurdish = 266,
        SouthernSami = 267,
        SouthernSotho = 268,
        SouthNdebele = 269,
        Spanish = 270,
        StandardMoroccanTamazight = 271,
        Sundanese = 272,
        Swahili = 273,
        Swati = 274,
        Swedish = 275,
        SwissGerman = 276,
        Syriac = 277,
        Tachelhit = 278,
        Tahitian = 279,
        TaiDam = 280,
        Taita = 281,
        Tajik = 282,
        Tamil = 283,
        Taroko = 284,
        Tasawaq = 285,
        Tatar = 286,
        Telugu = 287,
        Teso = 288,
        Thai = 289,
        Tibetan = 290,
        Tigre = 291,
        Tigrinya = 292,
        TokelauLanguage = 293,
        TokPisin = 294,
        Tongan = 295,
        Tsonga = 296,
        Tswana = 297,
        Turkish = 298,
        Turkmen = 299,
        TuvaluLanguage = 300,
        Tyap = 301,
        Ugaritic = 302,
        Ukrainian = 303,
        UpperSorbian = 304,
        Urdu = 305,
        Uyghur = 306,
        Uzbek = 307,
        Vai = 308,
        Venda = 309,
        Vietnamese = 310,
        Volapuk = 311,
        Vunjo = 312,
        Walloon = 313,
        Walser = 314,
        Warlpiri = 315,
        Welsh = 316,
        WesternBalochi = 317,
        WesternFrisian = 318,
        Wolaytta = 319,
        Wolof = 320,
        Xhosa = 321,
        Yangben = 322,
        Yiddish = 323,
        Yoruba = 324,
        Zarma = 325,
        Zhuang = 326,
        Zulu = 327,

        Afan = Oromo,
        Bengali = Bangla,
        Bhutani = Dzongkha,
        Byelorussian = Belarusian,
        Cambodian = Khmer,
        CentralMoroccoTamazight = CentralAtlasTamazight,
        Chewa = Nyanja,
        Frisian = WesternFrisian,
        Greenlandic = Kalaallisut,
        Inupiak = Inupiaq,
        Kirghiz = Kyrgyz,
        Kurundi = Rundi,
        Kwanyama = Kuanyama,
        Navaho = Navajo,
        Oriya = Odia,
        RhaetoRomance = Romansh,
        Uighur = Uyghur,
        Uigur = Uyghur,
        Walamo = Wolaytta,

        LastLanguage = Zulu
    };
};

QT_END_NAMESPACE

#endif //QLOCALE_H
