//
// Created by Yujie Zhao on 2023/9/26.
//
#include <QtCore/QVariant>
#include <QtCore/QRegularExpression>
#include <QtTest/QTest>
#include <QObject>

const qlonglong intMax1 = (qlonglong)INT_MAX + 1;
const qulonglong uintMax1 = (qulonglong)UINT_MAX + 1;

#define TST_QVARIANT_CANCONVERT_DATATABLE_HEADERS \
    QTest::addColumn<QVariant>("val"); \
    QTest::addColumn<bool>("BitArrayCast"); \
    QTest::addColumn<bool>("BitmapCast"); \
    QTest::addColumn<bool>("BoolCast"); \
    QTest::addColumn<bool>("BrushCast"); \
    QTest::addColumn<bool>("ByteArrayCast"); \
    QTest::addColumn<bool>("ColorCast"); \
    QTest::addColumn<bool>("CursorCast"); \
    QTest::addColumn<bool>("DateCast"); \
    QTest::addColumn<bool>("DateTimeCast"); \
    QTest::addColumn<bool>("DoubleCast"); \
    QTest::addColumn<bool>("FontCast"); \
    QTest::addColumn<bool>("ImageCast"); \
    QTest::addColumn<bool>("IntCast"); \
    QTest::addColumn<bool>("InvalidCast"); \
    QTest::addColumn<bool>("KeySequenceCast"); \
    QTest::addColumn<bool>("ListCast"); \
    QTest::addColumn<bool>("LongLongCast"); \
    QTest::addColumn<bool>("MapCast"); \
    QTest::addColumn<bool>("PaletteCast"); \
    QTest::addColumn<bool>("PenCast"); \
    QTest::addColumn<bool>("PixmapCast"); \
    QTest::addColumn<bool>("PointCast"); \
    QTest::addColumn<bool>("RectCast"); \
    QTest::addColumn<bool>("RegionCast"); \
    QTest::addColumn<bool>("SizeCast"); \
    QTest::addColumn<bool>("SizePolicyCast"); \
    QTest::addColumn<bool>("StringCast"); \
    QTest::addColumn<bool>("StringListCast"); \
    QTest::addColumn<bool>("TimeCast"); \
    QTest::addColumn<bool>("UIntCast"); \
    QTest::addColumn<bool>("ULongLongCast");

#define TST_QVARIANT_CANCONVERT_FETCH_DATA \
    QFETCH(QVariant, val); \
    QFETCH(bool, BitArrayCast); \
    QFETCH(bool, BitmapCast); \
    QFETCH(bool, BoolCast); \
    QFETCH(bool, BrushCast); \
    QFETCH(bool, ByteArrayCast); \
    QFETCH(bool, ColorCast); \
    QFETCH(bool, CursorCast); \
    QFETCH(bool, DateCast); \
    QFETCH(bool, DateTimeCast); \
    QFETCH(bool, DoubleCast); \
    QFETCH(bool, FontCast); \
    QFETCH(bool, ImageCast); \
    QFETCH(bool, IntCast); \
    QFETCH(bool, InvalidCast); \
    QFETCH(bool, KeySequenceCast); \
    QFETCH(bool, ListCast); \
    QFETCH(bool, LongLongCast); \
    QFETCH(bool, MapCast); \
    QFETCH(bool, PaletteCast); \
    QFETCH(bool, PenCast); \
    QFETCH(bool, PixmapCast); \
    QFETCH(bool, PointCast); \
    QFETCH(bool, RectCast); \
    QFETCH(bool, RegionCast); \
    QFETCH(bool, SizeCast); \
    QFETCH(bool, SizePolicyCast); \
    QFETCH(bool, StringCast); \
    QFETCH(bool, StringListCast); \
    QFETCH(bool, TimeCast); \
    QFETCH(bool, UIntCast); \
    QFETCH(bool, ULongLongCast);

