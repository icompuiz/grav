#ifndef INPUTHANDLER_H_
#define INPUTHANDLER_H_

/**
 * @file InputHandler.h
 * Processes keyboard and mouse input (passed from GLUT) and controls object
 * selection and movement accordingly.
 * @author Andrew Ford
 */

#include <vector>
#include <map>
#include <string>

#include <wx/wx.h>

#include "LayoutManager.h"

class VideoSource;
class RectangleBase;
class Earth;
class Group;
class gravManager;
class LayoutManager;
class Frame;

typedef double GLdouble;

class InputHandler;
typedef void (InputHandler::*MFP)(void);

class InputHandler : public wxEvtHandler
{

public:
    InputHandler( Earth* e, gravManager* g, Frame* f );
    ~InputHandler();

    void wxKeyDown( wxKeyEvent& evt );
    void wxCharEvt( wxKeyEvent& evt );
    void wxMouseMove( wxMouseEvent& evt );
    void wxMouseLDown( wxMouseEvent& evt );
    void wxMouseLUp( wxMouseEvent& evt );
    void wxMouseRDown( wxMouseEvent& evt );

    void handlePrintSelected();
    void handleRearrangeGroups();
    void handleUpdateGroupNames();
    void handlePerimeterArrange();
    void handleGridArrange();
    void handleFocusArrange();
    void handleFullscreenSelectedSingle();
    void handleRunwayToggle();
    void handleInvertSelection();
    void handleInformation();
    void handleToggleGroupLocks();
    void handleMuteSelected();
    void handleRandomTest();
    void handleNativeScaleAll();
    void handleNativeScaleSelected();
    void handleMoveAllToCenter();
    void handleToggleSiteGrouping();
    void handleToggleShowVenueClientController();
    void handleToggleRenderingSelected();
    void handleZoomout();
    void handleZoomin();
    void handleToggleAutoFocusRotate();
    void handleSelectAll();
    void handleToggleGraphicsDebug();
    void handleDownscaleSelected();
    void handleUpscaleSelected();
    void handleToggleFullscreen();
    void handleQuit();
    void handleClearSelected();
    void handleAddTestObject();

    void processKeyboard( int keyCode, int x, int y );

    void leftClick( int x, int y );
    void leftRelease( int x, int y );
    void mouseLeftHeldMove( int x, int y );

    bool selectVideos();
    static int propertyID;

    /*
     * Various accessors.
     */
    bool isLeftButtonHeld();
    float getDragStartX(); float getDragStartY();
    float getDragEndX(); float getDragEndY();

private:
    std::vector<RectangleBase*>* tempSelectedObjects;
    Earth* earth;

    // parent class
    gravManager* grav;

    // main gui window so we can trigger the proper quit sequence
    Frame* mainFrame;

    LayoutManager layouts;

    std::map<unsigned char, MFP> lookup;

    // special input modifiers like CTRL
    int special;

    GLdouble mouseX, mouseY, mouseZ;

    // start & end pos for click-and-dragging
    float dragStartX;
    float dragStartY;
    float dragEndX;
    float dragEndY;
    // the mouse pos from a previous activemotion call, for calculating drag
    // distance
    float dragPrevX;
    float dragPrevY;

    bool leftButtonHeld;

    // this should only be for mouse callbacks
    bool ctrlHeld;
    bool clickedInside;
    bool dragging;

    // use wx modifiers for keyboard callback - to easily check for "shift only"
    // "ctrl only" etc.
    int modifiers;

    DECLARE_EVENT_TABLE()

};

#endif /*INPUTHANDLER_H_*/
