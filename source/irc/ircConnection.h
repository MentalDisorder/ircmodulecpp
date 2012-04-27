#ifndef _IRC_CONNECTION_H_
#define _IRC_CONNECTION_H_
#include <string>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <libircclient.h>
#include <util/threadhelper.h>
#include <vector>
#include <util/util.h>

//ircConnection.h
//Author: Simon Wittenberg


typedef std::string String;

typedef std::vector<String> StringVector;

#define NullString String("")

struct IRCServerInfo
{
    IRCServerInfo()
    :   server(NULL),
        nick(NULL),
        channel(NULL)
    {}
    char     * server;
    char     * channel;
    char     * nick;

} ;

class IrcConnection;

class IrcConnection
{
public:
    IrcConnection();

    /********************************************************************/
    //                  Control Methods                                 //
    /********************************************************************/

    // This is the outside mutex, necessary when receiving updates from libirc
    // see source file for more information
    IRC_MUTEX_HANDLE* getMutex(){return &_mutex;};

    // sets and get the basic server info needed to connect to a server
    void setServerInfo(IRCServerInfo servInfo);
    IRCServerInfo* getServerInfo(){Unless(_running) return &_serverInfo; return NULL;}
    
    // sets and get the used port (defaults if not set)
    void setPort(unsigned int port){ Unless(_running)_port = port;};
    unsigned int getPort(){return _port;};

    // Start the connection to the server and optionally supply a new ServerInfo, if none was set earlier
    int start(IRCServerInfo* target = NULL);

    // internal function only do not use directly!
    // this is the method that is run in the thread.
    void run();

    // stop the connection
    void stop(){quit("I was told to");};
    
    bool isRunning(){ MutexHandle innerHandle(&_innerMutex); return _running;};

    // whether or not this object is currently trying to reconnect after a failed connection attempt or 
    // unexpected disconnect
    bool doesReconnect(){ MutexHandle innerHandle(&_innerMutex); return _tryingToConnect; };

    // cleanup method, used to reset the connection 
    void resetSession();
    
    // returns a pointer to the current session or NULL if we aren't connected/running
    irc_session_t* getSession(){return _session;};

    // set the time in seconds that this object should wait before attempting a reconnect when being disconnected 
    void setReconnectDelay(unsigned int sec){MutexHandle innerHandle(&_innerMutex); _reconectDelay = sec; };

    /********************************************************************/
    //                  Overwritable Methods                            //
    /********************************************************************/
    // Note:
    // 
    // Inherit and overwrite these methods to implement your features.
    // Make sure to include a 
    //                  typedef IrcConnection Parent;
    // in your derived clases and to call the parents implementation in each
    // derived method. E.g.:
    // void SomeDerivedClass :: on_connect(...)
    // {
    //     Parent::on_connect(...);
    //  
    //     Here be the new code.
    // }
    //
    // This is necessary if you overwrite on_unknown(...) and on_connect(...), 
    // but optional for  all other methods so far.



    // void IrCConnection :: on_connect(...)
    //
    // called upon successful connection to the server
    // params:
    // String event         - the name of the event
    // String server        - the ip or resolvable name of the server
    // String myNick        - the nick this object has on this server
    // StringVector params  - additional or non-standard params we received
    virtual void on_connect(const String event, const String server, const String myNick, const StringVector params)
    {
        MutexHandle innerHandle(&_innerMutex);
        _running = true;
    };

    // void IrCConnection :: on_nick(...)
    //
    // called upon nickchange in any channel or by ourselfes
    // params:
    // String event         - the name of the event
    // String oldNick       - the nick this connection got before changing it
    // String newNick       - the nick this connection changed its old nick to
    virtual void on_nick(const String event, const String oldNick, const String newNick){};

    // void IrCConnection :: on_quit(...)
    //
    // called upon a quite in any channel or by ourselfes
    // params:
    // String event         - the name of the event
    // String nick          - the name of the user (might be us) concerned
    // String reason        - the quit message
    virtual void on_quit(const String event, const String nick, const String reason){};

    // void IrCConnection :: on_join(...)
    //
    // called upon join of a user (including us) in any channel
    // params:
    // String event         - the name of the event
    // String nick          - the name of the user (might be us) concerned
    // String channel        - the channel concerned
    virtual void on_join(const String event, const String nick, const String channel){};

