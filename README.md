# Hotel-Software

Before executing the program, MAKE SURE THAT "HotelRooms.txt" and "CurrentPrice.txt" ARE IN THE SAME FILE AS THE PROGRAM.

Room logs are kept in file "HotelRooms.txt" in binary form, so they can be read and changed by using program.

Current price is kept in file "CurrentPrice.txt". 

If you want to execute program in Linux, you should change system(“cls”) to "system(“clear”).

# Technical Issues

"HotelRooms.txt" is in binary form because it is easy to store roomInfo struct (fixed-size) in binary form, and it is faster in that way. Though these informations could be serialized and kept in more human-readable form like "CurrentPrice.txt", in that way it could be more portable. I chose to store them in binary to use them easily.

# If I had more time I would change this

· Different types of rooms with different prices
· Developing menu designs more
