/**
   @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_GUIBASE_GRAPH_BAR_H_INCLUDED
#define CNOID_GUIBASE_GRAPH_BAR_H_INCLUDED

#include <cnoid/ToolBar>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/signals.hpp>

#include "exportdecl.h"

namespace cnoid {

    class GraphWidget;
    class GraphBarImpl;
    
    class CNOID_EXPORT GraphBar : public ToolBar
    {
      public:
        static void initialize(ExtensionManager* ext);
        static GraphBar* instance();

        GraphWidget* focusedGraphWidget();
        void focus(GraphWidget* graphWidget, bool forceUpdate = false);
        void releaseFocus(GraphWidget* graphWidget);

      private:
        GraphBar();
        virtual ~GraphBar();

        GraphBarImpl* impl;
    };

}


#endif

