/**
   @author Shin'ichiro NAKAOKA
*/

#include "Button.h"

using namespace cnoid;

CheckBox::CheckBox(QWidget* parent)
    : QCheckBox(parent)
{
    initialize();
}


CheckBox::CheckBox(const QString& text, QWidget* parent)
    : QCheckBox(text, parent)
{
    initialize();
}


void CheckBox::initialize()
{
    connect(this, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
}


void CheckBox::onStateChanged(int state)
{
    sigStateChanged_(state);
}

void CheckBox::onToggled(bool checked)
{
    sigToggled_(checked);
}


PushButton::PushButton(QWidget* parent)
    : QPushButton(parent)
{
    initialize();
}
    

PushButton::PushButton(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
    initialize();
}


PushButton::PushButton(const QIcon & icon, const QString & text, QWidget* parent)
    : QPushButton(icon, text, parent)
{
    initialize();
}


void PushButton::initialize()
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
}


void PushButton::onClicked(bool checked)
{
    sigClicked_(checked);
}


void PushButton::onToggled(bool checked)
{
    sigToggled_(checked);
}


ToggleButton::ToggleButton(QWidget* parent)
    : PushButton(parent)
{
    setCheckable(true);
}


ToggleButton::ToggleButton(const QString& text, QWidget* parent)
    : PushButton(text, parent)
{
    setCheckable(true);
}
    

ToggleButton::ToggleButton(const QIcon& icon, const QString& text, QWidget* parent)
    : PushButton(icon, text, parent)
{
    setCheckable(true);
}


RadioButton::RadioButton(QWidget* parent)
    : QRadioButton(parent)
{
    initialize();
}


RadioButton::RadioButton(const QString& text, QWidget* parent)
    : QRadioButton(text, parent)
{
    initialize();
}


void RadioButton::initialize()
{
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
}


void RadioButton::onToggled(bool checked)
{
    sigToggled_(checked);
}


ToolButton::ToolButton(QWidget* parent)
    : QToolButton(parent)
{
    initialize();
}
    

/*
ToolButton::ToolButton(const QString& text, QWidget* parent)
    : QToolButton(text, parent)
{
    initialize();
}


ToolButton::ToolButton(const QIcon & icon, const QString & text, QWidget* parent)
    : QToolButton(icon, text, parent)
{
    initialize();
}
*/


void ToolButton::initialize()
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
}


void ToolButton::onClicked(bool checked)
{
    sigClicked_(checked);
}


void ToolButton::onToggled(bool checked)
{
    sigToggled_(checked);
}


ToggleToolButton::ToggleToolButton(QWidget* parent)
    : ToolButton(parent)
{
    setCheckable(true);
}