    // void IrCConnection :: on_part(...)
    //
    // called upon part of a user (including us) in any channel
    // params:
    // String event         - the name of the event
    // String nick          - the name of the user (might be us) concerned
    // String reason        - the channel concerned
    virtual void on_part(const String event, const String nick, const String channel){};

    // void IrCConnection :: on_mode(...)
    //
    // called upon user (including us) changing modes in a channel or for a user
    // params:
    // String event         - the name of the event
    // String nick          - the name of the user invoking the mode change
    // String channel       - the channel concerned
    // String modes         - the modes, lead by their modifier (+-)
    // StringVector params  - additional or non-standard params we received
    virtual void on_mode(const String event, const String nick, const String channel, const String modes, const StringVector params){};

    // void IrCConnection :: on_umode(...)
    //
    // called upon user mode change
    // params:
    // String event         - the name of the event
    // String origin        - the nick of the user or the server invoking the kick event
    // StringVector params  - additional or non-standard params we received
    virtual void on_umode(const String event, const String origin, const StringVector params){};

    // void IrCConnection :: on_kick(...)
    //
    // called upon a kick event in a channel
    // params:
    // String event         - the name of the event
    // String nick          - the nick of the user invoking the kick event
    // String channel       - the channel
    // String target        - the kick target (users nick)
    // String kickMsg       - the supplied kickMsg
    virtual void on_kick(const String event, const String nick, const String channel, const String target, const String kickMsg){};

    // void IrCConnection :: on_topic(...)
    //
    // called upon a change of the channel topic is announced
    // params:
    // String event         - the name of the event
    // String nick          - the nick of the user changing the topic
    // String channel       - the channel
    // String topic         - the new channel topic
    virtual void on_topic(const String event, const String nick, const String channel, const String topic){};

    // void IrCConnection :: on_channel(...)
    //
    // called upon receiving a message in a channel
    // params:
    // String event         - the name of the event
    // String nick          - the nick of the user sending the message
    // String message       - the channel in which the message was received
    // String message       - the message itself
    virtual void on_channel(const String event, const String nick, const String channel, const String msg){};

    // void IrCConnection :: on_private_message(...)
    //
    // called upon successful connection to the server
    // params:
    // String event         - the name of the event
    // String nick          - the nick of the user sending the message (might be server)
    // String message       - the message contained
    virtual void on_private_message(const String event, const String nick, const String msg){};

    // void IrCConnection :: on_notice(...)
    //
    // called upon receiving a notice
    // params:
    // String event         - the name of the event
    // String nick          - the nick of the user sending the notice (might be server)
    // String message       - the message contained in the notice
    virtual void on_notice(const String event, const String nick, const String msg){};

    // void IrCConnection :: on_channel_notice(...)
    //
    // called upon receiving a channel notice
    // params:
    // String event         - the name of the event
    // String origin        - most likely the server that is sending the event
    // StringVector params  - additional or non-standard params we received
    virtual void on_channel_notice(const String event, const String origin, const StringVector params){};

    // void IrCConnection :: on_invite(...)
    //
    // called upon being invited to a channel
    // params:
    // String event         - the name of the event
    // String nick          - the nick of the user sending the invite
    // StringVector params  - additional or non-standard params we received
    virtual void on_invite(const String event, const String nick, const String channel){};
    
    // void IrCConnection :: on_ctcp_request(...)
    //
    // called upon a ctcp request
    // params:
    // String event         - the name of the event
    // String origin        - most likely the server that is sending the event
    // StringVector params  - additional or non-standard params we received
    virtual void on_ctcp_request(const String event, const String origin, const StringVector params){};
    
    // void IrCConnection :: on_ctcp_reply(...)
    //
    // called upon a ctcp reply
    // params:
    // String event         - the name of the event
    // String origin        - most likely the server that is sending the event
    // StringVector params  - additional or non-standard params we received
    virtual void on_ctcp_reply(const String event, const String origin, const StringVector params){};
    
    // void IrCConnection :: on_unknown(...)
    //
    // called upon unknown event
    // params:
    // String event         - the name of the event
    // String origin        - most likely the server that is sending the event
    // StringVector params  - additional or non-standard params we received
    virtual void on_unknown(const String event, const String origin, const StringVector params)
    {
        Unless(params.size() < 1)
        {
            String throttled("-- throttled");
            size_t found;

            found = params[0].find(throttled);
            if (found!=String::npos)
            {
                printf("We've been throttled.\n");
                setReconnectDelay(60);
            }
        }
    };

