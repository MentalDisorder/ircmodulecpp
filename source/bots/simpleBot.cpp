#include "simpleBot.h"

//simpleBot.cpp
//Author: Simon Wittenberg

void SimpleBot::on_connect(const String event, const String server, const String myNick, const StringVector params)
{
    Parent::on_connect(event, server, myNick, params);
    join(_serverInfo.channel, NullString);
}

void SimpleBot::on_join(const String event, const String nick, const String channel)
{
    //_dumpData("on_join",event,origin,params, count);

    if(nick.compare(_serverInfo.nick)==0)
    {
        sendMessage(channel, "Here I am!");
    }
    else
    {
        sendMessage(channel, String("Hi, ").append(nick));
    }
}

void SimpleBot::on_channel(const String event, const String nick, const String channel, const String msg)
{
    printf("on_channel\n"
          "[%s]::[%s] : [%s]\n", channel.c_str(), nick.c_str(), msg.c_str());

    if(nick.compare(_serverInfo.nick) == 0)
        return; // don't react further to anything we post ourselves
    size_t found;

    String cueWord("Cue");
    found = msg.find(cueWord);
    if (found!=String::npos)
        sendMessage(channel, String("@").append(nick).append(String(", ").append(String("My Cue!"))));
    
    String helloBot = String("Hallo ").append(_serverInfo.nick);
    found = msg.find(helloBot);
    if (found!=String::npos)
        sendMessage(channel, String("Hallo, ").append(nick));

    String quitcommand("?quit");
    found = msg.find(quitcommand);
    if (found!=String::npos)
        quit("Quit Command called.");
}

void SimpleBot::_dumpData(const char* funcName, const String event, const String origin, const StringVector params)
{
    unsigned int count = params.size();
    printf("Function name : %s \n"
           "event         : %s \n"
           "origin        : %s \n"
           "count         : %d \n", funcName, event.c_str(), origin.c_str(), count);
    for(unsigned int i = 0; i < params.size(); i++)
        printf("params[%d]     : %s \n", i, params[i].c_str());
    printf("---------------------\n");
}