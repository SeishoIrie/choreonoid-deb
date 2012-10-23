/**
   @author Shin'ichiro NAKAOKA
*/

#ifndef CNOID_GUIBASE_BUTTON_H_INCLUDED
#define CNOID_GUIBASE_BUTTON_H_INCLUDED

#include <cnoid/SignalProxy>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QToolButton>
#include "exportdecl.h"

namespace cnoid {

    class CNOID_EXPORT CheckBox : public QCheckBox
    {
        Q_OBJECT

      public:
        CheckBox(QWidget* parent = 0);
        CheckBox(const QString& text, QWidget* parent = 0);
                               
        inline SignalProxy< boost::signal<void(int)> > sigStateChanged() {
            return sigStateChanged_;
        }
        inline SignalProxy< boost::signal<void(bool)> > sigToggled() {
            return sigToggled_;
        }

      private Q_SLOTS:
        void onStateChanged(int state);
        void onToggled(bool checked);

      private:
        boost::signal<void(int)> sigStateChanged_;
        boost::signal<void(bool)> sigToggled_;

        void initialize();
    };


    class CNOID_EXPORT PushButton : public QPushButton
    {
        Q_OBJECT

      public:
        PushButton(QWidget* parent = 0);
	PushButton(const QString& text, QWidget* parent = 0);
	PushButton(const QIcon& icon, const QString& text, QWidget* parent = 0);

        inline SignalProxy< boost::signal<void(bool)> > sigClicked() {
            return sigClicked_;
        }
        inline SignalProxy< boost::signal<void(bool)> > sigToggled() {
            return sigToggled_;
        }

      private Q_SLOTS:
        void onClicked(bool checked);
        void onToggled(bool checked);

      private:
        boost::signal<void(bool)> sigClicked_;
        boost::signal<void(bool)> sigToggled_;

        void initialize();
    };


    class CNOID_EXPORT ToggleButton : public PushButton
    {
      public:
        ToggleButton(QWidget* parent = 0);
	ToggleButton(const QString& text, QWidget* parent = 0);
	ToggleButton(const QIcon& icon, const QString& text, QWidget* parent = 0);
    };
    

    class CNOID_EXPORT RadioButton : public QRadioButton
    {
        Q_OBJECT

      public:
        RadioButton(QWidget* parent = 0);
	RadioButton(const QString & text, QWidget* parent = 0);

        inline SignalProxy< boost::signal<void(bool)> > sigToggled() {
            return sigToggled_;
        }

      private Q_SLOTS:
        void onToggled(bool checked);

      private:
        boost::signal<void(bool)> sigToggled_;

        void initialize();
    };

    class CNOID_EXPORT ToolButton : public QToolButton
    {
        Q_OBJECT

     public:
        ToolButton(QWidget* parent = 0);

        inline SignalProxy< boost::signal<void(bool)> > sigClicked() {
            return sigClicked_;
        }
        inline SignalProxy< boost::signal<void(bool)> > sigToggled() {
            return sigToggled_;
        }

      private Q_SLOTS:
        void onClicked(bool checked);
        void onToggled(bool checked);

      private:
        boost::signal<void(bool)> sigClicked_;
        boost::signal<void(bool)> sigToggled_;

        void initialize();
    };


    class CNOID_EXPORT ToggleToolButton : public ToolButton
    {
      public:
        ToggleToolButton(QWidget* parent = 0);
	//ToggleButton(const QString& text, QWidget* parent = 0);
	//ToggleButton(const QIcon& icon, const QString& text, QWidget* parent = 0);
    };
    
}

#endif