#define TST_QVARIANT_CANCONVERT_COMPARE_DATA \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QBitArray)), BitArrayCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QBitmap)), BitmapCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::Bool)), BoolCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QBrush)), BrushCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QByteArray)), ByteArrayCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QColor)), ColorCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QCursor)), CursorCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QDate)), DateCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QDateTime)), DateTimeCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::Double)), DoubleCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::Float)), DoubleCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QFont)), FontCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QImage)), ImageCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::Int)), IntCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::UnknownType)), InvalidCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QKeySequence)), KeySequenceCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QVariantList)), ListCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::LongLong)), LongLongCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QVariantMap)), MapCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QPalette)), PaletteCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QPen)), PenCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QPixmap)), PixmapCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QPoint)), PointCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QRect)), RectCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QRegion)), RegionCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QSize)), SizeCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QSizePolicy)), SizePolicyCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QString)), StringCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QStringList)), StringListCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::QTime)), TimeCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::UInt)), UIntCast); \
    QCOMPARE(val.canConvert(QMetaType(QMetaType::ULongLong)), ULongLongCast);

class tst_QVariant : public QObject
{
public:
    void registerInvokeMethods() {
        REGISTER_OBJECT_INVOKE_METHOD(test)
        REGISTER_OBJECT_INVOKE_METHOD(cleanupTestCase)
        REGISTER_OBJECT_INVOKE_METHOD(constructor)
        REGISTER_OBJECT_INVOKE_METHOD(copy_constructor)
        REGISTER_OBJECT_INVOKE_METHOD(constructor_invalid_data)
        REGISTER_OBJECT_INVOKE_METHOD(constructor_invalid)
        REGISTER_OBJECT_INVOKE_METHOD(isNull)
        REGISTER_OBJECT_INVOKE_METHOD(swap)
        REGISTER_OBJECT_INVOKE_METHOD(canConvert_data)
        REGISTER_OBJECT_INVOKE_METHOD(canConvert)
        REGISTER_OBJECT_INVOKE_METHOD(convert)
    }

    void test() {
#define USE_TEST 0
#if USE_TEST != 0
        int t1 = QtPrivate::BuiltinMetaType<bool>::value;
        int metaType = QMetaTypeId2<bool>::MetaType;

        auto id =  QMetaType::fromType<bool>().id();

        QVariant var(1);
        int t = var.toInt();
        bool b = var.toBool();

        int sizeofPrivate = sizeof(QVariant::PrivateShared);
        int k = 0;
#endif
    }

    void cleanupTestCase();
    void constructor();
    void copy_constructor();
    void constructor_invalid_data();
    void constructor_invalid();
    void isNull();
    void swap();

    void canConvert_data();
    void canConvert();
    void convert();

    void toSize_data();
    void toSize();

    void toSizeF_data();
    void toSizeF();

    void toPoint_data();
    void toPoint();

    void toRect_data();
    void toRect();

    void toChar_data();
    void toChar();

    void toLine_data();
    void toLine();

    void toLineF_data();
    void toLineF();

    void toInt_data();
    void toInt();

    void toUInt_data();
    void toUInt();

    void toBool_data();
    void toBool();

    void toLongLong_data();
    void toLongLong();

    void toULongLong_data();
    void toULongLong();

    void toByteArray_data();
    void toByteArray();

    void toString_data();
    void toString();

    void toDate_data();
    void toDate();

    void toTime_data();
    void toTime();

    void toDateTime_data();
    void toDateTime();

    void toDouble_data();
    void toDouble();

    void toFloat_data();
    void toFloat();

    void toPointF_data();
    void toPointF();

    void toRectF_data();
    void toRectF();

    void qvariant_cast_QObject_data();
    void qvariant_cast_QObject();
    void qvariant_cast_QObject_derived();
    void qvariant_cast_QObject_wrapper();
    void qvariant_cast_QSharedPointerQObject();
    void qvariant_cast_const();

    void toLocale();

    void toRegularExpression();

    void url();

    void userType();
    void basicUserType();

    void variant_to();

    void writeToReadFromDataStream_data();
    void writeToReadFromDataStream();
    void writeToReadFromOldDataStream();
    void checkDataStream();

    void operator_eq_eq_data();
    void operator_eq_eq();

