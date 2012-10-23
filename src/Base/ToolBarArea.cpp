/**
   @author Shin'ichiro Nakaoka
*/

#include "ToolBarArea.h"
#include "Separator.h"
#include "ToolBar.h"
#include "MainWindow.h"
#include "LazyCaller.h"
#include <cnoid/YamlNodes>
#include <QResizeEvent>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <vector>
#include <list>
#include <limits>
#include <algorithm>

#include <iostream>

using namespace std;
using namespace boost;
using namespace cnoid;

namespace {

    const bool DEBUG_MODE = false;

    typedef std::vector<ToolBar*> ToolBarList;
        
    class ToolBarRow
    {
    public:
        ToolBarRow(ToolBarArea* toolBarArea) :
            toolBarArea(toolBarArea),
            separator(toolBarArea) {
            height = 0;
            separator.hide();
        }

        ToolBarArea* toolBarArea;
        ToolBarList toolBars;
        HSeparator separator;
        int height;
    };
    typedef shared_ptr<ToolBarRow> ToolBarRowPtr;


    // for archiving
    struct LayoutState
    {
    public:
        string name;
        int desiredX;
        int layoutPriority;
    };

    typedef list<LayoutState> RowLayoutState;
    typedef list<RowLayoutState> AreaLayoutState;
}


namespace cnoid {

    class ToolBarAreaImpl
    {
    public:
        ToolBarAreaImpl(ToolBarArea* self);
        ~ToolBarAreaImpl();

        void getAllToolBars(std::vector<ToolBar*>& bars);
        void storeLayout(YamlMappingPtr layout);
        void restoreLayout(const YamlMappingPtr layout);
        bool addToolBar(ToolBar* toolBar);
        void removeToolBar(ToolBar* toolBar);
        void dragToolBar(ToolBar* toolBar, const QPoint& globalPos);
        
        void setNewToolBars();
        void expandStrechableBars
        (ToolBarRowPtr& row, int portion, int shift, int barIndex, int numStrechables, int lastSpace);

        void useSpace(ToolBarRowPtr& row, int space, int shift, int barIndex, int numStrechables, int allspace );
        void setNewToolBar(ToolBar* toolBar, vector<int>& numStrechablesOfRow);
        void layoutToolBars();
        void layoutToolBarRow(ToolBarRowPtr toolBarRow, int& io_rowTop, bool isNewBottomRow);
        void layoutToolBarRowWithDraggedToolBar(ToolBarList& toolBars, int rowTop, int rowHeight);
        int normalizeLayoutPriorities(ToolBarList& toolBars);
        void layoutToolBarRowPart(ToolBarList& toolBars, int rowTop, int rowHeight, int partLeft, int partRight);

        void resizeEvent(QResizeEvent* event);

        ToolBarArea* self;

        bool isBeforeDoingInitialLayout;
        ToolBarList newToolBars;
        deque<ToolBarRowPtr> toolBarRows;
        ToolBarRowPtr spilledToolBarRow;
        YamlMappingPtr initialLayout;

        int areaHeight;
        int prevAreaHeight;

        ToolBar* draggedToolBar;
        bool draggedToolBarHasNotBeenInserted;
        int dragY;

        LazyCaller layoutToolBarsCaller;
    };
}


ToolBarArea::ToolBarArea(QWidget* parent)
    : QWidget(parent)
{
    impl = new ToolBarAreaImpl(this);
}


ToolBarAreaImpl::ToolBarAreaImpl(ToolBarArea* self)
    : self(self),
      spilledToolBarRow(new ToolBarRow(self)),
      layoutToolBarsCaller(bind(&ToolBarAreaImpl::layoutToolBars, this))
{
    isBeforeDoingInitialLayout = true;
    
    draggedToolBar = 0;
    draggedToolBarHasNotBeenInserted = false;

    areaHeight = 0;
    prevAreaHeight = 0;
}


