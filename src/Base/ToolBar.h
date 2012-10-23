/**
   @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_GUIBASE_TOOL_BAR_H_INCLUDED
#define CNOID_GUIBASE_TOOL_BAR_H_INCLUDED

#include "Button.h"
#include <QLabel>
#include <QWidget>
#include <QBoxLayout>
#include <QAction>
#include "exportdecl.h"

namespace cnoid {

    class Archive;
    class ExtensionManager;
    class ToolBarArea;
	class MainWindow;

    class CNOID_EXPORT ToolBar : public QWidget
    {
        Q_OBJECT
      public:

        ToolBar(const QString& title);
        virtual ~ToolBar();

        ToolButton* addButton(const QString& text, const QString& tooltip = QString());
        ToolButton* addButton(const QIcon& icon, const QString& tooltip = QString());
        ToolButton* addButton(const char* const* xpm, const QString& tooltip = QString());

        ToolButton* addToggleButton(const QString& text, const QString& tooltip = QString());
        ToolButton* addToggleButton(const QIcon& icon, const QString& tooltip = QString());
        ToolButton* addToggleButton(const char* const* xpm, const QString& tooltip = QString());

        void requestNewRadioGroup();
        QButtonGroup* currentRadioGroup();
        
        ToolButton* addRadioButton(const QString& text, const QString& tooltip = QString());
        ToolButton* addRadioButton(const QIcon& icon, const QString& tooltip = QString());
        ToolButton* addRadioButton(const char* const* xpm, const QString& tooltip = QString());

        void addAction(QAction* action);
        void addWidget(QWidget* widget);
        QLabel* addLabel(const QString& text);
        QLabel* addImage(const QString& filename);
        QWidget* addSeparator(int spacing = 0);
        void addSpacing(int size);

        inline void setStretchable(bool on) { isStretchable_ = on; }
        inline bool isStretchable() { return isStretchable_; }

        inline ToolBarArea* toolBarArea() { return toolBarArea_; }

        class LayoutPriorityCmp {
        public:
            inline bool operator() (ToolBar* bar1, ToolBar* bar2) {
                return (bar1->layoutPriority < bar2->layoutPriority);
            }
        };
        
        virtual bool storeState(Archive& archive);
        virtual bool restoreState(const Archive& archive);

      public Q_SLOTS:
        void setEnabled(bool on);
        void changeIconSize(const QSize& iconSize);

      private:

        QHBoxLayout* hbox;
        QWidget* handle;
        QButtonGroup* radioGroup;
        bool isNewRadioGroupRequested;
        MainWindow* mainWindow;
        ToolBarArea* toolBarArea_;

        // used for layouting tool bars on a ToolBarArea
        bool isStretchable_;
        int desiredX;
        int layoutPriority;

        void setRadioButton(ToolButton* button);

        friend class ToolBarAreaImpl;

        void changeIconSizeSub(QLayout* layout, const QSize& iconSize);
    };
}

#endif