    // void IrCConnection :: on_ctcp_action(...)
    //
    // called upon receiving a ctcp action
    // params:
    // String event         - the name of the event
    // String origin        - most likely the server that is sending the event
    // StringVector params  - additional or non-standard params we received
    virtual void on_ctcp_action(const String event, const String origin, const StringVector params){};

    // void IrCConnection :: on_numeric_code(...)
    //
    // called upon a numeric event, instead of a text event from the server
    // params:
    // const unsigned int event     - the name of the event
    // String origin                - most likely the server that is sending the event
    // StringVector params          - additional or non-standard params we received
    virtual void on_numeric_code(const unsigned int event, const String origin, const StringVector params)
    {
        #ifdef IRC_CONNECTION_DEBUG
        if ( event > 400 )
        {
            printf ("error %d: %s: %s %s %s %s\n", 
                event,
                origin.size() ? origin.c_str() : "unknown",
                params[0],
                count > 1 ? params[1] : "",
                count > 2 ? params[2] : "",
                count > 3 ? params[3] : "");
        }
        #endif
    };

    // void IrCConnection :: on_dcc_chat_req(...)
    //
    // called upon requested to intiate chat via dcc
    // params:
    // String nick          - the nick of the user sending the request
    // String addr          - the address string of the user
    // irc_dcc_t dccid      - the dcc id
    virtual void on_dcc_chat_req(const String nick, const String addr, irc_dcc_t dccid){};

    // void IrCConnection :: on_dcc_send_req(...)
    //
    // called upon being requested to receive a file via dcc
    // params:
    // String nick          - the nick of the user sending the request
    // String addr          - the address string of the user
    // String addr          - the name of the offered file
    // irc_dcc_t dccid      - the dcc id
    virtual void on_dcc_send_req(const String nick, const String addr, const String filename, unsigned long size, irc_dcc_t dccid){};

    //TODO: Figure out if necessary or should be cleaned up.
    /*int send_raw ( const char * format, ...)
    {
        irc_send_raw(_session, format, ...)
    };*/

