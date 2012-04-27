#ifndef _SIMPLE_BOT_H_
#define _SIMPLE_BOT_H_

//simpleBot.h
//Author: Simon Wittenberg

#include <irc/ircconnection.h>

class SimpleBot : public IrcConnection
{
public:
    typedef IrcConnection Parent;

    virtual void on_connect(const String event, const String server, const String myNick, const StringVector params);
    virtual void on_nick(const String event, const String oldNick, const String newNick){};
    virtual void on_quit(const String event, const String nick, const String reason)
    { Parent::on_quit(event, nick, reason); };
    virtual void on_join(const String event, const String nick, const String channel);
    virtual void on_part(const String event, const String nick, const String channel){};
    virtual void on_mode(const String event, const String nick, const String channel, const String modes, const StringVector params){};
    virtual void on_umode(const String event, const String origin, const StringVector params)
    {_dumpData("on_umode",event,origin,params);};
    virtual void on_kick(const String event, const String nick, const String channel, const String target, const String kickMsg){};
    virtual void on_topic(const String event, const String nick, const String channel, const String topic){};
    virtual void on_channel(const String event, const String nick, const String channel, const String msg);
    virtual void on_private_message(const String event, const String nick, const String msg){};
    virtual void on_notice(const String event, const String nick, const String msg){};
    virtual void on_channel_notice(const String event, const String origin, const StringVector params)
    {_dumpData("on_channel_notice",event,origin,params);};
    virtual void on_invite(const String event, const String nick, const String channel){};
    virtual void on_ctcp_request(const String event, const String origin, const StringVector params)
    {_dumpData("on_ctcp_request",event,origin,params);};
    virtual void on_ctcp_reply(const String event, const String origin, const StringVector params)
    {_dumpData("on_ctcp_reply",event,origin,params);};
    virtual void on_unknown(const String event, const String origin, const StringVector params)
    {
        _dumpData("on_unknown",event,origin,params);
        Parent::on_unknown(event, origin, params);
    };
    virtual void on_ctcp_action(const String event, const String origin, const StringVector params)
    {_dumpData("on_ctcp_action",event,origin,params);};
    virtual void on_numeric_code(const unsigned int event, const String origin, const StringVector params)
    {
        Parent::on_numeric_code(event, origin, params);
        unsigned int count = params.size();
        printf("Function name : %s \n"
               "event         : %s (%d) \n"
               "origin        : %s \n"
               "count         : %d \n", "on_numeric_code", irc_strerror(event), event , origin.c_str(), count);
        for(unsigned int i = 0; i < count; i++)
            printf("params[%d]    : %s \n", i, params[i].c_str());
        printf("---------------------\n");

    };
    virtual void on_dcc_chat_req(const String nick, const String addr, irc_dcc_t dccid)
    {
        printf("Function name : %s \n"
               "nick          : %s \n"
               "addr          : %s \n"
               "dccid         : %d \n", "on_dcc_chat_req", nick.c_str(), addr.c_str(), dccid);
        printf("---------------------\n");
    };
    virtual void on_dcc_send_req(const String nick, const String addr, const String filename, unsigned long size, irc_dcc_t dccid)
    {
        printf("Function name : %s \n"
               "nick          : %s \n"
               "addr          : %s \n"
               "fileName      : %s \n"
               "size          : %d \n"
               "dccid         : %d \n", "on_dcc_send_req", nick.c_str(), addr.c_str(), filename.c_str(), size, dccid);
        printf("---------------------\n");
    };
private:
    void _dumpData(const char* funcName, const String event, const String origin, const StringVector params);
};

#endif