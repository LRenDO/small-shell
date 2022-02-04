Movies By Year Program
------------------------------------------------------------------------------
# Description
Allows user to choose largest csv file, smallest csv file, or a file by file
name in current directory.  Processes the file and prints all of the movies
from a release year to a file labeled with that year.

------------------------------------------------------------------------------
# Requirements

-Linux
-gcc (std=gnu99)
-csv files for movie data in the format below:

title,year,language,rating

Where languages are separated by semicolons and bracketed [] and 
rating is float between 1 and 10.  CSV files must have prefix below and .csv
to be included when processing largest or smallest file.

movies_

------------------------------------------------------------------------------
# Create Executable

-Navigate to directory where files are saved
-Use command below:

make

------------------------------------------------------------------------------
# Run the Program

-Navigate to directory where movie files are saved
-Create Executable as described above
-Use command below:

movies_by_year

------------------------------------------------------------------------------
# Removing Executable Only

-Navigate to directory where files are saved
-Use command below:

make clean

------------------------------------------------------------------------------
# Removing All Files (except the movie data files)

-Navigate to directory where files are saved
-Use command below:

make deepclean