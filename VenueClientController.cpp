/*
 * VenueClientController.cpp
 *
 * Implementation of a class for controlling Access Grid Venue Clients.
 *
 * Created on: Nov 8, 2010
 * @author Andrew Ford
 */

#include "VenueClientController.h"
#include "gravManager.h"
#include "SessionTreeControl.h"
#include "VenueNode.h"
#include "PNGLoader.h"

VenueClientController::VenueClientController( float _x, float _y,
                                                gravManager* g )
    : Group( _x, _y ), grav( g )
{
    locked = false;
    selectable = false;
    userMovable = false;
    allowHiding = true;
    setName( "Venues" );
    debugDraw = false;

    baseBColor.R = 0.7f;
    baseBColor.G = 0.7f;
    baseBColor.B = 1.0f;
    baseBColor.A = 0.8f;
    destBColor = baseBColor;
    destBColor.A = 0.0f;
    borderColor = destBColor;

    setScale( 13.0f, 13.0f );

    // this should be safe since this constructor normally gets called after
    // GL stuff gets set up
    circleWidth = 256;
    circleHeight = 256;
    circleTex = PNGLoader::loadPNG( "circle.png", circleWidth, circleHeight );

    updateExitMap();

    setRendering( false );
}

void VenueClientController::draw()
{
    animateValues();

    if ( borderColor.A < 0.01f )
        return;

    glColor4f( borderColor.R, borderColor.G, borderColor.B, borderColor.A );

    // draw lines from center to each of the venues
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glLineWidth( 2.0f );
    glEnable( GL_LINE_SMOOTH );
    glBegin( GL_LINES );
    for ( unsigned int i = 0; i < objects.size(); i++ )
    {
        glVertex3f( objects[i]->getX(), objects[i]->getY(), 0.0f );
        glVertex3f( getX(), getY(), 0.0f );
    }
    glEnd();
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_BLEND );
    glLineWidth( 1.0f );

    for ( unsigned int i = 0; i < objects.size(); i++ )
    {
        objects[i]->draw();
    }
}

void VenueClientController::remove( RectangleBase* object, bool move )
{
    Group::remove( object, move );
    grav->removeFromLists( object, false );
    delete object;
}

std::vector<RectangleBase*>::iterator VenueClientController::remove(
                            std::vector<RectangleBase*>::iterator i, bool move )
{
    RectangleBase* object = (*i);
    std::vector<RectangleBase*>::iterator ret = Group::remove( i, move );
    grav->removeFromLists( object, false );
    delete object;
    return ret;
}

void VenueClientController::getVenueClient()
{
    PyObject* pRes = pyTools.call( "AGTools", "GetVenueClients" );
    std::vector<std::string> venueClients = pyTools.ltov( pRes );
    if ( venueClients.size() == 0 )
    {
        printf( "VenueClientController::getVenueClient(): error: no venue "
                "client found\n" );
    }
    else
    {
        // TODO just take first for now - maybe if multiple found, prompt user?
        venueClientUrl = venueClients[0];
    }
    Py_XDECREF( pRes );
}

void VenueClientController::updateExitMap()
{
    // if no venue client, try to get; if not then fail
    if ( venueClientUrl.compare( "" ) == 0 )
    {
        getVenueClient();
    }
    if ( venueClientUrl.compare( "" ) == 0 )
    {
        return;
    }

    PyObject* pRes = pyTools.call( "AGTools", "GetExits",
                                    venueClientUrl );
    exitMap = pyTools.dtom( pRes );
    Py_XDECREF( pRes );

    // TODO check if exitMap changes here, to avoid needless remake?
    removeAll();
    std::map<std::string, std::string>::iterator i;
    for ( i = exitMap.begin(); i != exitMap.end(); ++i )
    {
        VenueNode* node = new VenueNode();
        node->setName( i->first );
        node->setTexture( circleTex, circleWidth, circleHeight );
        grav->addToDrawList( node );
        add( node );
    }
}

