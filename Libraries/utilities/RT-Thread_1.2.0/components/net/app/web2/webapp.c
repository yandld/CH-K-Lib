
#include <string.h>
#include <stdio.h>
#include <rtthread.h>

#include "webfilesystem.h"
#include "webserver.h"

#include "htmlfile.h"
#include "imagefile.h"

#define STOP	0
#define STARTED	1

#define OFF		0
#define	ON		1

static const char *dow[]   = {"Sunday","Monday","Tuesday","Wednesday","Thursday",
								"Friday","Saturday"};
static const char *month[] = {"January","February","March","April","May","June",
								"July","August","September","October","November","December"};

static char valve1 = ON, valve2 = ON, heater = ON, cycle, state;
static char led_enable = 1;
static char autorefresh = 1;
static char refreshtime = 10;

static unsigned int liquid = 20, temp = 25;

static int HOUR = 20;
static int MIN = 0;
static int SEC = 0;
static int DOW = 6;
static int DOM = 8;
static int MONTH = 5;
static int YEAR = 2010;

void app_process(void)
{

}

int app_process_post(const char rxBuff[], int dataLen)
{
	static char to[50], subject[50], message[200];
    char value[200];
    int i = 0;
    int j;

    while (i < dataLen)
    {
        if (rxBuff[i] == '\r')
        {
            if (rxBuff[i + 1] == '\n' && rxBuff[i + 2] == '\r' &&
                rxBuff[i + 3] == '\n')
            {
                i += 4;
                break;
            }
            else
            {
                i += 4;
            }
        }
        else
        {
            i++;
        }
    }

    if (http_parse_variable(&rxBuff[i], "mail", dataLen - i, value))
    {
        if (!memcmp(value, "Send", 4))
        {
            if (http_parse_variable(&rxBuff[i], "to", dataLen - i, value))
            {
                strcpy(to, value);
            }
            if (http_parse_variable(&rxBuff[i], "subject", dataLen - i, value))
            {
                strcpy(subject, value);
            }
            if (http_parse_variable(&rxBuff[i], "message", dataLen - i, value))
            {
                memcpy(message, value, 200);
            }
			rt_kprintf("to = %s, subject = %s, message = %s\n", to, subject, message);
            return 1;
        }
    }

    if (http_parse_variable(&rxBuff[i], "options", dataLen - i, value))
    {
        if (!strcmp(value, "Start"))
        {
            if (state == STOP)
            {
                state = STARTED;
                app_process();
            }
        }
        cycle = 0;
        if (http_parse_variable(&rxBuff[i], "cycle", dataLen - i, value))
        {
            if (!strcmp(value, "Cycle"))
                cycle = 1;
        }
        autorefresh = 0;
        if (http_parse_variable(&rxBuff[i], "autorefresh", dataLen - i, value))
        {
            if (!strcmp(value, "Autorefresh"))
                autorefresh = 1;
        }
        if (http_parse_variable(&rxBuff[i], "refreshtime", dataLen - i, value))
        {
            if (value[1] == 0)
            {
                value[1] = value[0];
                value[0] = 0x30;
            }
            refreshtime = (value[0] - 0x30) *10+(value[1] - 0x30);
        }
        return 1;
    }

    if (http_parse_variable(&rxBuff[i], "send", dataLen - i, value))
    {
        if (!memcmp(value, "Cancel", 6))
            return 0;
    }

    led_enable = 0;
    if (http_parse_variable(&rxBuff[i], "led", dataLen - i, value))
    {
        if (!strcmp(value, "on"))
            led_enable = 1;
    }

    if (http_parse_variable(&rxBuff[i], "dow", dataLen - i, value))
    {
        for (j = 0; j < 7; j++)
        {
            if (strcmp(dow[j], value) == 0)
            {
                DOW = j;
                break;
            }
        }
    }

    if (http_parse_variable(&rxBuff[i], "month", dataLen - i, value))
    {
        for (j = 0; j < 12; j++)
        {
            if (strcmp(month[j], value) == 0)
            {
                MONTH = j + 1;
                break;
            }
        }
    }

    if (http_parse_variable(&rxBuff[i], "dom", dataLen - i, value))
    {
        DOM = (value[0] - 0x30)*10+(value[1] - 0x30);
    }

    if (http_parse_variable(&rxBuff[i], "year", dataLen - i, value))
    {
        YEAR = (value[0] - 0x30)*1000+(value[1] - 0x30)*100+(value[2] - 0x30)*10+(value[3] - 0x30);
    }

    if (http_parse_variable(&rxBuff[i], "hour", dataLen - i, value))
    {
        HOUR = (value[0] - 0x30)*10+(value[1] - 0x30);
    }

    if (http_parse_variable(&rxBuff[i], "min", dataLen - i, value))
    {
        MIN = (value[0] - 0x30)*10+(value[1] - 0x30);
    }

    if (http_parse_variable(&rxBuff[i], "sec", dataLen - i, value))
    {
        SEC = (value[0] - 0x30)*10+(value[1] - 0x30);
    }
    return 1;
}

