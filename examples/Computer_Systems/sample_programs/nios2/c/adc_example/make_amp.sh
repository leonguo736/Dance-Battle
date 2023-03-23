#!/bin/sh

progName=adc_example
progType=app_software_arm_C
systems=(DE0-Nano-SoC DE1-SoC DE10-Nano DE10-Standard)
curDir=U:/Computer_Systems/common/sample_programs/${progType}/${progName}/${progName}.amp

for i in ${systems[@]}; do
	sysDir=U:/Computer_Systems/${i}/${i}_Computer/${progType}/${progName}.amp
	if cp $curDir $sysDir; then
		chmod 666 $sysDir
		sed -i "s/\[system\]/${i} Computer/" $sysDir
		if [ $# -gt 0 ] && [ "$1" == "-v" ]; then
			echo "Project created at $sysDir"
		fi
	fi
done

