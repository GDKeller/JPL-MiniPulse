#include <XmlTestData.h>
#include <FS.h>
#include <LittleFS.h>
#include <DevUtils.h>
#include <FileUtils.h>

static const char XML_DATA_DIR[] = "/xml_data";

// Combined fallback snapshot — all approved spacecraft from minipulse_list (Feb 2026)
// Sources: DSN-Chronicle XML archive, dsn_timeseries DB, mission profile data
static const char data_fallback[] PROGMEM = R"==--==(<dsn>
<station name="gdscc" friendlyName="Goldstone" timeUTC="1772096403000" timeZoneOffset="-28800000"/>
<dish name="DSS14" azimuthAngle="108" elevationAngle="38" windSpeed="5" isMSPA="true" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="160" frequency="0" band="X" power="-160" spacecraft="VGR1" spacecraftID="-31"/>
<downSignal active="true" signalType="data" dataRate="1000" frequency="0" band="X" power="-160" spacecraft="NHPC" spacecraftID="-98"/>
<target name="VGR1" id="31" uplegRange="25400000000" downlegRange="25400000000" rtlt="170000"/>
<target name="NHPC" id="98" uplegRange="8200000000" downlegRange="8200000000" rtlt="54700"/>
</dish>
<dish name="DSS24" azimuthAngle="256" elevationAngle="53" windSpeed="5" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="4.9" spacecraft="JWST" spacecraftID="-170"/>
<downSignal active="true" signalType="data" dataRate="28000000" frequency="0" band="K" power="-90" spacecraft="JWST" spacecraftID="-170"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="0.3" spacecraft="ACE" spacecraftID="-92"/>
<downSignal active="true" signalType="data" dataRate="85331" frequency="0" band="S" power="-113" spacecraft="ACE" spacecraftID="-92"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="0.2" spacecraft="DSCO" spacecraftID="-184"/>
<downSignal active="true" signalType="data" dataRate="20000" frequency="0" band="X" power="-130" spacecraft="DSCO" spacecraftID="-184"/>
<target name="JWST" id="170" uplegRange="1300000" downlegRange="1300000" rtlt="8.69"/>
<target name="ACE" id="92" uplegRange="1490000" downlegRange="1490000" rtlt="9.93"/>
<target name="DSCO" id="184" uplegRange="1510000" downlegRange="1510000" rtlt="10.1"/>
</dish>
<dish name="DSS25" azimuthAngle="180" elevationAngle="90" windSpeed="5" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="18" spacecraft="EURC" spacecraftID="-159"/>
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="Ka" power="-140" spacecraft="EURC" spacecraftID="-159"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="18" spacecraft="PSYC" spacecraftID="-255"/>
<downSignal active="true" signalType="data" dataRate="216758" frequency="0" band="X" power="-130" spacecraft="PSYC" spacecraftID="-255"/>
<target name="EURC" id="159" uplegRange="291000000" downlegRange="291000000" rtlt="1940"/>
<target name="PSYC" id="255" uplegRange="400000000" downlegRange="400000000" rtlt="2670"/>
</dish>
<dish name="DSS26" azimuthAngle="140" elevationAngle="80" windSpeed="5" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="0.2" spacecraft="KPLO" spacecraftID="-155"/>
<downSignal active="true" signalType="data" dataRate="8500000" frequency="0" band="X" power="-95" spacecraft="KPLO" spacecraftID="-155"/>
<downSignal active="true" signalType="data" dataRate="131100" frequency="0" band="S" power="-120" spacecraft="THB" spacecraftID="-192"/>
<downSignal active="true" signalType="data" dataRate="235228" frequency="0" band="S" power="-120" spacecraft="THC" spacecraftID="-193"/>
<downSignal active="true" signalType="data" dataRate="263480" frequency="0" band="S" power="-110" spacecraft="CAPS" spacecraftID="-217"/>
<target name="KPLO" id="155" uplegRange="363000" downlegRange="363000" rtlt="2.42"/>
<target name="THB" id="192" uplegRange="361000" downlegRange="361000" rtlt="2.41"/>
<target name="THC" id="193" uplegRange="362000" downlegRange="362000" rtlt="2.41"/>
<target name="CAPS" id="217" uplegRange="370000" downlegRange="370000" rtlt="2.47"/>
</dish>
<station name="mdscc" friendlyName="Madrid" timeUTC="1772096403000" timeZoneOffset="3600000"/>
<dish name="DSS63" azimuthAngle="276" elevationAngle="29" windSpeed="4" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="19" spacecraft="JNO" spacecraftID="-61"/>
<downSignal active="true" signalType="data" dataRate="200000" frequency="0" band="X" power="-130" spacecraft="JNO" spacecraftID="-61"/>
<downSignal active="true" signalType="data" dataRate="50000" frequency="0" band="X" power="-140" spacecraft="BEPI" spacecraftID="-121"/>
<target name="JNO" id="61" uplegRange="681000000" downlegRange="681000000" rtlt="4540"/>
<target name="BEPI" id="121" uplegRange="150000000" downlegRange="150000000" rtlt="1000"/>
</dish>
<dish name="DSS54" azimuthAngle="169" elevationAngle="31" windSpeed="4" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="0.2" spacecraft="WIND" spacecraftID="-8"/>
<downSignal active="true" signalType="data" dataRate="73510" frequency="0" band="S" power="-120" spacecraft="WIND" spacecraftID="-8"/>
<downSignal active="true" signalType="data" dataRate="1000000" frequency="0" band="S" power="-120" spacecraft="CGO" spacecraftID="-171"/>
<target name="WIND" id="8" uplegRange="1330000" downlegRange="1330000" rtlt="8.89"/>
<target name="CGO" id="171" uplegRange="1580000" downlegRange="1580000" rtlt="10.5"/>
</dish>
<dish name="DSS55" azimuthAngle="135" elevationAngle="23" windSpeed="4" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="5" spacecraft="MRO" spacecraftID="-74"/>
<downSignal active="true" signalType="data" dataRate="1000000" frequency="0" band="X" power="-120" spacecraft="MRO" spacecraftID="-74"/>
<downSignal active="true" signalType="data" dataRate="14220" frequency="0" band="X" power="-140" spacecraft="M01O" spacecraftID="-53"/>
<downSignal active="true" signalType="data" dataRate="1541000" frequency="0" band="X" power="-120" spacecraft="TGO" spacecraftID="-143"/>
<downSignal active="true" signalType="data" dataRate="45" frequency="0" band="X" power="-160" spacecraft="MVN" spacecraftID="-202"/>
<downSignal active="true" signalType="data" dataRate="176" frequency="0" band="X" power="-150" spacecraft="M20" spacecraftID="-168"/>
<downSignal active="true" signalType="data" dataRate="87380" frequency="0" band="X" power="-130" spacecraft="MEX" spacecraftID="-41"/>
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="-480" spacecraft="MSL" spacecraftID="-76"/>
<downSignal active="true" signalType="data" dataRate="241895" frequency="0" band="X" power="-130" spacecraft="EMM" spacecraftID="-222"/>
<target name="MRO" id="74" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
<target name="M01O" id="53" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
<target name="TGO" id="143" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
<target name="MVN" id="202" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
<target name="M20" id="168" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
<target name="MEX" id="41" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
<target name="MSL" id="76" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
<target name="EMM" id="222" uplegRange="351000000" downlegRange="351000000" rtlt="2340"/>
</dish>
<dish name="DSS56" azimuthAngle="134" elevationAngle="11" windSpeed="4" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="10" spacecraft="STA" spacecraftID="-234"/>
<downSignal active="true" signalType="data" dataRate="720000" frequency="0" band="X" power="-120" spacecraft="STA" spacecraftID="-234"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="12" spacecraft="ORX" spacecraftID="-64"/>
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="-150" spacecraft="ORX" spacecraftID="-64"/>
<downSignal active="true" signalType="data" dataRate="32000" frequency="0" band="X" power="-140" spacecraft="HYB2" spacecraftID="-37"/>
<downSignal active="true" signalType="data" dataRate="28756" frequency="0" band="X" power="-130" spacecraft="LUCY" spacecraftID="-49"/>
<target name="STA" id="234" uplegRange="129000000" downlegRange="129000000" rtlt="859"/>
<target name="ORX" id="64" uplegRange="92200000" downlegRange="92200000" rtlt="615"/>
<target name="HYB2" id="37" uplegRange="350000000" downlegRange="350000000" rtlt="2330"/>
<target name="LUCY" id="49" uplegRange="500000000" downlegRange="500000000" rtlt="3330"/>
</dish>
<dish name="DSS65" azimuthAngle="80" elevationAngle="90" windSpeed="4" isMSPA="true" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="245800" frequency="0" band="S" power="-130" spacecraft="SOHO" spacecraftID="-21"/>
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="-120" spacecraft="CHDR" spacecraftID="-151"/>
<downSignal active="true" signalType="data" dataRate="80660" frequency="0" band="X" power="-130" spacecraft="XMM" spacecraftID="-122"/>
<downSignal active="true" signalType="data" dataRate="41063874" frequency="0" band="S" power="-100" spacecraft="TESS" spacecraftID="-95"/>
<target name="SOHO" id="21" uplegRange="1660000" downlegRange="1660000" rtlt="11"/>
<target name="CHDR" id="151" uplegRange="90500" downlegRange="90500" rtlt="0.6"/>
<target name="XMM" id="122" uplegRange="100000" downlegRange="100000" rtlt="0.67"/>
<target name="TESS" id="95" uplegRange="370000" downlegRange="370000" rtlt="2.47"/>
</dish>
<station name="cdscc" friendlyName="Canberra" timeUTC="1772096403000" timeZoneOffset="39600000"/>
<dish name="DSS43" azimuthAngle="218" elevationAngle="37" windSpeed="13" isMSPA="true" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="160" frequency="0" band="X" power="-290" spacecraft="VGR2" spacecraftID="-32"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="Ka" power="0.6" spacecraft="SPP" spacecraftID="-96"/>
<downSignal active="true" signalType="data" dataRate="71559" frequency="0" band="Ka" power="-130" spacecraft="SPP" spacecraftID="-96"/>
<target name="VGR2" id="32" uplegRange="21400000000" downlegRange="21400000000" rtlt="142000"/>
<target name="SPP" id="96" uplegRange="100000000" downlegRange="100000000" rtlt="667"/>
</dish>
<dish name="DSS34" azimuthAngle="161" elevationAngle="34" windSpeed="13" isMSPA="true" isArray="false" isDDOR="false">
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="X" power="0.2" spacecraft="ESCB" spacecraftID="-9"/>
<downSignal active="true" signalType="data" dataRate="8000" frequency="0" band="X" power="-130" spacecraft="ESCB" spacecraftID="-9"/>
<downSignal active="true" signalType="data" dataRate="23863" frequency="0" band="X" power="-130" spacecraft="ESCG" spacecraftID="-10"/>
<downSignal active="true" signalType="data" dataRate="10104" frequency="0" band="S" power="-130" spacecraft="BIOS" spacecraftID="-227"/>
<target name="ESCB" id="9" uplegRange="2000000" downlegRange="2000000" rtlt="13.4"/>
<target name="ESCG" id="10" uplegRange="2000000" downlegRange="2000000" rtlt="13.4"/>
<target name="BIOS" id="227" uplegRange="150000000" downlegRange="150000000" rtlt="1000"/>
</dish>
<dish name="DSS35" azimuthAngle="297" elevationAngle="36" windSpeed="13" isMSPA="true" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="714300" frequency="0" band="X" power="-120" spacecraft="IMAP" spacecraftID="-43"/>
<upSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="0.1" spacecraft="LRO" spacecraftID="-85"/>
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="-140" spacecraft="LRO" spacecraftID="-85"/>
<downSignal active="true" signalType="data" dataRate="4001" frequency="0" band="S" power="-120" spacecraft="SWFO" spacecraftID="-231"/>
<target name="IMAP" id="43" uplegRange="1480000" downlegRange="1480000" rtlt="9.88"/>
<target name="LRO" id="85" uplegRange="363000" downlegRange="363000" rtlt="2.42"/>
<target name="SWFO" id="231" uplegRange="1500000" downlegRange="1500000" rtlt="9.99"/>
</dish>
<dish name="DSS36" azimuthAngle="232" elevationAngle="58" windSpeed="13" isMSPA="true" isArray="false" isDDOR="false">
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="-110" spacecraft="MMS1" spacecraftID="-108"/>
<downSignal active="true" signalType="data" dataRate="2500000" frequency="0" band="S" power="-120" spacecraft="MMS2" spacecraftID="-109"/>
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="-110" spacecraft="MMS3" spacecraftID="-110"/>
<downSignal active="true" signalType="data" dataRate="0" frequency="0" band="S" power="-120" spacecraft="MMS4" spacecraftID="-113"/>
<target name="MMS1" id="108" uplegRange="66700" downlegRange="66700" rtlt="0.44"/>
<target name="MMS2" id="109" uplegRange="79600" downlegRange="79600" rtlt="0.53"/>
<target name="MMS3" id="110" uplegRange="91500" downlegRange="91500" rtlt="0.61"/>
<target name="MMS4" id="113" uplegRange="103000" downlegRange="103000" rtlt="0.69"/>
</dish>
<timestamp>1772096403000</timestamp>
</dsn>)==--==";

