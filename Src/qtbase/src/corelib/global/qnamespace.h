//
// Created by Yujie Zhao on 2023/2/28.
//

#ifndef QNAMESPACE_H
#define QNAMESPACE_H

#include <QtCore/qglobal.h>
#include <QtCore/qflags.h>

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
}

QT_END_NAMESPACE

#endif //QNAMESPACE_H
