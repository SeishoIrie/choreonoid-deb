/**
   @author Shin'ichiro Nakaoka
*/

#ifndef CNOID_BODYPLUGIN_LINK_TREE_WIDGET_H_INCLUDED
#define CNOID_BODYPLUGIN_LINK_TREE_WIDGET_H_INCLUDED

#include "BodyItem.h"
#include <cnoid/SignalProxy>
#include <cnoid/ComboBox>
#include <cnoid/TreeWidget>
#include <boost/dynamic_bitset.hpp>
#include <vector>
#include "exportdecl.h"

namespace cnoid {

    class MenuManager;
    class LinkGroup;
    class LinkTreeWidget;
    class LinkTreeWidgetImpl;

    class CNOID_EXPORT LinkTreeItem : public QTreeWidgetItem
    {
    public:
        LinkTreeItem(const std::string& name);

        inline int rowIndex() const { return rowIndex_; }
        
        inline const std::string& name() const { return name_; }
        inline const Link* link() const { return link_; }
        inline Link* link() { return link_; }
        inline bool isLinkGroup() const { return isLinkGroup_; }

        virtual QVariant data(int column, int role) const;
        virtual void setData(int column, int role, const QVariant& value);

      private:
        int rowIndex_;
        std::string name_;
        Link* link_;
        bool isLinkGroup_;

        LinkTreeItem(Link* link, LinkTreeWidgetImpl* treeImpl);
        LinkTreeItem(LinkGroup* linkGroup, LinkTreeWidgetImpl* treeImpl);

        friend class LinkTreeWidget;
        friend class LinkTreeWidgetImpl;
    };
    
        
    class CNOID_EXPORT LinkTreeWidget : public TreeWidget
    {
        Q_OBJECT

      public:
            
        LinkTreeWidget(QWidget* parent = 0);
        virtual ~LinkTreeWidget();

        void setDefaultExpansionLevel(int level);
        void enableCache(bool on);

        enum ListingMode {
            LINK_LIST, LINK_TREE, JOINT_LIST, JOINT_TREE, PART_TREE };

        ComboBox* listingModeCombo(); 
        void setListingMode(ListingMode mode);
        void fixListingMode(bool on = true);
            
        SignalProxy< boost::signal<void(bool isInitialCreation)> > sigUpdateRequest();
            
        void setBodyItem(BodyItemPtr bodyItem);
        BodyItem* bodyItem();

        typedef boost::function<void(const LinkTreeItem* item, int role, QVariant& out_value)> ColumnDataFunction;
        typedef boost::function<void(const LinkTreeItem* item, int role, const QVariant& value)> ColumnSetDataFunction;
        typedef boost::function<QWidget*(const LinkTreeItem* item)> ColumnWidgetFunction;

        int addColumn();
        int addColumn(const QString& headerText);
        void setColumnDataFunction(int column, ColumnDataFunction func);
        void setColumnSetDataFunction(int column, ColumnSetDataFunction func);
        void setColumnWidgetFunction(int column, ColumnWidgetFunction func);
        void moveVisualColumnIndex(int column, int visualIndex);
        int nameColumn();
        int jointIdColumn();

        void setAlignedItemWidget(LinkTreeItem* item, int column, QWidget* widget, Qt::Alignment alignment = Qt::AlignCenter);
        QWidget* alignedItemWidget(LinkTreeItem* item, int column);

        void addCustomRow(LinkTreeItem* treeItem);
        
        LinkTreeItem* itemOfLink(int linkIndex);

        int numLinkTreeItems();

        SignalProxy< boost::signal<void(LinkTreeItem* item, int column)> > sigItemChanged();
                    
        SignalProxy< boost::signal<void()> > sigSelectionChanged();
        const std::vector<int>& getSelectedLinkIndices();
        const boost::dynamic_bitset<>& getLinkSelection();

        /// This signal is available after calling 'enableCache(true)'.
        SignalProxy< boost::signal<void()> > sigSelectionChanged(BodyItemPtr bodyItem);
        /// This member is available after calling 'enableCache(true)'.
        const std::vector<int>& getSelectedLinkIndices(BodyItemPtr bodyItem);
        /// This member is available after calling 'enableCache(true)'.
        const boost::dynamic_bitset<>& getLinkSelection(BodyItemPtr bodyItem);

        MenuManager& popupMenuManager();

        bool makeSingleSelection(BodyItemPtr bodyItem, int linkIndex);

        void enableArchiveOfCurrentBodyItem(bool on);
        bool storeState(Archive& archive);
        bool restoreState(const Archive& archive);

      protected:
        virtual void changeEvent(QEvent* event);

      private Q_SLOTS:
        void onItemChanged(QTreeWidgetItem* item, int column);
        void onSelectionChanged();
        void onCustomContextMenuRequested(const QPoint& pos);
        void onItemExpanded(QTreeWidgetItem* treeWidgetItem);
        void onItemCollapsed(QTreeWidgetItem* treeWidgetItem);
        void onHeaderSectionResized();
        
      private:
            
        LinkTreeWidgetImpl* impl;

        friend class LinkTreeItem;
    };
}

#endif
