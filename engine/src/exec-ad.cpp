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
#include "mcio.h"

#include "mcerror.h"
#include "globals.h"
#include "exec.h"
#include "eventqueue.h"
#include "stack.h"
#include "card.h"

#include "mblad.h"
#include "mblsyntax.h"


////////////////////////////////////////////////////////////////////////////////

MC_EXEC_DEFINE_EXEC_METHOD(Ad, Register, 1)
MC_EXEC_DEFINE_EXEC_METHOD(Ad, Create, 5)
MC_EXEC_DEFINE_EXEC_METHOD(Ad, Delete, 1)
MC_EXEC_DEFINE_SET_METHOD(Ad, VisibleOfAd, 2)
MC_EXEC_DEFINE_GET_METHOD(Ad, VisibleOfAd, 2)
MC_EXEC_DEFINE_SET_METHOD(Ad, TopLeftOfAd, 3)
MC_EXEC_DEFINE_GET_METHOD(Ad, TopLeftOfAd, 3)
MC_EXEC_DEFINE_GET_METHOD(Ad, Ads, 1)


////////////////////////////////////////////////////////////////////////////////

void MCAdExecRegisterWithInneractive(MCExecContext& ctxt, MCStringRef p_key)
{
#ifdef /* MCAdExecRegisterWithInneractive */ LEGACY_EXEC
    MCCStringFree(s_inneractive_ad_key);
    /* UNCHECKED */ MCCStringClone(p_key, s_inneractive_ad_key);
#endif /* MCAdExecRegisterWithInneractive */
    if (MCAdSetInneractiveKey(p_key))
        return;
    
    ctxt.Throw();
}

void MCAdExecCreateAd(MCExecContext& ctxt, MCStringRef p_name, MCStringRef p_type, uint32_t p_topleft_x, uint32_t p_topleft_y, MCArrayRef p_metadata)
{
#ifdef /* MCAdExecCreateAd */ LEGACY_EXEC
    bool t_success;
    t_success = true;
    
    if (s_inneractive_ad_key == nil || MCCStringLength(s_inneractive_ad_key) == 0)
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        t_success = false;;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    
    if (t_success)
        if (MCAd::FindByNameOrId(p_name, t_ad))
        {
            ctxt.SetTheResultToStaticCString("ad already exists");
            t_success = false;
        }
    
    if (t_success)
    {
        uint32_t t_timeout;
        if (t_success)
        {
            t_timeout = 0;
            if (p_meta_data != nil && p_meta_data->fetch_element_if_exists(ctxt.GetEP(), "refresh", false))
                t_timeout = ctxt.GetEP().getint4();
            if (p_type == kMCAdTypeFullscreen)
                t_timeout = 0;
            else if (t_timeout < 30 || t_timeout > 500)
                t_timeout = 120;
        }
        
        if (t_success)
            t_success = MCSystemInneractiveAdCreate(ctxt, t_ad, p_type, p_top_left, t_timeout, p_meta_data);
        
        if (t_success)
            t_success = t_ad->Create();
        
        if (t_success)
        {
            t_ad->SetNext(s_ads);
            t_ad->SetName(p_name);
            t_ad->SetOwner(ctxt.GetObjectHandle());
            s_ads = t_ad;
        }
        else if (t_ad != nil)
            t_ad->Release();
        
        if (!t_success)
            ctxt.SetTheResultToStaticCString("could not create ad");
    }
    
#endif /* MCAdExecCreateAd */
    bool t_success;
    t_success = true;
    
    if (MCAdInneractiveKeyIsNil())
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        t_success = false;;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    
    if (t_success)
        if (MCAd::FindByNameOrId(p_name, t_ad))
        {
            ctxt.SetTheResultToStaticCString("ad already exists");            
            t_success = false;
        }
    
    if (t_success)
    {
        uint32_t t_timeout;
        MCAdType t_type;
        t_type = MCAdTypeFromCString(MCStringGetCString(p_type));
        
        if (t_success)
        {
            t_timeout = 0;
            MCNewAutoNameRef t_key;
            MCNameCreateWithCString("refresh", &t_key);
            
            MCValueRef t_value;
            
            if (p_metadata != nil && MCArrayFetchValue(p_metadata, false, *t_key, t_value))
                t_timeout = MCNumberFetchAsUnsignedInteger((MCNumberRef)t_value);
            if (t_type == kMCAdTypeFullscreen)
                t_timeout = 0;
            else if (t_timeout < 30 || t_timeout > 500)
                t_timeout = 120;
        }
        
        if (t_success)
        {
            MCAdTopLeft t_topleft;
            t_topleft.x = p_topleft_x;
            t_topleft.y = p_topleft_y;
            
            t_success = MCSystemInneractiveAdCreate(ctxt, t_ad, t_type, p_topleft_x, p_topleft_y, t_timeout, p_metadata);
        }
        
        if (t_success)
            t_success = t_ad->Create();
        
        if (t_success)
        {
            t_ad->SetNext(MCAdGetStaticAdsPtr());
            t_ad->SetName(p_name);
            t_ad->SetOwner(ctxt.GetObjectHandle());
            MCAdSetStaticAdsPtr(t_ad);
            
            return;
        }
        else if (t_ad != nil)
            t_ad->Release();
        
        ctxt.SetTheResultToStaticCString("could not create ad");
    }
    
    ctxt.Throw();
}