ToolBarArea::~ToolBarArea()
{
    delete impl;
}


ToolBarAreaImpl::~ToolBarAreaImpl()
{

}


std::vector<ToolBar*> ToolBarArea::getAllToolBars()
{
    std::vector<ToolBar*> bars;
    impl->getAllToolBars(bars);
    return bars;
}


void ToolBarAreaImpl::getAllToolBars(std::vector<ToolBar*>& bars)
{
    bars.clear();
    
    for(size_t i=0; i < toolBarRows.size(); ++i){
        ToolBarRowPtr& row = toolBarRows[i];
        vector<ToolBar*>& toolBars = row->toolBars;
        for(size_t j=0; j < toolBars.size(); ++j){
            bars.push_back(toolBars[j]);
        }
    }
}


bool ToolBarArea::addToolBar(ToolBar* toolBar)
{
    return impl->addToolBar(toolBar);
}


bool ToolBarAreaImpl::addToolBar(ToolBar* toolBar)
{
    if(toolBar){
        if(!toolBar->toolBarArea()){
            toolBar->toolBarArea_ = self;
            newToolBars.push_back(toolBar);

            if(!isBeforeDoingInitialLayout){
                layoutToolBarsCaller.request();
            }
            return true;
        }
    }
    return false;
}


void ToolBarArea::removeToolBar(ToolBar* toolBar)
{
    if(toolBar){
        impl->removeToolBar(toolBar);
    }
}


void ToolBarAreaImpl::removeToolBar(ToolBar* toolBar)
{
    for(size_t i=0; i < toolBarRows.size(); ++i){
        ToolBarRowPtr row = toolBarRows[i];
        vector<ToolBar*>& toolBars = row->toolBars;
        for(size_t j=0; j < toolBars.size(); ++j){
            if(toolBars[j] == toolBar){
                toolBar->hide();
                toolBar->setParent(0);
                toolBar->toolBarArea_ = 0;
                toolBars.erase(toolBars.begin() + j);

                if(!isBeforeDoingInitialLayout){
                    layoutToolBarsCaller.request();
                }
                return;
            }
        }
    }
}


void ToolBarArea::setInitialLayout(YamlMappingPtr layout)
{
    if(impl->isBeforeDoingInitialLayout){
        impl->initialLayout = layout;
    }
}


void ToolBarArea::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    impl->resizeEvent(event);
}


void ToolBarAreaImpl::resizeEvent(QResizeEvent* event)
{
    if(DEBUG_MODE){
        cout << "ToolBarAreaImpl::resizeEvent(" <<
            event->size().width() << ", " << event->size().height() << ")";
        cout << ", isVisible =" << self->isVisible() << endl;
    }

    if(isBeforeDoingInitialLayout){
        if(!(MainWindow::instance()->windowState() & (Qt::WindowMaximized | Qt::WindowFullScreen)) || self->isVisible()){
            isBeforeDoingInitialLayout = false;
            if(initialLayout){
                restoreLayout(initialLayout);
                initialLayout = 0;
            } else {
                layoutToolBars();
            }
        }
    } else {
        if(event->size().width() != event->oldSize().width()){
            layoutToolBars();
        }
    }
}


void ToolBarArea::doInitialLayout()
{
    if(DEBUG_MODE){
        cout << "ToolBarAreaImpl::doInitialLayout()" << endl;
    }
    
    if(impl->isBeforeDoingInitialLayout){
        impl->isBeforeDoingInitialLayout = false;
        if(impl->initialLayout){
            impl->restoreLayout(impl->initialLayout);
            impl->initialLayout = 0;
        } else {
            impl->layoutToolBars();
        }
    }
}


bool ToolBarArea::event(QEvent* event)
{
    if(event->type() == QEvent::LayoutRequest){
        if(DEBUG_MODE){
            cout << "ToolBarArea::event(QEvent::LayoutRequest)" << endl;
        }
        impl->layoutToolBars();
        return true;
    }
    return QWidget::event(event);
}