// Test dummy data that cycles through all rate classes
static const char data_animation_test[] PROGMEM = R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
    <station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" />
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="TEST" spacecraftID="-170" />
        <upSignal dataRate="0" frequency="2090" power="4.804" signalType="data" spacecraft="TEST" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="TEST" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS24" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="Rate1" spacecraftID="-170" />
        <upSignal dataRate="1.163e+01" frequency="2090" power="4.804" signalType="data" spacecraft="Rate1" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="Rate1" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
    <dish azimuthAngle="287.7" elevationAngle="18.74" isArray="false" isDDOR="false" isMSPA="false" name="DSS26" windSpeed="5.556">
        <downSignal dataRate="6.400e+03" frequency="8439000000" power="-138.1801" signalType="data" spacecraft="Rate2" spacecraftID="-74" />
        <upSignal dataRate="6.400e+03" frequency="7183" power="0.0000" signalType="data" spacecraft="Rate2" spacecraftID="-74" />
        <target downlegRange="8.207e+07" id="74" name="Rate2" rtlt="547.5" uplegRange="8.207e+07" />
    </dish>
    <station friendlyName="Madrid" name="mdscc" timeUTC="1670419133000" timeZoneOffset="3600000" />
    <dish azimuthAngle="103.0" elevationAngle="80.19" isArray="false" isDDOR="false" isMSPA="false" name="DSS56" windSpeed="5.556">
        <downSignal dataRate="5.000e+04" frequency="2250000000" power="-478.1842" signalType="data" spacecraft="Rate3" spacecraftID="-151" />
        <upSignal dataRate="5.000e+04" frequency="2250000000" power="-478.1842" signalType="data" spacecraft="Rate3" spacecraftID="-151" />
        <target downlegRange="1.417e+05" id="151" name="Rate3" rtlt="0.9455" uplegRange="1.417e+05" />
    </dish>
    <dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556">
        <downSignal dataRate="2.458e+05" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <upSignal dataRate="2.458e+05" frequency="2098" power="0.2630" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <target downlegRange="1.389e+06" id="92" name="Rate4" rtlt="9.266" uplegRange="1.389e+06" />
    </dish>
    <station friendlyName="Canberra" name="cdscc" timeUTC="1670419133000" timeZoneOffset="39600000" />
    <dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556">
        <downSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <upSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <target downlegRange="4.099e+06" id="210" name="Rate5" rtlt="27.34" uplegRange="4.099e+06" />
    </dish>
    <dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556">
        <downSignal dataRate="2.621e+06" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <upSignal dataRate="2.621e+06" frequency="2066" power="1.758" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <target downlegRange="1.331e+06" id="21" name="Rate6" rtlt="8.882" uplegRange="1.331e+06" />
    </dish>
    <timestamp>1670419133000</timestamp>
