XARIAS is embedded car computer system, based on ATMega32 MCU and programmed using C language and compiled with gcc compiler. Total binary code size is only 22Kb including all fonts definition!!!

Xarias is not utilizing OBD-II interface. All data is read directly from the sensors (speed, rpm, fuel injectors, etc).

http://lh4.ggpht.com/xtensa/SGq1yUVEsbI/AAAAAAAABls/jbzNeQDPBTg/P1160479.jpg?imgmax=512

The following features are available:
  * speed, fuel consumption, engine RPM calculating
  * numerous journey statistics gathering, including journey time, distance, average speed and fuel consumption, total fuel consumed and cost
  * external and internal temperatures displayed
  * automatic AC control (separate board developed) with smooth blower speed regulation
  * openned doors information
  * Graphical LCD with KS0108 driver is supported
  * 4x3 keyboard added.
  * I2C external interface added so that you can add external modules
  * Automatic Air Conditioning system developed as an external module
  * 1-Wire devices are supported on AC board

The newest version is v0.2 release candidate 2. The most important issue that rc2 resolves is s6b0108 display driver problems. Most of the displays should work with this driver properly now. Please let me know if yours is not working.

The most recent version (but not always stable) is available from SVN.

In the meantime I am glad to let you know that new XARIAS implementations appears in the world. Here is one of them:
http://lh3.ggpht.com/_7TzSR0Twwws/Scfq_4gsgaI/AAAAAAAADUk/cY4aPgq5mHY/s800/P3160298.JPG?imgmax=512

For more deatiled information please visit XARIAS wiki: http://code.google.com/p/xarias/w/list.

Whats next?
  * Diesel and LPG engines support.
  * Parking sensors.
  * Cruise control.
and maybe even more ...


All the new features in version v0.2 are discribed in this video on YouTube:

http://youtube.com/watch?v=R1mjBvfSwHk

If you still haven't seen version v0.1 in action (it was the proof of concept actually), please visit:

http://youtube.com/watch?v=39WC9hA0D9Y

And even more images are available in Picassa gallery:

http://picasaweb.google.com/xtensa/XARIASProject

Third parties about XARIAS project:

http://www.embedds.com/carputer-avr-based-car-computer/

http://www.rlocman.ru/shem/schematics.html?di=49958

http://www.rlocman.ru/shem/schematics.html?di=46685

http://ecomodder.com/forum/showthread.php/xarias-1719.html

http://www.pudn.com/downloads108/sourcecode/embed/detail443542.html