void ToolBarArea::storeLayout(YamlMappingPtr layout)
{
    impl->storeLayout(layout);
}


void ToolBarAreaImpl::storeLayout(YamlMappingPtr layout)
{
    YamlMapping* layoutOfToolBars = layout->createMapping("layoutOfToolBars");
    YamlSequence* rows = layoutOfToolBars->createSequence("rows");
    
    for(size_t i=0; i < toolBarRows.size(); ++i){
        ToolBarList& toolBars = toolBarRows[i]->toolBars;
        if(!toolBars.empty()){
            YamlSequence* bars = new YamlSequence();
            for(ToolBarList::iterator p = toolBars.begin(); p != toolBars.end(); ++p){
                ToolBar* toolBar = *p;
                YamlMapping* state = new YamlMapping();
                state->setFlowStyle(true);
                state->write("name", toolBar->objectName().toStdString(), YAML_DOUBLE_QUOTED);
                state->write("x", toolBar->desiredX);
                state->write("priority", toolBar->layoutPriority);
                bars->append(state);
            }
            rows->append(bars);
        }
    }
}


void ToolBarArea::restoreLayout(const YamlMappingPtr layout)
{
    impl->restoreLayout(layout);
}


void ToolBarAreaImpl::restoreLayout(const YamlMappingPtr layout)
{
    if(DEBUG_MODE){
        cout << "ToolBarAreaImpl::restoreLayout()" << endl;
    }

    if(isBeforeDoingInitialLayout){
        initialLayout = layout;
        return;
    }
       
    const YamlMappingPtr layoutOfToolBars = layout->findMapping("layoutOfToolBars");
    if(!layoutOfToolBars->isValid()){
        layoutToolBars();
        return;
    }
    
    // make the map from name to toolBar
    typedef map<QString, ToolBar*> ToolBarMap;
    ToolBarMap toolBarMap;
    for(size_t i=0; i < toolBarRows.size(); ++i){
        ToolBarList& toolBars = toolBarRows[i]->toolBars;
        for(ToolBarList::iterator q = toolBars.begin(); q != toolBars.end(); ++q){
            ToolBar* toolBar = *q;
            toolBarMap[toolBar->windowTitle()] = toolBar;
        }
    }
    toolBarRows.clear();

    for(size_t i=0; i < newToolBars.size(); ++i){
        toolBarMap[newToolBars[i]->windowTitle()] = newToolBars[i];
        newToolBars[i]->setParent(self);
    }
    newToolBars.clear();

    const YamlSequence* rows = layoutOfToolBars->get("rows").toSequence();

    for(int i=0; i < rows->size(); ++i){
        ToolBarRowPtr row(new ToolBarRow(self));
        toolBarRows.push_back(row);
        const YamlSequence* bars = rows->get(i).toSequence();
        for(int j=0; j < bars->size(); ++j){
            const YamlMapping* state = bars->get(j).toMapping();
            ToolBarMap::iterator it = toolBarMap.find(QString(state->get("name").toString().c_str()));
            if(it != toolBarMap.end()){
                ToolBar* toolBar = it->second;
                toolBarMap.erase(it);
                row->toolBars.push_back(toolBar);
                toolBar->desiredX = state->get("x").toInt();
                toolBar->layoutPriority = state->get("priority").toInt();
            }
        }
        if(row->toolBars.empty()){
            toolBarRows.pop_back();
        }
    }

    for(ToolBarMap::iterator p = toolBarMap.begin(); p != toolBarMap.end(); ++p){
        newToolBars.push_back(p->second);
    }

    layoutToolBars();
}


void ToolBarArea::dragToolBar(ToolBar* toolBar, const QPoint& globalPos)
{
    impl->dragToolBar(toolBar, globalPos);
}


