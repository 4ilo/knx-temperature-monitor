EESchema Schematic File Version 4
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Knx temerature monitor"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU_ST_STM32F0:STM32F072C8Tx U?
U 1 1 5CE3370C
P 4350 4100
F 0 "U?" H 4300 2511 50  0000 C CNN
F 1 "STM32F072C8Tx" H 4300 2420 50  0000 C CNN
F 2 "Package_QFP:LQFP-48_7x7mm_P0.5mm" H 3750 2700 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00090510.pdf" H 4350 4100 50  0001 C CNN
	1    4350 4100
	1    0    0    -1  
$EndComp
$Comp
L project_lib:MAX31865 U?
U 1 1 5CE44A16
P 8050 4150
F 0 "U?" H 8075 4815 50  0000 C CNN
F 1 "MAX31865" H 8075 4724 50  0000 C CNN
F 2 "" H 7450 4800 50  0001 C CNN
F 3 "" H 7450 4800 50  0001 C CNN
	1    8050 4150
	1    0    0    -1  
$EndComp
$EndSCHEMATC
