#include "ircConnection.h"

//ircConnection.cpp
//Author: Simon Wittenberg


StringVector paramsToStringVector(const char ** params, const unsigned int count, const unsigned int start = 0)
{
    StringVector returnVector;
    for(unsigned int i = start; i < count; i++)
    {
        returnVector.push_back(String(params[i]));
    }
    return returnVector;
}


void irc_connection_event_connect (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count >= 1);
    MutexHandle connectionMutex(connection->getMutex());
    String server(origin ? origin : "");
    String myNick(params[0]);
    StringVector parameter = paramsToStringVector(params, count, 2);
    connection->on_connect( String(event), server, myNick, parameter);
}
void irc_connection_event_nick (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count == 1);
    MutexHandle connectionMutex(connection->getMutex());
    String nick;
    connection->getNick(origin ? origin : "", &nick);
    connection->on_nick( String(event), nick, String(params[0]));
}
void irc_connection_event_quit (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count == 1);
    MutexHandle connectionMutex(connection->getMutex());
    String nick; 
    connection->getNick(origin ? origin : "", &nick);
    connection->on_quit( String(event), nick, String(params[0]));
}
void irc_connection_event_join (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count == 1);
    MutexHandle connectionMutex(connection->getMutex());
    String nick;
    connection->getNick(origin ? origin : "", &nick);
    connection->on_join( String(event), nick, String(params[0]));
}
void irc_connection_event_part (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    MutexHandle connectionMutex(connection->getMutex());
    String nick;
    connection->getNick(origin ? origin : "", &nick);
    connection->on_part( String(event), nick, String(params[0]));
}
void irc_connection_event_mode (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count >= 2);
    MutexHandle connectionMutex(connection->getMutex());
    String nick; 
    connection->getNick(origin ? origin : "", &nick);
    String channel(params[0]);
    String modes(params[1]);
    StringVector parameter = paramsToStringVector(params, count, 2);
    connection->on_mode( String(event), nick, channel, modes, parameter);
}
void irc_connection_event_umode (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    StringVector parameter = paramsToStringVector(params, count);
    connection->on_umode( String(event), String(origin ? origin : ""), parameter);
}
void irc_connection_event_kick (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count == 3);
    MutexHandle connectionMutex(connection->getMutex());
    String nick; 
    connection->getNick(origin ? origin : "", &nick);
    String channel(params[0]);
    String target(params[1]);
    String kickMsg(params[2]);
    connection->on_kick( String(event), nick, channel, target , kickMsg);
}
void irc_connection_event_topic (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    String nick; 
    connection->getNick(origin ? origin : "", &nick);
    String channel(params[0]);
    String topic(params[1]);
    connection->on_topic( String(event), nick, channel, topic);
}
void irc_connection_event_channel (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    Return_Void_Unless(origin && count == 2);
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    String nick;
    connection->getNick(origin ? origin : "", &nick);
    connection->on_channel( String(event), nick, String(params[0]), String(params[1]));
}
void irc_connection_event_privmsg (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count == 2);
    MutexHandle connectionMutex(connection->getMutex());
    String nick;
    connection->getNick(origin ? origin : "", &nick);
    String msg(params[1]); // params[0] is our own nick
    connection->on_private_message( String(event), nick, msg);
}
void irc_connection_event_notice (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count == 2);
    MutexHandle connectionMutex(connection->getMutex());
    String nick;
    connection->getNick(origin ? origin : "", &nick);
    String msg(params[1]); // params[0] is our own nick
    connection->on_notice( String(event), nick, msg);
}
void irc_connection_event_channel_notice (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    StringVector parameter = paramsToStringVector(params, count);
    connection->on_channel_notice( String(event), String(origin ? origin : ""), parameter);
    
    // I guess it would look like this:
    //String nick;
    //connection->getNick(origin ? origin : "");
    //String msg(params[1]); // params[0] is our own nick
    //connection->on_channel_notice( String(event), nick, channel, msg);
}
void irc_connection_event_invite (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection && count == 2);
    MutexHandle connectionMutex(connection->getMutex());
    String nick;
    connection->getNick(origin ? origin : "", &nick);
    String channel(params[1]); // params[0] is our own nick
    connection->on_invite( String(event), nick, channel);
}
void irc_connection_event_ctcp_req (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    StringVector parameter = paramsToStringVector(params, count);
    connection->on_ctcp_request( String(event), String(origin ? origin : ""), parameter);
}
void irc_connection_event_ctcp_rep (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    StringVector parameter = paramsToStringVector(params, count);
    connection->on_ctcp_reply( String(event), String(origin ? origin : ""), parameter);
}
void irc_connection_event_unknown (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    StringVector parameter = paramsToStringVector(params, count);
    connection->on_unknown( String(event), String(origin ? origin : ""), parameter);
}
void irc_connection_event_ctcp_action (irc_session_t * session, const char * event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    StringVector parameter = paramsToStringVector(params, count);
    connection->on_ctcp_action( String(event), String(origin ? origin : ""), parameter);
}
void irc_connection_event_numeric (irc_session_t * session, unsigned int event, const char * origin, const char ** params, unsigned int count)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    StringVector parameter = paramsToStringVector(params, count);
    connection->on_numeric_code( event, String(origin ? origin : ""), parameter);
}
void irc_connection_event_dcc_chat_req (irc_session_t * session, const char * nick, const char * addr, irc_dcc_t dccid)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    connection->on_dcc_chat_req( String(nick), String(addr), dccid);
}
void irc_connection_event_dcc_send_req (irc_session_t * session, const char * nick, const char * addr, const char * filename, unsigned long size, irc_dcc_t dccid)
{
    IrcConnection* connection = (IrcConnection*) irc_get_ctx( session );
    Return_Void_Unless(connection);
    MutexHandle connectionMutex(connection->getMutex());
    connection->on_dcc_send_req( String(nick), String(addr), String(filename), size, dccid);
}