    // void IrCConnection :: quit(...)
    //
    // user method to quit the connection to the server
    // params:
    // String reason        - the quit msg
    int quit ( const String reason)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        _tryingToConnect = false;
        return irc_cmd_quit(_session, reason.c_str());
    };

    // void IrCConnection :: quit(...)
    //
    // user method to quit the connection to the server
    // params:
    // String reason        - the quit msg
    void disconnect()
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_Void_Unless(_running);
        _tryingToConnect = false;
        irc_disconnect(_session);
    }

    // int IrCConnection :: join(...)
    //
    // user method to join a certain channel
    // params:
    // String channel       - the channel name
    // String key           - the channel key
    // return:          0 on success
    int join ( const String channel, const String key)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_join(_session, channel.c_str(), key.c_str());
    };

    // int IrCConnection :: part(...)
    //
    // user method to part a certain channel
    // params:
    // String channel       - the channel name
    // return:          0 on success
    int part ( const String channel)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_part(_session, channel.c_str());
    };
    
    // int IrCConnection :: invite(...)
    //
    // user method to quit the connection to the server
    // params:
    // String nick          - the users nick
    // String channel       - the channel to invite the user to
    // return:          0 on success
    int invite ( const String nick, const String channel)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_invite(_session, nick.c_str(), channel.c_str());
    };
    
    // int IrCConnection :: getNamesInChannel(...)
    //
    // user method to obtain all usernames in a channel
    // params:
    // String channel       (in)   - the name of the channel
    // String* channelNames (out)  - the string to contain the names in that channel
    // return:          0 on success
    int getNamesInChannel ( const String channel, String* channelNames)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);

        char namebuf[2048];
        strcpy(namebuf, channel.c_str());
        
        int retval = irc_cmd_names(_session, namebuf);

        Return_MinusOne_Unless(retval == 0);

        (*channelNames) = String(namebuf);
        return retval;
    };

    // int IrCConnection :: listChannels(...)
    //
    // user method to obtain all usernames in a channel
    // params:
    // String* channelNames (out)  - the string to contain the names in that channel
    // return:          0 on success
    int listChannels ( String* channelNames)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);

        char namebuf[2048];
        int retval = irc_cmd_list(_session, namebuf);

        (*channelNames) = String(namebuf);
        return retval;
    };

    // int IrCConnection :: setTopic(...)
    //
    // user method to set the topic in a certain channel
    // params:
    // String channel       - the channel
    // String topic         - the new topic
    // return:          0 on success
    int setTopic ( const String channel, const String topic)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_topic(_session, channel.c_str(), topic.c_str());
    };

    // int IrCConnection :: channelMode(...)
    //
    // user method to set a certain mode in a certain channel
    // params:
    // String channel       - the channel
    // String topic         - the new mode
    // return:          0 on success
    int channelMode ( const String channel, const String mode)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_channel_mode(_session, channel.c_str(), mode.c_str());
    };

    // int IrCConnection :: userMode(...)
    //
    // user method to set our user mode
    // params:
    // String mode          - the new mode
    // return:          0 on success
    int userMode ( const String mode)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_user_mode(_session, mode.c_str());
    };

    // int IrCConnection :: setNick(...)
    //
    // user method to set our nick
    // params:
    // String newnick       - the new nick
    // return:          0 on success
    int setNick ( const String newnick)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_nick(_session, newnick.c_str());
    };

    // int IrCConnection :: whois(...)
    //
    // user method to retrieve information about a user
    // params:
    // String reason        - the quit msg
    // return:          0 on success
    int whois ( const String nick, String* information)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);

        char whoisbuffer[2048];
        strcpy(whoisbuffer, nick.c_str());
        
        int retval = irc_cmd_whois(_session, whoisbuffer);

        Return_MinusOne_Unless(retval == 0);

        (*information) = String(whoisbuffer);
        return retval;
    };

    // int IrCConnection :: sendMessage(...)
    //
    // user method to send a message to a certain channel
    // params:
    // String channel       - the channel
    // String text          - the text to send
    // return:          0 on success
    int sendMessage  ( const String channel, const String text)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_msg(_session, channel.c_str(), text.c_str());
    };

    // int IrCConnection :: sendActionMessage(...)
    //
    // user method to send an action to a certain channel
    // params:
    // String channel       - the channel
    // String text          - the text to send
    // return:          0 on success
    int sendActionMessage ( const String channel, const String text)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_me(_session, channel.c_str(), text.c_str());
    };

    // int IrCConnection :: notice(...)
    //
    // user method to send a notice to a certain channel or user
    // params:
    // String chanOrNick    - the channel or users nick
    // String text          - the text to send
    // return:          0 on success
    int notice ( const String chanOrNick, const String text)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_notice(_session, chanOrNick.c_str(), text.c_str());
    };

    // int IrCConnection :: kick(...)
    //
    // user method to kick a certain user from a certain channel
    // params:
    // String channel       - the nick of the user to kick
    // String channel       - the channel
    // String reason        - the reason why the user is kicked
    // return:          0 on success
    int kick ( const String nick, const String channel, const String reason)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_kick(_session, nick.c_str(), channel.c_str(), reason.c_str());
    };

    // int IrCConnection :: ctcpRequest(...)
    //
    // user method to send a ctcp request to a user
    // params:
    // String reason        - the quit msg
    // return:          0 on success
    int ctcpRequest ( const String nick, const String request)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_ctcp_request(_session, nick.c_str(), request.c_str());
    };

    // int IrCConnection :: ctcpReply(...)
    //
    // user method to send a ctcp reply to a user
    // params:
    // String nick        - the nick of the user to reply to
    // String reply         - the reply
    // return:          0 on success
    int ctcpReply ( const String nick, const String reply)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_MinusOne_Unless(_running);
        return irc_cmd_ctcp_reply(_session, nick.c_str(), reply.c_str());
    };

    // bool IrCConnection :: getNick(...)
    //
    // user method to retrieve nick from string
    // params:
    // String target (in)       - the string the users nick should be extracted from 
    // String* nick  (out)      - pointer to the string containing the extracted users nick (may be empty) 
    // return:      true if a nick of length > 0 is returned,
    //              false otherwise
    bool getNick (const String target, String* nick) 
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_False_Unless(_running);
        char nickbuf[128];
        irc_target_get_nick(target.c_str(), nickbuf, sizeof(nickbuf));
        (*nick) = String(nickbuf);
        return nick->size() > 0;
    };

    // bool IrCConnection :: getHost(...)
    //
    // user method to retrieve hostname from string
    // params:
    // String target (in)       - the string the hostname/IP should be extracted from 
    // String* host  (out)      - pointer to the string containing the extracted host name/IP (may be empty) 
    // return:      true if a nick of length > 0 is returned,
    //              false otherwise
    bool getHost (const String target, String* host)
    {
        MutexHandle innerHandle(&_innerMutex);
        Return_False_Unless(_running);
        char hostbuf[128];
        irc_target_get_host(target.c_str(), hostbuf, sizeof(hostbuf));
        (*host) = String(hostbuf);
        return host->size() > 0;
    };



    // The following commented lines include not wrapped functions of the libirc
    // It is tbd wether or not they need to be wrapped 

    //int dcc_chat ( void * ctx, const String nick, irc_dcc_callback_t callback, irc_dcc_t * dccid);
    //int dcc_msg    ( unsigned int dccid, const String text);
    //int dcc_accept ( irc_dcc_t dccid, void * ctx, irc_dcc_callback_t callback);
    //int dcc_decline ( irc_dcc_t dccid);
    //int dcc_sendfile ( void * ctx, const String nick, const String filename, irc_dcc_callback_t callback, irc_dcc_t * dccid);
    //int dcc_destroy ( irc_dcc_t dccid);


