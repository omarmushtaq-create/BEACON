# BEACON
All the code, cad, and PCB design for my low cost air quailty monitoring station.


<img width="1832" height="1506" alt="Untitled Design" src="https://github.com/user-attachments/assets/13ec2648-fa00-40ea-87fe-be1ce306298b" />

## Prerequisites

1. You need to create an InfluxDB acount and Database. Here is a guide that will help you do this https://docs.influxdata.com/influxdb/v2/get-started/setup/

## How to use

1. First use the provided STEP file to create a 3d model of the case
2. Use whatever fabrication method you would prefer (3d printing, CNC, Woodworking, Pottery, Blacksmithing etc) to create the case. Just make sure that it is water tight.
3. Use the provided PCB schematics in your PCB design software of choice and create the necessary gbr and drl files to get the PCB made by a chosen PCB manufacturer (I would recomendeither PCBway or JLBPCB)
4. Buy and solder the components or use PCBA serveries to have your manufacturer assemble them for you
5. Update the config.h file in /Firmware/include/config.h and add you wifi SSID, Password and Influx DB data. This allows you to view the data from wherever you want and do anything with your data. 
6. Upload the code provided to the PCB.
7. Install the finished PCB into the case along with the solar panel and ZPH dust sensor and lithium ion batteries.
8. Install it to a pole or fixture using zip ties.
9. And your done! Now you can view and access your data from your influx DB dashboard.


## Adding to larger network
If you want to help contribute to the BEACON site at https://beaconoffical.netlify.app/, email me at beacon.offical2026@gmail.com

### If you have any ideas to improve the Code, Cad or PCB design make a pull request or email me. 