</dsn>)==--==";

// Test dummy data that cycles through specific rate classes
static const char data_animation_test_single[] PROGMEM = R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
    <station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" />
    <dish azimuthAngle="196.5" elevationAngle="30.71" isArray="false" isDDOR="false" isMSPA="false" name="DSS65" windSpeed="5.556">
        <downSignal dataRate="2.458e+05" frequency="2278000000" power="-112.7797" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <upSignal dataRate="2.458e+05" frequency="2098" power="0.2630" signalType="data" spacecraft="Rate4" spacecraftID="-92" />
        <target downlegRange="1.389e+06" id="92" name="Rate4" rtlt="9.266" uplegRange="1.389e+06" />
    </dish>
	<dish azimuthAngle="124.5" elevationAngle="53.41" isArray="false" isDDOR="false" isMSPA="false" name="DSS53" windSpeed="5.556">
        <downSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <upSignal dataRate="1.190e+06" frequency="8436000000" power="-170.1741" signalType="data" spacecraft="Rate5" spacecraftID="-210" />
        <target downlegRange="4.099e+06" id="210" name="Rate5" rtlt="27.34" uplegRange="4.099e+06" />
    </dish>
    <dish azimuthAngle="219.7" elevationAngle="22.84" isArray="false" isDDOR="false" isMSPA="false" name="DSS54" windSpeed="5.556">
        <downSignal dataRate="2.621e+06" frequency="2245000000" power="-110.7082" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <upSignal dataRate="2.621e+06" frequency="2066" power="1.758" signalType="data" spacecraft="Rate6" spacecraftID="-21" />
        <target downlegRange="1.331e+06" id="21" name="Rate6" rtlt="8.882" uplegRange="1.331e+06" />
    </dish>
    <timestamp>1670419133000</timestamp>
