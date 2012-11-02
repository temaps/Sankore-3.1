/*
 * UBDesktopPalette.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: julienbachmann
 */
#include "UBDesktopPalette.h"

#include <QtGui>

#include "frameworks/UBPlatformUtils.h"

#include "core/UBSettings.h"
#include "core/UBSetting.h"
#include "core/UBApplication.h"
#include "core/UBApplicationController.h"

#include "board/UBDrawingController.h"

#include "gui/UBMainWindow.h"

#include "core/memcheck.h"

UBDesktopPalette::UBDesktopPalette(QWidget *parent, UBRightPalette* _rightPalette)
    : UBActionPalette(Qt::TopLeftCorner, parent)
    , mShowHideAction(NULL)
    , mDisplaySelectAction(NULL)
    , rightPalette(_rightPalette)
{
    mActionUniboard = new QAction(QIcon(":/images/toolbar/board.png"), tr("Show Open-Sankore"), this);
    connect(mActionUniboard, SIGNAL(triggered()), this, SIGNAL(uniboardClick()));
    addAction(mActionUniboard);

    addAction(UBApplication::mainWindow->actionPen);
    addAction(UBApplication::mainWindow->actionEraser);
    addAction(UBApplication::mainWindow->actionMarker);
    addAction(UBApplication::mainWindow->actionSelector);
    addAction(UBApplication::mainWindow->actionPointer);

    if (UBPlatformUtils::hasVirtualKeyboard())
        addAction(UBApplication::mainWindow->actionVirtualKeyboard, false);

    mActionCustomSelect = new QAction(QIcon(":/images/toolbar/captureArea.png"), tr("Capture Part of the Screen"), this);
    connect(mActionCustomSelect, SIGNAL(triggered()), this, SIGNAL(customClick()));
    addAction(mActionCustomSelect);

    mDisplaySelectAction = new QAction(QIcon(":/images/toolbar/captureScreen.png"), tr("Capture the Screen"), this);
    connect(mDisplaySelectAction, SIGNAL(triggered()), this, SIGNAL(screenClick()));
    addAction(mDisplaySelectAction);

    QIcon showHideIcon;
    showHideIcon.addPixmap(QPixmap(":/images/toolbar/eyeOpened.png"), QIcon::Normal , QIcon::On);
    showHideIcon.addPixmap(QPixmap(":/images/toolbar/eyeClosed.png"), QIcon::Normal , QIcon::Off);
    mShowHideAction = new QAction(showHideIcon, "", this);
    mShowHideAction->setCheckable(true);

    connect(mShowHideAction, SIGNAL(triggered(bool)), this, SLOT(showHideClick(bool)));
    addAction(mShowHideAction);

    setButtonIconSize(QSize(42, 42));

    adjustSizeAndPosition();

    //  This palette can be minimized
    QIcon maximizeIcon;
    maximizeIcon.addPixmap(QPixmap(":/images/toolbar/stylusTab.png"), QIcon::Normal, QIcon::On);
    mMaximizeAction = new QAction(maximizeIcon, tr("Show the stylus palette"), this);
    connect(mMaximizeAction, SIGNAL(triggered()), this, SLOT(maximizeMe()));
    connect(this, SIGNAL(maximizeStart()), this, SLOT(maximizeMe()));
    connect(this, SIGNAL(minimizeStart(eMinimizedLocation)), this, SLOT(minimizeMe(eMinimizedLocation)));
    setMinimizePermission(true);

    connect(rightPalette, SIGNAL(resized()), this, SLOT(parentResized()));
}


UBDesktopPalette::~UBDesktopPalette()
{

}


void UBDesktopPalette::disappearForCapture()
{
    setWindowOpacity(0.0);
    qApp->processEvents();
}


void UBDesktopPalette::appear()
{
    setWindowOpacity(1.0);
}


void UBDesktopPalette::showHideClick(bool checked)
{
    UBApplication::applicationController->mirroringEnabled(checked);
}


void UBDesktopPalette::updateShowHideState(bool pShowEnabled)
{
    if (mShowHideAction)
        mShowHideAction->setChecked(pShowEnabled);

    if (mShowHideAction->isChecked())
        mShowHideAction->setToolTip(tr("Show Board on Secondary Screen"));
    else
        mShowHideAction->setToolTip(tr("Show Desktop on Secondary Screen"));

    if (pShowEnabled)
        raise();
}


void UBDesktopPalette::setShowHideButtonVisible(bool visible)
{
    mShowHideAction->setVisible(visible);
}