void ToolBarAreaImpl::dragToolBar(ToolBar* toolBar, const QPoint& globalPos)
{
    QPoint p = self->mapFromGlobal(globalPos);
        
    draggedToolBar = toolBar;
    draggedToolBarHasNotBeenInserted = true;

    draggedToolBar->desiredX = p.x();
    dragY = p.y();

    if(p.y() < 0){
        toolBarRows.push_front(ToolBarRowPtr(new ToolBarRow(self)));
    }
    
    layoutToolBars();
    draggedToolBar = 0;
}


void ToolBarAreaImpl::setNewToolBars()
{
    vector<int> numStrechablesOfRow;

    for(size_t i=0; i < newToolBars.size(); ++i){
        setNewToolBar(newToolBars[i], numStrechablesOfRow);
    }
    newToolBars.clear();

    for(size_t i=0; i < numStrechablesOfRow.size(); ++i){
        int numStrechables = numStrechablesOfRow[i];
        if(numStrechables > 0){
            ToolBarRowPtr& row = toolBarRows[i];
            ToolBar* lastToolBar = row->toolBars.back();
            QRect r = lastToolBar->geometry();
            int space = self->width() - (r.x() + r.width());
            if(space > 0){
                expandStrechableBars(row, (space / numStrechables), 0, 0, numStrechables, space);
            }
        }
    }
}


void ToolBarAreaImpl::expandStrechableBars
(ToolBarRowPtr& row, int portion, int shift, int barIndex, int numStrechables, int lastSpace)
{
    ToolBar* bar = row->toolBars[barIndex];
    bar->desiredX += shift;
    if(bar->isStretchable()){
        int addition = (numStrechables > 1) ? portion : lastSpace;
        shift += addition;
        lastSpace -= addition;
        --numStrechables;
    }
    ++barIndex;
    if(barIndex < (int)row->toolBars.size()){
        expandStrechableBars(row, portion, shift, barIndex,  numStrechables, lastSpace);
    }
}


void ToolBarAreaImpl::setNewToolBar(ToolBar* toolBar, vector<int>& numStrechablesOfRow)
{
    if(DEBUG_MODE){
        cout << "ToolBarAreaImpl::setNewToolBar()" << endl;
    }
    
    if(toolBar){

        ToolBarRowPtr toolBarRow;
        int rowIndex = -1;
        int width = toolBar->minimumSizeHint().width();

        for(size_t i=0; i < toolBarRows.size(); ++i){
            ToolBarRowPtr existingRow = toolBarRows[i];
            ToolBar* lastToolBar = existingRow->toolBars.back();
            if(lastToolBar){
                QRect r = lastToolBar->geometry();
                int lastX = r.x() + r.width();
                int lastSpace = self->width() - lastX;
                if(width <= lastSpace){
                    toolBar->desiredX = lastX + 1;
                    if(toolBar->isStretchable()){
                        width = std::min(toolBar->sizeHint().width(), lastSpace);
                    }
                    toolBarRow = existingRow;
                    rowIndex = i;
                    break;
                }
            }
        }
        if(!toolBarRow){
            toolBar->desiredX = 0;
            rowIndex = toolBarRows.size();
            toolBarRow.reset(new ToolBarRow(self));
            toolBarRows.push_back(toolBarRow);
            numStrechablesOfRow.push_back(0);
        }
        
        toolBarRow->toolBars.push_back(toolBar);
        if(toolBar->isStretchable()){
            numStrechablesOfRow[rowIndex]++;
        }
        toolBar->setParent(self);
        toolBar->setGeometry(toolBar->desiredX, 0, width, toolBar->sizeHint().height());
        toolBar->show();
    }
}


