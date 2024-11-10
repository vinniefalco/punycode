//
// Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/cppalliance/punycode
//

#include <boost/punycode/idna.hpp>
#include <boost/punycode/punycode.hpp>
#include <boost/punycode/utf8_count.hpp>
#include <boost/punycode/utf8_input.hpp>
#include <boost/punycode/utf8_output.hpp>
#include <algorithm>

namespace boost {
namespace punycode {

//------------------------------------------------
//
// NAMEPREP
//
// https://datatracker.ietf.org/doc/html/rfc3491
//

class nameprep_iterator_base
{
protected:
    enum : char32_t
    {
        invalid = 0xffffffff
    };

    char32_t cp_[4];
    char i_ = 0;

    void f(
        char32_t cp) noexcept
    {
        cp_[0] = cp;
        i_ = 0;
    }

    void f(
        char32_t cp0,
        char32_t cp1) noexcept
    {
        cp_[1] = cp0;
        cp_[0] = cp1;
        i_ = 1;
    }

    void f(
        char32_t cp0,
        char32_t cp1,
        char32_t cp2) noexcept
    {
        cp_[2] = cp0;
        cp_[1] = cp1;
        cp_[0] = cp2;
        i_ = 2;
    }

    void f(
        char32_t cp0,
        char32_t cp1,
        char32_t cp2,
        char32_t cp3) noexcept
    {
        cp_[3] = cp0;
        cp_[2] = cp1;
        cp_[1] = cp2;
        cp_[0] = cp3;
        i_ = 3;
    }

    static constexpr bool
    map_to_nothing(char32_t cp) noexcept;

