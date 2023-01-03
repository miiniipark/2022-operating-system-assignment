cmd_/home/pi/hw5/Module.symvers := sed 's/\.ko$$/\.o/' /home/pi/hw5/modules.order | scripts/mod/modpost -m -a  -o /home/pi/hw5/Module.symvers -e -i Module.symvers   -T -
