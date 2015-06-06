/*
 * Journal.cpp
 *
 *  Created on: Aug 22, 2013
 *  \copyright 2013 DCBlaha.  Distributed under the GPL.
 */

#include "Journal.h"

static Journal *gJournal;


JournalListener::~JournalListener()
    {
    }

JournalRecord::~JournalRecord()
    {
    }

std::string JournalRecord::getFullName(bool addSpace) const
    {
    std::string fullName = getRecordTypeName();
    if(addSpace)
	fullName += ' ';
    fullName += getName();
    return fullName;
    }

Journal::Journal():
    mCurrentRecord(0), mBuilder(nullptr), mModel(nullptr),
    mJournalListener(nullptr), mTaskStatusListener(nullptr)
    {
    gJournal = this;
    }

Journal::~Journal()
    {
    }

Journal *Journal::getJournal()
    {
    return gJournal;
    }

void Journal::clear()
    {
    for(auto &rec : mRecords)
	{
	delete rec;
	}
    mRecords.clear();
    }

void Journal::displayClass(OovStringRef const className)
    {
    JournalRecordClassDiagram *rec;
    size_t recordIndex = findRecord(RT_Class, className);
    if(recordIndex == NO_INDEX)
	{
	rec = new JournalRecordClassDiagram(*mBuilder, *mModel,
		*mJournalListener, *mTaskStatusListener);
	addRecord(rec, className);
	rec->mClassDiagram.clearGraphAndAddClass(className,
		ClassGraph::AN_ClassesAndChildren);
	}
    else
	{
	setCurrentRecord(recordIndex);
	rec = reinterpret_cast<JournalRecordClassDiagram*>(getCurrentRecord());
	rec->mClassDiagram.drawToDrawingArea();
	}
    }

void Journal::addClass(OovStringRef const className)
    {
    JournalRecord *rec = getCurrentRecord();
    if(rec && rec->getRecordType() == RT_Class)
	{
	JournalRecordClassDiagram *recClass =
		reinterpret_cast<JournalRecordClassDiagram*>(rec);
	recClass->mClassDiagram.addClass(className, ClassGraph::AN_AllStandard);
	}
    }

void Journal::displayOperation(OovStringRef const className,
	OovStringRef const operName, bool isConst)
    {
    JournalRecordOperationDiagram *rec;
    OovString fullOperName = operName;
    fullOperName += ' ';
    fullOperName += className;
    size_t recordIndex = findRecord(RT_Sequence, fullOperName);
    if(recordIndex == NO_INDEX)
	{
	rec = new JournalRecordOperationDiagram(*mBuilder, *mModel,
		*mJournalListener);
	addRecord(rec, fullOperName);
	rec->mOperationDiagram.clearGraphAndAddOperation(className, operName, isConst);
	}
    else
	{
	setCurrentRecord(recordIndex);
	rec = reinterpret_cast<JournalRecordOperationDiagram*>(getCurrentRecord());
	rec->mOperationDiagram.drawToDrawingArea();
	}
    }

void Journal::displayComponents()
    {
    if(mBuilder)
	{
	OovStringRef const componentName = "<Components>";
	size_t recordIndex = findRecord(RT_Component, componentName);
	JournalRecordComponentDiagram *rec;
	if(recordIndex == NO_INDEX)
	    {
	    rec = new JournalRecordComponentDiagram(*mBuilder, *mIncludeMap,
	            *mJournalListener);
	    rec->mComponentDiagram.drawToDrawingArea();
	    addRecord(rec, componentName);
	    }
	else
	    {
	    setCurrentRecord(recordIndex);
	    rec = reinterpret_cast<JournalRecordComponentDiagram*>(getCurrentRecord());
	    rec->mComponentDiagram.drawToDrawingArea();
	    }
	}
    }

void Journal::displayWorldZone()
    {
    if(mBuilder)
	{
	OovStringRef const zoneName = "<WorldZone>";
	size_t recordIndex = findRecord(RT_Zone, zoneName);
	JournalRecordZoneDiagram *rec;
	if(recordIndex == NO_INDEX)
	    {
	    rec = new JournalRecordZoneDiagram(*mBuilder, *mModel, *mJournalListener);
	    rec->mZoneDiagram.drawToDrawingArea();
	    addRecord(rec, zoneName);
	    rec->mZoneDiagram.clearGraphAndAddWorldZone();
	    }
	else
	    {
	    setCurrentRecord(recordIndex);
	    rec = reinterpret_cast<JournalRecordZoneDiagram*>(getCurrentRecord());
	    rec->mZoneDiagram.drawToDrawingArea();
	    }
	}
    }

void Journal::displayPortion(OovStringRef const className)
    {
    if(mBuilder)
	{
	size_t recordIndex = findRecord(RT_Portion, className);
	JournalRecordPortionDiagram *rec;
	if(recordIndex == NO_INDEX)
	    {
	    rec = new JournalRecordPortionDiagram(*mBuilder, *mModel, *mJournalListener);
	    rec->mPortionDiagram.clearGraphAndAddClass(className);
	    rec->mPortionDiagram.drawToDrawingArea();
	    addRecord(rec, className);
	    }
	else
	    {
	    setCurrentRecord(recordIndex);
	    rec = reinterpret_cast<JournalRecordPortionDiagram*>(getCurrentRecord());
	    rec->mPortionDiagram.drawToDrawingArea();
	    }
	}
    }