int app_process_expand_tag(int tagID, char txBuff[], int len)
{
    int i;

    switch (tagID)
    {
        case 10:
            if (len < 2)
                return  - 1;
            sprintf(txBuff, "%02d", HOUR);
            return 2;

        case 11:
            if (len < 2)
                return  - 1;
            sprintf(txBuff, "%02d", MIN);
            return 2;

        case 12:
            if (len < 2)
                return  - 1;
            sprintf(txBuff, "%02d", SEC);
            return 2;

        case 20:
            i = strlen(dow[DOW]);
            if (len < i)
                return  - 1;
            memcpy(txBuff, dow[DOW], i);
            return i;

        case 21:
            i = strlen(month[MONTH - 1]);
            if (len < i)
                return  - 1;
            memcpy(txBuff, month[MONTH - 1], i);
            return i;

        case 22:
            if (len < 2)
                return  - 1;
            sprintf(txBuff, "%02d", DOM);
            return 2;

        case 23:
            if (len < 4)
                return  - 1;
            sprintf(txBuff, "%04d", YEAR);
            return 4;

        case 24:
            if (led_enable)
            {
                if (len < 7)
                    return  - 1;
                memcpy(txBuff, "CHECKED", 7);
                return 7;
            }
            else
                return 0;

        case 25:
            if (led_enable)
            {
                if (len < 2)
                    return  - 1;
                memcpy(txBuff, "On", 2);
                return 2;
            }
            else
            {
                if (len < 3)
                    return  - 1;
                memcpy(txBuff, "Off", 3);
                return 3;
            }

            // applic.htm tags
        case 49:
            if (len < 7)
                return  - 1;
            switch (state)
            {
				case 0:
					sprintf(txBuff, "%s", "STOPPED");
					return 7;
				case 1:
					sprintf(txBuff, "%s", "STARTED");
					return 7;
				case 2:
					sprintf(txBuff, "%s", "FILLING");
					return 7;
				case 3:
					sprintf(txBuff, "%s", "HEATING");
					return 7;
				case 4:
					sprintf(txBuff, "%s", "EMTYING");
					return 7;
				default:
					sprintf(txBuff, "%s", "UNKNOWN");
					return 7;
            }

        case 50:
            if (valve1 == ON)
            {
                if (len < 41)
                    return  - 1;
                sprintf(txBuff, "%s", "<img src=level.gif height=6 width=30><br>");
                return 41;
            }
            else
                return 0;

        case 51:
            if (valve1 == ON)
            {
                if (len < 10)
                    return  - 1;
                sprintf(txBuff, "%s", "pumpon.gif");
                return 10;
            }
            else
            {
                if (len < 11)
                    return  - 1;
                sprintf(txBuff, "%s", "pumpoff.gif");
                return 11;
            }

        case 52:
            if (len < 3)
                return  - 1;
            sprintf(txBuff, "%03d", liquid *6);
            return 3;

        case 53:
            if (valve2 == ON)
            {
                if (len < 10)
                    return  - 1;
                sprintf(txBuff, "%s", "pumpon.gif");
                return 10;
            }
            else
            {
                if (len < 11)
                    return  - 1;
                sprintf(txBuff, "%s", "pumpoff.gif");
                return 11;
            }

        case 54:
            if (valve2 == ON)
            {
                if (len < 41)
                    return  - 1;
                sprintf(txBuff, "%s", "<img src=level.gif height=6 width=30><br>");
                return 41;
            }
            else
                return 0;

        case 55:
            if (len < 3)
                return  - 1;
            sprintf(txBuff, "%03d", temp *4);
            return 3;

        case 56:
            if (valve1 == ON)
            {
                if (len < 10)
                    return  - 1;
                sprintf(txBuff, "%s", "arrows.gif");
                return 10;
            }
            else
            {
                if (len < 9)
                    return  - 1;
                sprintf(txBuff, "%s", "clear.gif");
                return 9;
            }

        case 57:
            if (heater == ON)
            {
                if (len < 8)
                    return  - 1;
                sprintf(txBuff, "%s", "fire.gif");
                return 8;
            }
            else
            {
                if (len < 9)
                    return  - 1;
                sprintf(txBuff, "%s", "clear.gif");
                return 9;
            }

        case 58:
            if (valve2 == ON)
            {
                if (len < 10)
                    return  - 1;
                sprintf(txBuff, "%s", "arrows.gif");
                return 10;
            }
            else
            {
                if (len < 9)
                    return  - 1;
                sprintf(txBuff, "%s", "clear.gif");
                return 9;
            }

        case 59:
            if (valve1 == ON)
            {
                if (len < 2)
                    return  - 1;
                sprintf(txBuff, "%s", "ON");
                return 2;
            }
            else
            {
                if (len < 3)
                    return  - 1;
                sprintf(txBuff, "%s", "OFF");
                return 3;
            }

        case 60:
            if (heater == ON)
            {
                if (len < 2)
                    return  - 1;
                sprintf(txBuff, "%s", "ON");
                return 2;
            }
            else
            {
                if (len < 3)
                    return  - 1;
                sprintf(txBuff, "%s", "OFF");
                return 3;
            }

        case 61:
            if (valve2 == ON)
            {
                if (len < 2)
                    return  - 1;
                sprintf(txBuff, "%s", "ON");
                return 2;
            }
            else
            {
                if (len < 3)
                    return  - 1;
                sprintf(txBuff, "%s", "OFF");
                return 3;
            }

        case 62:
            if (len < 2)
                return  - 1;
            sprintf(txBuff, "%2d", liquid);
            return 2;

        case 63:
            if (len < 2)
                return  - 1;
            sprintf(txBuff, "%2d", temp);
            return 2;

        case 64:
            if (cycle)
            {
                if (len < 7)
                    return  - 1;
                memcpy(txBuff, "CHECKED", 7);
                return 7;
            }
            else
                return 0;

        case 65:
            if (autorefresh)
            {
                if (len < 7)
                    return  - 1;
                memcpy(txBuff, "CHECKED", 7);
                return 7;
            }
            else
                return 0;

        case 66:
            if (autorefresh)
            {
                if (len < 33)
                    return  - 1;
                sprintf(txBuff, "%s", "<meta http-equiv=refresh content=");
                return 33;
            }
            else
                return 0;

        case 67:
            if (autorefresh)
            {
                if (len < 3)
                    return  - 1;
                sprintf(txBuff, "%2d>", refreshtime);
                return 3;
            }
            else
                return 0;

        case 68:
            if (len < 2)
                return  - 1;
            sprintf(txBuff, "%2d", refreshtime);
            return 2;

        default:
            return  - 2;
    }
}

