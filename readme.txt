========================================================================
    MobileB C++/WinRT MobileB Project Overview
========================================================================

This UWP project is to test WinRT API for mobile broadband. Specifically 
Call establishmenet and teardown.

A seperate thread is started when the Connect button is clicked to run 
ConnectMobile(). This calls ConnectivityManager::AcquireConnectionAsync()
to establish a call. When the call completes the thread acquires the 
ConnectionSession and then waits for the disconnect event triggered by
a condition variable. 
When the disconnect butto is clicked the call is then disconnected by
calling session.Close(). Closing the call does not seem to work.

========================================================================
