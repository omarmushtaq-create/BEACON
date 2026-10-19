# BEACON
All the code, cad, and PCB design for my low cost air quailty monitoring station.


<img width="1832" height="1506" alt="Untitled Design" src="https://github.com/user-attachments/assets/13ec2648-fa00-40ea-87fe-be1ce306298b" />

## Prerequisites

1. You need to create an InfluxDB account and Database. Here is a guide that will help you do this https://docs.influxdata.com/influxdb/v2/get-started/setup/

## How to use

1. First use the provided STEP file to create a 3d model of the case
2. Use whatever fabrication method you would prefer (3d printing, CNC, Woodworking, Pottery, Blacksmithing etc) to create the case. Just make sure that it is water tight.
3. Use the provided Gerbers.zip in /PCB/Gerbers.zip to manufacture your PCB with your PCB manufacturer of choice (I recommend either JLBPCB or PCBway)
4. Buy and solder the components or use PCBA serveries to have your manufacturer assemble them for you
5. Install the Solar Panel Power Manager (SPPM), the ZPH02 sensor and the solar panel in the correct positions. For the solar panel you will need to solder jumpers from the + and - out terminals to the + and - in terminals of the SPPM. Connect the the 5v and GND out of the SPPM to the 2 pin connector on the board. Then connect the ZHP02 pinouts to the 5pin input on the PCB. (You can see a basic flow in /Assets/Wire_Diagarm.drawio.png). To make sure you have connected all the pins to the correct place you can reference the schematic at /PCB/BEACON.kicad_pcb
6. Update the config.h file in /Firmware/include/config.h and add you wifi SSID, Password and add a new device name (The device name you should not enter at this point are *esp32_node_01*). This allows the data to be sent to the central server and displayed on the website. 
7. Upload the modified code provided to the PCB.
8. Install the finished PCB into the case along with the solar panel and ZPH dust sensor and lithium ion batteries.
9. Install it to a pole or fixture using zip ties.
10. And your done! Now you can view and access your data from the website.



## Adding to BEACON
If you want to help contribute to the BEACON site at https://beaconoffical.netlify.app/, email me at beacon.offical2026@gmail.com

[![View PCB on KiCanvas](https://hack.club/pcb-badge)](https://kicanvas.org/?repo=https%3A%2F%2Fgithub.com%2Fomarmushtaq-create%2FBEACON%2Fblob%2Fmain%2FPCB%2FBEACON.kicad_pcb)

### If you have any ideas to improve the Code, Cad or PCB design email me!

Made with ❤️ By Omar