void MCAdExecDeleteAd(MCExecContext& ctxt, MCStringRef p_name)
{
#ifdef /* MCAdExecDeleteAd */ LEGACY_EXEC
    if (s_inneractive_ad_key == nil || MCCStringLength(s_inneractive_ad_key) == 0)
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        return;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (!MCAd::FindByNameOrId(p_name, t_ad))
        ctxt.SetTheResultToStaticCString("could not find ad");
    else
        t_ad->Release();
#endif /* MCAdExecDeleteAd */
    if (MCAdInneractiveKeyIsNil())
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        return;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (MCAd::FindByNameOrId(p_name, t_ad))
    {
        t_ad->Release();
        return;
    }
    
    ctxt.SetTheResultToStaticCString("could not find ad");
    ctxt.Throw();
}

void MCAdGetTopLeftOfAd(MCExecContext& ctxt, MCStringRef p_name, uint32_t& r_topleft_x, uint32_t& r_topleft_y)
{
#ifdef /* MCAdGetTopLeftOfAd */ LEGACY_EXEC
    if (s_inneractive_ad_key == nil || MCCStringLength(s_inneractive_ad_key) == 0)
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        return false;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (!MCAd::FindByNameOrId(p_name, t_ad))
    {
        ctxt.SetTheResultToStaticCString("could not find ad");
        return false;
    }
    else
    {
        r_top_left = t_ad->GetTopLeft();
        return true;
    }
#endif /* MCAdGetTopLeftOfAd */
    if (MCAdInneractiveKeyIsNil())
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        ctxt.Throw();
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (MCAd::FindByNameOrId(p_name, t_ad))
    {
        r_topleft_x = t_ad->GetTopLeft().x;
        r_topleft_y = t_ad->GetTopLeft().y;
        return;
    }
    
    ctxt.SetTheResultToStaticCString("could not find ad");
    ctxt.Throw();
}

void MCAdSetTopLeftOfAd(MCExecContext& ctxt, MCStringRef p_name, uint32_t p_topleft_x, uint32_t p_topleft_y)
{
#ifdef /* MCAdSetTopLeftOfAd */ LEGACY_EXEC
    if (s_inneractive_ad_key == nil || MCCStringLength(s_inneractive_ad_key) == 0)
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        return;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (!MCAd::FindByNameOrId(p_name, t_ad))
        ctxt.SetTheResultToStaticCString("could not find ad");
    else
        t_ad->SetTopLeft(p_top_left);
#endif /* MCAdSetTopLeftOfAd */
    if (MCAdInneractiveKeyIsNil())
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        return;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (MCAd::FindByNameOrId(p_name, t_ad))
    {
        MCAdTopLeft t_topleft;
        t_topleft.x = p_topleft_x;
        t_topleft.y = p_topleft_y;
        t_ad->SetTopLeft(t_topleft);
        
        return;
    }
    
    ctxt.SetTheResultToStaticCString("could not find ad");
    ctxt.Throw();
}