THREAD_FUNCTION(irc_connection_run_thread)
{
    IrcConnection* connection = (IrcConnection*) arg;
    connection->run();
    return 0;
}


IrcConnection::IrcConnection()
{
    _session = NULL;
    _setCallbacks();
    _port = 6667;
    _running = false;
    _tryingToConnect = false;
    _reconectDelay = 0;
}

void IrcConnection::setServerInfo(IRCServerInfo servInfo)
{
    Unless(_running)
        _serverInfo = servInfo;
}

int IrcConnection::start(IRCServerInfo* serverInfo/* = NULL*/)
{
    

    MutexHandle innerHandle(&_innerMutex);
    if(_running)
        return 1;

    // just to make sure all values are reset and there's no lingering connection
    // this is not perfect yet ...
    _tryingToConnect = false;
    if(_session)
        irc_cmd_quit(_session, "Default.\n");

    if(serverInfo)
        _serverInfo = *serverInfo;

    // this flag is set to true, so we reconnect later, if we have a connection abort
    // not to be confused with the retries on irc_connect!
    _tryingToConnect = true;

    innerHandle.release();
    thread_id_t tid;
    CREATE_THREAD(&tid, irc_connection_run_thread, this);
    return 0;
}

void IrcConnection::run()
{
    while(doesReconnect())
    {
        MutexHandle innerHandle(&_innerMutex);
        if(_reconectDelay > 0)
        {
            printf("Delaying reconnect for %d seconds.\n", _reconectDelay);
            sleep(_reconectDelay);
            _reconectDelay = 0;
            printf("Resuming reconnection attempts.\n");

        }
        if(_running)
            return;

        if(_session)
        {
            irc_destroy_session(_session);
            _session = NULL;
        }

        _session = irc_create_session (&_callbacks);

        if ( !_session )
        {
            printf ("Could not create IRC session\n");
            return;
        }

        irc_set_ctx (_session, this);
        irc_option_set (_session, LIBIRC_OPTION_STRIPNICKS);

        // If the port number is specified in the server string, use the port 0 so it gets parsed
        if ( strchr( _serverInfo.server, ':' ) != 0 )
            _port = 0;

        // To handle the "SSL certificate verify failed" from command line we allow passing ## in front 
        // of the server name, and in this case tell libircclient not to verify the cert
        if ( _serverInfo.server[0] == '#' && _serverInfo.server[1] == '#' )
        {
            // Skip the first character as libircclient needs only one # for SSL support, i.e. #irc.freenode.net
            _serverInfo.server++;
            
            irc_option_set( _session, LIBIRC_OPTION_SSL_NO_VERIFY );
        }


        // Initiate the IRC server connection
        int max_retry_count = 10;
        int retry_count = 0;
        while(true)
        {
            if ( irc_connect (_session, _serverInfo.server, _port, 0, _serverInfo.nick, 0, 0) )
            {
                printf ("Try #%d : Could not connect: %s (Server: %s ) \n", retry_count+1, irc_strerror (irc_errno(_session)), _serverInfo.server );
                if(retry_count == max_retry_count)
                {
                    printf("Giving up.\n\n");
                    return;
                }
                else
                {
                    printf("Failed. Waiting 5 Sec. \n");
                    sleep(5);
                    retry_count++;
                }
            }
            else
            {
                printf("Success! We're connected.\n");
                break;
            }
        }
        _running = true;
        innerHandle.release();
        if ( irc_run (_session) )
        {
            MutexHandle innerHandle(&_innerMutex);
            _running = false;
            innerHandle.release();
            printf ("Could not connect or I/O error: %s (Server: %s )\n", irc_strerror (irc_errno(_session)), _serverInfo.server);
        }
    }
}

void IrcConnection::resetSession()
{
    MutexHandle innerHandle(&_innerMutex);
    if(_running)
        return;

    if(_session)
        irc_destroy_session(_session);
    _session = NULL;
}


void IrcConnection::_setCallbacks()
{
    memset (&_callbacks, 0, sizeof(_callbacks));
    
    _callbacks.event_connect        = irc_connection_event_connect;
    _callbacks.event_nick           = irc_connection_event_nick;
    _callbacks.event_quit           = irc_connection_event_quit;
    _callbacks.event_join           = irc_connection_event_join;
    _callbacks.event_part           = irc_connection_event_part;
    _callbacks.event_mode           = irc_connection_event_mode;
    _callbacks.event_umode          = irc_connection_event_umode;
    _callbacks.event_topic          = irc_connection_event_topic;
    _callbacks.event_kick           = irc_connection_event_kick;
    _callbacks.event_channel        = irc_connection_event_channel;
    _callbacks.event_privmsg        = irc_connection_event_privmsg;
    _callbacks.event_notice         = irc_connection_event_notice;
    _callbacks.event_channel_notice = irc_connection_event_channel_notice;
    _callbacks.event_invite         = irc_connection_event_invite;
    _callbacks.event_ctcp_req       = irc_connection_event_ctcp_req;
    _callbacks.event_ctcp_rep       = irc_connection_event_ctcp_rep;
    _callbacks.event_ctcp_action    = irc_connection_event_ctcp_action;
    _callbacks.event_unknown        = irc_connection_event_unknown;
    _callbacks.event_numeric        = irc_connection_event_numeric;
    _callbacks.event_dcc_chat_req   = irc_connection_event_dcc_chat_req;
    _callbacks.event_dcc_send_req   = irc_connection_event_dcc_send_req;
}