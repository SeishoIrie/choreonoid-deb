/*!
  @file
  @author Shin'ichiro Nakaoka
*/

#include <cnoid/Config>
#include <cnoid/Plugin>
#include <cnoid/App>
#include "PoseSeqItem.h"
#include "PoseSeqEngine.h"
#include "BodyMotionGenerationBar.h"
//#include "PoseSeqView.h"
#include "PoseRollView.h"
#include "FcpFileLoader.h"
#include <boost/format.hpp>
#include "gettext.h"

using namespace cnoid;

namespace {
  
    class PoseSeqPlugin : public Plugin
    {
    public:
        PoseSeqPlugin() : Plugin("PoseSeq") {
            require("Body");
            addOldName("Choreography");
        }

        virtual bool initialize(){

            PoseSeqItem::initializeClass(this);
            initializePoseSeqEngine(this);
            BodyMotionGenerationBar::initializeInstance(this);
            //addView(new PoseSeqView(*this));
            addView(new PoseRollView());
            initializeFcpFileLoader(*this);
            
            return true;
        }

        virtual const char* description() {
            static std::string text =
                str(boost::format(_("PoseSeq Plugin Version %1%\n")) % CNOID_FULL_VERSION_STRING) +
                "\n" +
                _("This plugin has been developed by Shin'ichiro Nakaoka and Choreonoid Development Team, AIST, "
                  "and is distributed as a part of the Choreonoid package.\n"
                  "\n") +
                LGPLtext();

            return text.c_str();
        }
    };
}

CNOID_IMPLEMENT_PLUGIN_ENTRY(PoseSeqPlugin);