</dsn>)==--==";

// Test font by displaying all alphanumeric characters
static const char data_font_test[] PROGMEM = R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
    <station friendlyName="Goldstone" name="gdscc" timeUTC="1670419133000" timeZoneOffset="-28800000" />
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS2" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <upSignal dataRate="1.163e+01" frequency="2090" power="4.804" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="abcdefghijklmnopqrstuvwxyz 0123456789" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
	<dish azimuthAngle="265.6" elevationAngle="29.25" isArray="false" isDDOR="false" isMSPA="false" name="DSS2" windSpeed="5.556">
        <downSignal dataRate="1.163e+01" frequency="2270000000" power="-121.9500" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <upSignal dataRate="1.163e+01" frequency="2090" power="4.804" signalType="data" spacecraft="abcdefghijklmnopqrstuvwxyz 0123456789" spacecraftID="-170" />
        <target downlegRange="1.653e+06" id="170" name="abcdefghijklmnopqrstuvwxyz 0123456789" rtlt="11.03" uplegRange="1.653e+06" />
    </dish>
	<timestamp>1670419133000</timestamp>
</dsn>)==--==";

// Test DSN Maintenance mode XML
static const char data_dsn_maintenance[] PROGMEM = R"==--==(<?xml version='1.0' encoding='utf-8'?>
<dsn>
	<station name="gdscc" friendlyName="Goldstone" timeUTC="1698785318000" timeZoneOffset="-25200000"/>
	<station name="mdscc" friendlyName="Madrid" timeUTC="1698785318000" timeZoneOffset="3600000"/>
	<station name="cdscc" friendlyName="Canberra" timeUTC="1698785318000" timeZoneOffset="39600000"/>
	<timestamp>1698785318000</timestamp>
