/**
   @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_GUIBASE_PROJECT_MANAGER_H_INCLUDED
#define CNOID_GUIBASE_PROJECT_MANAGER_H_INCLUDED

#include "Archive.h"
#include <string>
#include <boost/function.hpp>
#include "exportdecl.h"

namespace cnoid {

    class ExtensionManager;
    class ProjectManagerImpl;

    class CNOID_EXPORT ProjectManager
    {
      public:
        static ProjectManager* instance();
        
        void loadProject(const std::string& filename);
        void saveProject(const std::string& filename);
        void overwriteCurrentProject();

        static void initialize(ExtensionManager* em);

      private:
        ProjectManager(ExtensionManager* em);
        ~ProjectManager();

        ProjectManagerImpl* impl;

        friend class ExtensionManager;
        friend class ExtensionManagerImpl;

        void connectArchiver(
            const std::string& moduleName,
            const std::string& objectName,
            boost::function<bool(Archive&)> storeFunction,
            boost::function<void(const Archive&)> restoreFunction);

        void disconnectArchivers(const std::string& moduleName);
    };
}

#endif
