/**
   @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_GUIBASE_GRAPH_WIDGET_H_INCLUDED
#define CNOID_GUIBASE_GRAPH_WIDGET_H_INCLUDED

#include <cnoid/Archive>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/signals.hpp>
#include <QWidget>
#include <QLabel>

#include "exportdecl.h"


namespace cnoid {

    class View;
    class ToolBar;
    class Archive;
    class GraphDataHandler;
    class GraphDataHandlerImpl;
    class GraphWidgetImpl;
    
    typedef boost::shared_ptr<GraphDataHandler> GraphDataHandlerPtr;
    
    class CNOID_EXPORT GraphDataHandler
    {
      public:

        GraphDataHandler();
        ~GraphDataHandler();

        void setColor(float r, float g, float b);
        void setLabel(const std::string& label);
        void setFrameProperties(int numFrames, double frameRate, double offset = 0.0);

        void setValueLimits(double lower, double upper);
        void setVelocityLimits(double lower, double upper);

        void addVerticalLine(double x, const std::string& label);
        void addHorizontalLine(double y, const std::string& label);
        void clearLines();
        
        void update();

        typedef boost::function<void(int frame, int size, double* out_values)> DataRequestCallback;
        void setDataRequestCallback(DataRequestCallback callback);

        typedef boost::function<void(int frame, int size, double* values)> DataModifiedCallback;
        void setDataModifiedCallback(DataModifiedCallback callback);

      private:
        
        friend class GraphWidgetImpl;

        GraphDataHandlerImpl* impl;
    };


    class CNOID_EXPORT GraphWidget : public QWidget, public boost::signals::trackable
    {
      public:

        GraphWidget(View* parentView);
        ~GraphWidget();

        void addDataHandler(GraphDataHandlerPtr handler);
        void clearDataHandlers();

        void setRenderingTypes(bool showOriginalValues, bool showVelocities, bool showAccelerations);
        void getRenderingTypes(bool& showOriginalValues, bool& showVelocities, bool& showAccelerations);

        bool setCursorPosition(double pos);

        void setTimeBarSyncMode(bool on);
        bool isTimeBarSyncMode();
        
        enum ScrollMode { OFF, CONTINUOUS, PAGE };
        void setAutoScrollMode(ScrollMode on);
        ScrollMode autoScrollMode();
        
        void setVerticalValueRange(double lower, double upper);
        void getVerticalValueRange(double& lower, double& upper);

        void setLineWidth(double width);
        double getLineWidth();

        void showRulers(bool show);
        bool showsRulers();
        
        void showLimits(bool show);
        bool showsLimits();
        
        void showGrid(bool show);
        bool showsGrid();
        
        void setGridSize(double width, double height);
        void getGridSize(double& width, double& height);
        
        void setControlPointStep(int step, int offset = 0);
        void getControlPointStep(int& step, int& offset);
        
        void highlightControlPoints(bool on);
        bool highlightsControlPoints();

        enum Mode { VIEW_MODE, EDIT_MODE };
        void changeMode(Mode mode);
        Mode mode();

        enum EditMode { FREE_LINE_MODE, LINE_MODE };
        void changeEditMode(EditMode mode);
        EditMode editMode();

        QLabel& statusLabel();

        virtual bool storeState(Archive& archive);
        virtual bool restoreState(const Archive& archive);

      protected:
        virtual bool eventFilter(QObject* obj, QEvent* event);

      private:

        friend class GraphDataHandler;
        
        GraphWidgetImpl* impl;
    };

}


#endif