void Journal::displayInclude(OovStringRef const incName)
    {
    if(mBuilder)
        {
        size_t recordIndex = findRecord(RT_Include, incName);
        JournalRecordIncludeDiagram *rec;
        if(recordIndex == NO_INDEX)
            {
            rec = new JournalRecordIncludeDiagram(*mBuilder, *mIncludeMap,
                    *mJournalListener);
            rec->mIncludeDiagram.clearGraphAndAddInclude(incName);
            rec->mIncludeDiagram.drawToDrawingArea();
            addRecord(rec, incName);
            }
        else
            {
            setCurrentRecord(recordIndex);
            rec = reinterpret_cast<JournalRecordIncludeDiagram*>(getCurrentRecord());
            rec->mIncludeDiagram.drawToDrawingArea();
            }
        }
    }

void Journal::addRecord(JournalRecord *record, OovStringRef const name)
    {
    record->setName(name);
    removeUnmodifiedRecords();
    mRecords.push_back(record);
    setCurrentRecord(mRecords.size()-1);
    }

size_t Journal::findRecord(eRecordTypes rt, OovStringRef const name)
    {
    size_t index = NO_INDEX;
    for(size_t i=0; i<mRecords.size(); i++)
	{
	if(mRecords[i]->getRecordType() == rt &&
		std::string(mRecords[i]->getName()).compare(name) == 0)
	    {
	    index = i;
	    break;
	    }
	}
    return index;
    }

void Journal::removeUnmodifiedRecords()
    {
    for(size_t i=0; i<mRecords.size(); i++)
	{
	if(!mRecords[i]->isModified())
	    {
	    removeRecord(i);
	    break;
	    }
	}
    }

void Journal::removeRecord(size_t index)
    {
    delete mRecords[index];
    mRecords.erase(mRecords.begin() + index);
    }

void Journal::saveFile(FILE *fp)
    {
    JournalRecord *rec = getCurrentRecord();
    if(rec)
	{
	rec->saveFile(fp);
	}
    }

void Journal::cppArgOptionsChangedUpdateDrawings()
    {
    JournalRecord *rec = getCurrentRecord();
    if(rec)
	rec->cppArgOptionsChangedUpdateDrawings();
    }

JournalRecordClassDiagram::~JournalRecordClassDiagram()
    {
    }

JournalRecordOperationDiagram::~JournalRecordOperationDiagram()
    {
    }

JournalRecordComponentDiagram::~JournalRecordComponentDiagram()
    {
    }

JournalRecordZoneDiagram::~JournalRecordZoneDiagram()
    {
    }

JournalRecordPortionDiagram::~JournalRecordPortionDiagram()
    {
    }

JournalRecordIncludeDiagram::~JournalRecordIncludeDiagram()
    {
    }


extern "C" G_MODULE_EXPORT void on_DiagramDrawingarea_draw(
    GtkWidget * /*widget*/,
	GdkEventExpose * /*event*/, gpointer /*user_data*/)
    {
    JournalRecord *rec = gJournal->getCurrentRecord();
    if(rec)
	rec->drawingAreaDrawEvent();
    else
	{
	// Clear background if there are no records
	GtkWidget *widget = Builder::getBuilder()->getWidget("DiagramDrawingarea");
	GtkCairoContext cairo(widget);
	cairo_set_source_rgb(cairo.getCairo(), 255,255,255);
	cairo_paint(cairo.getCairo());
	}
    }

extern "C" G_MODULE_EXPORT void on_DiagramDrawingarea_button_press_event(
    GtkWidget * /*widget*/,
	GdkEventExpose *event, gpointer /*user_data*/)
    {
    JournalRecord *rec = gJournal->getCurrentRecord();
    if(rec)
	rec->drawingAreaButtonPressEvent(reinterpret_cast<GdkEventButton*>(event));
    }

extern "C" G_MODULE_EXPORT void on_DiagramDrawingarea_button_release_event(
    GtkWidget * /*widget*/,
	GdkEventExpose *event, gpointer /*user_data*/)
    {
    JournalRecord *rec = gJournal->getCurrentRecord();
    if(rec)
	rec->drawingAreaButtonReleaseEvent(reinterpret_cast<GdkEventButton*>(event));
    }

// "Focus Change" must be set in Glade Events for this to work.
extern "C" G_MODULE_EXPORT bool on_DiagramDrawingarea_focus_out_event(
    GtkWidget * /*widget*/,
	GdkEvent * /*event*/, gpointer /*user_data*/)
    {
    JournalRecord *rec = gJournal->getCurrentRecord();
    if(rec)
	rec->drawingLoseFocusEvent();
    return false;
    }

// "Pointer Motion" must be set in Glade Events for this to work.
extern "C" G_MODULE_EXPORT bool on_DiagramDrawingarea_motion_notify_event(
    GtkWidget * /*widget*/,
	GdkEvent *event, gpointer /*user_data*/)
    {
    JournalRecord *rec = gJournal->getCurrentRecord();
    if(rec)
	rec->drawingAreaMotionEvent(reinterpret_cast<GdkEventMotion*>(event));
    return false;
    }
