#include "util.hpp"

static const uint32_t verboten[] = { L',', L'/', L'\\', L'<', L'>', L':', L'"', L'|', L'?', L'*', L'™', L'©', L'®'};

static bool isVerboten(const uint32_t& t)
{
    for(unsigned i = 0; i < 13; i++)
    {
        if(t == verboten[i])
            return true;
    }

    return false;
}

uint32_t replaceChar(uint32_t c)
{
    switch(c)
    {
        case L'é':
            return 'e';
            break;
    }

    return c;
}

std::string util::safeString(const std::string& s)
{
    std::string ret = "";
    for(unsigned i = 0; i < s.length(); )
    {
        uint32_t tmpChr = 0;
        ssize_t untCnt = decode_utf8(&tmpChr, (uint8_t *)&s.data()[i]);

        i += untCnt;

        tmpChr = replaceChar(tmpChr);

        if(isVerboten(tmpChr))
            ret += ' ';
        else if(!isASCII(tmpChr))
            return ""; //return empty string so titledata::init defaults to titleID
        else
            ret += (char)tmpChr;
    }

    //Check for spaces at end
    while(ret[ret.length() - 1] == ' ' || ret[ret.length() - 1] == '.')
        ret.erase(ret.length() - 1, 1);

    return ret;
}