protected:

    void _setCallbacks();

    irc_callbacks_t         _callbacks;
    IRCServerInfo           _serverInfo;
    irc_session_t*          _session;
    unsigned short          _port;
    bool                    _running;
    bool                    _tryingToConnect;
    unsigned int            _reconectDelay;
    IRC_MUTEX_HANDLE        _mutex;
    IRC_MUTEX_HANDLE        _innerMutex;
};


#endif


// These are the libircclient functions that we mostly mask behind member functions (aka methods :D )

//int irc_send_raw (irc_session_t * session, const char * format, ...);
//int irc_cmd_quit (irc_session_t * session, const char * reason);
//int irc_cmd_join (irc_session_t * session, const char * channel, const char * key);
//int irc_cmd_part (irc_session_t * session, const char * channel);
//int irc_cmd_invite (irc_session_t * session, const char * nick, const char * channel);
//int irc_cmd_names (irc_session_t * session, const char * channel);
//int irc_cmd_list (irc_session_t * session, const char * channel);
//int irc_cmd_topic (irc_session_t * session, const char * channel, const char * topic);
//int irc_cmd_channel_mode (irc_session_t * session, const char * channel, const char * mode);
//int irc_cmd_user_mode (irc_session_t * session, const char * mode);
//int irc_cmd_nick (irc_session_t * session, const char * newnick);
//int irc_cmd_whois (irc_session_t * session, const char * nick);
//int irc_cmd_msg  (irc_session_t * session, const char * nch, const char * text);
//int irc_cmd_me (irc_session_t * session, const char * nch, const char * text);
//int irc_cmd_notice (irc_session_t * session, const char * nch, const char * text);
//int irc_cmd_kick (irc_session_t * session, const char * nick, const char * channel, const char * reason);
//int irc_cmd_ctcp_request (irc_session_t * session, const char * nick, const char * request);
//int irc_cmd_ctcp_reply (irc_session_t * session, const char * nick, const char * reply);
//void irc_target_get_nick (const char * target, char *nick, size_t size);
//void irc_target_get_host (const char * target, char *nick, size_t size);
//int irc_dcc_chat (irc_session_t * session, void * ctx, const char * nick, irc_dcc_callback_t callback, irc_dcc_t * dccid);
//int irc_dcc_msg    (irc_session_t * session, irc_dcc_t dccid, const char * text);
//int    irc_dcc_accept (irc_session_t * session, irc_dcc_t dccid, void * ctx, irc_dcc_callback_t callback);
//int irc_dcc_decline (irc_session_t * session, irc_dcc_t dccid);
//int irc_dcc_sendfile (irc_session_t * session, void * ctx, const char * nick, const char * filename, irc_dcc_callback_t callback, irc_dcc_t * dccid);
//int irc_dcc_destroy (irc_session_t * session, irc_dcc_t dccid);