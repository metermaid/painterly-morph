README

This program depends on openFrameworks, so if you don't have that, download it here:

There's a codeblocks project file, so if you have a similar path structure to me (e.g. "your openframeworks installation/projects/graphics/extracted folder") it should run instantly.

All the code is in src. I've removed the whole gradient field thing because it wasn't working correctly (despite my best efforts, if I had more time I might fix it) and added a dependency, making installation even harder.

I changed the really buggy face detector to a system wherein the user defines their own points. However, I did not write my own system to do so. There are two utility programs in "utilities/" (one for OSX and one for Windows) that should work. I have only looked at the Windows one.

Otherwise, if you don't want a demo, there are examples provided.

UPDATES SINCE FINAL PRESENTATION
I changed the face morphing to use user defined points (STILL BROKEN)
I removed the gradient field orientation-- it looks better with random brush orientations, actually!
I have added strokes via edge detection... This really should have proper orientations, if I had the time.

FUTURE WORK (still)
- Fix stroke orientations
- Colour match via image parsing
- Possibly use image parsing to customise stroke types

CREDITS
- http://itp.nyu.edu/varwiki/Syllabus/Pixels-S09
  I took a lot of the helper functions from here, particularly convolve and find edges
- Mingtian Zhao, for the idea of a brush dictionary
- This PowerPoint presentation for the face morphing stuff
http://www.cs.toronto.edu/~arnold/320/05s/lectures/week11/notes.pdf
http://www.hammerhead.com/thad/morph.html
(based on this paper)
- Jason Saragih, for face tracker, even if I didn't end up using it