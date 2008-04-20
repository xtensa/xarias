v 20070526 1
T 81425 33925 9 10 1 0 0 0 1
AC 6-12V (+12V DC)
T 45575 34250 9 16 1 0 0 0 1
XARIAS BOARD_02
N 79650 34000 70950 34000 4
N 62900 33400 62900 32700 4
N 69000 34900 70050 34900 4
C 65325 34000 1 270 0 capacitor-2.sym
{
T 66025 33800 5 10 0 0 270 0 1
device=POLARIZED_CAPACITOR
T 66000 33775 5 10 1 1 270 0 1
refdes=C1
T 66225 33800 5 10 0 0 270 0 1
symversion=0.1
T 65825 33775 5 10 1 1 270 0 1
value=47uF
T 65325 34000 5 10 0 1 0 0 1
footprint=RCY200P
}
C 64000 34000 1 270 0 capacitor-1.sym
{
T 64700 33800 5 10 0 0 270 0 1
device=CAPACITOR
T 64675 33775 5 10 1 1 270 0 1
refdes=C2
T 64900 33800 5 10 0 0 270 0 1
symversion=0.1
T 64000 34000 5 10 0 1 0 0 1
footprint=CK05_type_Capacitor
T 64475 33775 5 10 1 1 270 0 1
value=0.33uF
}
N 63700 34000 70050 34000 4
N 70950 34900 74975 34900 4
N 64200 33100 64200 32700 4
N 65525 33100 65525 32700 4
C 53425 45275 1 0 0 7414-2.sym
{
T 53725 45275 5 10 1 1 0 0 1
device=7414
T 53725 48275 5 10 0 0 0 0 1
footprint=DIP14
T 53725 48875 5 10 0 0 0 0 1
symversion=1.0
T 53925 46175 5 10 1 1 0 0 1
refdes=U2
T 53425 45275 5 10 0 0 0 0 1
slot=3
T 53425 45275 5 10 0 0 0 0 1
net=GND:7
T 53425 45275 5 10 0 0 0 0 1
net=Vdd:14
}
C 50200 47500 1 0 0 7414-2.sym
{
T 50500 47500 5 10 1 1 0 0 1
device=7414
T 50600 48375 5 10 1 1 0 0 1
refdes=U2
T 50500 50500 5 10 0 0 0 0 1
footprint=DIP14
T 50500 51100 5 10 0 0 0 0 1
symversion=1.0
T 50200 47500 5 10 0 0 0 0 1
slot=1
}
C 51400 47500 1 0 0 7414-2.sym
{
T 51700 47500 5 10 1 1 0 0 1
device=7414
T 51800 48350 5 10 1 1 0 0 1
refdes=U2
T 51700 50500 5 10 0 0 0 0 1
footprint=DIP14
T 51700 51100 5 10 0 0 0 0 1
symversion=1.0
T 51400 47500 5 10 0 0 0 0 1
slot=2
}
C 57800 38400 1 0 0 DS1307-1.sym
{
T 59500 40500 5 10 1 1 0 6 1
refdes=U7
T 58100 40850 5 10 0 0 0 0 1
device=ds1307
T 58100 41050 5 10 0 0 0 0 1
footprint=DIP8
}
C 50250 50300 1 0 0 isp_stk200.sym
{
T 51750 53000 5 10 1 1 0 6 1
refdes=U1
T 50650 53100 5 10 0 0 0 0 1
device=connector
T 50650 53300 5 10 0 0 0 0 1
footprint=10
}
N 50350 52000 49950 52000 4
N 49950 52000 49950 50400 4
N 50350 51600 49950 51600 4
N 50350 51200 49950 51200 4
N 50350 50800 49950 50800 4
N 50350 52400 49425 52400 4
N 49425 52400 49425 49675 4
N 49425 49675 54925 49675 4
N 53500 53000 52825 53000 4
N 52050 52400 55350 52400 4
N 55350 52400 55350 51200 4
N 55350 51200 63000 51200 4
C 53125 51900 1 0 0 resistor-2.sym
{
T 53525 52250 5 10 0 0 0 0 1
device=RESISTOR
T 53400 51675 5 10 1 1 0 0 1
refdes=R1
T 53400 51930 5 10 1 1 0 0 1
value=4K7
T 53125 51900 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 52050 52000 53125 52000 4
C 54025 51800 1 0 0 lightdiode-2.sym
{
T 54425 52400 5 10 0 0 0 0 1
device=DIODE
T 54250 51600 5 10 1 1 0 0 1
refdes=D5
T 54025 51800 5 10 0 0 0 0 1
footprint=LED3
}
N 54925 49675 54925 53525 4
N 52050 51600 52825 51600 4
C 54400 53100 1 180 0 resistor-2.sym
{
T 54000 52750 5 10 0 0 180 0 1
device=RESISTOR
T 54175 53300 5 10 1 1 180 0 1
refdes=R2
T 54125 53070 5 10 1 1 180 0 1
value=10K
T 54400 53100 5 10 0 0 90 0 1
footprint=RN55_type_0.1W_Resistor
}
N 63000 50800 52050 50800 4
C 49025 47900 1 0 0 resistor-2.sym
{
T 49425 48250 5 10 0 0 0 0 1
device=RESISTOR
T 49225 48200 5 10 1 1 0 0 1
refdes=R3
T 49300 47930 5 10 1 1 0 0 1
value=4K7
T 49025 47900 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 49925 48000 50200 48000 4
N 52825 54525 52825 51600 4
N 63000 48000 52600 48000 4
C 49600 45675 1 0 0 resistor-2.sym
{
T 50000 46025 5 10 0 0 0 0 1
device=RESISTOR
T 49800 45975 5 10 1 1 0 0 1
refdes=R4
T 49875 45705 5 10 1 1 0 0 1
value=10K
T 49600 45675 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
C 52100 45675 1 0 0 resistor-2.sym
{
T 52500 46025 5 10 0 0 0 0 1
device=RESISTOR
T 52300 45975 5 10 1 1 0 0 1
refdes=R5
T 52375 45705 5 10 1 1 0 0 1
value=1K
T 52100 45675 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 50500 45775 52100 45775 4
N 53000 45775 53425 45775 4
N 68400 47600 70600 47600 4
N 70600 38800 59800 38800 4
{
T 70600 38800 5 10 0 0 0 0 1
netname=TWI-SDA
}
N 68400 47200 70000 47200 4
N 60150 41300 67275 41300 4
N 70000 39200 59800 39200 4
{
T 70000 39200 5 10 0 0 0 0 1
netname=TWI-SCL
}
C 68125 42025 1 180 0 resistor-2.sym
{
T 67725 41675 5 10 0 0 180 0 1
device=RESISTOR
T 67825 42225 5 10 1 1 180 0 1
refdes=R13
T 67875 42000 5 10 1 1 180 0 1
value=4K7
T 68125 42025 5 10 0 0 90 0 1
footprint=RN55_type_0.1W_Resistor
}
C 68175 41400 1 180 0 resistor-2.sym
{
T 67775 41050 5 10 0 0 180 0 1
device=RESISTOR
T 67875 41600 5 10 1 1 180 0 1
refdes=R14
T 67950 41375 5 10 1 1 180 0 1
value=4K7
T 68175 41400 5 10 0 0 90 0 1
footprint=RN55_type_0.1W_Resistor
}
N 70600 41300 68175 41300 4
N 68125 41925 70000 41925 4
N 65000 54525 52825 54525 4
{
T 65000 54525 5 10 0 0 0 0 1
refdes=sdfsdf
}
N 61900 48400 61900 42700 4
N 61900 42850 63200 42850 4
N 63200 42850 63200 39600 4
C 62000 41800 1 90 0 resistor-2.sym
{
T 61650 42200 5 10 0 0 90 0 1
device=RESISTOR
T 61675 42175 5 10 1 1 90 0 1
refdes=R11
T 61970 42075 5 10 1 1 90 0 1
value=10K
T 62000 41800 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 56900 39200 57800 39200 4
C 56825 39900 1 0 0 crystal-1.sym
{
T 57025 40400 5 10 0 0 0 0 1
device=CRYSTAL
T 57025 40200 5 10 1 1 0 0 1
refdes=U8
T 57025 40600 5 10 0 0 0 0 1
symversion=0.1
T 56725 39700 5 10 1 1 0 0 1
value=32.768 kHz
T 56825 39900 5 10 0 0 0 0 1
footprint=LED3
}
N 57525 40000 57800 40000 4
N 57800 39600 56625 39600 4
N 56625 39600 56625 40000 4
N 56625 40000 56825 40000 4
C 67075 55250 1 0 0 capacitor-1.sym
{
T 67275 55950 5 10 0 0 0 0 1
device=CAPACITOR
T 67275 55700 5 10 1 1 0 0 1
refdes=C3
T 67275 56150 5 10 0 0 0 0 1
symversion=0.1
T 67075 55250 5 10 0 0 0 0 1
footprint=CK05_type_Capacitor
T 67750 55225 5 10 1 1 0 0 1
value=12pF
}
C 68000 54550 1 180 0 capacitor-1.sym
{
T 67800 53850 5 10 0 0 180 0 1
device=CAPACITOR
T 67525 54750 5 10 1 1 180 0 1
refdes=C4
T 67800 53650 5 10 0 0 180 0 1
symversion=0.1
T 68000 54550 5 10 0 0 0 0 1
footprint=CK05_type_Capacitor
T 67800 54525 5 10 1 1 0 0 1
value=12pF
}
C 66700 54525 1 90 0 crystal-1.sym
{
T 66200 54725 5 10 0 0 90 0 1
device=CRYSTAL
T 66950 54625 5 10 1 1 90 0 1
refdes=U6
T 66000 54725 5 10 0 0 90 0 1
symversion=0.1
T 66700 54525 5 10 0 0 0 0 1
footprint=crystal
T 66425 54575 5 10 1 1 90 0 1
value=16MHz
}
N 66600 55450 66600 55225 4
N 68600 54350 68000 54350 4
N 66600 53900 66600 54525 4
N 66200 53900 66200 55450 4
T 45525 34925 10 16 1 0 0 0 1
Licensed under the TAPR Open Hardware License (www.tapr.org/OHL)
T 50225 34225 9 16 1 0 0 0 1
Version: v0.2
T 50325 33775 9 16 1 0 0 0 1
Author: R.Pszonczenko
N 59800 40000 60150 40000 4
N 60150 40000 60150 41625 4
N 59800 39600 69475 39600 4
{
T 59800 39600 5 10 0 0 0 0 1
netname=TIMER_SIG
}
N 67225 41925 66000 41925 4
C 63700 33400 1 0 1 lm7805-1.sym
{
T 62100 34700 5 10 0 0 0 6 1
device=L7805C
T 62300 34400 5 10 1 1 0 0 1
refdes=U11
T 63700 33400 5 10 0 0 0 0 1
footprint=78T05
T 63700 33400 5 10 0 0 0 0 1
value=L7805C
}
N 69000 34900 69000 33100 4
N 69000 33100 79650 33100 4
N 70000 47200 70000 37725 4
N 70600 47600 70600 38025 4
N 68400 53200 80400 53200 4
{
T 68400 53200 5 10 0 0 0 0 1
netname=RS
}
N 68400 52800 80400 52800 4
{
T 68400 52800 5 10 0 0 0 0 1
netname=RW
}
N 80000 46325 80000 54400 4
N 68400 52400 80400 52400 4
{
T 68400 52400 5 10 0 0 0 0 1
netname=E
}
N 68400 52000 80400 52000 4
{
T 68400 52000 5 10 0 0 0 0 1
netname=DB0
}
N 68400 51600 80400 51600 4
{
T 68400 51600 5 10 0 0 0 0 1
netname=DB1
}
N 68400 51200 80400 51200 4
{
T 68400 51200 5 10 0 0 0 0 1
netname=DB2
}
N 80400 54400 80000 54400 4
N 68400 48400 80400 48400 4
{
T 68400 48400 5 10 0 0 0 0 1
netname=CS2
}
N 80400 54000 74150 54000 4
N 79425 53600 79425 45525 4
N 79425 53600 80400 53600 4
{
T 79425 53600 5 10 0 0 0 0 1
netname=V0
}
N 68400 50800 80400 50800 4
{
T 68400 50800 5 10 0 0 0 0 1
netname=DB3
}
N 68400 50400 80400 50400 4
{
T 68400 50400 5 10 0 0 0 0 1
netname=DB4
}
N 63000 48400 61900 48400 4
N 67075 55450 66200 55450 4
N 66600 54350 67100 54350 4
N 54625 45775 55775 45775 4
N 62100 34000 59900 34000 4
N 68400 50000 80400 50000 4
{
T 68400 50000 5 10 0 0 0 0 1
netname=DB5
}
N 68400 49600 80400 49600 4
{
T 68400 49600 5 10 0 0 0 0 1
netname=DB6
}
N 68400 49200 80400 49200 4
{
T 68400 49200 5 10 0 0 0 0 1
netname=DB7
}
N 68400 48800 80400 48800 4
{
T 68400 48800 5 10 0 0 0 0 1
netname=CS1
}
N 80400 46800 80000 46800 4
N 78625 47600 80400 47600 4
{
T 78625 47600 5 10 0 0 0 0 1
netname=LCD-VEE
}
N 80400 47200 71675 47200 4
{
T 80400 47200 5 10 0 0 0 0 1
netname=LCD-LEDA
}
N 68400 48000 80400 48000 4
{
T 68400 48000 5 10 0 0 0 0 1
netname=RST
}
N 67975 55450 68600 55450 4
N 68600 55450 68600 54350 4
N 65000 53900 65000 60000 4
N 63000 50400 52350 50400 4
N 52350 50400 52350 51200 4
N 52350 51200 52050 51200 4
N 55775 53200 63000 53200 4
N 55775 45775 55775 53200 4
N 61900 41800 61900 41300 4
N 64600 46300 64600 45900 4
N 64600 45900 65000 45900 4
N 65000 45900 65000 46300 4
N 66000 46300 66000 41300 4
N 59450 56000 59450 46800 4
N 56650 52000 63000 52000 4
N 59050 56400 59050 47200 4
N 57050 51600 63000 51600 4
N 58650 56800 58650 47600 4
N 58250 57200 58250 50400 4
N 57850 57600 57850 50800 4
N 57450 58000 57425 51200 4
N 58650 47600 63000 47600 4
N 59050 47200 63000 47200 4
N 57050 58400 57050 51600 4
N 59450 46800 63000 46800 4
N 56650 58800 56650 52000 4
C 56900 39400 1 180 0 battery-1.sym
{
T 56600 38500 5 10 0 0 180 0 1
device=BATTERY
T 56500 39500 5 10 1 1 180 0 1
refdes=B1
T 56600 38100 5 10 0 0 180 0 1
symversion=0.1
T 56900 39400 5 10 0 0 0 0 1
footprint=CR2032_BAT
}
N 55925 39200 55925 38800 4
N 55925 38800 57800 38800 4
N 55925 39200 56200 39200 4
C 53425 43275 1 0 0 7414-2.sym
{
T 53725 43275 5 10 1 1 0 0 1
device=7414
T 53725 46275 5 10 0 0 0 0 1
footprint=DIP14
T 53725 46875 5 10 0 0 0 0 1
symversion=1.0
T 53925 44175 5 10 1 1 0 0 1
refdes=U2
T 53425 43275 5 10 0 0 0 0 1
slot=4
}
C 49700 43675 1 0 0 resistor-2.sym
{
T 50100 44025 5 10 0 0 0 0 1
device=RESISTOR
T 49975 43975 5 10 1 1 0 0 1
refdes=R6
T 50050 43705 5 10 1 1 0 0 1
value=10K
T 49700 43675 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
C 52100 43675 1 0 0 resistor-2.sym
{
T 52500 44025 5 10 0 0 0 0 1
device=RESISTOR
T 52300 43975 5 10 1 1 0 0 1
refdes=R7
T 52375 43705 5 10 1 1 0 0 1
value=1K
T 52100 43675 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 50600 43775 52100 43775 4
N 53000 43775 53425 43775 4
N 54625 43775 56200 43775 4
N 56200 43775 56200 52800 4
N 56200 52800 63000 52800 4
C 63000 46000 0 0 0 EMBEDDEDATMega32_PDIP.sym
[
P 63000 53200 63300 53200 1 0 0
{
T 63200 53250 5 8 1 1 0 6 1
pinnumber=1
T 63200 53150 5 8 0 1 0 8 1
pinseq=1
T 63350 53200 9 8 1 1 0 0 1
pinlabel=PB0 (XCK/T0)
T 63350 53200 5 8 0 1 0 2 1
pintype=io
}
P 63000 52800 63300 52800 1 0 0
{
T 63200 52850 5 8 1 1 0 6 1
pinnumber=2
T 63200 52750 5 8 0 1 0 8 1
pinseq=2
T 63350 52800 9 8 1 1 0 0 1
pinlabel=PB1 (T1)
T 63350 52800 5 8 0 1 0 2 1
pintype=io
}
P 63000 52400 63300 52400 1 0 0
{
T 63200 52450 5 8 1 1 0 6 1
pinnumber=3
T 63200 52350 5 8 0 1 0 8 1
pinseq=3
T 63350 52400 9 8 1 1 0 0 1
pinlabel=PB2 (INT2/AIN0)
T 63350 52400 5 8 0 1 0 2 1
pintype=io
}
P 63000 52000 63300 52000 1 0 0
{
T 63200 52050 5 8 1 1 0 6 1
pinnumber=4
T 63200 51950 5 8 0 1 0 8 1
pinseq=4
T 63350 52000 9 8 1 1 0 0 1
pinlabel=PB3 (OC0/AIN1)
T 63350 52000 5 8 0 1 0 2 1
pintype=io
}
P 63000 51600 63300 51600 1 0 0
{
T 63200 51650 5 8 1 1 0 6 1
pinnumber=5
T 63200 51550 5 8 0 1 0 8 1
pinseq=5
T 63350 51600 9 8 1 1 0 0 1
pinlabel=PB4 (SS)
T 63350 51600 5 8 0 1 0 2 1
pintype=io
}
P 63000 51200 63300 51200 1 0 0
{
T 63200 51250 5 8 1 1 0 6 1
pinnumber=6
T 63200 51150 5 8 0 1 0 8 1
pinseq=6
T 63350 51200 9 8 1 1 0 0 1
pinlabel=PB5 (MOSI)
T 63350 51200 5 8 0 1 0 2 1
pintype=io
}
P 63000 50800 63300 50800 1 0 0
{
T 63200 50850 5 8 1 1 0 6 1
pinnumber=7
T 63200 50750 5 8 0 1 0 8 1
pinseq=7
T 63350 50800 9 8 1 1 0 0 1
pinlabel=PB6 (MISO)
T 63350 50800 5 8 0 1 0 2 1
pintype=io
}
P 63000 50400 63300 50400 1 0 0
{
T 63200 50450 5 8 1 1 0 6 1
pinnumber=8
T 63200 50350 5 8 0 1 0 8 1
pinseq=8
T 63350 50400 9 8 1 1 0 0 1
pinlabel=PB7 (SCK)
T 63350 50400 5 8 0 1 0 2 1
pintype=io
}
P 65000 53900 65000 53600 1 0 0
{
T 64950 53695 5 8 1 1 90 0 1
pinnumber=9
T 64950 53700 5 8 0 1 270 8 1
pinseq=9
T 65000 53545 9 8 1 1 90 6 1
pinlabel=RESET
T 65000 53550 5 8 0 1 270 2 1
pintype=io
}
P 66000 46300 66000 46600 1 0 0
{
T 65950 46505 5 8 1 1 90 6 1
pinnumber=10
T 66050 46500 5 8 0 1 90 8 1
pinseq=10
T 66000 46655 9 8 1 1 90 0 1
pinlabel=VCC
T 66000 46650 5 8 0 1 90 2 1
pintype=io
}
P 64600 46300 64600 46600 1 0 0
{
T 64550 46505 5 8 1 1 90 6 1
pinnumber=11
T 64650 46500 5 8 0 1 90 8 1
pinseq=11
T 64600 46680 9 8 1 1 90 0 1
pinlabel=GND
T 64600 46650 5 8 0 1 90 2 1
pintype=io
}
P 66600 53900 66600 53600 1 0 0
{
T 66550 53695 5 8 1 1 90 0 1
pinnumber=12
T 66550 53700 5 8 0 1 270 8 1
pinseq=12
T 66600 53545 9 8 1 1 90 6 1
pinlabel=XTAL2
T 66600 53550 5 8 0 1 270 2 1
pintype=io
}
P 66200 53900 66200 53600 1 0 0
{
T 66150 53695 5 8 1 1 90 0 1
pinnumber=13
T 66150 53700 5 8 0 1 270 8 1
pinseq=13
T 66200 53545 9 8 1 1 90 6 1
pinlabel=XTAL1
T 66200 53550 5 8 0 1 270 2 1
pintype=io
}
P 63000 49200 63300 49200 1 0 0
{
T 63200 49250 5 8 1 1 0 6 1
pinnumber=14
T 63200 49150 5 8 0 1 0 8 1
pinseq=14
T 63350 49200 9 8 1 1 0 0 1
pinlabel=PD0 (RXD)
T 63350 49200 5 8 0 1 0 2 1
pintype=io
}
P 63000 48800 63300 48800 1 0 0
{
T 63200 48850 5 8 1 1 0 6 1
pinnumber=15
T 63200 48750 5 8 0 1 0 8 1
pinseq=15
T 63350 48800 9 8 1 1 0 0 1
pinlabel=PD1 (TXD)
T 63350 48800 5 8 0 1 0 2 1
pintype=io
}
P 63000 48400 63300 48400 1 0 0
{
T 63200 48450 5 8 1 1 0 6 1
pinnumber=16
T 63200 48350 5 8 0 1 0 8 1
pinseq=16
T 63350 48400 9 8 1 1 0 0 1
pinlabel=PD2 (INT0)
T 63350 48400 5 8 0 1 0 2 1
pintype=io
}
P 63000 48000 63300 48000 1 0 0
{
T 63200 48050 5 8 1 1 0 6 1
pinnumber=17
T 63200 47950 5 8 0 1 0 8 1
pinseq=17
T 63350 48000 9 8 1 1 0 0 1
pinlabel=PD3 (INT1)
T 63350 48000 5 8 0 1 0 2 1
pintype=io
}
P 63000 47600 63300 47600 1 0 0
{
T 63200 47650 5 8 1 1 0 6 1
pinnumber=18
T 63200 47550 5 8 0 1 0 8 1
pinseq=18
T 63350 47600 9 8 1 1 0 0 1
pinlabel=PD4 (OC1B)
T 63350 47600 5 8 0 1 0 2 1
pintype=io
}
P 63000 47200 63300 47200 1 0 0
{
T 63200 47250 5 8 1 1 0 6 1
pinnumber=19
T 63200 47150 5 8 0 1 0 8 1
pinseq=19
T 63350 47200 9 8 1 1 0 0 1
pinlabel=PD5 (OC1A)
T 63350 47200 5 8 0 1 0 2 1
pintype=io
}
P 63000 46800 63300 46800 1 0 0
{
T 63200 46850 5 8 1 1 0 6 1
pinnumber=20
T 63200 46750 5 8 0 1 0 8 1
pinseq=20
T 63350 46800 9 8 1 1 0 0 1
pinlabel=PD6 (ICP1)
T 63350 46800 5 8 0 1 0 2 1
pintype=io
}
P 68400 53200 68100 53200 1 0 0
{
T 68200 53250 5 8 1 1 0 0 1
pinnumber=40
T 68200 53150 5 8 0 1 0 2 1
pinseq=21
T 68050 53200 9 8 1 1 0 6 1
pinlabel=(ADC0) PA0
T 68050 53200 5 8 0 1 0 8 1
pintype=io
}
P 68400 52800 68100 52800 1 0 0
{
T 68200 52850 5 8 1 1 0 0 1
pinnumber=39
T 68200 52750 5 8 0 1 0 2 1
pinseq=22
T 68050 52800 9 8 1 1 0 6 1
pinlabel=(ADC1) PA1
T 68050 52800 5 8 0 1 0 8 1
pintype=io
}
P 68400 52400 68100 52400 1 0 0
{
T 68200 52450 5 8 1 1 0 0 1
pinnumber=38
T 68200 52350 5 8 0 1 0 2 1
pinseq=23
T 68050 52400 9 8 1 1 0 6 1
pinlabel=(ADC2) PA2
T 68050 52400 5 8 0 1 0 8 1
pintype=io
}
P 68400 52000 68100 52000 1 0 0
{
T 68200 52050 5 8 1 1 0 0 1
pinnumber=37
T 68200 51950 5 8 0 1 0 2 1
pinseq=24
T 68050 52000 9 8 1 1 0 6 1
pinlabel=(ADC3) PA3
T 68050 52000 5 8 0 1 0 8 1
pintype=io
}
P 68400 51600 68100 51600 1 0 0
{
T 68200 51650 5 8 1 1 0 0 1
pinnumber=36
T 68200 51550 5 8 0 1 0 2 1
pinseq=25
T 68050 51600 9 8 1 1 0 6 1
pinlabel=(ADC4) PA4
T 68050 51600 5 8 0 1 0 8 1
pintype=io
}
P 68400 51200 68100 51200 1 0 0
{
T 68200 51250 5 8 1 1 0 0 1
pinnumber=35
T 68200 51150 5 8 0 1 0 2 1
pinseq=26
T 68050 51200 9 8 1 1 0 6 1
pinlabel=(ADC5) PA5
T 68050 51200 5 8 0 1 0 8 1
pintype=io
}
P 68400 50800 68100 50800 1 0 0
{
T 68200 50850 5 8 1 1 0 0 1
pinnumber=34
T 68200 50750 5 8 0 1 0 2 1
pinseq=27
T 68050 50800 9 8 1 1 0 6 1
pinlabel=(ADC6) PA6
T 68050 50800 5 8 0 1 0 8 1
pintype=io
}
P 68400 50400 68100 50400 1 0 0
{
T 68200 50450 5 8 1 1 0 0 1
pinnumber=33
T 68200 50350 5 8 0 1 0 2 1
pinseq=28
T 68050 50400 9 8 1 1 0 6 1
pinlabel=(ADC7) PA7
T 68050 50400 5 8 0 1 0 8 1
pintype=io
}
P 66800 46300 66800 46600 1 0 0
{
T 66750 46505 5 8 1 1 90 6 1
pinnumber=32
T 66750 46500 5 8 0 1 270 2 1
pinseq=29
T 66800 46655 9 8 1 1 90 0 1
pinlabel=AREF
T 66800 46650 5 8 0 1 270 8 1
pintype=io
}
P 65000 46300 65000 46600 1 0 0
{
T 64950 46505 5 8 1 1 90 6 1
pinnumber=31
T 64950 46500 5 8 0 1 270 2 1
pinseq=30
T 65000 46680 9 8 1 1 90 0 1
pinlabel=GND
T 65000 46650 5 8 0 1 270 8 1
pintype=io
}
P 66400 46300 66400 46600 1 0 0
{
T 66350 46505 5 8 1 1 90 6 1
pinnumber=30
T 66350 46500 5 8 0 1 270 2 1
pinseq=31
T 66400 46655 9 8 1 1 90 0 1
pinlabel=AVCC
T 66400 46650 5 8 0 1 270 8 1
pintype=io
}
P 68400 50000 68100 50000 1 0 0
{
T 68200 50050 5 8 1 1 0 0 1
pinnumber=29
T 68200 49950 5 8 0 1 0 2 1
pinseq=32
T 68050 50000 9 8 1 1 0 6 1
pinlabel=(TOSC2) PC7
T 68050 50000 5 8 0 1 0 8 1
pintype=io
}
P 68400 49600 68100 49600 1 0 0
{
T 68200 49650 5 8 1 1 0 0 1
pinnumber=28
T 68200 49550 5 8 0 1 0 2 1
pinseq=33
T 68050 49600 9 8 1 1 0 6 1
pinlabel=(TOSC1) PC6
T 68050 49600 5 8 0 1 0 8 1
pintype=io
}
P 68400 49200 68100 49200 1 0 0
{
T 68200 49250 5 8 1 1 0 0 1
pinnumber=27
T 68200 49150 5 8 0 1 0 2 1
pinseq=34
T 68050 49200 9 8 1 1 0 6 1
pinlabel=(TDI) PC5
T 68050 49200 5 8 0 1 0 8 1
pintype=io
}
P 68400 48800 68100 48800 1 0 0
{
T 68200 48850 5 8 1 1 0 0 1
pinnumber=26
T 68200 48750 5 8 0 1 0 2 1
pinseq=35
T 68050 48800 9 8 1 1 0 6 1
pinlabel=(TDO) PC4
T 68050 48800 5 8 0 1 0 8 1
pintype=io
}
P 68400 48400 68100 48400 1 0 0
{
T 68200 48450 5 8 1 1 0 0 1
pinnumber=25
T 68200 48350 5 8 0 1 0 2 1
pinseq=36
T 68050 48400 9 8 1 1 0 6 1
pinlabel=(TMS) PC3
T 68050 48400 5 8 0 1 0 8 1
pintype=io
}
P 68400 48000 68100 48000 1 0 0
{
T 68200 48050 5 8 1 1 0 0 1
pinnumber=24
T 68200 47950 5 8 0 1 0 2 1
pinseq=37
T 68050 48000 9 8 1 1 0 6 1
pinlabel=(TCK) PC2
T 68050 48000 5 8 0 1 0 8 1
pintype=io
}
P 68400 47600 68100 47600 1 0 0
{
T 68200 47650 5 8 1 1 0 0 1
pinnumber=23
T 68200 47550 5 8 0 1 0 2 1
pinseq=38
T 68050 47600 9 8 1 1 0 6 1
pinlabel=(SDA) PC1
T 68050 47600 5 8 0 1 0 8 1
pintype=io
}
P 68400 47200 68100 47200 1 0 0
{
T 68200 47250 5 8 1 1 0 0 1
pinnumber=22
T 68200 47150 5 8 0 1 0 2 1
pinseq=39
T 68050 47200 9 8 1 1 0 6 1
pinlabel=(SCL) PC0
T 68050 47200 5 8 0 1 0 8 1
pintype=io
}
P 68400 46800 68100 46800 1 0 0
{
T 68200 46850 5 8 1 1 0 0 1
pinnumber=21
T 68200 46750 5 8 0 1 0 2 1
pinseq=40
T 68050 46800 9 8 1 1 0 6 1
pinlabel=(OC2) PD7
T 68050 46800 5 8 0 1 0 8 1
pintype=io
}
B 63300 46600 4800 7000 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 63325 53750 9 10 1 0 0 0 1
ATMega32
T 63400 51000 5 10 0 0 0 0 1
author=rp9
T 63400 51200 5 10 0 0 0 0 1
documentation=none
T 63400 51400 5 10 0 0 0 0 1
description=ATMega32
T 63400 51600 5 10 0 0 0 0 1
numslots=0
]
{
T 67800 53725 5 10 1 1 0 6 1
refdes=U5
T 63400 50600 5 10 0 0 0 0 1
device=ATMega32
T 63400 50800 5 10 0 0 0 0 1
footprint=DIP40
}
C 46500 44675 1 0 0 connector6-1.sym
{
T 48300 46475 5 10 0 0 0 0 1
device=CONNECTOR_6
T 46600 46675 5 10 1 1 0 0 1
refdes=CONN1
T 46500 44675 5 10 0 0 0 0 1
footprint=CONN_XRP_6
}
T 45475 46050 9 10 1 0 0 0 1
Injector IN
T 45575 45750 9 10 1 0 0 0 1
Speed IN
T 45675 45450 9 10 1 0 0 0 1
RPM IN
N 49600 45775 48200 45775 4
N 49025 48000 48900 48000 4
N 48900 48000 48900 46075 4
N 48900 46075 48200 46075 4
N 49700 43775 49300 43775 4
N 49300 43775 49300 45475 4
N 49300 45475 48200 45475 4
C 81350 34200 1 180 0 connector4-1.sym
{
T 79550 33300 5 10 0 0 180 0 1
device=CONNECTOR_4
T 81350 32800 5 10 1 1 180 0 1
refdes=CONN3
T 81350 34200 5 10 0 0 0 0 1
footprint=CONN_XRP_4
}
T 81425 33025 9 10 1 0 0 0 1
AC 6-12V (GND)
C 81600 38525 1 180 0 connector6-1.sym
{
T 79800 36725 5 10 0 0 180 0 1
device=CONNECTOR_6
T 81500 36525 5 10 1 1 180 0 1
refdes=CONN2
T 81600 38525 5 10 0 0 180 0 1
footprint=CONN_XRP_6
}
N 70000 37725 79900 37725 4
N 70600 38025 79900 38025 4
T 81725 37950 9 10 1 0 0 0 1
SDA
N 79900 36825 79175 36825 4
N 79175 36825 79175 36400 4
N 63000 52400 60950 52400 4
N 60950 52400 60950 42725 4
N 60950 43400 71200 43400 4
N 71200 43400 71200 38325 4
N 71200 38325 79900 38325 4
N 69475 39600 69475 37425 4
N 69475 37425 79900 37425 4
N 70600 38800 79800 38800 4
N 79800 38800 79800 41450 4
N 70000 39200 79400 39200 4
N 79400 39200 79400 41050 4
N 71675 47200 71675 42050 4
N 75450 43850 75950 43850 4
N 74150 43050 74150 54000 4
N 79150 41050 79400 41050 4
N 79800 41450 79150 41450 4
N 73925 42650 73525 42650 4
N 79150 43850 79900 43850 4
N 74150 44275 67575 44275 4
N 79150 43050 81175 43050 4
N 75950 41050 75450 41050 4
N 75450 43850 75450 40400 4
N 80800 40150 80450 40150 4
N 59900 34000 59900 34325 4
C 59700 34325 1 0 0 vdd-1.sym
C 59950 41625 1 0 0 vdd-1.sym
N 54400 53000 54925 53000 4
C 54725 53525 1 0 0 vdd-1.sym
N 79150 42250 79275 42250 4
N 75950 41450 75450 41450 4
N 75950 41850 75450 41850 4
N 75950 42250 75450 42250 4
N 75950 43050 74150 43050 4
N 74825 42650 75950 42650 4
N 76025 35900 76025 33700 4
N 76025 33700 79650 33700 4
N 69000 46800 69000 35900 4
N 69000 35900 72975 35900 4
T 81400 33625 9 10 1 0 0 0 1
+12V Ign
N 72325 35000 72325 34900 4
C 73925 42550 1 0 0 resistor-2.sym
{
T 74325 42900 5 10 0 0 0 0 1
device=RESISTOR
T 74125 42850 5 10 1 1 0 0 1
refdes=R15
T 74200 42580 5 10 1 1 0 0 1
value=4K7
T 73925 42550 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
C 79275 42150 1 0 0 resistor-2.sym
{
T 79675 42500 5 10 0 0 0 0 1
device=RESISTOR
T 79475 42450 5 10 1 1 0 0 1
refdes=R16
T 79550 42180 5 10 1 1 0 0 1
value=10K
T 79275 42150 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
T 81725 37650 9 10 1 0 0 0 1
SCL
T 81700 38250 9 10 1 0 0 0 1
INT
T 81750 36750 9 10 1 0 0 0 1
GND
T 81725 37375 9 10 1 0 0 0 1
TIMER
N 48200 45175 49000 45175 4
N 49000 45175 49000 41275 4
N 49000 41275 50200 41275 4
N 51800 41275 52400 41275 4
C 52400 41175 1 0 0 resistor-2.sym
{
T 52800 41525 5 10 0 0 0 0 1
device=RESISTOR
T 52600 41475 5 10 1 1 0 0 1
refdes=R8
T 52675 41205 5 10 1 1 0 0 1
value=3K6
T 52400 41175 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 53300 41275 57850 41275 4
C 53825 41275 1 270 0 resistor-2.sym
{
T 54175 40875 5 10 0 0 270 0 1
device=RESISTOR
T 54125 41075 5 10 1 1 270 0 1
refdes=R9
T 53855 41000 5 10 1 1 270 0 1
value=10K
T 53825 41275 5 10 0 0 270 0 1
footprint=RN55_type_0.1W_Resistor
}
N 51000 40375 51000 40675 4
N 57850 41275 57850 49200 4
N 63000 49200 57850 49200 4
T 45275 45150 9 10 1 0 0 0 1
+12V Lights
N 62525 45175 62525 44375 4
N 62525 46075 62525 48800 4
N 63000 48800 62525 48800 4
N 74175 35900 73875 35900 4
N 69000 46800 68400 46800 4
N 74975 35300 74975 34675 4
C 72975 35800 1 0 0 resistor-2.sym
{
T 73375 36150 5 10 0 0 0 0 1
device=RESISTOR
T 73175 36100 5 10 1 1 0 0 1
refdes=R18
T 73250 35830 5 10 1 1 0 0 1
value=3K6
T 72975 35800 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
C 72225 35900 1 270 0 resistor-2.sym
{
T 72575 35500 5 10 0 0 270 0 1
device=RESISTOR
T 72525 35700 5 10 1 1 270 0 1
refdes=R17
T 72255 35625 5 10 1 1 270 0 1
value=10K
T 72225 35900 5 10 0 0 270 0 1
footprint=RN55_type_0.1W_Resistor
}
N 75775 35900 76025 35900 4
C 61050 41825 1 90 0 resistor-2.sym
{
T 60700 42225 5 10 0 0 90 0 1
device=RESISTOR
T 60750 42025 5 10 1 1 90 0 1
refdes=R10
T 61020 42100 5 10 1 1 90 0 1
value=4K7
T 61050 41825 5 10 0 0 90 0 1
footprint=RN55_type_0.1W_Resistor
}
N 60950 41825 60950 41300 4
C 62625 45175 1 90 0 resistor-2.sym
{
T 62275 45575 5 10 0 0 90 0 1
device=RESISTOR
T 62825 45475 5 10 1 1 90 0 1
refdes=R12
T 62600 45425 5 10 1 1 90 0 1
value=3K6
T 62625 45175 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 62525 44375 63075 44375 4
N 67575 45400 67575 45675 4
C 79500 44250 1 0 0 vdd-1.sym
N 79700 44250 79700 43850 4
C 67375 45675 1 0 0 vdd-1.sym
C 62800 32400 1 0 0 gnd-1.sym
C 64100 32400 1 0 0 gnd-1.sym
C 65425 32400 1 0 0 gnd-1.sym
C 74875 34375 1 0 0 gnd-1.sym
C 79075 36100 1 0 0 gnd-1.sym
C 75350 40100 1 0 0 gnd-1.sym
C 79900 46025 1 0 0 gnd-1.sym
C 68500 54050 1 0 0 gnd-1.sym
C 49850 50100 1 0 0 gnd-1.sym
C 50750 44575 1 0 0 gnd-1.sym
C 51625 42575 1 0 0 gnd-1.sym
C 50900 40075 1 0 0 gnd-1.sym
C 53825 40075 1 0 0 gnd-1.sym
C 55825 38500 1 0 0 gnd-1.sym
C 64700 45600 1 0 0 gnd-1.sym
C 80300 60500 0 180 1 connector34-2.sym
{
T 81100 46300 5 10 1 1 180 0 1
refdes=CONN4
T 80700 46350 5 10 0 0 180 6 1
device=CONNECTOR_34
T 80700 46150 5 10 0 0 180 6 1
footprint=34s
}
T 81200 47100 9 10 1 0 0 0 1
LEDA
T 81200 46700 9 10 1 0 0 0 1
LEDK
T 81200 47500 9 10 1 0 0 0 1
VEE
T 81200 47900 9 10 1 0 0 0 1
RST
T 81200 48300 9 10 1 0 0 0 1
CS2
T 81200 48700 9 10 1 0 0 0 1
CS1
T 81200 50300 9 10 1 0 0 0 1
DB7
T 81200 50700 9 10 1 0 0 0 1
DB6
T 81200 51100 9 10 1 0 0 0 1
DB5
T 81200 51500 9 10 1 0 0 0 1
DB4
T 81200 51900 9 10 1 0 0 0 1
DB3
T 81200 52300 9 10 1 0 0 0 1
DB2
T 81200 52700 9 10 1 0 0 0 1
DB1
T 81200 53100 9 10 1 0 0 0 1
DB0
T 81200 49100 9 10 1 0 0 0 1
E
T 81200 49500 9 10 1 0 0 0 1
R/W
T 81200 49900 9 10 1 0 0 0 1
RS
T 81200 53500 9 10 1 0 0 0 1
V0
T 81200 53900 9 10 1 0 0 0 1
VDD (VCC)
T 81200 54300 9 10 1 0 0 0 1
VSS (GND)
N 58650 56800 80400 56800 4
{
T 58650 56800 5 10 0 0 0 0 1
netname=COL2
}
N 58250 57200 80400 57200 4
{
T 58250 57200 5 10 0 0 0 0 1
netname=COL1
}
N 59450 56000 80400 56000 4
{
T 59450 56000 5 10 0 0 0 0 1
netname=COL4
}
N 59050 56400 80400 56400 4
{
T 59050 56400 5 10 0 0 0 0 1
netname=COL3
}
N 57850 57600 80400 57600 4
{
T 57850 57600 5 10 0 0 0 0 1
netname=F4
}
N 57450 58000 80400 58000 4
{
T 57450 58000 5 10 0 0 0 0 1
netname=F3
}
N 57050 58400 80400 58400 4
{
T 57050 58400 5 10 0 0 0 0 1
netname=F2
}
N 56650 58800 80400 58800 4
{
T 56650 58800 5 10 0 0 0 0 1
netname=F1
}
T 81200 58700 9 10 1 0 0 0 1
F1
T 81200 58300 9 10 1 0 0 0 1
F2
T 81200 57900 9 10 1 0 0 0 1
F3
T 81200 57500 9 10 1 0 0 0 1
F4
T 81200 57100 9 10 1 0 0 0 1
COL1
T 81200 56700 9 10 1 0 0 0 1
COL2
T 81200 56300 9 10 1 0 0 0 1
COL3
T 81200 55900 9 10 1 0 0 0 1
COL4
C 75775 35300 1 0 0 EMBEDDED78L05_st.sym
[
T 74850 35900 9 8 1 0 0 6 1
OUT
T 75402 35900 9 8 1 0 0 6 1
IN
T 74875 35400 9 8 1 0 0 6 1
78L05
B 74475 35600 1000 600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 75119 35701 9 8 1 0 0 6 1
GND
P 75475 35900 75775 35900 1 0 1
{
T 75675 35950 5 8 1 1 0 6 1
pinnumber=1
T 75675 35950 5 8 0 0 0 6 1
pinseq=1
}
P 74975 35300 74975 35600 1 0 0
{
T 75075 35400 5 8 1 1 0 6 1
pinnumber=2
T 75075 35400 5 8 0 0 0 6 1
pinseq=2
}
P 74475 35900 74175 35900 1 0 1
{
T 74345 35950 5 8 1 1 0 6 1
pinnumber=3
T 74345 35950 5 8 0 0 0 6 1
pinseq=3
}
T 74175 36400 5 10 0 0 0 6 1
pins=3
T 74175 36200 5 10 0 0 0 6 1
net=GND:2
]
{
T 74175 36600 5 10 0 0 0 6 1
device=78L05
T 75175 36275 5 10 1 1 0 0 1
refdes=U10
T 75780 35295 5 10 0 1 0 6 1
footprint=TO92
}
C 50200 40675 1 0 0 EMBEDDED78L05_st.sym
[
T 51125 41275 9 8 1 0 0 0 1
OUT
T 50573 41275 9 8 1 0 0 0 1
IN
T 51100 40775 9 8 1 0 0 0 1
78L05
B 50500 40975 1000 600 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
T 50856 41076 9 8 1 0 0 0 1
GND
P 50500 41275 50200 41275 1 0 1
{
T 50300 41325 5 8 1 1 0 0 1
pinnumber=1
T 50300 41325 5 8 0 0 0 0 1
pinseq=1
}
P 51000 40675 51000 40975 1 0 0
{
T 50900 40775 5 8 1 1 0 0 1
pinnumber=2
T 50900 40775 5 8 0 0 0 0 1
pinseq=2
}
P 51500 41275 51800 41275 1 0 1
{
T 51630 41325 5 8 1 1 0 0 1
pinnumber=3
T 51630 41325 5 8 0 0 0 0 1
pinseq=3
}
T 51800 41775 5 10 0 0 0 0 1
pins=3
T 51800 41575 5 10 0 0 0 0 1
net=GND:2
]
{
T 51800 41975 5 10 0 0 0 0 1
device=78L05
T 51525 41650 5 10 1 1 0 6 1
refdes=U3
T 50195 40670 5 10 0 1 0 0 1
footprint=TO92
}
N 65000 60000 80400 60000 4
T 81200 59875 9 10 1 0 0 0 1
RST
C 50650 46675 1 0 0 vdd-1.sym
C 51525 44675 1 0 0 vdd-1.sym
C 70950 34200 1 180 0 diode-rpp.sym
{
T 70550 33600 5 10 0 0 180 0 1
device=DIODE
T 70650 33700 5 10 1 1 180 0 1
refdes=D1
T 70955 34200 5 10 0 1 180 0 1
footprint=ALF300
}
C 70750 34900 1 270 0 diode-rpp.sym
{
T 71350 34500 5 10 0 0 270 0 1
device=DIODE
T 71250 34600 5 10 1 1 270 0 1
refdes=D2
T 70750 34905 5 10 0 1 270 0 1
footprint=ALF300
}
C 69850 34900 1 270 0 diode-rpp.sym
{
T 70450 34500 5 10 0 0 270 0 1
device=DIODE
T 69650 34600 5 10 1 1 270 0 1
refdes=D4
T 69850 34905 5 10 0 1 270 0 1
footprint=ALF300
}
C 70950 35100 1 180 0 diode-rpp.sym
{
T 70550 34500 5 10 0 0 180 0 1
device=DIODE
T 70650 35250 5 10 1 1 180 0 1
refdes=D3
T 70955 35100 5 10 0 1 180 0 1
footprint=ALF300
}
C 51925 42875 1 90 0 diode-rpp.sym
{
T 51325 43275 5 10 0 0 90 0 1
device=DIODE
T 52075 43175 5 10 1 1 90 0 1
refdes=D9
T 51925 42870 5 10 0 1 90 0 1
footprint=ALF300
}
C 51050 45775 1 90 0 diode-rpp.sym
{
T 50450 46175 5 10 0 0 90 0 1
device=DIODE
T 51200 46075 5 10 1 1 90 0 1
refdes=D6
T 51050 45770 5 10 0 1 90 0 1
footprint=ALF300
}
C 51050 44875 1 90 0 diode-rpp.sym
{
T 50450 45275 5 10 0 0 90 0 1
device=DIODE
T 51200 45175 5 10 1 1 90 0 1
refdes=D7
T 51050 44870 5 10 0 1 90 0 1
footprint=ALF300
}
C 51925 43775 1 90 0 diode-rpp.sym
{
T 51325 44175 5 10 0 0 90 0 1
device=DIODE
T 52075 44075 5 10 1 1 90 0 1
refdes=D8
T 51925 43770 5 10 0 1 90 0 1
footprint=ALF300
}
C 63575 43575 1 0 0 gnd-1.sym
N 63675 44875 64525 44875 4
C 65425 44975 1 180 0 resistor-2.sym
{
T 65025 44625 5 10 0 0 180 0 1
device=RESISTOR
T 65125 45175 5 10 1 1 180 0 1
refdes=R19
T 65175 44950 5 10 1 1 180 0 1
value=4K7
T 65425 44975 5 10 0 0 90 0 1
footprint=RN55_type_0.1W_Resistor
}
N 65425 44875 66950 44875 4
N 48575 45175 48575 59600 4
N 48575 59600 80400 59600 4
N 67575 44350 67575 44275 4
C 66975 44375 1 0 0 EMBEDDEDBD238.sym
[
P 67575 45400 67575 45175 1 0 0
{
T 67502 45231 5 6 1 1 0 0 1
pinnumber=2
T 67616 45218 5 6 0 0 0 0 1
pinseq=2
T 67634 45313 5 10 0 1 0 0 1
pintype=pas
T 67310 45216 5 10 0 1 0 0 1
pinlabel=C
}
P 67575 44575 67575 44350 1 0 1
{
T 67475 44425 5 6 1 1 0 0 1
pinnumber=1
T 67610 44433 5 6 0 0 0 0 1
pinseq=1
T 67668 44518 5 10 0 1 0 0 1
pintype=pas
T 67362 44393 5 10 0 1 0 0 1
pinlabel=E
}
V 67475 44876 316 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
L 67575 44575 67375 44775 3 0 0 0 -1 -1
L 67575 45175 67375 44975 3 0 0 0 -1 -1
L 67375 45075 67375 44675 3 0 0 0 -1 -1
P 66950 44875 67159 44875 1 0 0
{
T 67075 44925 5 6 1 1 0 0 1
pinnumber=3
T 67046 44920 5 6 0 0 90 0 1
pinseq=3
T 67058 44543 5 10 0 1 90 0 1
pintype=pas
T 67099 45042 5 10 0 1 0 0 1
pinlabel=B
}
L 67375 44875 67159 44875 3 0 0 0 -1 -1
L 67375 44775 67411 44703 3 0 0 0 -1 -1
L 67375 44775 67447 44739 3 0 0 0 -1 -1
L 67447 44739 67411 44703 3 0 0 0 -1 -1
T 67575 45475 8 10 0 0 0 0 1
description=BD238 PNP transistor
T 67575 45675 8 10 0 0 0 0 1
documentation=
T 67575 45875 8 10 0 0 0 0 1
numslots=0
]
{
T 67875 44675 5 10 1 1 0 0 1
device=BD238
T 67875 44875 5 10 1 1 0 0 1
refdes=T1
T 67575 46075 5 10 0 0 0 0 1
footprint=TO126
}
C 63075 43875 1 0 0 EMBEDDEDBC547-1.sym
[
P 63675 44875 63675 44675 1 0 0
{
T 63575 44725 5 6 1 1 0 0 1
pinnumber=1
T 63575 44725 5 6 0 0 0 0 1
pinseq=1
T 63575 44725 5 6 0 1 0 0 1
pinlabel=1
T 63575 44725 5 6 0 1 0 0 1
pintype=pas
}
P 63675 44075 63675 43875 1 0 1
{
T 63575 43925 5 6 1 1 0 0 1
pinnumber=3
T 63575 43925 5 6 0 0 0 0 1
pinseq=3
T 63575 43925 5 6 0 1 0 0 1
pinlabel=3
T 63575 43925 5 6 0 1 0 0 1
pintype=pas
}
V 63575 44376 316 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
L 63675 44075 63475 44275 3 0 0 0 -1 -1
L 63675 44675 63475 44475 3 0 0 0 -1 -1
L 63475 44575 63475 44175 3 0 0 0 -1 -1
P 63075 44375 63259 44375 1 0 0
{
T 63175 44425 5 6 1 1 0 0 1
pinnumber=2
T 63175 44425 5 6 0 0 0 0 1
pinseq=2
T 63175 44425 5 6 0 1 0 0 1
pinlabel=2
T 63175 44425 5 6 0 1 0 0 1
pintype=pas
}
L 63475 44375 63259 44375 3 0 0 0 -1 -1
L 63675 44075 63639 44147 3 0 0 0 -1 -1
L 63675 44075 63603 44111 3 0 0 0 -1 -1
L 63603 44111 63639 44147 3 0 0 0 -1 -1
T 63975 44175 9 10 1 0 0 0 1
BC547
]
{
T 63975 44575 5 10 0 0 0 0 1
device=BC547
T 63975 44375 5 10 1 1 0 0 1
refdes=T2
T 63875 43975 5 10 0 0 0 0 1
footprint=TO92
}
N 71675 42050 72825 42050 4
N 72825 43250 72825 44275 4
C 73525 42050 1 0 0 EMBEDDEDBC557-1.sym
[
P 72825 43250 72825 42950 1 0 0
{
T 72925 43000 5 8 0 1 0 6 1
pinnumber=3
T 73425 43300 5 8 0 0 0 6 1
pinseq=3
T 72725 43050 9 8 1 1 0 6 1
pinlabel=3
T 73425 43150 5 8 0 1 0 6 1
pintype=pas
}
P 72825 42350 72825 42050 1 0 1
{
T 72925 42200 5 8 0 1 0 6 1
pinnumber=1
T 72625 42100 5 8 0 0 0 6 1
pinseq=1
T 72725 42150 9 8 1 1 0 6 1
pinlabel=1
T 72625 42250 5 8 0 1 0 6 1
pintype=pas
}
V 72925 42651 300 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
L 72825 42350 73025 42550 3 0 0 0 -1 -1
L 72825 42950 73025 42750 3 0 0 0 -1 -1
L 73025 42850 73025 42450 3 0 0 0 -1 -1
P 73525 42650 73241 42650 1 0 0
{
T 73325 42500 5 8 0 1 0 6 1
pinnumber=2
T 73525 42200 5 6 0 0 0 6 1
pinseq=2
T 73425 42450 9 8 1 1 0 6 1
pinlabel=2
T 73525 42350 5 8 0 1 0 6 1
pintype=pas
}
L 73025 42650 73241 42650 3 0 0 0 -1 -1
L 72925 42450 72861 42422 3 0 0 0 -1 -1
L 72925 42450 72897 42386 3 0 0 0 -1 -1
L 72897 42386 72861 42422 3 0 0 0 -1 -1
T 72525 43150 5 8 0 0 0 6 1
numslots=0
T 72525 43050 5 8 0 0 0 6 1
description=Small Signal PNP Bipolar
T 72525 42450 9 10 1 0 0 6 1
BC557
T 72525 43250 5 8 0 0 0 6 1
documentation=http://www.onsemi.com/pub/Collateral/BC556B-D.PDF
]
{
T 72525 42850 5 8 0 0 0 6 1
device=BC557
T 72525 42650 5 10 1 1 0 6 1
refdes=T3
T 72525 42550 5 8 0 0 0 6 1
value=BC557
T 72525 42950 5 8 0 0 0 6 1
footprint=TO92
T 72525 43050 5 10 0 0 0 6 1
symversion=1.0
}
C 82150 40150 1 0 0 EMBEDDEDBC557-1.sym
[
P 82850 41350 82850 41050 1 0 0
{
T 82750 41100 5 8 0 1 0 0 1
pinnumber=3
T 82250 41400 5 8 0 0 0 0 1
pinseq=3
T 82950 41150 9 8 1 1 0 0 1
pinlabel=3
T 82250 41250 5 8 0 1 0 0 1
pintype=pas
}
P 82850 40450 82850 40150 1 0 1
{
T 82750 40300 5 8 0 1 0 0 1
pinnumber=1
T 83050 40200 5 8 0 0 0 0 1
pinseq=1
T 82950 40250 9 8 1 1 0 0 1
pinlabel=1
T 83050 40350 5 8 0 1 0 0 1
pintype=pas
}
V 82750 40751 300 3 0 0 0 -1 -1 0 -1 -1 -1 -1 -1
L 82850 40450 82650 40650 3 0 0 0 -1 -1
L 82850 41050 82650 40850 3 0 0 0 -1 -1
L 82650 40950 82650 40550 3 0 0 0 -1 -1
P 82150 40750 82434 40750 1 0 0
{
T 82350 40600 5 8 0 1 0 0 1
pinnumber=2
T 82150 40300 5 6 0 0 0 0 1
pinseq=2
T 82250 40550 9 8 1 1 0 0 1
pinlabel=2
T 82150 40450 5 8 0 1 0 0 1
pintype=pas
}
L 82650 40750 82434 40750 3 0 0 0 -1 -1
L 82750 40550 82814 40522 3 0 0 0 -1 -1
L 82750 40550 82778 40486 3 0 0 0 -1 -1
L 82778 40486 82814 40522 3 0 0 0 -1 -1
T 83150 41250 5 8 0 0 0 0 1
numslots=0
T 83150 41150 5 8 0 0 0 0 1
description=Small Signal PNP Bipolar
T 83150 40550 9 10 1 0 0 0 1
BC557
T 83150 41350 5 8 0 0 0 0 1
documentation=http://www.onsemi.com/pub/Collateral/BC556B-D.PDF
]
{
T 83150 40950 5 8 0 0 0 0 1
device=BC557
T 83150 40750 5 10 1 1 0 0 1
refdes=T4
T 83150 40650 5 8 0 0 0 0 1
value=BC557
T 83150 41050 5 8 0 0 0 0 1
footprint=TO92
T 83150 41150 5 10 0 0 0 0 1
symversion=1.0
}
C 81050 42250 1 270 0 resistor-2.sym
{
T 81400 41850 5 10 0 0 270 0 1
device=RESISTOR
T 81350 42050 5 10 1 1 270 0 1
refdes=R20
T 81080 41975 5 10 1 1 270 0 1
value=4K7
T 81050 42250 5 10 0 0 270 0 1
footprint=RN55_type_0.1W_Resistor
}
C 81600 42150 1 0 0 resistor-2.sym
{
T 82000 42500 5 10 0 0 0 0 1
device=RESISTOR
T 81800 42450 5 10 1 1 0 0 1
refdes=R21
T 81900 42180 5 10 1 1 0 0 1
value=18K
T 81600 42150 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 80175 42250 81600 42250 4
C 80800 40050 1 0 0 resistor-2.sym
{
T 81200 40400 5 10 0 0 0 0 1
device=RESISTOR
T 81000 40350 5 10 1 1 0 0 1
refdes=R23
T 81175 40080 5 10 1 1 0 0 1
value=1K
T 80800 40050 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
C 82750 42250 1 270 0 resistor-2.sym
{
T 83100 41850 5 10 0 0 270 0 1
device=RESISTOR
T 83050 42050 5 10 1 1 270 0 1
refdes=R22
T 82780 41975 5 10 1 1 270 0 1
value=4K7
T 82750 42250 5 10 0 0 270 0 1
footprint=RN55_type_0.1W_Resistor
}
N 82500 42250 82850 42250 4
N 82150 40750 81150 40750 4
N 81150 40750 81150 41350 4
N 82850 40150 81700 40150 4
N 79425 45525 83600 45525 4
N 83600 45525 83600 41350 4
N 83600 41350 82850 41350 4
N 82850 42250 82850 45000 4
N 82850 45000 78625 45000 4
N 78625 45000 78625 47600 4
N 79150 42650 80450 42650 4
N 80450 42650 80450 41825 4
C 80350 41825 1 270 0 resistor-2.sym
{
T 80700 41425 5 10 0 0 270 0 1
device=RESISTOR
T 80650 41625 5 10 1 1 270 0 1
refdes=R24
T 80380 41550 5 10 1 1 270 0 1
value=3K6
T 80350 41825 5 10 0 0 270 0 1
footprint=RN55_type_0.1W_Resistor
}
N 80450 40925 80450 39625 4
C 79900 43750 1 0 0 resistor-2.sym
{
T 80300 44100 5 10 0 0 0 0 1
device=RESISTOR
T 80100 44050 5 10 1 1 0 0 1
refdes=R24
T 80175 43780 5 10 1 1 0 0 1
value=3K6
T 79900 43750 5 10 0 0 0 0 1
footprint=RN55_type_0.1W_Resistor
}
N 80800 43850 81175 43850 4
N 81175 43850 81175 43050 4
C 75950 40150 1 0 0 DS1803-DIP16.sym
{
T 78800 44275 5 10 1 1 0 6 1
refdes=U9
T 76350 44750 5 10 0 0 0 0 1
device=DS1803
T 76350 44950 5 10 0 0 0 0 1
footprint=DIP16
}
C 80350 39325 1 0 0 gnd-1.sym