</dsn>)==--==";

struct XmlFileEntry {
    const char* filename;
    const char* data;
};

static const XmlFileEntry xmlFiles[] = {
    { "fallback",              data_fallback },
    { "animation_test",        data_animation_test },
    { "animation_test_single", data_animation_test_single },
    { "font_test",             data_font_test },
    { "dsn_maintenance",       data_dsn_maintenance },
};

static const size_t xmlFileCount = sizeof(xmlFiles) / sizeof(xmlFiles[0]);

void XmlTestData::init() {
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    if (showSerial) Serial.println("Initializing XML test data...");

    if (!LittleFS.exists(XML_DATA_DIR)) {
        if (!LittleFS.mkdir(XML_DATA_DIR)) {
            if (showSerial) Serial.printf("%sFailed to create %s — skipping seed%s\n",
                DevUtils::termColor("red").c_str(), XML_DATA_DIR,
                DevUtils::termColor("reset").c_str());
            return;
        }
    }

    for (size_t i = 0; i < xmlFileCount; i++) {
        char path[64];
        snprintf(path, sizeof(path), "%s/%s.xml", XML_DATA_DIR, xmlFiles[i].filename);
        seedFileIfMissing(path, xmlFiles[i].data);
    }

    if (showSerial) Serial.println("XML test data ready");
}

