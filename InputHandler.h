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
    
    void processKeyboard( int keyCode, int x, int y );
    
    void leftClick( int x, int y );
    void leftRelease( int x, int y );
    void mouseLeftHeldMove( int x, int y );
    
    bool selectVideos();
    void spawnPropertyWindow( wxCommandEvent& evt );
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

    // note shiftheld and altheld are for when ONLY those are held down - maybe
    // change this to be more consistent
    bool ctrlHeld;
    bool shiftHeld;
    bool altHeld;
    bool clickedInside;
    bool dragging;
    
    DECLARE_EVENT_TABLE()
    
};

#endif /*INPUTHANDLER_H_*/
