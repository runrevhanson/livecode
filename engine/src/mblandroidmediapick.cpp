/* Copyright (C) 2003-2013 Runtime Revolution Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#include "prefix.h"

#include "globdefs.h"
#include "filedefs.h"
#include "objdefs.h"
#include "parsedef.h"

#include "execpt.h"
#include "globals.h"
#include "stack.h"
#include "system.h"
#include "player.h"
#include "eventqueue.h"
#include "osspec.h"

#include "mblandroid.h"
#include "mblandroidutil.h"

#include "mblandroidjava.h"
#include "mblsyntax.h"

#include <jni.h>
#include "mblandroidjava.h"

#include "mblad.h"

///////////////////////////////////////////////////////////////////////////////

typedef enum
{
    kMCAndroidMediaWaiting,
    kMCAndroidMediaDone,
    kMCAndroidMediaCanceled,
} MCAndroidMediaStatus;

static MCAndroidMediaStatus s_media_status = kMCAndroidMediaWaiting;
static char *s_media_content = NULL;

bool MCSystemPickMedia(MCMediaType p_types, bool p_multiple, MCStringRef& r_result)
{
    s_media_status = kMCAndroidMediaWaiting;
    
    bool t_audio, t_video;
    t_audio = (kMCMediaTypeAnyAudio & p_types) != 0;
    t_video = (kMCMediaTypeAnyVideo & p_types) != 0;
    
    if (t_audio && !t_video)
	{
        MCAndroidEngineCall("pickMedia", "vs", nil, "audio/*");
	}
	else if (!t_audio && t_video)
	{
        MCAndroidEngineCall("pickMedia", "vs", nil, "video/*");
	}
    else
	{
        MCAndroidEngineCall("pickMedia", "vs", nil, "audio/* video/*");
	}
    
    while (s_media_status == kMCAndroidMediaWaiting)
        MCscreen->wait(60.0, False, True);
    /* UNCHECKED */ MCStringCreateWithCString(s_media_content, r_result);
    //    MCLog("Media Types Returned: %s", s_media_content);
}

void MCAndroidMediaDone(char *p_media_content)
{
    s_media_content = p_media_content;
    //    MCLog("MCAndroidMediaDone() called %s", p_media_content);
	s_media_status = kMCAndroidMediaDone;
}

void MCAndroidMediaCanceled()
{
    //    MCLog("MCAndroidMediaCanceled() called", nil);
	s_media_status = kMCAndroidMediaCanceled;
}