//
// Created by Yujie Zhao on 2023/11/20.
//

#ifndef TST_QOBJECTHELPER_H
#define TST_QOBJECTHELPER_H

#include <QtCore/QObject>

class SenderObject : public QObject
{
    Q_OBJECT
public:
    SenderObject() : aPublicSlotCalled(0), recursionCount(0)
    {
    }

    void emitSignal1AfterRecursion() {
        if (recursionCount++ < 100) {
            emitSignal1AfterRecursion();
        }
        else {
            emitSignal1();
        }
    }

    void emitSignal1() { emit signal1(); }
    void emitSignal2() { emit signal2(); }
    void emitSignal3() { emit signal3(); }
    void emitSignal4() { emit signal4(); }

signals:
    void signal1();
    void signal2();
    void signal3();
    void signal4();
    QT_MOC_COMPAT void signal5();
    void signal6(void);
    void signal7(int, const QString &);

public slots:
    void aPublicSlot() { aPublicSlotCalled++; };

public:
    Q_INVOKABLE void invoke1() {}
    Q_SCRIPTABLE void sinvoke1() {}
    int aPublicSlotCalled;

protected:
    Q_INVOKABLE QT_MOC_COMPAT void invoke2() {}
    Q_INVOKABLE QT_MOC_COMPAT void invoke2(int) {}
    Q_SCRIPTABLE QT_MOC_COMPAT void sinvoke2() {}

private:
    Q_INVOKABLE void invoke3(int hinz = 0, int kunz = 0)
    {
        Q_UNUSED(hinz);
        Q_UNUSED(kunz);
    }
    Q_SCRIPTABLE void sinboke3() {}

    int recursionCount;
};

class ReceiverObject : public QObject
{
    Q_OBJECT
public:
    ReceiverObject()
        : sequence_slot1(0)
        , sequence_slot2(0)
        , sequence_slot3(0)
        , sequence_slot4(0)
    {}

    void reset() {
        sequence_slot4 = 0;
        sequence_slot3 = 0;
        sequence_slot2 = 0;
        sequence_slot1 = 0;
        count_slot1 = 0;
        count_slot2 = 0;
        count_slot3 = 0;
        count_slot4 = 0;
    }

    int sequence_slot1;
    int sequence_slot2;
    int sequence_slot3;
    int sequence_slot4;
    int count_slot1;
    int count_slot2;
    int count_slot3;
    int count_slot4;

    bool called(int slot)
    {
        switch (slot) {
            case 1: return sequence_slot1;
            case 2: return sequence_slot2;
            case 3: return sequence_slot3;
            case 4: return sequence_slot4;
            default: return false;
        }
    }

    static int sequence;

public slots:
    void slot1() { sequence_slot1 = ++sequence; count_slot1++; }
    void slot2() { sequence_slot2 = ++sequence; count_slot2++; }
    void slot3() { sequence_slot3 = ++sequence; count_slot3++; }
    void slot4() { sequence_slot4 = ++sequence; count_slot4++; }
};

#endif //TST_QOBJECTHELPER_H