    void map(char32_t cp) noexcept;
};

//------------------------------------------------

// https://datatracker.ietf.org/doc/html/rfc3454#appendix-B.1
constexpr bool
nameprep_iterator_base::
map_to_nothing(char32_t cp) noexcept
{
    switch(cp)
    {
    case 0x00AD:
    case 0x034F:
    case 0x1806:
    case 0x180B:
    case 0x180C:
    case 0x180D:
    case 0x200B:
    case 0x200C:
    case 0x200D:
    case 0x2060:
    case 0xFE00:
    case 0xFE01:
    case 0xFE02:
    case 0xFE03:
    case 0xFE04:
    case 0xFE05:
    case 0xFE06:
    case 0xFE07:
    case 0xFE08:
    case 0xFE09:
    case 0xFE0A:
    case 0xFE0B:
    case 0xFE0C:
    case 0xFE0D:
    case 0xFE0E:
    case 0xFE0F:
    case 0xFEFF:
        return true;
    default:
        break;
    }
    return false;
}

// https://datatracker.ietf.org/doc/html/rfc3454#appendix-B.2
inline
void
nameprep_iterator_base::
map(char32_t cp) noexcept
{
    switch(cp)
    {
case 0x0041:  return f(0x0061); // Case map
case 0x0042:  return f(0x0062); // Case map
case 0x0043:  return f(0x0063); // Case map
case 0x0044:  return f(0x0064); // Case map
case 0x0045:  return f(0x0065); // Case map
case 0x0046:  return f(0x0066); // Case map
case 0x0047:  return f(0x0067); // Case map
case 0x0048:  return f(0x0068); // Case map
case 0x0049:  return f(0x0069); // Case map
case 0x004A:  return f(0x006A); // Case map
case 0x004B:  return f(0x006B); // Case map
case 0x004C:  return f(0x006C); // Case map
case 0x004D:  return f(0x006D); // Case map
case 0x004E:  return f(0x006E); // Case map
case 0x004F:  return f(0x006F); // Case map
case 0x0050:  return f(0x0070); // Case map
case 0x0051:  return f(0x0071); // Case map
case 0x0052:  return f(0x0072); // Case map
case 0x0053:  return f(0x0073); // Case map
case 0x0054:  return f(0x0074); // Case map
case 0x0055:  return f(0x0075); // Case map
case 0x0056:  return f(0x0076); // Case map
case 0x0057:  return f(0x0077); // Case map
case 0x0058:  return f(0x0078); // Case map
case 0x0059:  return f(0x0079); // Case map
case 0x005A:  return f(0x007A); // Case map
case 0x00B5:  return f(0x03BC); // Case map
case 0x00C0:  return f(0x00E0); // Case map
case 0x00C1:  return f(0x00E1); // Case map
case 0x00C2:  return f(0x00E2); // Case map
case 0x00C3:  return f(0x00E3); // Case map
case 0x00C4:  return f(0x00E4); // Case map
case 0x00C5:  return f(0x00E5); // Case map
case 0x00C6:  return f(0x00E6); // Case map
case 0x00C7:  return f(0x00E7); // Case map
case 0x00C8:  return f(0x00E8); // Case map
case 0x00C9:  return f(0x00E9); // Case map
case 0x00CA:  return f(0x00EA); // Case map
case 0x00CB:  return f(0x00EB); // Case map
case 0x00CC:  return f(0x00EC); // Case map
case 0x00CD:  return f(0x00ED); // Case map
case 0x00CE:  return f(0x00EE); // Case map
case 0x00CF:  return f(0x00EF); // Case map
case 0x00D0:  return f(0x00F0); // Case map
case 0x00D1:  return f(0x00F1); // Case map
case 0x00D2:  return f(0x00F2); // Case map
case 0x00D3:  return f(0x00F3); // Case map
case 0x00D4:  return f(0x00F4); // Case map
case 0x00D5:  return f(0x00F5); // Case map
case 0x00D6:  return f(0x00F6); // Case map
case 0x00D8:  return f(0x00F8); // Case map
case 0x00D9:  return f(0x00F9); // Case map
case 0x00DA:  return f(0x00FA); // Case map
case 0x00DB:  return f(0x00FB); // Case map
case 0x00DC:  return f(0x00FC); // Case map
case 0x00DD:  return f(0x00FD); // Case map
case 0x00DE:  return f(0x00FE); // Case map
case 0x00DF:  return f(0x0073, 0x0073); // Case map
case 0x0100:  return f(0x0101); // Case map
case 0x0102:  return f(0x0103); // Case map
case 0x0104:  return f(0x0105); // Case map
case 0x0106:  return f(0x0107); // Case map
case 0x0108:  return f(0x0109); // Case map
case 0x010A:  return f(0x010B); // Case map
case 0x010C:  return f(0x010D); // Case map
case 0x010E:  return f(0x010F); // Case map
case 0x0110:  return f(0x0111); // Case map
case 0x0112:  return f(0x0113); // Case map
case 0x0114:  return f(0x0115); // Case map
case 0x0116:  return f(0x0117); // Case map
case 0x0118:  return f(0x0119); // Case map
case 0x011A:  return f(0x011B); // Case map
case 0x011C:  return f(0x011D); // Case map
case 0x011E:  return f(0x011F); // Case map
case 0x0120:  return f(0x0121); // Case map
case 0x0122:  return f(0x0123); // Case map
case 0x0124:  return f(0x0125); // Case map
case 0x0126:  return f(0x0127); // Case map
case 0x0128:  return f(0x0129); // Case map
case 0x012A:  return f(0x012B); // Case map
case 0x012C:  return f(0x012D); // Case map
case 0x012E:  return f(0x012F); // Case map
case 0x0130:  return f(0x0069, 0x0307); // Case map
case 0x0132:  return f(0x0133); // Case map
case 0x0134:  return f(0x0135); // Case map
case 0x0136:  return f(0x0137); // Case map
case 0x0139:  return f(0x013A); // Case map
case 0x013B:  return f(0x013C); // Case map
case 0x013D:  return f(0x013E); // Case map
case 0x013F:  return f(0x0140); // Case map
case 0x0141:  return f(0x0142); // Case map
case 0x0143:  return f(0x0144); // Case map
case 0x0145:  return f(0x0146); // Case map
case 0x0147:  return f(0x0148); // Case map
case 0x0149:  return f(0x02BC, 0x006E); // Case map
case 0x014A:  return f(0x014B); // Case map
case 0x014C:  return f(0x014D); // Case map
case 0x014E:  return f(0x014F); // Case map
case 0x0150:  return f(0x0151); // Case map
case 0x0152:  return f(0x0153); // Case map
case 0x0154:  return f(0x0155); // Case map
case 0x0156:  return f(0x0157); // Case map
case 0x0158:  return f(0x0159); // Case map
case 0x015A:  return f(0x015B); // Case map
case 0x015C:  return f(0x015D); // Case map
case 0x015E:  return f(0x015F); // Case map
case 0x0160:  return f(0x0161); // Case map
case 0x0162:  return f(0x0163); // Case map
case 0x0164:  return f(0x0165); // Case map
case 0x0166:  return f(0x0167); // Case map
case 0x0168:  return f(0x0169); // Case map
case 0x016A:  return f(0x016B); // Case map
case 0x016C:  return f(0x016D); // Case map
case 0x016E:  return f(0x016F); // Case map
case 0x0170:  return f(0x0171); // Case map
case 0x0172:  return f(0x0173); // Case map
case 0x0174:  return f(0x0175); // Case map
case 0x0176:  return f(0x0177); // Case map
case 0x0178:  return f(0x00FF); // Case map
case 0x0179:  return f(0x017A); // Case map
case 0x017B:  return f(0x017C); // Case map
case 0x017D:  return f(0x017E); // Case map
case 0x017F:  return f(0x0073); // Case map
case 0x0181:  return f(0x0253); // Case map
case 0x0182:  return f(0x0183); // Case map
case 0x0184:  return f(0x0185); // Case map
case 0x0186:  return f(0x0254); // Case map
case 0x0187:  return f(0x0188); // Case map
case 0x0189:  return f(0x0256); // Case map
case 0x018A:  return f(0x0257); // Case map
case 0x018B:  return f(0x018C); // Case map
case 0x018E:  return f(0x01DD); // Case map
case 0x018F:  return f(0x0259); // Case map
case 0x0190:  return f(0x025B); // Case map
case 0x0191:  return f(0x0192); // Case map
case 0x0193:  return f(0x0260); // Case map
case 0x0194:  return f(0x0263); // Case map
case 0x0196:  return f(0x0269); // Case map
case 0x0197:  return f(0x0268); // Case map
case 0x0198:  return f(0x0199); // Case map
case 0x019C:  return f(0x026F); // Case map
case 0x019D:  return f(0x0272); // Case map
case 0x019F:  return f(0x0275); // Case map
case 0x01A0:  return f(0x01A1); // Case map
case 0x01A2:  return f(0x01A3); // Case map
case 0x01A4:  return f(0x01A5); // Case map
case 0x01A6:  return f(0x0280); // Case map
case 0x01A7:  return f(0x01A8); // Case map
case 0x01A9:  return f(0x0283); // Case map
case 0x01AC:  return f(0x01AD); // Case map
case 0x01AE:  return f(0x0288); // Case map
case 0x01AF:  return f(0x01B0); // Case map
case 0x01B1:  return f(0x028A); // Case map
case 0x01B2:  return f(0x028B); // Case map
case 0x01B3:  return f(0x01B4); // Case map
case 0x01B5:  return f(0x01B6); // Case map
case 0x01B7:  return f(0x0292); // Case map
case 0x01B8:  return f(0x01B9); // Case map
case 0x01BC:  return f(0x01BD); // Case map
case 0x01C4:  return f(0x01C6); // Case map
case 0x01C5:  return f(0x01C6); // Case map
case 0x01C7:  return f(0x01C9); // Case map
case 0x01C8:  return f(0x01C9); // Case map
case 0x01CA:  return f(0x01CC); // Case map
case 0x01CB:  return f(0x01CC); // Case map
case 0x01CD:  return f(0x01CE); // Case map
case 0x01CF:  return f(0x01D0); // Case map
case 0x01D1:  return f(0x01D2); // Case map
case 0x01D3:  return f(0x01D4); // Case map
case 0x01D5:  return f(0x01D6); // Case map
case 0x01D7:  return f(0x01D8); // Case map
case 0x01D9:  return f(0x01DA); // Case map
case 0x01DB:  return f(0x01DC); // Case map
case 0x01DE:  return f(0x01DF); // Case map
case 0x01E0:  return f(0x01E1); // Case map
case 0x01E2:  return f(0x01E3); // Case map
case 0x01E4:  return f(0x01E5); // Case map
case 0x01E6:  return f(0x01E7); // Case map
case 0x01E8:  return f(0x01E9); // Case map
case 0x01EA:  return f(0x01EB); // Case map
case 0x01EC:  return f(0x01ED); // Case map
case 0x01EE:  return f(0x01EF); // Case map
case 0x01F0:  return f(0x006A, 0x030C); // Case map
case 0x01F1:  return f(0x01F3); // Case map
case 0x01F2:  return f(0x01F3); // Case map
case 0x01F4:  return f(0x01F5); // Case map
case 0x01F6:  return f(0x0195); // Case map
case 0x01F7:  return f(0x01BF); // Case map
case 0x01F8:  return f(0x01F9); // Case map
case 0x01FA:  return f(0x01FB); // Case map
case 0x01FC:  return f(0x01FD); // Case map
case 0x01FE:  return f(0x01FF); // Case map
case 0x0200:  return f(0x0201); // Case map
case 0x0202:  return f(0x0203); // Case map
case 0x0204:  return f(0x0205); // Case map
case 0x0206:  return f(0x0207); // Case map
case 0x0208:  return f(0x0209); // Case map
case 0x020A:  return f(0x020B); // Case map
case 0x020C:  return f(0x020D); // Case map
case 0x020E:  return f(0x020F); // Case map
case 0x0210:  return f(0x0211); // Case map
case 0x0212:  return f(0x0213); // Case map
case 0x0214:  return f(0x0215); // Case map
case 0x0216:  return f(0x0217); // Case map
case 0x0218:  return f(0x0219); // Case map
case 0x021A:  return f(0x021B); // Case map
case 0x021C:  return f(0x021D); // Case map
case 0x021E:  return f(0x021F); // Case map
case 0x0220:  return f(0x019E); // Case map
case 0x0222:  return f(0x0223); // Case map
case 0x0224:  return f(0x0225); // Case map
case 0x0226:  return f(0x0227); // Case map
case 0x0228:  return f(0x0229); // Case map
case 0x022A:  return f(0x022B); // Case map
case 0x022C:  return f(0x022D); // Case map
case 0x022E:  return f(0x022F); // Case map
case 0x0230:  return f(0x0231); // Case map
case 0x0232:  return f(0x0233); // Case map
case 0x0345:  return f(0x03B9); // Case map
case 0x037A:  return f(0x0020, 0x03B9); // Additional folding
case 0x0386:  return f(0x03AC); // Case map
case 0x0388:  return f(0x03AD); // Case map
case 0x0389:  return f(0x03AE); // Case map
case 0x038A:  return f(0x03AF); // Case map
case 0x038C:  return f(0x03CC); // Case map
case 0x038E:  return f(0x03CD); // Case map
case 0x038F:  return f(0x03CE); // Case map
case 0x0390:  return f(0x03B9, 0x0308, 0x0301); // Case map
case 0x0391:  return f(0x03B1); // Case map
case 0x0392:  return f(0x03B2); // Case map
case 0x0393:  return f(0x03B3); // Case map
case 0x0394:  return f(0x03B4); // Case map
case 0x0395:  return f(0x03B5); // Case map
case 0x0396:  return f(0x03B6); // Case map
case 0x0397:  return f(0x03B7); // Case map
case 0x0398:  return f(0x03B8); // Case map
case 0x0399:  return f(0x03B9); // Case map
case 0x039A:  return f(0x03BA); // Case map
case 0x039B:  return f(0x03BB); // Case map
case 0x039C:  return f(0x03BC); // Case map
case 0x039D:  return f(0x03BD); // Case map
case 0x039E:  return f(0x03BE); // Case map
case 0x039F:  return f(0x03BF); // Case map
case 0x03A0:  return f(0x03C0); // Case map
case 0x03A1:  return f(0x03C1); // Case map
case 0x03A3:  return f(0x03C3); // Case map
case 0x03A4:  return f(0x03C4); // Case map
case 0x03A5:  return f(0x03C5); // Case map
case 0x03A6:  return f(0x03C6); // Case map
case 0x03A7:  return f(0x03C7); // Case map
case 0x03A8:  return f(0x03C8); // Case map
case 0x03A9:  return f(0x03C9); // Case map
case 0x03AA:  return f(0x03CA); // Case map
case 0x03AB:  return f(0x03CB); // Case map
case 0x03B0:  return f(0x03C5, 0x0308, 0x0301); // Case map
case 0x03C2:  return f(0x03C3); // Case map
case 0x03D0:  return f(0x03B2); // Case map
case 0x03D1:  return f(0x03B8); // Case map
case 0x03D2:  return f(0x03C5); // Additional folding
case 0x03D3:  return f(0x03CD); // Additional folding
case 0x03D4:  return f(0x03CB); // Additional folding
case 0x03D5:  return f(0x03C6); // Case map
case 0x03D6:  return f(0x03C0); // Case map
case 0x03D8:  return f(0x03D9); // Case map
case 0x03DA:  return f(0x03DB); // Case map
case 0x03DC:  return f(0x03DD); // Case map
case 0x03DE:  return f(0x03DF); // Case map
case 0x03E0:  return f(0x03E1); // Case map
case 0x03E2:  return f(0x03E3); // Case map
case 0x03E4:  return f(0x03E5); // Case map
case 0x03E6:  return f(0x03E7); // Case map
case 0x03E8:  return f(0x03E9); // Case map
case 0x03EA:  return f(0x03EB); // Case map
case 0x03EC:  return f(0x03ED); // Case map
case 0x03EE:  return f(0x03EF); // Case map
case 0x03F0:  return f(0x03BA); // Case map
case 0x03F1:  return f(0x03C1); // Case map
case 0x03F2:  return f(0x03C3); // Case map
case 0x03F4:  return f(0x03B8); // Case map
case 0x03F5:  return f(0x03B5); // Case map
case 0x0400:  return f(0x0450); // Case map
case 0x0401:  return f(0x0451); // Case map
case 0x0402:  return f(0x0452); // Case map
case 0x0403:  return f(0x0453); // Case map
case 0x0404:  return f(0x0454); // Case map
case 0x0405:  return f(0x0455); // Case map
case 0x0406:  return f(0x0456); // Case map
case 0x0407:  return f(0x0457); // Case map
case 0x0408:  return f(0x0458); // Case map
case 0x0409:  return f(0x0459); // Case map
case 0x040A:  return f(0x045A); // Case map
case 0x040B:  return f(0x045B); // Case map
case 0x040C:  return f(0x045C); // Case map
case 0x040D:  return f(0x045D); // Case map
case 0x040E:  return f(0x045E); // Case map
case 0x040F:  return f(0x045F); // Case map
case 0x0410:  return f(0x0430); // Case map
case 0x0411:  return f(0x0431); // Case map
case 0x0412:  return f(0x0432); // Case map
case 0x0413:  return f(0x0433); // Case map
case 0x0414:  return f(0x0434); // Case map
case 0x0415:  return f(0x0435); // Case map
case 0x0416:  return f(0x0436); // Case map
case 0x0417:  return f(0x0437); // Case map
case 0x0418:  return f(0x0438); // Case map
case 0x0419:  return f(0x0439); // Case map
case 0x041A:  return f(0x043A); // Case map
case 0x041B:  return f(0x043B); // Case map
case 0x041C:  return f(0x043C); // Case map
case 0x041D:  return f(0x043D); // Case map
case 0x041E:  return f(0x043E); // Case map
case 0x041F:  return f(0x043F); // Case map
case 0x0420:  return f(0x0440); // Case map
case 0x0421:  return f(0x0441); // Case map
case 0x0422:  return f(0x0442); // Case map
case 0x0423:  return f(0x0443); // Case map
case 0x0424:  return f(0x0444); // Case map
case 0x0425:  return f(0x0445); // Case map
case 0x0426:  return f(0x0446); // Case map
case 0x0427:  return f(0x0447); // Case map
case 0x0428:  return f(0x0448); // Case map
case 0x0429:  return f(0x0449); // Case map
case 0x042A:  return f(0x044A); // Case map
case 0x042B:  return f(0x044B); // Case map
case 0x042C:  return f(0x044C); // Case map
case 0x042D:  return f(0x044D); // Case map
case 0x042E:  return f(0x044E); // Case map
case 0x042F:  return f(0x044F); // Case map
case 0x0460:  return f(0x0461); // Case map
case 0x0462:  return f(0x0463); // Case map
case 0x0464:  return f(0x0465); // Case map
case 0x0466:  return f(0x0467); // Case map
case 0x0468:  return f(0x0469); // Case map
case 0x046A:  return f(0x046B); // Case map
case 0x046C:  return f(0x046D); // Case map
case 0x046E:  return f(0x046F); // Case map
case 0x0470:  return f(0x0471); // Case map
case 0x0472:  return f(0x0473); // Case map
case 0x0474:  return f(0x0475); // Case map
case 0x0476:  return f(0x0477); // Case map
case 0x0478:  return f(0x0479); // Case map
case 0x047A:  return f(0x047B); // Case map
case 0x047C:  return f(0x047D); // Case map
case 0x047E:  return f(0x047F); // Case map
case 0x0480:  return f(0x0481); // Case map
case 0x048A:  return f(0x048B); // Case map
case 0x048C:  return f(0x048D); // Case map
case 0x048E:  return f(0x048F); // Case map
case 0x0490:  return f(0x0491); // Case map
case 0x0492:  return f(0x0493); // Case map
case 0x0494:  return f(0x0495); // Case map
case 0x0496:  return f(0x0497); // Case map
case 0x0498:  return f(0x0499); // Case map
case 0x049A:  return f(0x049B); // Case map
case 0x049C:  return f(0x049D); // Case map
case 0x049E:  return f(0x049F); // Case map
case 0x04A0:  return f(0x04A1); // Case map
case 0x04A2:  return f(0x04A3); // Case map
case 0x04A4:  return f(0x04A5); // Case map
case 0x04A6:  return f(0x04A7); // Case map
case 0x04A8:  return f(0x04A9); // Case map
case 0x04AA:  return f(0x04AB); // Case map
case 0x04AC:  return f(0x04AD); // Case map
case 0x04AE:  return f(0x04AF); // Case map
case 0x04B0:  return f(0x04B1); // Case map
case 0x04B2:  return f(0x04B3); // Case map
case 0x04B4:  return f(0x04B5); // Case map
case 0x04B6:  return f(0x04B7); // Case map
case 0x04B8:  return f(0x04B9); // Case map
case 0x04BA:  return f(0x04BB); // Case map
case 0x04BC:  return f(0x04BD); // Case map
case 0x04BE:  return f(0x04BF); // Case map
case 0x04C1:  return f(0x04C2); // Case map
case 0x04C3:  return f(0x04C4); // Case map
case 0x04C5:  return f(0x04C6); // Case map
case 0x04C7:  return f(0x04C8); // Case map
case 0x04C9:  return f(0x04CA); // Case map
case 0x04CB:  return f(0x04CC); // Case map
case 0x04CD:  return f(0x04CE); // Case map
case 0x04D0:  return f(0x04D1); // Case map
case 0x04D2:  return f(0x04D3); // Case map
case 0x04D4:  return f(0x04D5); // Case map
case 0x04D6:  return f(0x04D7); // Case map
case 0x04D8:  return f(0x04D9); // Case map
case 0x04DA:  return f(0x04DB); // Case map
case 0x04DC:  return f(0x04DD); // Case map
case 0x04DE:  return f(0x04DF); // Case map
case 0x04E0:  return f(0x04E1); // Case map
case 0x04E2:  return f(0x04E3); // Case map
case 0x04E4:  return f(0x04E5); // Case map
case 0x04E6:  return f(0x04E7); // Case map
case 0x04E8:  return f(0x04E9); // Case map
case 0x04EA:  return f(0x04EB); // Case map
case 0x04EC:  return f(0x04ED); // Case map
case 0x04EE:  return f(0x04EF); // Case map
case 0x04F0:  return f(0x04F1); // Case map
case 0x04F2:  return f(0x04F3); // Case map
case 0x04F4:  return f(0x04F5); // Case map
case 0x04F8:  return f(0x04F9); // Case map
case 0x0500:  return f(0x0501); // Case map
case 0x0502:  return f(0x0503); // Case map
case 0x0504:  return f(0x0505); // Case map
case 0x0506:  return f(0x0507); // Case map
case 0x0508:  return f(0x0509); // Case map
case 0x050A:  return f(0x050B); // Case map
case 0x050C:  return f(0x050D); // Case map
case 0x050E:  return f(0x050F); // Case map
case 0x0531:  return f(0x0561); // Case map
case 0x0532:  return f(0x0562); // Case map
case 0x0533:  return f(0x0563); // Case map
case 0x0534:  return f(0x0564); // Case map
case 0x0535:  return f(0x0565); // Case map
case 0x0536:  return f(0x0566); // Case map
case 0x0537:  return f(0x0567); // Case map
case 0x0538:  return f(0x0568); // Case map
case 0x0539:  return f(0x0569); // Case map
case 0x053A:  return f(0x056A); // Case map
case 0x053B:  return f(0x056B); // Case map
case 0x053C:  return f(0x056C); // Case map
case 0x053D:  return f(0x056D); // Case map
case 0x053E:  return f(0x056E); // Case map
case 0x053F:  return f(0x056F); // Case map
case 0x0540:  return f(0x0570); // Case map
case 0x0541:  return f(0x0571); // Case map
case 0x0542:  return f(0x0572); // Case map
case 0x0543:  return f(0x0573); // Case map
case 0x0544:  return f(0x0574); // Case map
case 0x0545:  return f(0x0575); // Case map
case 0x0546:  return f(0x0576); // Case map
case 0x0547:  return f(0x0577); // Case map
case 0x0548:  return f(0x0578); // Case map
case 0x0549:  return f(0x0579); // Case map
case 0x054A:  return f(0x057A); // Case map
case 0x054B:  return f(0x057B); // Case map
case 0x054C:  return f(0x057C); // Case map
case 0x054D:  return f(0x057D); // Case map
case 0x054E:  return f(0x057E); // Case map
case 0x054F:  return f(0x057F); // Case map
case 0x0550:  return f(0x0580); // Case map
case 0x0551:  return f(0x0581); // Case map
case 0x0552:  return f(0x0582); // Case map
case 0x0553:  return f(0x0583); // Case map
case 0x0554:  return f(0x0584); // Case map
case 0x0555:  return f(0x0585); // Case map
case 0x0556:  return f(0x0586); // Case map
case 0x0587:  return f(0x0565, 0x0582); // Case map
case 0x1E00:  return f(0x1E01); // Case map
case 0x1E02:  return f(0x1E03); // Case map
case 0x1E04:  return f(0x1E05); // Case map
case 0x1E06:  return f(0x1E07); // Case map
case 0x1E08:  return f(0x1E09); // Case map
case 0x1E0A:  return f(0x1E0B); // Case map
case 0x1E0C:  return f(0x1E0D); // Case map
case 0x1E0E:  return f(0x1E0F); // Case map
case 0x1E10:  return f(0x1E11); // Case map
case 0x1E12:  return f(0x1E13); // Case map
case 0x1E14:  return f(0x1E15); // Case map
case 0x1E16:  return f(0x1E17); // Case map
case 0x1E18:  return f(0x1E19); // Case map
case 0x1E1A:  return f(0x1E1B); // Case map
case 0x1E1C:  return f(0x1E1D); // Case map
case 0x1E1E:  return f(0x1E1F); // Case map
case 0x1E20:  return f(0x1E21); // Case map
case 0x1E22:  return f(0x1E23); // Case map
case 0x1E24:  return f(0x1E25); // Case map
case 0x1E26:  return f(0x1E27); // Case map
case 0x1E28:  return f(0x1E29); // Case map
case 0x1E2A:  return f(0x1E2B); // Case map
case 0x1E2C:  return f(0x1E2D); // Case map
case 0x1E2E:  return f(0x1E2F); // Case map
case 0x1E30:  return f(0x1E31); // Case map
case 0x1E32:  return f(0x1E33); // Case map
case 0x1E34:  return f(0x1E35); // Case map
case 0x1E36:  return f(0x1E37); // Case map
case 0x1E38:  return f(0x1E39); // Case map
case 0x1E3A:  return f(0x1E3B); // Case map
case 0x1E3C:  return f(0x1E3D); // Case map
case 0x1E3E:  return f(0x1E3F); // Case map
case 0x1E40:  return f(0x1E41); // Case map
case 0x1E42:  return f(0x1E43); // Case map
case 0x1E44:  return f(0x1E45); // Case map
case 0x1E46:  return f(0x1E47); // Case map
case 0x1E48:  return f(0x1E49); // Case map
case 0x1E4A:  return f(0x1E4B); // Case map
case 0x1E4C:  return f(0x1E4D); // Case map
case 0x1E4E:  return f(0x1E4F); // Case map
case 0x1E50:  return f(0x1E51); // Case map
case 0x1E52:  return f(0x1E53); // Case map
case 0x1E54:  return f(0x1E55); // Case map
case 0x1E56:  return f(0x1E57); // Case map
case 0x1E58:  return f(0x1E59); // Case map
case 0x1E5A:  return f(0x1E5B); // Case map
case 0x1E5C:  return f(0x1E5D); // Case map
case 0x1E5E:  return f(0x1E5F); // Case map
case 0x1E60:  return f(0x1E61); // Case map
case 0x1E62:  return f(0x1E63); // Case map
case 0x1E64:  return f(0x1E65); // Case map
case 0x1E66:  return f(0x1E67); // Case map
case 0x1E68:  return f(0x1E69); // Case map
case 0x1E6A:  return f(0x1E6B); // Case map
case 0x1E6C:  return f(0x1E6D); // Case map
case 0x1E6E:  return f(0x1E6F); // Case map
case 0x1E70:  return f(0x1E71); // Case map
case 0x1E72:  return f(0x1E73); // Case map
case 0x1E74:  return f(0x1E75); // Case map
case 0x1E76:  return f(0x1E77); // Case map
case 0x1E78:  return f(0x1E79); // Case map
case 0x1E7A:  return f(0x1E7B); // Case map
case 0x1E7C:  return f(0x1E7D); // Case map
case 0x1E7E:  return f(0x1E7F); // Case map
case 0x1E80:  return f(0x1E81); // Case map
case 0x1E82:  return f(0x1E83); // Case map
case 0x1E84:  return f(0x1E85); // Case map
case 0x1E86:  return f(0x1E87); // Case map
case 0x1E88:  return f(0x1E89); // Case map
case 0x1E8A:  return f(0x1E8B); // Case map
case 0x1E8C:  return f(0x1E8D); // Case map
case 0x1E8E:  return f(0x1E8F); // Case map
case 0x1E90:  return f(0x1E91); // Case map
case 0x1E92:  return f(0x1E93); // Case map
case 0x1E94:  return f(0x1E95); // Case map
case 0x1E96:  return f(0x0068, 0x0331); // Case map
case 0x1E97:  return f(0x0074, 0x0308); // Case map
case 0x1E98:  return f(0x0077, 0x030A); // Case map
case 0x1E99:  return f(0x0079, 0x030A); // Case map
case 0x1E9A:  return f(0x0061, 0x02BE); // Case map
case 0x1E9B:  return f(0x1E61); // Case map
case 0x1EA0:  return f(0x1EA1); // Case map
case 0x1EA2:  return f(0x1EA3); // Case map
case 0x1EA4:  return f(0x1EA5); // Case map
case 0x1EA6:  return f(0x1EA7); // Case map
case 0x1EA8:  return f(0x1EA9); // Case map
case 0x1EAA:  return f(0x1EAB); // Case map
case 0x1EAC:  return f(0x1EAD); // Case map
case 0x1EAE:  return f(0x1EAF); // Case map
case 0x1EB0:  return f(0x1EB1); // Case map
case 0x1EB2:  return f(0x1EB3); // Case map
case 0x1EB4:  return f(0x1EB5); // Case map
case 0x1EB6:  return f(0x1EB7); // Case map
case 0x1EB8:  return f(0x1EB9); // Case map
case 0x1EBA:  return f(0x1EBB); // Case map
case 0x1EBC:  return f(0x1EBD); // Case map
case 0x1EBE:  return f(0x1EBF); // Case map
case 0x1EC0:  return f(0x1EC1); // Case map
case 0x1EC2:  return f(0x1EC3); // Case map
case 0x1EC4:  return f(0x1EC5); // Case map
case 0x1EC6:  return f(0x1EC7); // Case map
case 0x1EC8:  return f(0x1EC9); // Case map
case 0x1ECA:  return f(0x1ECB); // Case map
case 0x1ECC:  return f(0x1ECD); // Case map
case 0x1ECE:  return f(0x1ECF); // Case map
case 0x1ED0:  return f(0x1ED1); // Case map
case 0x1ED2:  return f(0x1ED3); // Case map
case 0x1ED4:  return f(0x1ED5); // Case map
case 0x1ED6:  return f(0x1ED7); // Case map
case 0x1ED8:  return f(0x1ED9); // Case map
case 0x1EDA:  return f(0x1EDB); // Case map
case 0x1EDC:  return f(0x1EDD); // Case map
case 0x1EDE:  return f(0x1EDF); // Case map
case 0x1EE0:  return f(0x1EE1); // Case map
case 0x1EE2:  return f(0x1EE3); // Case map
case 0x1EE4:  return f(0x1EE5); // Case map
case 0x1EE6:  return f(0x1EE7); // Case map
case 0x1EE8:  return f(0x1EE9); // Case map
case 0x1EEA:  return f(0x1EEB); // Case map
case 0x1EEC:  return f(0x1EED); // Case map
case 0x1EEE:  return f(0x1EEF); // Case map
case 0x1EF0:  return f(0x1EF1); // Case map
case 0x1EF2:  return f(0x1EF3); // Case map
case 0x1EF4:  return f(0x1EF5); // Case map
case 0x1EF6:  return f(0x1EF7); // Case map
case 0x1EF8:  return f(0x1EF9); // Case map
case 0x1F08:  return f(0x1F00); // Case map
case 0x1F09:  return f(0x1F01); // Case map
case 0x1F0A:  return f(0x1F02); // Case map
case 0x1F0B:  return f(0x1F03); // Case map
case 0x1F0C:  return f(0x1F04); // Case map
case 0x1F0D:  return f(0x1F05); // Case map
case 0x1F0E:  return f(0x1F06); // Case map
case 0x1F0F:  return f(0x1F07); // Case map
case 0x1F18:  return f(0x1F10); // Case map
case 0x1F19:  return f(0x1F11); // Case map
case 0x1F1A:  return f(0x1F12); // Case map
case 0x1F1B:  return f(0x1F13); // Case map
case 0x1F1C:  return f(0x1F14); // Case map
case 0x1F1D:  return f(0x1F15); // Case map
case 0x1F28:  return f(0x1F20); // Case map
case 0x1F29:  return f(0x1F21); // Case map
case 0x1F2A:  return f(0x1F22); // Case map
case 0x1F2B:  return f(0x1F23); // Case map
case 0x1F2C:  return f(0x1F24); // Case map
case 0x1F2D:  return f(0x1F25); // Case map
case 0x1F2E:  return f(0x1F26); // Case map
case 0x1F2F:  return f(0x1F27); // Case map
case 0x1F38:  return f(0x1F30); // Case map
case 0x1F39:  return f(0x1F31); // Case map
case 0x1F3A:  return f(0x1F32); // Case map
case 0x1F3B:  return f(0x1F33); // Case map
case 0x1F3C:  return f(0x1F34); // Case map
case 0x1F3D:  return f(0x1F35); // Case map
case 0x1F3E:  return f(0x1F36); // Case map
case 0x1F3F:  return f(0x1F37); // Case map
case 0x1F48:  return f(0x1F40); // Case map
case 0x1F49:  return f(0x1F41); // Case map
case 0x1F4A:  return f(0x1F42); // Case map
case 0x1F4B:  return f(0x1F43); // Case map
case 0x1F4C:  return f(0x1F44); // Case map
case 0x1F4D:  return f(0x1F45); // Case map
case 0x1F50:  return f(0x03C5, 0x0313); // Case map
case 0x1F52:  return f(0x03C5, 0x0313, 0x0300); // Case map
case 0x1F54:  return f(0x03C5, 0x0313, 0x0301); // Case map
case 0x1F56:  return f(0x03C5, 0x0313, 0x0342); // Case map
case 0x1F59:  return f(0x1F51); // Case map
case 0x1F5B:  return f(0x1F53); // Case map
case 0x1F5D:  return f(0x1F55); // Case map
case 0x1F5F:  return f(0x1F57); // Case map
case 0x1F68:  return f(0x1F60); // Case map
case 0x1F69:  return f(0x1F61); // Case map
case 0x1F6A:  return f(0x1F62); // Case map
case 0x1F6B:  return f(0x1F63); // Case map
case 0x1F6C:  return f(0x1F64); // Case map
case 0x1F6D:  return f(0x1F65); // Case map
case 0x1F6E:  return f(0x1F66); // Case map
case 0x1F6F:  return f(0x1F67); // Case map
case 0x1F80:  return f(0x1F00, 0x03B9); // Case map
case 0x1F81:  return f(0x1F01, 0x03B9); // Case map
case 0x1F82:  return f(0x1F02, 0x03B9); // Case map
case 0x1F83:  return f(0x1F03, 0x03B9); // Case map
case 0x1F84:  return f(0x1F04, 0x03B9); // Case map
case 0x1F85:  return f(0x1F05, 0x03B9); // Case map
case 0x1F86:  return f(0x1F06, 0x03B9); // Case map
case 0x1F87:  return f(0x1F07, 0x03B9); // Case map
case 0x1F88:  return f(0x1F00, 0x03B9); // Case map
case 0x1F89:  return f(0x1F01, 0x03B9); // Case map
case 0x1F8A:  return f(0x1F02, 0x03B9); // Case map
case 0x1F8B:  return f(0x1F03, 0x03B9); // Case map
case 0x1F8C:  return f(0x1F04, 0x03B9); // Case map
case 0x1F8D:  return f(0x1F05, 0x03B9); // Case map
case 0x1F8E:  return f(0x1F06, 0x03B9); // Case map
case 0x1F8F:  return f(0x1F07, 0x03B9); // Case map
case 0x1F90:  return f(0x1F20, 0x03B9); // Case map
case 0x1F91:  return f(0x1F21, 0x03B9); // Case map
case 0x1F92:  return f(0x1F22, 0x03B9); // Case map
case 0x1F93:  return f(0x1F23, 0x03B9); // Case map
case 0x1F94:  return f(0x1F24, 0x03B9); // Case map
case 0x1F95:  return f(0x1F25, 0x03B9); // Case map
case 0x1F96:  return f(0x1F26, 0x03B9); // Case map
case 0x1F97:  return f(0x1F27, 0x03B9); // Case map
case 0x1F98:  return f(0x1F20, 0x03B9); // Case map
case 0x1F99:  return f(0x1F21, 0x03B9); // Case map
case 0x1F9A:  return f(0x1F22, 0x03B9); // Case map
case 0x1F9B:  return f(0x1F23, 0x03B9); // Case map
case 0x1F9C:  return f(0x1F24, 0x03B9); // Case map
case 0x1F9D:  return f(0x1F25, 0x03B9); // Case map
case 0x1F9E:  return f(0x1F26, 0x03B9); // Case map
case 0x1F9F:  return f(0x1F27, 0x03B9); // Case map
case 0x1FA0:  return f(0x1F60, 0x03B9); // Case map
case 0x1FA1:  return f(0x1F61, 0x03B9); // Case map
case 0x1FA2:  return f(0x1F62, 0x03B9); // Case map
case 0x1FA3:  return f(0x1F63, 0x03B9); // Case map
case 0x1FA4:  return f(0x1F64, 0x03B9); // Case map
case 0x1FA5:  return f(0x1F65, 0x03B9); // Case map
case 0x1FA6:  return f(0x1F66, 0x03B9); // Case map
case 0x1FA7:  return f(0x1F67, 0x03B9); // Case map
case 0x1FA8:  return f(0x1F60, 0x03B9); // Case map
case 0x1FA9:  return f(0x1F61, 0x03B9); // Case map
case 0x1FAA:  return f(0x1F62, 0x03B9); // Case map
case 0x1FAB:  return f(0x1F63, 0x03B9); // Case map
case 0x1FAC:  return f(0x1F64, 0x03B9); // Case map
case 0x1FAD:  return f(0x1F65, 0x03B9); // Case map
case 0x1FAE:  return f(0x1F66, 0x03B9); // Case map
case 0x1FAF:  return f(0x1F67, 0x03B9); // Case map
case 0x1FB2:  return f(0x1F70, 0x03B9); // Case map
case 0x1FB3:  return f(0x03B1, 0x03B9); // Case map
case 0x1FB4:  return f(0x03AC, 0x03B9); // Case map
case 0x1FB6:  return f(0x03B1, 0x0342); // Case map
case 0x1FB7:  return f(0x03B1, 0x0342, 0x03B9); // Case map
case 0x1FB8:  return f(0x1FB0); // Case map
case 0x1FB9:  return f(0x1FB1); // Case map
case 0x1FBA:  return f(0x1F70); // Case map
case 0x1FBB:  return f(0x1F71); // Case map
case 0x1FBC:  return f(0x03B1, 0x03B9); // Case map
case 0x1FBE:  return f(0x03B9); // Case map
case 0x1FC2:  return f(0x1F74, 0x03B9); // Case map
case 0x1FC3:  return f(0x03B7, 0x03B9); // Case map
case 0x1FC4:  return f(0x03AE, 0x03B9); // Case map
case 0x1FC6:  return f(0x03B7, 0x0342); // Case map
case 0x1FC7:  return f(0x03B7, 0x0342, 0x03B9); // Case map
case 0x1FC8:  return f(0x1F72); // Case map
case 0x1FC9:  return f(0x1F73); // Case map
case 0x1FCA:  return f(0x1F74); // Case map
case 0x1FCB:  return f(0x1F75); // Case map
case 0x1FCC:  return f(0x03B7, 0x03B9); // Case map
case 0x1FD2:  return f(0x03B9, 0x0308, 0x0300); // Case map
case 0x1FD3:  return f(0x03B9, 0x0308, 0x0301); // Case map
case 0x1FD6:  return f(0x03B9, 0x0342); // Case map
case 0x1FD7:  return f(0x03B9, 0x0308, 0x0342); // Case map
case 0x1FD8:  return f(0x1FD0); // Case map
case 0x1FD9:  return f(0x1FD1); // Case map
case 0x1FDA:  return f(0x1F76); // Case map
case 0x1FDB:  return f(0x1F77); // Case map
case 0x1FE2:  return f(0x03C5, 0x0308, 0x0300); // Case map
case 0x1FE3:  return f(0x03C5, 0x0308, 0x0301); // Case map
case 0x1FE4:  return f(0x03C1, 0x0313); // Case map
case 0x1FE6:  return f(0x03C5, 0x0342); // Case map
case 0x1FE7:  return f(0x03C5, 0x0308, 0x0342); // Case map
case 0x1FE8:  return f(0x1FE0); // Case map
case 0x1FE9:  return f(0x1FE1); // Case map
case 0x1FEA:  return f(0x1F7A); // Case map
case 0x1FEB:  return f(0x1F7B); // Case map
case 0x1FEC:  return f(0x1FE5); // Case map
case 0x1FF2:  return f(0x1F7C, 0x03B9); // Case map
case 0x1FF3:  return f(0x03C9, 0x03B9); // Case map
case 0x1FF4:  return f(0x03CE, 0x03B9); // Case map
case 0x1FF6:  return f(0x03C9, 0x0342); // Case map
case 0x1FF7:  return f(0x03C9, 0x0342, 0x03B9); // Case map
case 0x1FF8:  return f(0x1F78); // Case map
case 0x1FF9:  return f(0x1F79); // Case map
case 0x1FFA:  return f(0x1F7C); // Case map
case 0x1FFB:  return f(0x1F7D); // Case map
case 0x1FFC:  return f(0x03C9, 0x03B9); // Case map
case 0x20A8:  return f(0x0072, 0x0073); // Additional folding
case 0x2102:  return f(0x0063); // Additional folding
case 0x2103:  return f(0x00B0, 0x0063); // Additional folding
case 0x2107:  return f(0x025B); // Additional folding
case 0x2109:  return f(0x00B0, 0x0066); // Additional folding
case 0x210B:  return f(0x0068); // Additional folding
case 0x210C:  return f(0x0068); // Additional folding
case 0x210D:  return f(0x0068); // Additional folding
case 0x2110:  return f(0x0069); // Additional folding
case 0x2111:  return f(0x0069); // Additional folding
case 0x2112:  return f(0x006C); // Additional folding
case 0x2115:  return f(0x006E); // Additional folding
case 0x2116:  return f(0x006E, 0x006F); // Additional folding
case 0x2119:  return f(0x0070); // Additional folding
case 0x211A:  return f(0x0071); // Additional folding
case 0x211B:  return f(0x0072); // Additional folding
case 0x211C:  return f(0x0072); // Additional folding
case 0x211D:  return f(0x0072); // Additional folding
case 0x2120:  return f(0x0073, 0x06D); // Additional folding
case 0x2121:  return f(0x0074, 0x0065, 0x006C); // Additional folding
case 0x2122:  return f(0x0074, 0x006D); // Additional folding
case 0x2124:  return f(0x007A); // Additional folding
case 0x2126:  return f(0x03C9); // Case map
case 0x2128:  return f(0x007A); // Additional folding
case 0x212A:  return f(0x006B); // Case map
case 0x212B:  return f(0x00E5); // Case map
case 0x212C:  return f(0x0062); // Additional folding
case 0x212D:  return f(0x0063); // Additional folding
case 0x2130:  return f(0x0065); // Additional folding
case 0x2131:  return f(0x0066); // Additional folding
case 0x2133:  return f(0x006D); // Additional folding
case 0x213E:  return f(0x03B3); // Additional folding
case 0x213F:  return f(0x03C0); // Additional folding
case 0x2145:  return f(0x0064); // Additional folding
case 0x2160:  return f(0x2170); // Case map
case 0x2161:  return f(0x2171); // Case map
case 0x2162:  return f(0x2172); // Case map
case 0x2163:  return f(0x2173); // Case map
case 0x2164:  return f(0x2174); // Case map
case 0x2165:  return f(0x2175); // Case map
case 0x2166:  return f(0x2176); // Case map
case 0x2167:  return f(0x2177); // Case map
case 0x2168:  return f(0x2178); // Case map
case 0x2169:  return f(0x2179); // Case map
case 0x216A:  return f(0x217A); // Case map
case 0x216B:  return f(0x217B); // Case map
case 0x216C:  return f(0x217C); // Case map
case 0x216D:  return f(0x217D); // Case map
case 0x216E:  return f(0x217E); // Case map
case 0x216F:  return f(0x217F); // Case map
case 0x24B6:  return f(0x24D0); // Case map
case 0x24B7:  return f(0x24D1); // Case map
case 0x24B8:  return f(0x24D2); // Case map
case 0x24B9:  return f(0x24D3); // Case map
case 0x24BA:  return f(0x24D4); // Case map
case 0x24BB:  return f(0x24D5); // Case map
case 0x24BC:  return f(0x24D6); // Case map
case 0x24BD:  return f(0x24D7); // Case map
case 0x24BE:  return f(0x24D8); // Case map
case 0x24BF:  return f(0x24D9); // Case map
case 0x24C0:  return f(0x24DA); // Case map
case 0x24C1:  return f(0x24DB); // Case map
case 0x24C2:  return f(0x24DC); // Case map
case 0x24C3:  return f(0x24DD); // Case map
case 0x24C4:  return f(0x24DE); // Case map
case 0x24C5:  return f(0x24DF); // Case map
case 0x24C6:  return f(0x24E0); // Case map
case 0x24C7:  return f(0x24E1); // Case map
case 0x24C8:  return f(0x24E2); // Case map
case 0x24C9:  return f(0x24E3); // Case map
case 0x24CA:  return f(0x24E4); // Case map
case 0x24CB:  return f(0x24E5); // Case map
case 0x24CC:  return f(0x24E6); // Case map
case 0x24CD:  return f(0x24E7); // Case map
case 0x24CE:  return f(0x24E8); // Case map
case 0x24CF:  return f(0x24E9); // Case map
case 0x3371:  return f(0x0068, 0x0070, 0x0061); // Additional folding
case 0x3373:  return f(0x0061, 0x0075); // Additional folding
case 0x3375:  return f(0x006F, 0x0076); // Additional folding
case 0x3380:  return f(0x0070, 0x0061); // Additional folding
case 0x3381:  return f(0x006E, 0x0061); // Additional folding
case 0x3382:  return f(0x03BC, 0x0061); // Additional folding
case 0x3383:  return f(0x006D, 0x0061); // Additional folding
case 0x3384:  return f(0x006B, 0x0061); // Additional folding
case 0x3385:  return f(0x006B, 0x0062); // Additional folding
case 0x3386:  return f(0x006D, 0x0062); // Additional folding
case 0x3387:  return f(0x0067, 0x0062); // Additional folding
case 0x338A:  return f(0x0070, 0x0066); // Additional folding
case 0x338B:  return f(0x006E, 0x0066); // Additional folding
case 0x338C:  return f(0x03BC, 0x0066); // Additional folding
case 0x3390:  return f(0x0068, 0x007A); // Additional folding
case 0x3391:  return f(0x006B, 0x0068, 0x007A); // Additional folding
case 0x3392:  return f(0x006D, 0x0068, 0x007A); // Additional folding
case 0x3393:  return f(0x0067, 0x0068, 0x007A); // Additional folding
case 0x3394:  return f(0x0074, 0x0068, 0x007A); // Additional folding
case 0x33A9:  return f(0x0070, 0x0061); // Additional folding
case 0x33AA:  return f(0x006B, 0x0070, 0x0061); // Additional folding
case 0x33AB:  return f(0x006D, 0x0070, 0x0061); // Additional folding
case 0x33AC:  return f(0x0067, 0x0070, 0x0061); // Additional folding
case 0x33B4:  return f(0x0070, 0x0076); // Additional folding
case 0x33B5:  return f(0x006E, 0x0076); // Additional folding
case 0x33B6:  return f(0x03BC, 0x0076); // Additional folding
case 0x33B7:  return f(0x006D, 0x0076); // Additional folding
case 0x33B8:  return f(0x006B, 0x0076); // Additional folding
case 0x33B9:  return f(0x006D, 0x0076); // Additional folding
case 0x33BA:  return f(0x0070, 0x0077); // Additional folding
case 0x33BB:  return f(0x006E, 0x0077); // Additional folding
case 0x33BC:  return f(0x03BC, 0x0077); // Additional folding
case 0x33BD:  return f(0x006D, 0x0077); // Additional folding
case 0x33BE:  return f(0x006B, 0x0077); // Additional folding
case 0x33BF:  return f(0x006D, 0x0077); // Additional folding
case 0x33C0:  return f(0x006B, 0x03C9); // Additional folding
case 0x33C1:  return f(0x006D, 0x03C9); // Additional folding
case 0x33C3:  return f(0x0062, 0x0071); // Additional folding
case 0x33C6:  return f(0x0063, 0x2215, 0x006B, 0x0067); // Additional folding
case 0x33C7:  return f(0x0063, 0x006F, 0x002E); // Additional folding
case 0x33C8:  return f(0x0064, 0x0062); // Additional folding
case 0x33C9:  return f(0x0067, 0x0079); // Additional folding
case 0x33CB:  return f(0x0068, 0x0070); // Additional folding
case 0x33CD:  return f(0x006B, 0x006B); // Additional folding
case 0x33CE:  return f(0x006B, 0x006D); // Additional folding
case 0x33D7:  return f(0x0070, 0x0068); // Additional folding
case 0x33D9:  return f(0x0070, 0x0070, 0x006D); // Additional folding
case 0x33DA:  return f(0x0070, 0x0072); // Additional folding
case 0x33DC:  return f(0x0073, 0x0076); // Additional folding
case 0x33DD:  return f(0x0077, 0x0062); // Additional folding
case 0xFB00:  return f(0x0066, 0x0066); // Case map
case 0xFB01:  return f(0x0066, 0x0069); // Case map
case 0xFB02:  return f(0x0066, 0x006C); // Case map
case 0xFB03:  return f(0x0066, 0x0066, 0x0069); // Case map
case 0xFB04:  return f(0x0066, 0x0066, 0x006C); // Case map
case 0xFB05:  return f(0x0073, 0x0074); // Case map
case 0xFB06:  return f(0x0073, 0x0074); // Case map
case 0xFB13:  return f(0x0574, 0x0576); // Case map
case 0xFB14:  return f(0x0574, 0x0565); // Case map
case 0xFB15:  return f(0x0574, 0x056B); // Case map
case 0xFB16:  return f(0x057E, 0x0576); // Case map
case 0xFB17:  return f(0x0574, 0x056D); // Case map
case 0xFF21:  return f(0xFF41); // Case map
case 0xFF22:  return f(0xFF42); // Case map
case 0xFF23:  return f(0xFF43); // Case map
case 0xFF24:  return f(0xFF44); // Case map
case 0xFF25:  return f(0xFF45); // Case map
case 0xFF26:  return f(0xFF46); // Case map
case 0xFF27:  return f(0xFF47); // Case map
case 0xFF28:  return f(0xFF48); // Case map
case 0xFF29:  return f(0xFF49); // Case map
case 0xFF2A:  return f(0xFF4A); // Case map
case 0xFF2B:  return f(0xFF4B); // Case map
case 0xFF2C:  return f(0xFF4C); // Case map
case 0xFF2D:  return f(0xFF4D); // Case map
case 0xFF2E:  return f(0xFF4E); // Case map
case 0xFF2F:  return f(0xFF4F); // Case map
case 0xFF30:  return f(0xFF50); // Case map
case 0xFF31:  return f(0xFF51); // Case map
case 0xFF32:  return f(0xFF52); // Case map
case 0xFF33:  return f(0xFF53); // Case map
case 0xFF34:  return f(0xFF54); // Case map
case 0xFF35:  return f(0xFF55); // Case map
case 0xFF36:  return f(0xFF56); // Case map
case 0xFF37:  return f(0xFF57); // Case map
case 0xFF38:  return f(0xFF58); // Case map
case 0xFF39:  return f(0xFF59); // Case map
case 0xFF3A:  return f(0xFF5A); // Case map
case 0x10400: return f(0x10428); // Case map
case 0x10401: return f(0x10429); // Case map
case 0x10402: return f(0x1042A); // Case map
case 0x10403: return f(0x1042B); // Case map
case 0x10404: return f(0x1042C); // Case map
case 0x10405: return f(0x1042D); // Case map
case 0x10406: return f(0x1042E); // Case map
case 0x10407: return f(0x1042F); // Case map
case 0x10408: return f(0x10430); // Case map
case 0x10409: return f(0x10431); // Case map
case 0x1040A: return f(0x10432); // Case map
case 0x1040B: return f(0x10433); // Case map
case 0x1040C: return f(0x10434); // Case map
case 0x1040D: return f(0x10435); // Case map
case 0x1040E: return f(0x10436); // Case map
case 0x1040F: return f(0x10437); // Case map
case 0x10410: return f(0x10438); // Case map
case 0x10411: return f(0x10439); // Case map
case 0x10412: return f(0x1043A); // Case map
case 0x10413: return f(0x1043B); // Case map
case 0x10414: return f(0x1043C); // Case map
case 0x10415: return f(0x1043D); // Case map
case 0x10416: return f(0x1043E); // Case map
case 0x10417: return f(0x1043F); // Case map
case 0x10418: return f(0x10440); // Case map
case 0x10419: return f(0x10441); // Case map
case 0x1041A: return f(0x10442); // Case map
case 0x1041B: return f(0x10443); // Case map
case 0x1041C: return f(0x10444); // Case map
case 0x1041D: return f(0x10445); // Case map
case 0x1041E: return f(0x10446); // Case map
case 0x1041F: return f(0x10447); // Case map
case 0x10420: return f(0x10448); // Case map
case 0x10421: return f(0x10449); // Case map
case 0x10422: return f(0x1044A); // Case map
case 0x10423: return f(0x1044B); // Case map
case 0x10424: return f(0x1044C); // Case map
case 0x10425: return f(0x1044D); // Case map
case 0x1D400: return f(0x0061); // Additional folding
case 0x1D401: return f(0x0062); // Additional folding
case 0x1D402: return f(0x0063); // Additional folding
case 0x1D403: return f(0x0064); // Additional folding
case 0x1D404: return f(0x0065); // Additional folding
case 0x1D405: return f(0x0066); // Additional folding
case 0x1D406: return f(0x0067); // Additional folding
case 0x1D407: return f(0x0068); // Additional folding
case 0x1D408: return f(0x0069); // Additional folding
case 0x1D409: return f(0x006A); // Additional folding
case 0x1D40A: return f(0x006B); // Additional folding
case 0x1D40B: return f(0x006C); // Additional folding
case 0x1D40C: return f(0x006D); // Additional folding
case 0x1D40D: return f(0x006E); // Additional folding
case 0x1D40E: return f(0x006F); // Additional folding
case 0x1D40F: return f(0x0070); // Additional folding
case 0x1D410: return f(0x0071); // Additional folding
case 0x1D411: return f(0x0072); // Additional folding
case 0x1D412: return f(0x0073); // Additional folding
case 0x1D413: return f(0x0074); // Additional folding
case 0x1D414: return f(0x0075); // Additional folding
case 0x1D415: return f(0x0076); // Additional folding
case 0x1D416: return f(0x0077); // Additional folding
case 0x1D417: return f(0x0078); // Additional folding
case 0x1D418: return f(0x0079); // Additional folding
case 0x1D419: return f(0x007A); // Additional folding
case 0x1D434: return f(0x0061); // Additional folding
case 0x1D435: return f(0x0062); // Additional folding
case 0x1D436: return f(0x0063); // Additional folding
case 0x1D437: return f(0x0064); // Additional folding
case 0x1D438: return f(0x0065); // Additional folding
case 0x1D439: return f(0x0066); // Additional folding
case 0x1D43A: return f(0x0067); // Additional folding
case 0x1D43B: return f(0x0068); // Additional folding
case 0x1D43C: return f(0x0069); // Additional folding
case 0x1D43D: return f(0x006A); // Additional folding
case 0x1D43E: return f(0x006B); // Additional folding
case 0x1D43F: return f(0x006C); // Additional folding
case 0x1D440: return f(0x006D); // Additional folding
case 0x1D441: return f(0x006E); // Additional folding
case 0x1D442: return f(0x006F); // Additional folding
case 0x1D443: return f(0x0070); // Additional folding
case 0x1D444: return f(0x0071); // Additional folding
case 0x1D445: return f(0x0072); // Additional folding
case 0x1D446: return f(0x0073); // Additional folding
case 0x1D447: return f(0x0074); // Additional folding
case 0x1D448: return f(0x0075); // Additional folding
case 0x1D449: return f(0x0076); // Additional folding
case 0x1D44A: return f(0x0077); // Additional folding
case 0x1D44B: return f(0x0078); // Additional folding
case 0x1D44C: return f(0x0079); // Additional folding
case 0x1D44D: return f(0x007A); // Additional folding
case 0x1D468: return f(0x0061); // Additional folding
case 0x1D469: return f(0x0062); // Additional folding
case 0x1D46A: return f(0x0063); // Additional folding
case 0x1D46B: return f(0x0064); // Additional folding
case 0x1D46C: return f(0x0065); // Additional folding
case 0x1D46D: return f(0x0066); // Additional folding
case 0x1D46E: return f(0x0067); // Additional folding
case 0x1D46F: return f(0x0068); // Additional folding
case 0x1D470: return f(0x0069); // Additional folding
case 0x1D471: return f(0x006A); // Additional folding
case 0x1D472: return f(0x006B); // Additional folding
case 0x1D473: return f(0x006C); // Additional folding
case 0x1D474: return f(0x006D); // Additional folding
case 0x1D475: return f(0x006E); // Additional folding
case 0x1D476: return f(0x006F); // Additional folding
case 0x1D477: return f(0x0070); // Additional folding
case 0x1D478: return f(0x0071); // Additional folding
case 0x1D479: return f(0x0072); // Additional folding
case 0x1D47A: return f(0x0073); // Additional folding
case 0x1D47B: return f(0x0074); // Additional folding
case 0x1D47C: return f(0x0075); // Additional folding
case 0x1D47D: return f(0x0076); // Additional folding
case 0x1D47E: return f(0x0077); // Additional folding
case 0x1D47F: return f(0x0078); // Additional folding
case 0x1D480: return f(0x0079); // Additional folding
case 0x1D481: return f(0x007A); // Additional folding
case 0x1D49C: return f(0x0061); // Additional folding
case 0x1D49E: return f(0x0063); // Additional folding
case 0x1D49F: return f(0x0064); // Additional folding
case 0x1D4A2: return f(0x0067); // Additional folding
case 0x1D4A5: return f(0x006A); // Additional folding
case 0x1D4A6: return f(0x006B); // Additional folding
case 0x1D4A9: return f(0x006E); // Additional folding
case 0x1D4AA: return f(0x006F); // Additional folding
case 0x1D4AB: return f(0x0070); // Additional folding
case 0x1D4AC: return f(0x0071); // Additional folding
case 0x1D4AE: return f(0x0073); // Additional folding
case 0x1D4AF: return f(0x0074); // Additional folding
case 0x1D4B0: return f(0x0075); // Additional folding
case 0x1D4B1: return f(0x0076); // Additional folding
case 0x1D4B2: return f(0x0077); // Additional folding
case 0x1D4B3: return f(0x0078); // Additional folding
case 0x1D4B4: return f(0x0079); // Additional folding
case 0x1D4B5: return f(0x007A); // Additional folding
case 0x1D4D0: return f(0x0061); // Additional folding
case 0x1D4D1: return f(0x0062); // Additional folding
case 0x1D4D2: return f(0x0063); // Additional folding
case 0x1D4D3: return f(0x0064); // Additional folding
case 0x1D4D4: return f(0x0065); // Additional folding
case 0x1D4D5: return f(0x0066); // Additional folding
case 0x1D4D6: return f(0x0067); // Additional folding
case 0x1D4D7: return f(0x0068); // Additional folding
case 0x1D4D8: return f(0x0069); // Additional folding
case 0x1D4D9: return f(0x006A); // Additional folding
case 0x1D4DA: return f(0x006B); // Additional folding
case 0x1D4DB: return f(0x006C); // Additional folding
case 0x1D4DC: return f(0x006D); // Additional folding
case 0x1D4DD: return f(0x006E); // Additional folding
case 0x1D4DE: return f(0x006F); // Additional folding
case 0x1D4DF: return f(0x0070); // Additional folding
case 0x1D4E0: return f(0x0071); // Additional folding
case 0x1D4E1: return f(0x0072); // Additional folding
case 0x1D4E2: return f(0x0073); // Additional folding
case 0x1D4E3: return f(0x0074); // Additional folding
case 0x1D4E4: return f(0x0075); // Additional folding
case 0x1D4E5: return f(0x0076); // Additional folding
case 0x1D4E6: return f(0x0077); // Additional folding
case 0x1D4E7: return f(0x0078); // Additional folding
case 0x1D4E8: return f(0x0079); // Additional folding
case 0x1D4E9: return f(0x007A); // Additional folding
case 0x1D504: return f(0x0061); // Additional folding
case 0x1D505: return f(0x0062); // Additional folding
case 0x1D507: return f(0x0064); // Additional folding
case 0x1D508: return f(0x0065); // Additional folding
case 0x1D509: return f(0x0066); // Additional folding
case 0x1D50A: return f(0x0067); // Additional folding
case 0x1D50D: return f(0x006A); // Additional folding
case 0x1D50E: return f(0x006B); // Additional folding
case 0x1D50F: return f(0x006C); // Additional folding
case 0x1D510: return f(0x006D); // Additional folding
case 0x1D511: return f(0x006E); // Additional folding
case 0x1D512: return f(0x006F); // Additional folding
case 0x1D513: return f(0x0070); // Additional folding
case 0x1D514: return f(0x0071); // Additional folding
case 0x1D516: return f(0x0073); // Additional folding
case 0x1D517: return f(0x0074); // Additional folding
case 0x1D518: return f(0x0075); // Additional folding
case 0x1D519: return f(0x0076); // Additional folding
case 0x1D51A: return f(0x0077); // Additional folding
case 0x1D51B: return f(0x0078); // Additional folding
case 0x1D51C: return f(0x0079); // Additional folding
case 0x1D538: return f(0x0061); // Additional folding
case 0x1D539: return f(0x0062); // Additional folding
case 0x1D53B: return f(0x0064); // Additional folding
case 0x1D53C: return f(0x0065); // Additional folding
case 0x1D53D: return f(0x0066); // Additional folding
case 0x1D53E: return f(0x0067); // Additional folding
case 0x1D540: return f(0x0069); // Additional folding
case 0x1D541: return f(0x006A); // Additional folding
case 0x1D542: return f(0x006B); // Additional folding
case 0x1D543: return f(0x006C); // Additional folding
case 0x1D544: return f(0x006D); // Additional folding
case 0x1D546: return f(0x006F); // Additional folding
case 0x1D54A: return f(0x0073); // Additional folding
case 0x1D54B: return f(0x0074); // Additional folding
case 0x1D54C: return f(0x0075); // Additional folding
case 0x1D54D: return f(0x0076); // Additional folding
case 0x1D54E: return f(0x0077); // Additional folding
case 0x1D54F: return f(0x0078); // Additional folding
case 0x1D550: return f(0x0079); // Additional folding
case 0x1D56C: return f(0x0061); // Additional folding
case 0x1D56D: return f(0x0062); // Additional folding
case 0x1D56E: return f(0x0063); // Additional folding
case 0x1D56F: return f(0x0064); // Additional folding
case 0x1D570: return f(0x0065); // Additional folding
case 0x1D571: return f(0x0066); // Additional folding
case 0x1D572: return f(0x0067); // Additional folding
case 0x1D573: return f(0x0068); // Additional folding
case 0x1D574: return f(0x0069); // Additional folding
case 0x1D575: return f(0x006A); // Additional folding
case 0x1D576: return f(0x006B); // Additional folding
case 0x1D577: return f(0x006C); // Additional folding
case 0x1D578: return f(0x006D); // Additional folding
case 0x1D579: return f(0x006E); // Additional folding
case 0x1D57A: return f(0x006F); // Additional folding
case 0x1D57B: return f(0x0070); // Additional folding
case 0x1D57C: return f(0x0071); // Additional folding
case 0x1D57D: return f(0x0072); // Additional folding
case 0x1D57E: return f(0x0073); // Additional folding
case 0x1D57F: return f(0x0074); // Additional folding
case 0x1D580: return f(0x0075); // Additional folding
case 0x1D581: return f(0x0076); // Additional folding
case 0x1D582: return f(0x0077); // Additional folding
case 0x1D583: return f(0x0078); // Additional folding
case 0x1D584: return f(0x0079); // Additional folding
case 0x1D585: return f(0x007A); // Additional folding
case 0x1D5A0: return f(0x0061); // Additional folding
case 0x1D5A1: return f(0x0062); // Additional folding
case 0x1D5A2: return f(0x0063); // Additional folding
case 0x1D5A3: return f(0x0064); // Additional folding
case 0x1D5A4: return f(0x0065); // Additional folding
case 0x1D5A5: return f(0x0066); // Additional folding
case 0x1D5A6: return f(0x0067); // Additional folding
case 0x1D5A7: return f(0x0068); // Additional folding
case 0x1D5A8: return f(0x0069); // Additional folding
case 0x1D5A9: return f(0x006A); // Additional folding
case 0x1D5AA: return f(0x006B); // Additional folding
case 0x1D5AB: return f(0x006C); // Additional folding
case 0x1D5AC: return f(0x006D); // Additional folding
case 0x1D5AD: return f(0x006E); // Additional folding
case 0x1D5AE: return f(0x006F); // Additional folding
case 0x1D5AF: return f(0x0070); // Additional folding
case 0x1D5B0: return f(0x0071); // Additional folding
case 0x1D5B1: return f(0x0072); // Additional folding
case 0x1D5B2: return f(0x0073); // Additional folding
case 0x1D5B3: return f(0x0074); // Additional folding
case 0x1D5B4: return f(0x0075); // Additional folding
case 0x1D5B5: return f(0x0076); // Additional folding
case 0x1D5B6: return f(0x0077); // Additional folding
case 0x1D5B7: return f(0x0078); // Additional folding
case 0x1D5B8: return f(0x0079); // Additional folding
case 0x1D5B9: return f(0x007A); // Additional folding
case 0x1D5D4: return f(0x0061); // Additional folding
case 0x1D5D5: return f(0x0062); // Additional folding
case 0x1D5D6: return f(0x0063); // Additional folding
case 0x1D5D7: return f(0x0064); // Additional folding
case 0x1D5D8: return f(0x0065); // Additional folding
case 0x1D5D9: return f(0x0066); // Additional folding
case 0x1D5DA: return f(0x0067); // Additional folding
case 0x1D5DB: return f(0x0068); // Additional folding
case 0x1D5DC: return f(0x0069); // Additional folding
case 0x1D5DD: return f(0x006A); // Additional folding
case 0x1D5DE: return f(0x006B); // Additional folding
case 0x1D5DF: return f(0x006C); // Additional folding
case 0x1D5E0: return f(0x006D); // Additional folding
case 0x1D5E1: return f(0x006E); // Additional folding
case 0x1D5E2: return f(0x006F); // Additional folding
case 0x1D5E3: return f(0x0070); // Additional folding
case 0x1D5E4: return f(0x0071); // Additional folding
case 0x1D5E5: return f(0x0072); // Additional folding
case 0x1D5E6: return f(0x0073); // Additional folding
case 0x1D5E7: return f(0x0074); // Additional folding
case 0x1D5E8: return f(0x0075); // Additional folding
case 0x1D5E9: return f(0x0076); // Additional folding
case 0x1D5EA: return f(0x0077); // Additional folding
case 0x1D5EB: return f(0x0078); // Additional folding
case 0x1D5EC: return f(0x0079); // Additional folding
case 0x1D5ED: return f(0x007A); // Additional folding
case 0x1D608: return f(0x0061); // Additional folding
case 0x1D609: return f(0x0062); // Additional folding
case 0x1D60A: return f(0x0063); // Additional folding
case 0x1D60B: return f(0x0064); // Additional folding
case 0x1D60C: return f(0x0065); // Additional folding
case 0x1D60D: return f(0x0066); // Additional folding
case 0x1D60E: return f(0x0067); // Additional folding
case 0x1D60F: return f(0x0068); // Additional folding
case 0x1D610: return f(0x0069); // Additional folding
case 0x1D611: return f(0x006A); // Additional folding
case 0x1D612: return f(0x006B); // Additional folding
case 0x1D613: return f(0x006C); // Additional folding
case 0x1D614: return f(0x006D); // Additional folding
case 0x1D615: return f(0x006E); // Additional folding
case 0x1D616: return f(0x006F); // Additional folding
case 0x1D617: return f(0x0070); // Additional folding
case 0x1D618: return f(0x0071); // Additional folding
case 0x1D619: return f(0x0072); // Additional folding
case 0x1D61A: return f(0x0073); // Additional folding
case 0x1D61B: return f(0x0074); // Additional folding
case 0x1D61C: return f(0x0075); // Additional folding
case 0x1D61D: return f(0x0076); // Additional folding
case 0x1D61E: return f(0x0077); // Additional folding
case 0x1D61F: return f(0x0078); // Additional folding
case 0x1D620: return f(0x0079); // Additional folding
case 0x1D621: return f(0x007A); // Additional folding
case 0x1D63C: return f(0x0061); // Additional folding
case 0x1D63D: return f(0x0062); // Additional folding
case 0x1D63E: return f(0x0063); // Additional folding
case 0x1D63F: return f(0x0064); // Additional folding
case 0x1D640: return f(0x0065); // Additional folding
case 0x1D641: return f(0x0066); // Additional folding
case 0x1D642: return f(0x0067); // Additional folding
case 0x1D643: return f(0x0068); // Additional folding
case 0x1D644: return f(0x0069); // Additional folding
case 0x1D645: return f(0x006A); // Additional folding
case 0x1D646: return f(0x006B); // Additional folding
case 0x1D647: return f(0x006C); // Additional folding
case 0x1D648: return f(0x006D); // Additional folding
case 0x1D649: return f(0x006E); // Additional folding
case 0x1D64A: return f(0x006F); // Additional folding
case 0x1D64B: return f(0x0070); // Additional folding
case 0x1D64C: return f(0x0071); // Additional folding
case 0x1D64D: return f(0x0072); // Additional folding
case 0x1D64E: return f(0x0073); // Additional folding
case 0x1D64F: return f(0x0074); // Additional folding
case 0x1D650: return f(0x0075); // Additional folding
case 0x1D651: return f(0x0076); // Additional folding
case 0x1D652: return f(0x0077); // Additional folding
case 0x1D653: return f(0x0078); // Additional folding
case 0x1D654: return f(0x0079); // Additional folding
case 0x1D655: return f(0x007A); // Additional folding
case 0x1D670: return f(0x0061); // Additional folding
case 0x1D671: return f(0x0062); // Additional folding
case 0x1D672: return f(0x0063); // Additional folding
case 0x1D673: return f(0x0064); // Additional folding
case 0x1D674: return f(0x0065); // Additional folding
case 0x1D675: return f(0x0066); // Additional folding
case 0x1D676: return f(0x0067); // Additional folding
case 0x1D677: return f(0x0068); // Additional folding
case 0x1D678: return f(0x0069); // Additional folding
case 0x1D679: return f(0x006A); // Additional folding
case 0x1D67A: return f(0x006B); // Additional folding
case 0x1D67B: return f(0x006C); // Additional folding
case 0x1D67C: return f(0x006D); // Additional folding
case 0x1D67D: return f(0x006E); // Additional folding
case 0x1D67E: return f(0x006F); // Additional folding
case 0x1D67F: return f(0x0070); // Additional folding
case 0x1D680: return f(0x0071); // Additional folding
case 0x1D681: return f(0x0072); // Additional folding
case 0x1D682: return f(0x0073); // Additional folding
case 0x1D683: return f(0x0074); // Additional folding
case 0x1D684: return f(0x0075); // Additional folding
case 0x1D685: return f(0x0076); // Additional folding
case 0x1D686: return f(0x0077); // Additional folding
case 0x1D687: return f(0x0078); // Additional folding
case 0x1D688: return f(0x0079); // Additional folding
case 0x1D689: return f(0x007A); // Additional folding
case 0x1D6A8: return f(0x03B1); // Additional folding
case 0x1D6A9: return f(0x03B2); // Additional folding
case 0x1D6AA: return f(0x03B3); // Additional folding
case 0x1D6AB: return f(0x03B4); // Additional folding
case 0x1D6AC: return f(0x03B5); // Additional folding
case 0x1D6AD: return f(0x03B6); // Additional folding
case 0x1D6AE: return f(0x03B7); // Additional folding
case 0x1D6AF: return f(0x03B8); // Additional folding
case 0x1D6B0: return f(0x03B9); // Additional folding
case 0x1D6B1: return f(0x03BA); // Additional folding
case 0x1D6B2: return f(0x03BB); // Additional folding
case 0x1D6B3: return f(0x03BC); // Additional folding
case 0x1D6B4: return f(0x03BD); // Additional folding
case 0x1D6B5: return f(0x03BE); // Additional folding
case 0x1D6B6: return f(0x03BF); // Additional folding
case 0x1D6B7: return f(0x03C0); // Additional folding
case 0x1D6B8: return f(0x03C1); // Additional folding
case 0x1D6B9: return f(0x03B8); // Additional folding
case 0x1D6BA: return f(0x03C3); // Additional folding
case 0x1D6BB: return f(0x03C4); // Additional folding
case 0x1D6BC: return f(0x03C5); // Additional folding
case 0x1D6BD: return f(0x03C6); // Additional folding
case 0x1D6BE: return f(0x03C7); // Additional folding
case 0x1D6BF: return f(0x03C8); // Additional folding
case 0x1D6C0: return f(0x03C9); // Additional folding
case 0x1D6D3: return f(0x03C3); // Additional folding
case 0x1D6E2: return f(0x03B1); // Additional folding
case 0x1D6E3: return f(0x03B2); // Additional folding
case 0x1D6E4: return f(0x03B3); // Additional folding
case 0x1D6E5: return f(0x03B4); // Additional folding
case 0x1D6E6: return f(0x03B5); // Additional folding
case 0x1D6E7: return f(0x03B6); // Additional folding
case 0x1D6E8: return f(0x03B7); // Additional folding
case 0x1D6E9: return f(0x03B8); // Additional folding
case 0x1D6EA: return f(0x03B9); // Additional folding
case 0x1D6EB: return f(0x03BA); // Additional folding
case 0x1D6EC: return f(0x03BB); // Additional folding
case 0x1D6ED: return f(0x03BC); // Additional folding
case 0x1D6EE: return f(0x03BD); // Additional folding
case 0x1D6EF: return f(0x03BE); // Additional folding
case 0x1D6F0: return f(0x03BF); // Additional folding
case 0x1D6F1: return f(0x03C0); // Additional folding
case 0x1D6F2: return f(0x03C1); // Additional folding
case 0x1D6F3: return f(0x03B8); // Additional folding
case 0x1D6F4: return f(0x03C3); // Additional folding
case 0x1D6F5: return f(0x03C4); // Additional folding
case 0x1D6F6: return f(0x03C5); // Additional folding
case 0x1D6F7: return f(0x03C6); // Additional folding
case 0x1D6F8: return f(0x03C7); // Additional folding
case 0x1D6F9: return f(0x03C8); // Additional folding
case 0x1D6FA: return f(0x03C9); // Additional folding
case 0x1D70D: return f(0x03C3); // Additional folding
case 0x1D71C: return f(0x03B1); // Additional folding
case 0x1D71D: return f(0x03B2); // Additional folding
case 0x1D71E: return f(0x03B3); // Additional folding
case 0x1D71F: return f(0x03B4); // Additional folding
case 0x1D720: return f(0x03B5); // Additional folding
case 0x1D721: return f(0x03B6); // Additional folding
case 0x1D722: return f(0x03B7); // Additional folding
case 0x1D723: return f(0x03B8); // Additional folding
case 0x1D724: return f(0x03B9); // Additional folding
case 0x1D725: return f(0x03BA); // Additional folding
case 0x1D726: return f(0x03BB); // Additional folding
case 0x1D727: return f(0x03BC); // Additional folding
case 0x1D728: return f(0x03BD); // Additional folding
case 0x1D729: return f(0x03BE); // Additional folding
case 0x1D72A: return f(0x03BF); // Additional folding
case 0x1D72B: return f(0x03C0); // Additional folding
case 0x1D72C: return f(0x03C1); // Additional folding
case 0x1D72D: return f(0x03B8); // Additional folding
case 0x1D72E: return f(0x03C3); // Additional folding
case 0x1D72F: return f(0x03C4); // Additional folding
case 0x1D730: return f(0x03C5); // Additional folding
case 0x1D731: return f(0x03C6); // Additional folding
case 0x1D732: return f(0x03C7); // Additional folding
case 0x1D733: return f(0x03C8); // Additional folding
case 0x1D734: return f(0x03C9); // Additional folding
case 0x1D747: return f(0x03C3); // Additional folding
case 0x1D756: return f(0x03B1); // Additional folding
case 0x1D757: return f(0x03B2); // Additional folding
case 0x1D758: return f(0x03B3); // Additional folding
case 0x1D759: return f(0x03B4); // Additional folding
case 0x1D75A: return f(0x03B5); // Additional folding
case 0x1D75B: return f(0x03B6); // Additional folding
case 0x1D75C: return f(0x03B7); // Additional folding
case 0x1D75D: return f(0x03B8); // Additional folding
case 0x1D75E: return f(0x03B9); // Additional folding
case 0x1D75F: return f(0x03BA); // Additional folding
case 0x1D760: return f(0x03BB); // Additional folding
case 0x1D761: return f(0x03BC); // Additional folding
case 0x1D762: return f(0x03BD); // Additional folding
case 0x1D763: return f(0x03BE); // Additional folding
case 0x1D764: return f(0x03BF); // Additional folding
case 0x1D765: return f(0x03C0); // Additional folding
case 0x1D766: return f(0x03C1); // Additional folding
case 0x1D767: return f(0x03B8); // Additional folding
case 0x1D768: return f(0x03C3); // Additional folding
case 0x1D769: return f(0x03C4); // Additional folding
case 0x1D76A: return f(0x03C5); // Additional folding
case 0x1D76B: return f(0x03C6); // Additional folding
case 0x1D76C: return f(0x03C7); // Additional folding
case 0x1D76D: return f(0x03C8); // Additional folding
case 0x1D76E: return f(0x03C9); // Additional folding
case 0x1D781: return f(0x03C3); // Additional folding
case 0x1D790: return f(0x03B1); // Additional folding
case 0x1D791: return f(0x03B2); // Additional folding
case 0x1D792: return f(0x03B3); // Additional folding
case 0x1D793: return f(0x03B4); // Additional folding
case 0x1D794: return f(0x03B5); // Additional folding
case 0x1D795: return f(0x03B6); // Additional folding
case 0x1D796: return f(0x03B7); // Additional folding
case 0x1D797: return f(0x03B8); // Additional folding
case 0x1D798: return f(0x03B9); // Additional folding
case 0x1D799: return f(0x03BA); // Additional folding
case 0x1D79A: return f(0x03BB); // Additional folding
case 0x1D79B: return f(0x03BC); // Additional folding
case 0x1D79C: return f(0x03BD); // Additional folding
case 0x1D79D: return f(0x03BE); // Additional folding
case 0x1D79E: return f(0x03BF); // Additional folding
case 0x1D79F: return f(0x03C0); // Additional folding
case 0x1D7A0: return f(0x03C1); // Additional folding
case 0x1D7A1: return f(0x03B8); // Additional folding
case 0x1D7A2: return f(0x03C3); // Additional folding
case 0x1D7A3: return f(0x03C4); // Additional folding
case 0x1D7A4: return f(0x03C5); // Additional folding
case 0x1D7A5: return f(0x03C6); // Additional folding
case 0x1D7A6: return f(0x03C7); // Additional folding
case 0x1D7A7: return f(0x03C8); // Additional folding
case 0x1D7A8: return f(0x03C9); // Additional folding
case 0x1D7BB: return f(0x03C3); // Additional folding
    default:
        f(cp);
        break;
    }
}

//------------------------------------------------

/** Iterate over a utf32 sequence, applying nameprep.
*/
template<class InputIt>
class nameprep_iterator
    : private nameprep_iterator_base
{
    // VFALCO typecheck on InputIt

    InputIt it_;
    InputIt end_; // VFALCO only needed for debug?

public:
    using value_type        = char32_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type const*;
    using reference         = value_type const&;
    using iterator_category =
        std::forward_iterator_tag;

    nameprep_iterator(nameprep_iterator const&) = default;
    nameprep_iterator& operator=(nameprep_iterator const&) = default;

    nameprep_iterator() = default;

    nameprep_iterator(
        InputIt it,
        InputIt end)
        : it_(it)
        , end_(end)
    {
        get();
    }

    explicit
    nameprep_iterator(
        InputIt end)
        : it_(end)
        , end_(end)
    {
        cp_[0] = invalid;
        i_ = 0;
    }

    bool
    operator==(
        nameprep_iterator const& other) const noexcept
    {
        BOOST_ASSERT(end_ == other.end_);
        return
            it_ == other.it_ &&
            i_ == other.i_;
    }

    bool
    operator!=(
        nameprep_iterator const& other) const noexcept
    {
        return ! (*this == other);
    }

    char32_t
    operator*() const noexcept
    {
        BOOST_ASSERT(i_ > 0 || cp_[i_] != invalid);
        return cp_[i_];
    }

    nameprep_iterator&
    operator++()
    {
        BOOST_ASSERT(i_ > 0 || cp_[i_] != invalid);
        if(i_ > 0)
        {
            --i_;
            return *this;
        }
        ++it_;
        get();
        return *this;
    }

    nameprep_iterator
    operator++(int)
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

private:
    void
    get()
    {
        while(it_ != end_)
        {
            auto cp = *it_;
            if(map_to_nothing(cp))
            {
                ++it_;
                continue;
            }
            return map(cp);
        }
        cp_[0] = invalid;
        i_ = 0;
    }
};

//------------------------------------------------

static
bool
is_ascii(char32_t cp)
{
    return cp < 128;
}

/** Write a utf32 IDNA from a utf32 IRI
*/
template<
    class OutputIt,
    class InputIt>
OutputIt
encode_idna(
    OutputIt out,
    InputIt first0,
    InputIt last0)
{
    if(first0 == last0)
        return out;
    nameprep_iterator<InputIt> first(first0, last0);
    nameprep_iterator<InputIt> last(last0);
    auto it = first;
    for(;;)
    {
        auto cp = *it;
        if(cp == '.')
        {
            // copy ascii
            out = std::copy(first, it, out);
            *out++ = '.';
            ++it;
            if(it == last)
                return out;
            first = it;
            continue;
        }
        if(is_ascii(cp))
        {
            ++it;
            if(it == last)
            {
                // copy ascii
                out = std::copy(first, it, out);
                return out;
            }
            continue;
        }
        for(;;)
        {
            ++it;
            if(it == last)
            {
                // convert puny
                *out++ = 'x';
                *out++ = 'n';
                *out++ = '-';
                *out++ = '-';
                out = encode(out, first, it);
                return out;
            }
            cp = *it;
            if(cp == '.')
            {
                // convert puny
                *out++ = 'x';
                *out++ = 'n';
                *out++ = '-';
                *out++ = '-';
                out = encode(out, first, it);
                *out++ = '.';
                ++it;
                if(it == last)
                    return out;
                first = it;
                break;
            }
        }
    }
}

system::result<std::string>
utf8_to_idna(
    core::string_view s,
    std::string&& storage)
{
    std::size_t n;
    utf8_input const first(
        s.data(), s.data() + s.size());
    utf8_input const last(
        s.data() + s.size());
    encode_idna(utf8_count(n), first, last );
    storage.resize(n);
    encode_idna(
        utf8_output(&storage[0]), first, last);
    return std::move(storage);
}

} // url
} // boost

