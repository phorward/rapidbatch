J.M.K S.F. RapidBATCH 5.1 for 32-Bit Windows
INFORMATIONS ON EXTENDING LIBRARIES
###########################################################

This folder contains extending libraries that are implemen-
ted in RapidBATCH themselve. Theses libraries are imported
by many RapidBATCH scripts using the INCLUDE-instruction,
so the functions and procedures defined in the particular
library can be used within the script, to solve much pro-
blems faster than before.

The extending libraries are inherent parts of the Rapid-
BATCH scripting language package, and may not, as declared
in the license agreements, be provided to third parties
without the software package itself.

But you may compile and use the libraries for free within
your scripts (also you may distribute them in compiled
scripts) and you may modify them FOR YOUR OWN PURPOSES.

If you wrote a new, useful function or even a whole, new
library, it can be added and provided in a future release
of the RapidBATCH software package to other programmers.
Just send an e-Mail to mail@jmksf.com to let us check and
test your idea.

Detailed informations on each function and procedure de-
fined in the libraries can be found directly in the lib's
source code as a comment above each implementation.

With this release package, the following libraries are
provided:


+ ARRAY.RB	Functionalities to work on arrays.

+ DATABASE.RB	Implementation of a simple database
		engine,	saving its data to text files.
		These library is only a beta-version and
		may be unstable.

+ DATE.RB	Useful functions for date calculation

+ DEFAULT.RB	This is not a library, but a default in-
		clude file that is always included automa-
		tically each script call be the RapidBATCH
		interpreter; Edit other INCLUDE-calls or
		some code to this file if you want to use
		a set of functions in each script without
		explicitly import the libraries.

+ DIALOG.RB	Useful functionalities that can be used in
		association with user-defined dialogs;
		Also, this library contains universal use-
		able dialog-windows that are called like
		the build-in LISTBOX or INPUTBOX-dialogs.

+ MATH.RB	Mathematical functions and calculations.

+ STRING.RB	Extending and simplifying string functions.

+ SYSTEM.RB	Additional system-specific functionalities.

--
J.M.K S.F. RapidBATCH v5.1.0 (32-Bit) - Professional Edition
Copyright © 2000-2006 by J.M.K S.F., Jan Max Meyer