README

This program depends on openFrameworks, so if you don't have that, download it here:
http://www.openframeworks.cc/download/

APPARENTLY, it doesn't work out of box on OSX Lion/Mountain Lion, to my great chagrin. I'll try to compile on there so you can have a binary ASAP.

I am really hoping that you are not running Lion, because I've attached an .exe for Windows, and I'm waiting to get a binary for OSX Snow Leopard, etc.

All the code is in src. You'll also need to download the OfxVectorField plugin and place it in your Ofx installation/addons/ folder.

I changed the really buggy face detector to a system wherein the user defines their own points.
However, I did not write my own system to do so. There are two utility programs in "utilities/" (one for OSX and one for Windows) that should work. I have only looked at the Windows one.

UPDATES SINCE FINAL PRESENTATION
I changed the face morphing to use user defined points (STILL BROKEN)
Stroke orientations look a bit better than before, with the simply reasoning that most people do paint from the top left corner outwards
There's a sort of simplistic colour matching going on, with a predetermined palette based on Modigliani's pre existing paintings
Edge detection because he defines his borders :)

FUTURE WORK
Curve those brush strokes/add more to the brush dictionary
Image parsing!
Different styles?

CREDITS

PAINTERLY RENDERING
http://itp.nyu.edu/varwiki/Syllabus/Pixels-S09
I took a lot of the helper functions from here, particularly convolve and find edges
http://www.stat.ucla.edu/~mtzhao/research/parse2paint/
Mingtian Zhao, for the idea of a brush dictionary

FACE MORPHING
http://www.cs.toronto.edu/~arnold/320/05s/lectures/week11/notes.pdf
http://www.hammerhead.com/thad/morph.html
(based on this paper)
Jason Saragih, for face tracker, even if I didn't end up using it

COLOURS
http://www.colorhunter.com/tag/modigliani/