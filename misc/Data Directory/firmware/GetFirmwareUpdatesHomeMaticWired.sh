#!/bin/sh
SCRIPTDIR="$( cd "$(dirname $0)" && pwd )"
FIRMWAREDIR=/tmp/HomegearTemp/rootfs/rootfs.ubi/174024608/root/firmware

if test ! -d $FIRMWAREDIR; then
	rm -Rf /tmp/HomegearTemp
	[ $? -ne 0 ] && exit 1
	mkdir /tmp/HomegearTemp
	[ $? -ne 0 ] && exit 1
	wget -P /tmp/HomegearTemp/ http://www.eq-3.de/Downloads/Software/HM-CCU2-Firmware_Updates/HM-CCU2%202.17.16/HM-CCU-2.17.16.tar.gz
	[ $? -ne 0 ] && exit 1
	tar -zxf /tmp/HomegearTemp/HM-CCU-2.17.16.tar.gz -C /tmp/HomegearTemp
	[ $? -ne 0 ] && exit 1
	rm -f /tmp/HomegearTemp/HM-CCU-2.17.16.tar.gz
	
	echo "Downloading UBI Reader..."
	echo "(C) 2013 Jason Pruitt (Jason Pruitt), see https://github.com/jrspruitt/ubi_reader"
	wget -P /tmp/HomegearTemp/ https://github.com/jrspruitt/ubi_reader/archive/v2_ui.tar.gz
	[ $? -ne 0 ] && exit 1
	tar -zxf /tmp/HomegearTemp/v2_ui.tar.gz -C /tmp/HomegearTemp
	[ $? -ne 0 ] && exit 1
	
	/tmp/HomegearTemp/ubi_reader-2_ui/extract_files.py -o /tmp/HomegearTemp/rootfs /tmp/HomegearTemp/rootfs.ubi
	[ $? -ne 0 ] && exit 1
fi

rm -f $SCRIPTDIR/0001.*

mv $FIRMWAREDIR/hmw_io_4_fm_hw0.hex $SCRIPTDIR/0001.00001000.fw
[ $? -ne 0 ] && exit 1
echo "0306" > $SCRIPTDIR/0001.00001000.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_lc_sw2_dr_hw0.hex $SCRIPTDIR/0001.00001100.fw
[ $? -ne 0 ] && exit 1
echo "0306" > $SCRIPTDIR/0001.00001100.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_io12_sw7_dr_hw0.hex $SCRIPTDIR/0001.00001200.fw
[ $? -ne 0 ] && exit 1
echo "0306" > $SCRIPTDIR/0001.00001200.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_lc_dim1l_dr_hw0.hex $SCRIPTDIR/0001.00001400.fw
[ $? -ne 0 ] && exit 1
echo "0303" > $SCRIPTDIR/0001.00001400.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_lc_bl1_dr_hw0.hex $SCRIPTDIR/0001.00001500.fw
[ $? -ne 0 ] && exit 1
echo "0306" > $SCRIPTDIR/0001.00001500.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_io_sr_fm_hw0_unstable.hex $SCRIPTDIR/0001.00001600.fw
[ $? -ne 0 ] && exit 1
echo "0001" > $SCRIPTDIR/0001.00001600.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_sen_sc_12_dr_hw0.hex $SCRIPTDIR/0001.00001900.fw
[ $? -ne 0 ] && exit 1
echo "0301" > $SCRIPTDIR/0001.00001900.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_sen_sc_12_fm_hw0.hex $SCRIPTDIR/0001.00001A00.fw
[ $? -ne 0 ] && exit 1
echo "0301" > $SCRIPTDIR/0001.00001A00.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_io_12_fm_hw0.hex $SCRIPTDIR/0001.00001B00.fw
[ $? -ne 0 ] && exit 1
echo "0300" > $SCRIPTDIR/0001.00001B00.version
[ $? -ne 0 ] && exit 1

mv $FIRMWAREDIR/hmw_io12_sw14_dr_hw0.hex $SCRIPTDIR/0001.00001C00.fw
[ $? -ne 0 ] && exit 1
echo "0100" > $SCRIPTDIR/0001.00001C00.version
[ $? -ne 0 ] && exit 1

rm -Rf /tmp/HomegearTemp

chown homegear:homegear $SCRIPTDIR/*.fw
chown homegear:homegear $SCRIPTDIR/*.version
chmod 444 $SCRIPTDIR/*.fw
chmod 444 $SCRIPTDIR/*.version
