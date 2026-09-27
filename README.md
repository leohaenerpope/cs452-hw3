# HW 1 - Queue\<Anon\>

* Author: Leo Haener-Pope
* Class: CS452 Section #002
* Semester: Fall 2026

## Overview

This program implements a doubly linked list using C. 
Void pointers are used and the main deq.c utilizes Node structs for each node in the
list, as well as Rep anonymous structs to represent the void pointer linked list structure itself.
In this homework, I implemented the four main functions outlined in deq.h, which are put, get, ith, and rem.

## Reflection

This was a refreshing experience with using C. I have not utilized C too much, as the only classes I have used it
with was the 25~ C class as well as the ECE 330 class. Though the syntax can be a little confusing sometimes,
I enjoy working with pointers and I think C being so low level is very fun to work with. Also, the review of the
homework that we had on Monday (8/31) was very helpful especially for getting started, as I was definitely a little
confused to start.

I am on windows so getting everything set up for C properly is a little difficult but I was able to do it successfully
without too much trouble. One thing though is I have to use WSL in order to use valgrind which was unfortunate since I was developing not on WSL at first so maybe for future projects I will just keep with staying on WSL only (since I also feel
quite comfortable with it, I have used WSL for some classes already).

## Compiling and Using

Inside of the source directory, run the `make` command.

To run main.c, run the `./deq` command.

Currently, main.c is configured to run a program that is helpful to run valgrind with.

To run valgrind with main, use `valgrind ./deq` (make sure that the program has been established with `make` already)

## Testing

This program's method of testing is just a simple unit test suite inside of the main.c file.

In order to run specific tests, comment the other tests out, and uncomment the test that you want to run. Results will be displayed in the terminal and should line up for what the test is trying to do (with no unintentional errors).

Currently, main.c runs a small test that is helpful to use with valgrind. To use valgrind, please see Compiling and Using above.


## Results

After creating and finishing all my tests, they fortunately all successfully passed.

Valgrind reported no memory leaks as well.