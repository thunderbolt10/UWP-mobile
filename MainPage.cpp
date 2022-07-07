#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <winrt/Windows.Networking.Connectivity.h>
#include <condition_variable>
#include <thread>
#include <mutex>

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Connectivity;

namespace winrt::MobileB::implementation
{
    std::thread m_call_thread;
    std::condition_variable m_cv;
    std::mutex m_call_mutex;
    bool m_disconnected = false;
    bool m_disconnect = false;


    MainPage::MainPage()
    {
        InitializeComponent();
    }

    int32_t MainPage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainPage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void ConnectMobile()
    {
        CellularApnContext apn_context;

        apn_context.AccessPointName(L"everywhere");
        apn_context.UserName(L"eesecure");
        apn_context.Password(L"secure");
        apn_context.AuthenticationType(CellularApnAuthenticationType::None);
        apn_context.IsCompressionEnabled(false);
        try
        {
            auto call = ConnectivityManager::AcquireConnectionAsync(apn_context);
            OutputDebugString(L"Connecting...\n");
            while (call.Status() == AsyncStatus::Started())
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            };

            if (call.Status() == AsyncStatus::Completed)
            {
                ConnectionSession session = call.GetResults();
                OutputDebugString(L"Connected\n");
        
                // Wait for the signal to disconnect the call from the UI thread
                std::unique_lock lk(m_call_mutex);
                m_cv.wait(lk, [] {return m_disconnect; });


                OutputDebugString(L"Disconnecting...\n");
                session.Close();   // <---- This should drop the connection!
                OutputDebugString(L"Disconnected\n");
                m_disconnected = true;
                lk.unlock();
                m_cv.notify_one();


            }
            else
            {
                switch (call.Status())
                {
                case AsyncStatus::Canceled:
                    OutputDebugString(L"Cancelled\n");
                    break;
                case AsyncStatus::Error:
                    OutputDebugString(L"Error\n");
                    int ec = call.ErrorCode();
                    std::wstring wec = std::to_wstring(ec);
                    OutputDebugString(wec.c_str());
                    OutputDebugString(L"\n");
                    break;
                }
            }

        }
        catch (const std::exception& e)
        {
            std::string err = e.what();
            std::wstring werr(err.begin(), err.end());
            OutputDebugString(werr.c_str());
        }

    }

    void MainPage::Disconnect_Click(IInspectable const&, RoutedEventArgs const&)
    {
        Disconnect().IsEnabled(false);

        {
            std::lock_guard lk(m_call_mutex);
            m_disconnect = true;

        }
        m_cv.notify_one();

        {
            std::unique_lock lk(m_call_mutex);
            m_cv.wait(lk, [] {return m_disconnected; });
        }
        m_call_thread.join();

        Connect().IsEnabled(true);
        
    }

  

    void MainPage::Connect_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        Connect().IsEnabled(false);

        m_call_thread = std::thread(ConnectMobile);
        
        Disconnect().IsEnabled(true);
    }

   

}



