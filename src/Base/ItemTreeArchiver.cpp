/**
   @author Shin'ichiro Nakaoka
*/

#include "ItemTreeArchiver.h"
#include "RootItem.h"
#include "ItemManager.h"
#include "PluginManager.h"
#include "MessageView.h"
#include "Archive.h"
#include <cnoid/YamlReader>
#include <cnoid/YamlWriter>
#include <vector>
#include "gettext.h"

using namespace std;
using namespace boost;
using namespace cnoid;


ItemTreeArchiver::ItemTreeArchiver()
{
    messageView = MessageView::mainInstance();
}


ItemTreeArchiver::~ItemTreeArchiver()
{

}


ArchivePtr ItemTreeArchiver::store(ArchivePtr parentArchive, ItemPtr item)
{
    itemIdCounter = 0;
    return storeIter(parentArchive, item.get());
}


ArchivePtr ItemTreeArchiver::storeIter(ArchivePtr& parentArchive, Item* item)
{
    string pluginName;
    string className;
    
    if(!ItemManager::getClassIdentifier(item, pluginName, className)){
        messageView->putln(format(_("\"%1%\" cannot be stored. Its type is not registered.")) % item->name());
        return 0;
    }

    ArchivePtr archive = new Archive();
    archive->inheritSharedInfoFrom(parentArchive);

    messageView->putln(format(_("Storing %1% \"%2%\"")) % className % item->name());
    messageView->flush();
    
    ArchivePtr dataArchive = new Archive();
    dataArchive->inheritSharedInfoFrom(parentArchive);

    if(!item->store(*dataArchive)){
        messageView->putln(format(_("\"%1%\" cannot be stored.")) % item->name());
        return 0;
    }

    archive->registerItemId(item, itemIdCounter);
    archive->write("id", itemIdCounter);
    itemIdCounter++;

    archive->write("name", item->name(), YAML_DOUBLE_QUOTED);
    archive->write("plugin", pluginName);
    archive->write("class", className);

    if(!dataArchive->empty()){
        archive->insert("data", dataArchive);
    }

    YamlSequencePtr children = new YamlSequence();

    for(Item* childItem = item->childItem(); childItem; childItem = childItem->nextItem()){
        if(childItem->isSubItem()){
            continue;
        }
        ArchivePtr childArchive = storeIter(archive, childItem);
        if(childArchive){
            children->append(childArchive);
        }
    }

    if(!children->empty()){
        archive->insert("children", children);
    }

    return archive;
}


bool ItemTreeArchiver::restore(ArchivePtr archive, ItemPtr parentItem)
{
    bool result = false;

    archive->setCurrentParentItem(0);
    
    try {
        result = restoreItemIter(archive.get(), parentItem);
    } catch (const YamlNode::Exception& ex){
        messageView->put(ex.message());
    }

    archive->setCurrentParentItem(0);

    return result;
}


bool ItemTreeArchiver::restoreItemIter(Archive* archive, ItemPtr& parentItem)
{
    ItemPtr item;
    
    string pluginName;
    string className;

    if(!(archive->read("plugin", pluginName) && archive->read("class", className))){
        messageView->putln(_("Archive is broken."));
        return false;
    }

    const char* actualPluginName = PluginManager::instance()->guessActualPluginName(pluginName);
    if(actualPluginName){
        item = ItemManager::create(actualPluginName, className);
    }

    if(!item){
        messageView->putln(
            format(_("Item type %1% of %2% cannot be restored. It's not a registered type."))
            % className % pluginName);
    } else {
        bool restored = false;
        string name;
        if(archive->read("name", name)){
            
            item->setName(name);

            bool isRootItem = dynamic_pointer_cast<RootItem>(item);

            if(isRootItem){
                item = parentItem;
                restored = true;
            } else {

                messageView->putln(format(_("Restoring %1% \"%2%\"")) % className % name);
                messageView->flush();

                YamlNodePtr dataNode = archive->find("data");
            
                if(!dataNode->isValid()){
                    restored = true;
                
                } else if(dataNode->type() == YAML_MAPPING){
                    Archive* dataArchive = static_cast<Archive*>(dataNode->toMapping());
                    dataArchive->inheritSharedInfoFrom(archive);
                    dataArchive->setCurrentParentItem(parentItem.get());
                    restored = item->restore(*dataArchive);
                }

                if(restored){
                    parentItem->addChildItem(item);
                }
            }
            
            if(!restored){
                messageView->putln(format(_("%1% \"%2%\" cannot be restored.")) % className % name);
                item = 0;
            } else {
                int id;
                if(archive->read("id", id) && (id >= 0)){
                    archive->registerItemId(item.get(), id);
                }

                YamlSequencePtr children = archive->findSequence("children");
                if(children->isValid()){
                    for(int i=0; i < children->size(); ++i){
                        Archive* childArchive = dynamic_cast<Archive*>(children->get(i).toMapping());
                        childArchive->inheritSharedInfoFrom(archive);
                        restoreItemIter(childArchive, item);
                    }
                }
            }
        }
    }
    
    return (item);
}