void app_load_webfilesystem(void)
{
	AddFile("index.htm",    HTML, index,    sizeof(index));
	AddFile("about.htm",    HTML, about,    sizeof(about));
	AddFile("applic.htm",   HTML, applic,   sizeof(applic));
	AddFile("mail.htm",     HTML, mail,     sizeof(mail));
	AddFile("sent.htm",     HTML, sent,     sizeof(sent));
	AddFile("setup.htm",    HTML, setup,    sizeof(setup));
	AddFile("setupok.htm",  HTML, setupok,  sizeof(setupok));

	AddFile("arrows.gif",   GIF,  arrows,   sizeof(arrows));
	AddFile("black.gif",    GIF,  black,    sizeof(black));
	AddFile("clear.gif",    GIF,  clear,    sizeof(clear));
	AddFile("fire.gif",     GIF,  fire,     sizeof(fire));
	AddFile("iar.gif",      GIF,  iar,      sizeof(iar));
	AddFile("level.gif",    GIF,  level,    sizeof(level));
	AddFile("logo.gif",     GIF,  logo,     sizeof(logo));
	AddFile("mercury.gif",  GIF,  mercury,  sizeof(mercury));
	AddFile("pearson.gif",  GIF,  pearson,  sizeof(pearson));
	AddFile("psl_logo.gif", GIF,  psl_logo, sizeof(psl_logo));
	AddFile("pumpoff.gif",  GIF,  pumpoff,  sizeof(pumpoff));
	AddFile("pumpon.gif",   GIF,  pumpon,   sizeof(pumpon));
	AddFile("scale.gif",    GIF,  scale,    sizeof(scale));

	AddFile("philips.jpg",  JPEG, philips,  sizeof(philips));
	AddFile("sergio.jpg",   JPEG, sergio,   sizeof(sergio));
	AddFile("valve.jpg",    JPEG, valve,    sizeof(valve));
}