    void typeName_data();
    void typeName();
    void typeToName();

    void streamInvalidVariant();

    void podUserType();

    void data();
    void constData();

    void saveLoadCustomTypes();

    void variantMap();
    void variantHash();

    void convertToQUint8() const;
    void comparePointers() const;
    void voidStar() const;
    void dataStar() const;
    void canConvertQStringList() const;
    void canConvertQStringList_data() const;
    void canConvertMetaTypeToInt() const;
    void variantToDateTimeWithoutWarnings() const;
    void invalidDateTime() const;

    void loadUnknownUserType();
    void loadBrokenUserType();

    void invalidDate() const;
    void compareCustomTypes() const;
    void timeToDateTime() const;
    void copyingUserTypes() const;
    void valueClassHierarchyConversion() const;
    void convertBoolToByteArray() const;
    void convertBoolToByteArray_data() const;
    void convertByteArrayToBool() const;
    void convertByteArrayToBool_data() const;
    void convertIterables() const;
    void convertConstNonConst() const;
    void toIntFromQString() const;
    void toIntFromDouble() const;
    void setValue();
    void fpStringRoundtrip_data() const;
    void fpStringRoundtrip() const;

    void numericalConvert_data();
    void numericalConvert();
    void moreCustomTypes();
    void movabilityTest();
    void variantInVariant();
    void userConversion();
    void modelIndexConversion();

    void forwardDeclare();
    void debugStream_data();
    void debugStream();
    void debugStreamType_data();
    void debugStreamType();

    void loadQt4Stream_data();
    void loadQt4Stream();
    void saveQt4Stream_data();
    void saveQt4Stream();
    void loadQt5Stream_data();
    void loadQt5Stream();
    void saveQt5Stream_data();
    void saveQt5Stream();
    void saveInvalid_data();
    void saveInvalid();
    void saveNewBuiltinWithOldStream();

    void implicitConstruction();

    void iterateContainerElements();
    void pairElements();

    void enums();
    void metaEnums();

    void nullConvert();

    void accessSequentialContainerKey();
    void shouldDeleteVariantDataWorksForSequential();
    void shouldDeleteVariantDataWorksForAssociative();
    void fromStdVariant();
    void qt4UuidDataStream();
    void sequentialIterableEndianessSanityCheck();
    void sequentialIterableAppend();

    void preferDirectConversionOverInterfaces();
    void mutableView();

    void moveOperations();
    void equalsWithoutMetaObject();
};

void tst_QVariant::cleanupTestCase()
{

}

void tst_QVariant::constructor()
{
    QVariant variant;
    QVERIFY(!variant.isValid());
    QVERIFY(variant.isNull());

    QVariant var2(variant);
    QVERIFY(!var2.isValid());
    QVERIFY(variant.isNull());

    QVariant varll(intMax1);
    QVariant varll2(varll);
    QCOMPARE(varll2, varll);

    QVariant var3(QVariant::String);
    QCOMPARE(var3.typeName(), "QString");
    QVERIFY(var3.isNull());
    QVERIFY(var3.isValid());

    QVariant var4(QVariant::Invalid);
    QCOMPARE(var4.type(), QVariant::Invalid);
    QVERIFY(var4.isNull());
    QVERIFY(!var4.isValid());

    QVariant var5(QLatin1String("hallo"));
    QCOMPARE(var5.type(), QVariant::String);
    QCOMPARE(var5.typeName(), "QString");

    QVariant var6(qlonglong(0));
    QCOMPARE(var6.type(), QVariant::LongLong);
    QCOMPARE(var6.typeName(), "qlonglong");

    QVariant var7 = 5;
    QVERIFY(var7.isValid());
    QVERIFY(!var7.isNull());
    QVariant var8;
    var8.setValue(5);
    QVERIFY(var8.isValid());
    QVERIFY(!var8.isNull());
}

void tst_QVariant::copy_constructor()
{
    QVariant var7(QVariant::Int);
    QVariant var8(var7);
    QCOMPARE(var8.type(), QVariant::Int);
    QVERIFY(var8.isNull());
}