void ToolBarAreaImpl::layoutToolBars()
{
    if(DEBUG_MODE){
        cout << "ToolBarAreaImpl::layoutToolBars()" << endl;
    }

    if(!newToolBars.empty()){
        setNewToolBars();
    }

    if(DEBUG_MODE){
        cout << "actually do ToolBarAreaImpl::layoutToolBars()" << endl;
    }
    areaHeight = 0;
    int rowTop = 0;
    spilledToolBarRow->toolBars.clear();
    
    size_t i = 0;
    while(i < toolBarRows.size()){
        layoutToolBarRow(toolBarRows[i], rowTop, false);
        if(!spilledToolBarRow->toolBars.empty()){
            layoutToolBarRow(spilledToolBarRow, rowTop, false);
        }
        if(toolBarRows[i]->toolBars.empty()){
            toolBarRows.erase(toolBarRows.begin() + i);
        } else {
            ++i;
        }
    }
    if(draggedToolBarHasNotBeenInserted){
        ToolBarRowPtr row(new ToolBarRow(self));
        toolBarRows.push_back(row);
        layoutToolBarRow(row, rowTop, true);
    }
    
    areaHeight = rowTop;

    if(areaHeight != prevAreaHeight){
        self->setMinimumHeight(areaHeight);
        prevAreaHeight = areaHeight;
    }

    isBeforeDoingInitialLayout = false;
}


void ToolBarAreaImpl::layoutToolBarRow(ToolBarRowPtr toolBarRow, int& io_rowTop, bool isNewBottomRow)
{
    int rowHeight = 0;

    bool draggedToolBarExists = false;
    ToolBarList& toolBars = toolBarRow->toolBars;

    // calculate the row height and remove the dragged tool bar if it is originally in this row
    ToolBarList::iterator p = toolBars.begin();
    while(p != toolBars.end()){
        ToolBar* toolBar = *p;
        if(toolBar == draggedToolBar){
            draggedToolBarExists = true;
            p = toolBars.erase(p);
        } else {
            rowHeight = std::max(rowHeight, toolBar->sizeHint().height());
            ++p;
        }
    }

    // Is the dragged tool bar moving to this row ?
    bool isDraggedToolBarMovingToThisRow = false;
    
    if(draggedToolBarHasNotBeenInserted){
        int bottomBorder = 0;
        if(rowHeight == 0 && draggedToolBarExists){
            int h = draggedToolBar->sizeHint().height();
            bottomBorder = io_rowTop + h * 4 / 5;
        } else {
            bottomBorder = io_rowTop + rowHeight;
        }
        if(dragY < bottomBorder || isNewBottomRow){
            isDraggedToolBarMovingToThisRow = true;
            rowHeight = std::max(rowHeight, draggedToolBar->sizeHint().height());
            layoutToolBarRowWithDraggedToolBar(toolBars, io_rowTop, rowHeight);
        }
    }

    if(toolBars.empty()){
        toolBarRow->separator.hide();
    } else {
        if(!isDraggedToolBarMovingToThisRow){
            layoutToolBarRowPart(toolBars, io_rowTop, rowHeight, 0, self->width());
        }

        for(ToolBarList::iterator p = toolBars.begin(); p != toolBars.end(); ++p){
            ToolBar* toolBar = *p;
            if(toolBar->height() != rowHeight){
                toolBar->setGeometry(toolBar->x(), io_rowTop, toolBar->width(), rowHeight);
            }
        }
    
        io_rowTop += rowHeight;

        HSeparator& sep = toolBarRow->separator;
        int h = sep.sizeHint().height();
        sep.setGeometry(0, io_rowTop, self->width(), h);
        sep.show();
        io_rowTop += sep.sizeHint().height();
    }
}


void ToolBarAreaImpl::layoutToolBarRowWithDraggedToolBar(ToolBarList& toolBars, int rowTop, int rowHeight)
{
    int maxPriorty = normalizeLayoutPriorities(toolBars);
    draggedToolBar->layoutPriority = maxPriorty + 1;

    for(size_t i=0; i < toolBars.size(); ++i){
        if(draggedToolBar->desiredX <= toolBars[i]->desiredX){
            toolBars.insert(toolBars.begin() + i, draggedToolBar);
            draggedToolBarHasNotBeenInserted = false;
            break;
        }
    }
    if(draggedToolBarHasNotBeenInserted){
        toolBars.push_back(draggedToolBar);
        draggedToolBarHasNotBeenInserted = false;
    }

    layoutToolBarRowPart(toolBars, rowTop, rowHeight, 0, self->width());

    draggedToolBar->desiredX = draggedToolBar->geometry().x();
}


