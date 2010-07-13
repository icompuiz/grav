#ifndef GLCANVAS_H_
#define GLCANVAS_H_

/**
 * @file GLCanvas.h
 * Declaration of the GL canvas class, which defines the main rendered area
 * and draws it.
 * @author Andrew Ford
 */

#include <wx/glcanvas.h>
#include <wx/timer.h>

#include <sys/time.h>

class gravManager;

class GLCanvas : public wxGLCanvas
{

public:
    GLCanvas( wxWindow* parent, gravManager* g, int* attributes, int width,
                int height );
    void handlePaintEvent( wxPaintEvent& evt );
    void draw();
    void resize( wxSizeEvent& evt );
    void GLreshape( int w, int h );
    
private:
    gravManager* grav;
    wxGLContext* glContext;
    DECLARE_EVENT_TABLE()
    
    void testDraw();
    void testKey( wxKeyEvent& evt );
    
    // tracks the aspect ratio of the screen for reshaping
    float screen_width, screen_height;

    struct timeval time;
    time_t lastTimeMS;

};

class Timer : public wxTimer
{
    
public:
    Timer( GLCanvas* c, int i );
    void Notify();
    void Start();
    
    // print number of microseconds since last call
    void printTiming();
    time_t getTiming();
    void resetTiming();

private:
    GLCanvas* canvas;

    // interval between timer firing, in milliseconds
    int interval;

    struct timeval time;
    time_t lastTimeMS;
    
};

#endif /*GLCANVAS_H_*/
