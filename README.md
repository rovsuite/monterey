![Monterey Screenshot - Windows](https://raw.github.com/rovsuite/monterey/master/Monterey/readmeresources/Monterey%203.0%20Demonstration.PNG)

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

Monterey can compile on OSX, Windows and Linux just fine, but keep in mind that Monterey is being developed in a Windows environment (tested on MinGW 4.8 32bit).  

How can I help make Monterey better?
-------------------------------------
There are many ways you can help (even if you can't write code), and all of them are greatly appreciated!

-Bug reports: These are incredibly useful and help make Monterey better for everyone.  If bugs aren't reported to the developers, how can they fix them?

-Feature requests: Want a feature added? Simply ask for it!  Go to the "Issues" page and create an issue tagged as "enhancement".  It's that simple!

-Code contributions:  If you've written some cool code or fixed some bugs, don't be afraid to ask for a pull request!

-Hardware: If you've made a cool ROV, write about it online and, if you want, share plans with people!  Monterey is a cool application, but it doesn't do much without an ROV attached to the other end of it...

-Join the project!  Anyone who wants to is allowed to join the ROV-Suite project.

-Provide feedback!  Do you like something we did?  Do you want to see a feature added?  Did we mess something up?  Let us know!  You can email Chris Konstad at chriskon149 [at] gmail [dot] com with your feedback.

-Finally, spread the word!

-You can find our wiki at monterey.wikia.com
