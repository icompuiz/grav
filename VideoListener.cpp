/*
 * @file VideoListener.h
 * Implementation of the VideoListener.
 */

#include "VideoListener.h"
#include "VideoSource.h"
#include "glutVideo.h"
#include "Group.h"
#include "TreeControl.h"
#include "GLUtil.h"

#include <VPMedia/video/VPMVideoDecoder.h>
#include <VPMedia/video/VPMVideoBufferSink.h>

VideoListener::VideoListener( gravManager* g ) :
    grav( g )
{
    x = -7.0f;
    y = 5.5f;
}

void
VideoListener::vpmsession_source_created(VPMSession &session,
                    uint32_t ssrc,
                    uint32_t pt,
                    VPMPayload type,
                    VPMPayloadDecoder *decoder)
{
  VPMVideoDecoder *d = dynamic_cast<VPMVideoDecoder*>(decoder);

  if (d) {
    VPMVideoFormat format = d->getOutputFormat();
    VPMVideoBufferSink *sink;
    printf( "VideoListener::vpmsession_source_created: "
            "creating source, have shaders? %i format? %i (yuv420p: %i)\n",
            GLUtil::getInstance()->haveShaders(), format, VIDEO_FORMAT_YUV420 );
    if ( GLUtil::getInstance()->haveShaders() && format == VIDEO_FORMAT_YUV420 )
        sink = new VPMVideoBufferSink( format );
    else
        sink = new VPMVideoBufferSink( VIDEO_FORMAT_RGB24 );
        
    if (!sink->initialise()) {
      fprintf(stderr, "Failed to initialise video sink\n");
      return;
    }

    d->connectVideoProcessor(sink);

    printf( "creating new source at %f,%f\n", x, y );

    VideoSource* source = new VideoSource( &session, ssrc, sink, x, y );
    grav->addNewSource( source );
    
    // do some basic grid positions
    x += 6.0f;
    if ( x > 9.0f )
    {
        x = -7.5f;
        y -= 6.0f;
    }
    
    //printf( "size of sources is %i\n", sources->size() );

    //session_sinks.push_back(sink);
    //session_sink_current = session_sinks.begin();
  }
}

void 
VideoListener::vpmsession_source_deleted(VPMSession &session,
                    uint32_t ssrc,
                    const char *reason)
{
    std::vector<VideoSource*>::iterator si;
    printf( "grav: deleting ssrc 0x%08x\n", ssrc );
    for ( si = grav->getSources()->begin();
            si != grav->getSources()->end(); si++ )
    {
        if ( (*si)->getssrc() == ssrc )
        {
            grav->deleteSource( si );
            return;
        }
    }
}

void 
VideoListener::vpmsession_source_description(VPMSession &session,
                    uint32_t ssrc)
{
  // Ignore
}

void 
VideoListener::vpmsession_source_app(VPMSession &session, 
                uint32_t ssrc, 
                const char *app , 
                const char *data, 
                uint32_t data_len)
{
    //printf( "RTP app data received\n" );
    //printf( "app: %s\n", app );
    //printf( "data: %s\n", data );
    
    std::string appS( app, 4 );
    std::string dataS( data, data_len );
    //printf( "listener::RTCP_APP: %s,%s\n", appS.c_str(), dataS.c_str() );
    //printf( "listener::RTCP_APP: data length is %i\n", data_len );
    
    if ( appS.compare( "site" ) == 0 && grav->usingSiteIDGroups() )
    {
        // vic sends 4 nulls at the end of the rtcp_app string for some
        // reason, so chop those off
        dataS = std::string( dataS, 0, 32 );
        std::vector<VideoSource*>::iterator i = grav->getSources()->begin();
        printf( "in rtcp app, got %i sources\n", grav->getSources()->size() );
        
        // sometimes, if groups are enabled by default, we can get RTCP APP
        // before we get any sources added, resulting in a crash when we try
        // and dereference the sources pointer - so skip this if we don't have
        // any sources yet
        if ( grav->getSources()->size() == 0 ) return;
        
        while ( (*i)->getssrc() != ssrc )
        {
            i++;
            if ( i == grav->getSources()->end() ) return;
        }
        
        if ( !(*i)->isGrouped() )
        {
            Group* g;
            std::map<std::string,Group*>::iterator mapi =
                                     grav->getSiteIDGroups()->find(dataS);
            
            if ( mapi == grav->getSiteIDGroups()->end() )
                g = grav->createSiteIDGroup( dataS );
            else
                g = mapi->second;
            
            (*i)->setSiteID( dataS );
            g->add( *i );
            
            // adding & removing will replace the object under its group
            grav->getTree()->removeObject( (*i) );
            grav->getTree()->addObject( (*i) );
            
            grav->getTree()->updateObjectName( g );
            
            grav->retileVideos();
        }
    }
}