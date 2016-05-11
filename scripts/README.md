Once the touchee library has been built, simply execute 'refreshlib' and the required shared object and python interface shall be copied to the script directory.

Example:
$ cd elec5727_project
$ make
$ cd scripts
$ refreshlib
$ python symbol_detection.py input.png output.png