void VenueClientController::printExitMap()
{
    //updateExitMap();

    if ( venueClientUrl.compare( "" ) == 0 )
    {
        return;
    }
    printf( "VenueClientController::printExitMap: from venue client at %s\n",
                venueClientUrl.c_str() );
    std::map<std::string, std::string>::iterator it;
    for ( it = exitMap.begin(); it != exitMap.end(); ++it )
    {
        printf( "%s : %s\n", it->first.c_str(), it->second.c_str() );
    }

    printf( "Objects, should correspond to venues:\n" );
    for ( unsigned int i = 0; i < objects.size(); i++ )
    {
        printf( "%i: %s\n", i, objects[i]->getSubName().c_str() );
    }
}

void VenueClientController::enterVenue( std::string venueName )
{
    std::map<std::string, std::string>::iterator it = exitMap.find( venueName );
    if ( it == exitMap.end() )
    {
        printf( "VenueClientController::enterVenue: venue %s not found\n",
                    venueName.c_str() );
        return;
    }

    removeAllVenueStreams();

    PyObject* args = PyTuple_New( 2 );
    PyTuple_SetItem( args, 0, PyString_FromString( venueClientUrl.c_str() ) );
    PyTuple_SetItem( args, 1, PyString_FromString( it->second.c_str() ) );

    pyTools.call( "AGTools", "EnterVenue", args );

    updateExitMap();

    updateVenueStreams();
    addAllVenueStreams();
}

void VenueClientController::updateVenueStreams()
{
    std::string type = "video";
    PyObject* args = PyTuple_New( 2 );
    PyTuple_SetItem( args, 0, PyString_FromString( venueClientUrl.c_str() ) );
    PyTuple_SetItem( args, 1, PyString_FromString( type.c_str() ) );

    PyObject* res = pyTools.call( "AGTools", "GetFormattedVenueStreams", args );
    currentVenueStreams = pyTools.dtom( res );
    Py_XDECREF( res );
}

void VenueClientController::removeAllVenueStreams()
{
    std::map<std::string, std::string>::iterator it;
    for ( it = currentVenueStreams.begin(); it != currentVenueStreams.end();
            ++it )
    {
        printf( "VenueClientController::remove(): Video stream: %s\n", it->first.c_str() );
        sessionControl->removeSession( it->first );
    }
}

void VenueClientController::addAllVenueStreams()
{
    std::map<std::string, std::string>::iterator it;
    for ( it = currentVenueStreams.begin(); it != currentVenueStreams.end();
            ++it )
    {
        printf( "VenueClientController::add(): Video stream: %s\n", it->first.c_str() );
        sessionControl->addSession( it->first, false, false );
        // __NO_KEY__ is a dummy value to indicate there is no encryption on the
        // stream in question
        if ( it->second.compare( "__NO_KEY__" ) != 0 )
        {
            sessionControl->setEncryptionKey( it->first, it->second );
        }
    }
}

void VenueClientController::rearrange()
{
    std::map<std::string, std::string> opts;

    std::map<std::string, std::vector<RectangleBase*> > data;
    data["objects"] = objects;

    RectangleBase smaller = *this;
    // uneven since most screens will be widescreen - so make vertical area
    // bigger so objects on top/bottom are bigger and text more readable
    smaller.setScale( smaller.getScaleX() * 0.6f,
                        smaller.getScaleY() * 0.45f );
    layouts.arrange( "perimeter", *this, smaller, data, opts );
}

bool VenueClientController::updateName()
{
    // like runway, this does nothing
    return false;
}

void VenueClientController::setRendering( bool r )
{
    Group::setRendering( r );
    if ( !r )
    {
        // move objects for a nice animation effect
        for ( unsigned int i = 0; i < objects.size(); i++ )
        {
            objects[i]->move( getX(), getY() );
        }
    }
    else
        rearrange();
}

void VenueClientController::setSessionControl( SessionTreeControl* s )
{
    sessionControl = s;
}
