Monterey
========

Monterey is the computer GUI for ROV-Suite, an open source ROV control system, written in Qt with SDL for joystick support.  It works in conjunction with the bottom-side code, written for Arduino microprocessors and Raspberry Pis, to operate an ROV.  

Features:
----------

-Multithreaded: For faster performance

-Joystick support: Nearly ANY USB joystick can work with Monterey, and all buttons, hats and axes are mappable to whichever commands you want

-Customizable: Joystick settings, mapping settings, and sensor settings let you tweak Monterey to your liking without having to look at any source code

-Open design: A documented UDP-based communications protcol makes it easy to write your own bottom-, or top-, side software to replace any component of ROV-Suite, including Monterey

-Expandable:  Monterey has been designed from the ground up to be easy to expand upon.  Need more relays?  Change a few lines of the source code and the number of required relays will be quickly and easily added

Requirements:
-------------

-An Arduino with ethernet capabilities and a Raspberry Pi

-A USB joystick or gamepad

Dependencies required for development:
---------------------------------------

-Qt 5.1

-SDL version 1.2.15 (sudo apt-get install libsdl-dev)

Notes:
-------

Monterey can compile on OSX, Windows and Linux just fine, but keep in mind that Monterey is being developed in a Windows environment.  