/**
   @return The maximum priority in the normalized priorities
*/
int ToolBarAreaImpl::normalizeLayoutPriorities(ToolBarList& toolBars)
{
    int maxPriority = 0;

    if(!toolBars.empty()){

        ToolBarList sortedToolBars(toolBars);
        std::sort(sortedToolBars.begin(), sortedToolBars.end(), ToolBar::LayoutPriorityCmp());

        int prevOldPriority = -1;
        int priority = -1;
        for(size_t i=0; i < sortedToolBars.size(); ++i){
            ToolBar* bar = sortedToolBars[i];
            if(bar->layoutPriority > prevOldPriority){
                prevOldPriority = bar->layoutPriority;
                priority += 1;
            }
            bar->layoutPriority = priority;
            if(priority > maxPriority){
                maxPriority = priority;
            }
        }
    }

    return maxPriority;
}


void ToolBarAreaImpl::layoutToolBarRowPart
(ToolBarList& toolBars, int rowTop, int rowHeight, int partLeft, int partRight)
{
    // find the index of the tool bar with the maximum priority
    int pivotIndex = 0;
    int maxPriority = -1;
    for(size_t i=0; i < toolBars.size(); ++i){
        if(toolBars[i]->layoutPriority > maxPriority){
            maxPriority = toolBars[i]->layoutPriority;
            pivotIndex = i;
        }
    }

    ToolBarList leftPartToolBars;
    int pivotAreaLeft = partLeft;
    for(int i=0; i < pivotIndex; ++i){
        leftPartToolBars.push_back(toolBars[i]);
        pivotAreaLeft += toolBars[i]->minimumSizeHint().width();
    }

    ToolBarList rightPartToolBars;
    int pivotAreaRight = partRight;
    for(size_t i = pivotIndex + 1; i < toolBars.size(); ++i){
        rightPartToolBars.push_back(toolBars[i]);
        pivotAreaRight -= toolBars[i]->minimumSizeHint().width();
    }

    ToolBar* pivotToolBar = toolBars[pivotIndex];
    int x = pivotToolBar->desiredX;
    const QSize size = pivotToolBar->minimumSizeHint();
    int width = size.width();
    if(width < 0){
        width = 0;
    }
    if(x + width > pivotAreaRight){
        x = pivotAreaRight - width;
    }
    if(x < pivotAreaLeft){
        x = pivotAreaLeft;
    }

    if(!leftPartToolBars.empty()){
        layoutToolBarRowPart(leftPartToolBars, rowTop, rowHeight, partLeft, x);
        QRect r = leftPartToolBars.back()->geometry();
        int leftPartRight = r.x() + r.width();
        if(x <= leftPartRight){
            x = leftPartRight + 1;
        }
    }

    int rightPartLeft = partRight;
    if(!rightPartToolBars.empty()){
        layoutToolBarRowPart(rightPartToolBars, rowTop, rowHeight, x + width, partRight);
        rightPartLeft = rightPartToolBars.front()->geometry().x();
    }

    if(pivotToolBar->isStretchable()){
        width = pivotToolBar->maximumWidth();
        int possibleWidth = rightPartLeft - x;
        if(width > possibleWidth){
            width = possibleWidth;
        }
    }

    int height = (size.height() >= 0) ? size.height() : rowHeight;
    int y = rowTop + (rowHeight - height) / 2;

    pivotToolBar->setGeometry(x, y, width, height);
    pivotToolBar->show();
}


