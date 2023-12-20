//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QNAMESPACE_H
#define QNAMESPACE_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

namespace Qt {
    //大小写敏感
    enum CaseSensitivity {
        CaseInsensitive,
        CaseSensitive
    };

    //鼠标按钮
    enum MouseButton {
        NoButton        = 0x00000000,
        LeftButton      = 0x00000001,
        RightButton     = 0x00000002,
        MiddleButton    = 0x00000004,
        MouseButtonMask = 0xffffffff,
    };
    Q_DECLARE_FLAGS(MouseButtons, MouseButton)
    Q_DECLARE_OPERATORS_FOR_FLAGS(MouseButtons)

    //窗口类型
    enum WindowType {
        Widget = 0x00000000,
        Window = 0x00000001,
        Dialog = 0x00000002 | Window,
    };
    Q_DECLARE_FLAGS(WindowFlags, WindowType)
    Q_DECLARE_OPERATORS_FOR_FLAGS(WindowFlags)

    enum TextInteractionFlag {
        NoTextInteraction = 0,
        TextSelectableByMouse = 1,
        TextSelectableByKeyboard = 2,
        LinksAccessibleByMouse = 4,
        LinksAccessibleByKeyboard = 8,
        TextEditable = 16,
        TextEditorInteraction = TextSelectableByMouse | TextSelectableByKeyboard | TextEditable,
        TextBrowserInteraction    = TextSelectableByMouse | LinksAccessibleByMouse | LinksAccessibleByKeyboard
    };
    Q_DECLARE_FLAGS(TextInteractionFlags, TextInteractionFlag)
    Q_DECLARE_OPERATORS_FOR_FLAGS(TextInteractionFlags)

    enum AlignmentFlag {
        AlignLeft = 0x0001,
        AlignLeading = AlignLeft,
        AlignRight = 0x0002,
        AlignTrailing = AlignRight,
        AlignHCenter = 0x0004,
        AlignJustify = 0x0008,
        AlignAbsolute = 0x0010,
        AlignHorizontal_Mask = AlignLeft | AlignRight | AlignHCenter | AlignJustify | AlignAbsolute,

        AlignTop = 0x0020,
        AlignBottom = 0x0040,
        AlignVCenter = 0x0080,
        AlignBaseline = 0x0100,
        // Note that 0x100 will clash with Qt::TextSingleLine = 0x100 due to what the comment above
        // this enum declaration states. However, since Qt::AlignBaseline is only used by layouts,
        // it doesn't make sense to pass Qt::AlignBaseline to QPainter::drawText(), so there
        // shouldn't really be any ambiguity between the two overlapping enum values.
        AlignVertical_Mask = AlignTop | AlignBottom | AlignVCenter | AlignBaseline,

        AlignCenter = AlignVCenter | AlignHCenter
    };

    Q_DECLARE_FLAGS(Alignment, AlignmentFlag)
    Q_DECLARE_OPERATORS_FOR_FLAGS(Alignment)

    enum class Initialization {
        Uninitialized
    };
    inline constexpr Initialization Uninitialized = Initialization::Uninitialized;

    enum ChecksumType {
        ChecksumIso3309,
        ChecksumItuV41
    };

    enum AspectRatioMode {
        IgnoreAspectRatio,  //忽略宽高哔哩
        KeepAspectRatio,  //保持宽高比
        KeepAspectRatioByExpanding  //扩展保持宽高比
    };

    //信号槽连接类型
    enum ConnectionType {
        AutoConnection,     //自动连接，根据发射者与接受者所处的线程自动选择
        DirectConnection,   //直连，直接调用
        QueuedConnection,   //放到线程队列中立刻返回
        BlockingQueuedConnection,  //放到线程队列中并阻塞当前代码，等待槽函数执行完
        UniqueConnection =  0x80,  //同一个连接只能连接一次
        SingleShotConnection = 0x100,   //只触发一次，调用完毕自动断开连接
    };

    enum FindChildOption {
        FindDirectChildrenOnly = 0x0,
        FindChildrenRecursively = 0x1
    };
    Q_DECLARE_FLAGS(FindChildOptions, FindChildOption)


    typedef void * HANDLE;
}

QT_END_NAMESPACE

#endif //QNAMESPACE_H