void MCAdGetVisibleOfAd(MCExecContext& ctxt,  MCStringRef p_name, bool &r_visible)
{
#ifdef /* MCAdGetVisibleOfAd */ LEGACY_EXEC
    if (s_inneractive_ad_key == nil || MCCStringLength(s_inneractive_ad_key) == 0)
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        return false;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (!MCAd::FindByNameOrId(p_name, t_ad))
    {
        ctxt.SetTheResultToStaticCString("could not find ad");
        return false;
    }
    else
    {
        r_visible = t_ad->GetVisible();
        return true;
    }
#endif /* MCAdGetVisibleOfAd */
    if (MCAdInneractiveKeyIsNil())
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        ctxt.Throw();
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (MCAd::FindByNameOrId(p_name, t_ad))
    {
        r_visible = t_ad->GetVisible();
        return;
    }
    
    ctxt.SetTheResultToStaticCString("could not find ad");
    ctxt.Throw();
}

void MCAdSetVisibleOfAd(MCExecContext& ctxt, MCStringRef p_name, bool p_visible)
{
#ifdef /* MCAdSetVisibleOfAd */ LEGACY_EXEC
    if (s_inneractive_ad_key == nil || MCCStringLength(s_inneractive_ad_key) == 0)
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        return;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (!MCAd::FindByNameOrId(p_name, t_ad))
        ctxt.SetTheResultToStaticCString("could not find ad");
    else
        t_ad->SetVisible(p_visible);
#endif /* MCAdSetVisibleOfAd */
    if (MCAdInneractiveKeyIsNil())
    {
        ctxt.SetTheResultToStaticCString("not registered with ad service");
        ctxt.Throw();
        return;
    }
    
    MCAd *t_ad;
    t_ad = nil;
    if (!MCAd::FindByNameOrId(p_name, t_ad))
    {
        t_ad->SetVisible(p_visible);
        return;
    }
    
    ctxt.SetTheResultToStaticCString("could not find ad");
    ctxt.Throw();
}

void MCAdGetAds(MCExecContext& ctxt, MCStringRef& r_ads)
{
#ifdef /* MCAdGetAds */ LEGACY_EXEC
    bool t_success;
    t_success = true;
	for(MCAd *t_ad = s_ads; t_ad != nil && t_success; t_ad = t_ad->GetNext())
		if (t_ad->GetName() != nil)
        {
            if (r_ads == nil)
                t_success = MCCStringClone(t_ad->GetName(), r_ads);
            else
                t_success = MCCStringAppendFormat(r_ads, "\n%s", t_ad->GetName());
        }
    return t_success;
#endif /* MCAdGetAds */
    bool t_success;
    t_success = false;
    
    MCAutoStringRef t_ads;
	for(MCAd *t_ad = MCAdGetStaticAdsPtr(); t_ad != nil && t_success; t_ad = t_ad->GetNext())
    {
		if (t_ad->GetName() != nil)
        {
            if (*t_ads == nil)
            {
                MCAutoStringRef t_first_ad;
                t_success = MCStringCreateWithCString(t_ad->GetName(), &t_first_ad);
                if (t_success)
                    t_success = MCStringMutableCopy(*t_first_ad, &t_ads);
            }
            else
                t_success = MCStringAppendFormat(*t_ads, "\n%s", t_ad->GetName());
        }
    }
    
    if (t_success)
    {
        r_ads = MCValueRetain(*t_ads);
        return;
    }

    ctxt.Throw();
}

////////////////////////////////////////////////////////////////////////////////