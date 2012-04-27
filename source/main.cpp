//main.cpp
//Author: Simon Wittenberg
//Based on libirclient examples

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <util/threadhelper.h>

#include "irc/ircConnection.h"
#include "bots/simplebot.h"



int main (int argc, char **argv)
{
    //sleep(15);
    IRCServerInfo       irc_server_info;
    /*IrcConnection*/SimpleBot       irc_connection;

    if ( argc != 4 )
    {
        printf ("Usage: %s <server> <nick> <channel>\n", argv[0]);
        return 1;
    }

    irc_server_info.server = argv[1];
    irc_server_info.channel = argv[3];
    irc_server_info.nick = argv[2];
    
    irc_connection.setServerInfo(irc_server_info);

    int cycle_length = 900;
    unsigned int sleep_cycle = 30;
    int cycle_counter = 0;
    int message_counter = 0;
    int maxCount = cycle_length / sleep_cycle;
    
    bool keep_running = true;
    while(keep_running)
    {
        //printf("We're at time mark #%d and the bot is %s running.\n",times,  irc_connection.isRunning() ? "smoothly" : "not");
        if(!irc_connection.isRunning())
        {
            printf("Trying to start the bot.\n");
            message_counter = 0;
            irc_connection.start();
        }
        else
        {
            message_counter++;
            if(message_counter >= maxCount)
            {
                message_counter = 0;
                cycle_counter++;
                float totalcycletime = cycle_length*cycle_counter;
                float totalMinFloat = totalcycletime / 60.f;
                char textbuf[168];
                sprintf(textbuf, "I have been running for %10.2f minutes without a crash.\n", totalMinFloat);
                irc_connection.sendMessage(irc_server_info.channel, String(textbuf));
            }
        }
        // this is the actual main loop;
        sleep(sleep_cycle);
    }

    return 1;
}
