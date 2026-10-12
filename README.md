# HW 3 - Execution Integrity and Do Process

* Author: Leo Haener-Pope
* Class: CS452 Section #002
* Semester: Fall 2026

## Overview

This is a C implementation of a shell terminal program. It features sequencing, pipelining, and background capabilities
along with some built in commands `history pwd cd exit`.

## Reflection

This assignment was pretty interesting for me to do. It kind of reminded me of the interpreter assignments from CS354, but this was definitely pretty different. I think that it gave me more experience with other C libraries and made concepts of process management more apparent to me. I was confused on where to start, since I was unfamiliar with how things were working with this C program, and really where I needed to change things at. I ultimately did most of my changes in the Command.c file as well as the Pipeline.c file.

I think that the testing suite was really helpful and I also liked being able to run the terminal and test things out on my own computer. The most fun part of this assignment was designing the built in functions like cd and history, because that let me be creative with what I was doing and it felt like I wasn't relying a lot on google search and Gemini to understand what sort of C shenanigans were going on, though I think I am getting better with C. I think that I just have a tough time on these assignments because I have only ever had the one intro to C class before, and this stuff is pretty complex for the amount of C experience that I have and it makes me pretty uncomfortable with working with the code and kind of freezes me up. I should read the book more.

## Compiling and Using

To run the shell, simply use
`make run`
and the shell terminal program will start up in the console.

## Testing

To run testing suite, simply use
`make test`
To run all the tests.

Run valgrind with testing suite
`VG=1 Test/run shell`

Run valgrind with a shell session with
`make`
`./vg ./shell`

For me, the background test `bg3` would fail occasionally as it seemed the child proccess of printing /var would be completed instantly, making /var be the output first then the foreground process after (which should print /etc) would be then printed. (Though I think the /etc should print first if we are wanting to make the child process run in the background while letting other foreground commands go at the same time)

Also note that this assumes that a /var and /etc folder are available on your system (linux). For me, I am running WSL Ubuntu.

## Results

Most of the tests that I have created within the testing suite pass well. As mentioned above in the testing section, there are some finicky tests for using the & background sign in the shell, but I think that they are working as I intended (hopefully).

I believe that running valgrind also proves that memory is managed correctly, accounting for certain library command usage.

## References

Gemini was helpful for figuring out the C libraries/functions necessary for this, as well as debugging.