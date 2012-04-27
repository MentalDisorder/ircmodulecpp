#ifndef _THREAD_HELPER_H_
#define _THREAD_HELPER_H_

//threadhelper.h
//Author: Simon Wittenberg
//Partly based on libirclient examples


#if !defined (WIN32)
    #include <sys/select.h>    /* fd_set */
#else
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif

#if defined (WIN32)
    /*#include <windows.h>*/ // this actually causes it to fail

    #define CREATE_THREAD_CHECKED(id,func,param)    (CreateThread(0, 0, func, param, 0, id) == 0)
    #define CREATE_THREAD(id,func,param)    CreateThread(0, 0, func, param, 0, id)
    #define THREAD_FUNCTION(funcname)        static DWORD WINAPI funcname (LPVOID arg)
    #define thread_id_t        DWORD
    #define sleep(a)        Sleep (a*1000)

    #define IRC_MUTEX_HANDLE HANDLE
    #define DEFINE_MUTEX(x) IRC_MUTEX_HANDLE x = CreateMutex( NULL, FALSE, NULL );
    #define AQUIRE_MUTEX(x) WaitForSingleObject( x , INFINITE )
    #define RELEASE_MUTEX(x) ReleaseMutex( x )
#else
    #include <unistd.h>
    #include <pthread.h>

    #define CREATE_THREAD(id,func,param)    (pthread_create (id, 0, func, (void *) param) != 0)
    #define THREAD_FUNCTION(funcname)        static void * funcname (void * arg)
    #define thread_id_t        pthread_t
    
    #define IRC_MUTEX_HANDLE pthread_mutex_t
    #define DEFINE_MUTEX(x) IRC_MUTEX_HANDLE x;                  \
                            pthread_mutex_init( &x, NULL );

    #define AQUIRE_MUTEX(x) pthread_mutex_lock( &x )
    #define RELEASE_MUTEX(x) pthread_mutex_unlock( &x )
#endif // ifdef(WIN32)


class MutexHandle
{
public:
    MutexHandle(IRC_MUTEX_HANDLE* mutex)
    {
        aquire(mutex);
    };
    void aquire(IRC_MUTEX_HANDLE* mutex)
    {
        AQUIRE_MUTEX(*mutex);
        _mutex = mutex;
    }
    void release()
    {
        if(_mutex)
        {
            RELEASE_MUTEX(*_mutex);
            _mutex = NULL;
        }
    };
    ~MutexHandle()
    {
        release();
    };
private:
    IRC_MUTEX_HANDLE* _mutex;
};



#endif //_THREAD_HELPER_H_