void XmlTestData::seedFileIfMissing(const char* path, const char* data) {
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    if (LittleFS.exists(path)) {
        // Guard against partial writes from power loss — re-seed empty/corrupt files
        File check = LittleFS.open(path, "r");
        if (check && check.size() > 0) {
            check.close();
            if (showSerial) Serial.printf("  %s exists, skipping\n", path);
            return;
        }
        if (check) check.close();
        if (showSerial) Serial.printf("  %s exists but is empty/corrupt, re-seeding\n", path);
    }

    if (showSerial) Serial.printf("  Seeding %s...\n", path);
    File file = LittleFS.open(path, "w");
    if (!file) {
        if (showSerial) Serial.printf("%sFailed to create %s%s\n",
            DevUtils::termColor("red").c_str(), path,
            DevUtils::termColor("reset").c_str());
        return;
    }

    // Write from PROGMEM — on ESP32 flash strings are memory-mapped,
    // so file.print() works directly with PROGMEM pointers.
    file.print(data);
    file.close();
    if (showSerial) Serial.printf("  Seeded %s (%u bytes)\n", path, (unsigned)strlen(data));
}

bool XmlTestData::loadFile(const char* filename, char* buffer, size_t bufferSize) {
    bool showSerial = FileUtils::config.debugUtils.showSerial;
    char path[64];
    snprintf(path, sizeof(path), "%s/%s.xml", XML_DATA_DIR, filename);

    File file = LittleFS.open(path, "r");
    if (!file) {
        if (showSerial) Serial.printf("%sFailed to open %s%s\n",
            DevUtils::termColor("red").c_str(), path,
            DevUtils::termColor("reset").c_str());
        return false;
    }

    size_t fileSize = file.size();
    if (fileSize >= bufferSize) {
        if (showSerial) Serial.printf("%s%s too large (%u bytes, buffer %u)%s\n",
            DevUtils::termColor("red").c_str(), path,
            (unsigned)fileSize, (unsigned)bufferSize,
            DevUtils::termColor("reset").c_str());
        file.close();
        return false;
    }

    size_t bytesRead = file.readBytes(buffer, fileSize);
    file.close();

    if (bytesRead != fileSize) {
        if (showSerial) Serial.printf("%sShort read on %s (%u of %u bytes)%s\n",
            DevUtils::termColor("red").c_str(), path,
            (unsigned)bytesRead, (unsigned)fileSize,
            DevUtils::termColor("reset").c_str());
        return false;
    }

    buffer[bytesRead] = '\0';
    if (showSerial) Serial.printf("Loaded %s (%u bytes)\n", path, (unsigned)bytesRead);
    return true;
}

const char* XmlTestData::getFallbackData() {
    return data_fallback;
}
