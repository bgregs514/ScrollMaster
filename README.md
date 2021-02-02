# ScrollMaster
A(nother) scrolling solution for the Logitech Trackman Marble.

# Overview
The goal of this project is to serve as a template for those that are interested in using global keyboard/mouse hooks to replace scrolling functionality for their Trackman Marble.  Because of that limited initial scope, an effort was made to fully comment the important areas of code so that this can be ported to fit new use cases.  Think of this as a transparent template that can be used to write your own custom scrolling solution.

# Why this over the other options?
When I first started looking for a software workaround for the lack of scroll wheel on the Trackman Marble, I only found solutions that were either closed source or written in C#.  When it comes to keyboard/mouse hooks, I've always personally felt that these sorts of tasks were better left for compiled languages such as C/C++.  On top of that, I wanted to learn what was actually happening behind the scenes, and so I thought I would create a minimal working example that demonstrates how to implement a scroll-like feature into the Trackman Marble.

# What this is not
While this solution does work completely, it is not as robust as other available options.  For instance, to change the SCROLL_ACTION key, you must modify code directly and recompile the project.  While this is not overly complex, it is also not a solution for someone that just wants to run it and leave it.

# Future Plans
At this time, there are no major enhancements planned.  Aside from any bug/best practice code fixes, the current project meets the goals of being minimal and providing a fit-for-purpose solution.