void tst_QVariant::constructor_invalid_data()
{
    QTest::addColumn<uint>("typeId");

    QTest::newRow("-1") << uint(-1);
    QTest::newRow("-122234567") << uint(-122234567);
    QTest::newRow("0xfffffffff") << uint(0xfffffffff);
    QTest::newRow("LastCoreType + 1") << uint(QMetaType::LastCoreType + 1);
    QVERIFY(!QMetaType::isRegistered(QMetaType::LastCoreType + 1));
}

void tst_QVariant::constructor_invalid()
{
    QFETCH(uint, typeId);
    {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("^Trying to construct an instance of an invalid type"));
        QVariant variant(static_cast<QVariant::Type>(typeId));
        QVERIFY(!variant.isValid());
        QVERIFY(variant.isNull());
        QCOMPARE(int(variant.type()), int(QMetaType::UnknownType));
        QCOMPARE(variant.userType(), int(QMetaType::UnknownType));
    }
    {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression("^Trying to construct an instance of an invalid type"));
        QVariant variant(QMetaType(typeId), /* copy */ nullptr);
        QVERIFY(!variant.isValid());
        QVERIFY(variant.isNull());
        QCOMPARE(variant.userType(), int(QMetaType::UnknownType));
    }
}

void tst_QVariant::isNull()
{
    QVariant var;
    QVERIFY( var.isNull() );

    QString str1;
    QVariant var1( str1 );
    QVERIFY( !var1.isNull() );

    QVariant var3(QString("blah"));
    QVERIFY( !var3.isNull() );

    var3 = QVariant(QVariant::String);
    QVERIFY( var3.isNull() );

    QVariant var4( 0 );
    QVERIFY( !var4.isNull() );

    QVariant var5 = QString();
    QVERIFY( !var5.isNull() );

    QVariant var6( QString( "blah" ) );
    QVERIFY( !var6.isNull() );
    var6 = QVariant();
    QVERIFY( var6.isNull() );
    var6.convert( QVariant::String );
    QVERIFY( var6.isNull() );
    QVariant varLL( (qlonglong)0 );
    QVERIFY( !varLL.isNull() );

    QVariant var8(QMetaType(QMetaType::Nullptr), nullptr);
    QVERIFY(var8.isNull());
    var8 = QVariant::fromValue<std::nullptr_t>(nullptr);
    QVERIFY(var8.isNull());
//    QVariant var9 = QVariant(QJsonValue(QJsonValue::Null));
//    QVERIFY(!var9.isNull());
//    var9 = QVariant::fromValue<QJsonValue>(QJsonValue(QJsonValue::Null));
//    QVERIFY(!var9.isNull());

    QVariant var10(QMetaType(QMetaType::VoidStar), nullptr);
    QVERIFY(var10.isNull());
    var10 = QVariant::fromValue<void*>(nullptr);
    QVERIFY(var10.isNull());

    QVariant var11(QMetaType(QMetaType::QObjectStar), nullptr);
    QVERIFY(var11.isNull());
    var11 = QVariant::fromValue<QObject*>(nullptr);
    QVERIFY(var11.isNull());

    QVERIFY(QVariant::fromValue<int*>(nullptr).isNull());

    QVariant var12(QVariant::fromValue<QString>(QString()));
    QVERIFY(!var12.isNull());
}

void tst_QVariant::swap()
{
    QVariant v1 = 1;
    QVariant v2 = 2.0;
    v1.swap(v2);
    QCOMPARE(v1.type(), QVariant::Double);
    QCOMPARE(v1.toDouble(), 2.0);
    QCOMPARE(v2.typeId(), QVariant::Int);
    QCOMPARE(v2.toInt(), 1);
}

void tst_QVariant::canConvert_data()
{
    //zhaoyujie TODO
}

void tst_QVariant::canConvert()
{
    //zhaoyujie TODO
}

void tst_QVariant::convert()
{
    //zhaoyujie TODO
}

void tst_QVariant::toSize_data()
{

}

void tst_QVariant::toSize()
{

}


QTEST_APPLESS_MAIN(tst_QVariant)