void UBDesktopPalette::setDisplaySelectButtonVisible(bool visible)
{
    mDisplaySelectAction->setVisible(visible);
}

//  Called when the palette is near the border and must be minimized
void UBDesktopPalette::minimizeMe(eMinimizedLocation location)
{
    Q_UNUSED(location);
    QList<QAction*> actions;
    clearLayout();

    actions << mMaximizeAction;
    setActions(actions);

    adjustSizeAndPosition();

#ifdef Q_WS_X11
        emit refreshMask();
#endif
}

//  Called when the user wants to maximize the palette
void UBDesktopPalette::maximizeMe()
{
    clearLayout();

    addAction(mActionUniboard);
    addAction(UBApplication::mainWindow->actionPen);
    addAction(UBApplication::mainWindow->actionEraser);
    addAction(UBApplication::mainWindow->actionMarker);
    addAction(UBApplication::mainWindow->actionSelector);
    addAction(UBApplication::mainWindow->actionPointer);
    if (UBPlatformUtils::hasVirtualKeyboard())
        addAction(UBApplication::mainWindow->actionVirtualKeyboard, false);

    addAction(mActionCustomSelect);
    addAction(mDisplaySelectAction);
    addAction(mShowHideAction);

    adjustSizeAndPosition();

    // Notify that the maximization has been done
    emit maximized();
#ifdef Q_WS_X11
        emit refreshMask();
#endif
}

void UBDesktopPalette::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    QIcon penIcon;
    QIcon markerIcon;
    QIcon eraserIcon;
    penIcon.addPixmap(QPixmap(":images/stylusPalette/penArrow.png"), QIcon::Normal, QIcon::Off);
    penIcon.addPixmap(QPixmap(":images/stylusPalette/penOnArrow.png"), QIcon::Normal, QIcon::On);
    UBApplication::mainWindow->actionPen->setIcon(penIcon);
    markerIcon.addPixmap(QPixmap(":images/stylusPalette/markerArrow.png"), QIcon::Normal, QIcon::Off);
    markerIcon.addPixmap(QPixmap(":images/stylusPalette/markerOnArrow.png"), QIcon::Normal, QIcon::On);
    UBApplication::mainWindow->actionMarker->setIcon(markerIcon);
    eraserIcon.addPixmap(QPixmap(":images/stylusPalette/eraserArrow.png"), QIcon::Normal, QIcon::Off);
    eraserIcon.addPixmap(QPixmap(":images/stylusPalette/eraserOnArrow.png"), QIcon::Normal, QIcon::On);
    UBApplication::mainWindow->actionEraser->setIcon(eraserIcon);

}

void UBDesktopPalette::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    QIcon penIcon;
    QIcon markerIcon;
    QIcon eraserIcon;
    penIcon.addPixmap(QPixmap(":images/stylusPalette/pen.png"), QIcon::Normal, QIcon::Off);
    penIcon.addPixmap(QPixmap(":images/stylusPalette/penOn.png"), QIcon::Normal, QIcon::On);
    UBApplication::mainWindow->actionPen->setIcon(penIcon);
    markerIcon.addPixmap(QPixmap(":images/stylusPalette/marker.png"), QIcon::Normal, QIcon::Off);
    markerIcon.addPixmap(QPixmap(":images/stylusPalette/markerOn.png"), QIcon::Normal, QIcon::On);
    UBApplication::mainWindow->actionMarker->setIcon(markerIcon);
    eraserIcon.addPixmap(QPixmap(":images/stylusPalette/eraser.png"), QIcon::Normal, QIcon::Off);
    eraserIcon.addPixmap(QPixmap(":images/stylusPalette/eraserOn.png"), QIcon::Normal, QIcon::On);
    UBApplication::mainWindow->actionEraser->setIcon(eraserIcon);
}

QPoint UBDesktopPalette::buttonPos(QAction *action)
{
    QPoint p;

    UBActionPaletteButton* pB = mMapActionToButton[action];
    if(NULL != pB)
    {
        p = pB->pos();
    }

    return p;
}


int UBDesktopPalette::getParentRightOffset()
{
    return rightPalette->width();
}

void UBDesktopPalette::parentResized()
{
    QPoint p = pos();
    if (minimizedLocation() == eMinimizedLocation_Right)
    {
        p.setX(parentWidget()->width() - getParentRightOffset() -width());
    }

    moveInsideParent(p);
}