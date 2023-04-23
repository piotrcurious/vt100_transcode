# vt100_transcode
vt100 as input tvterm as output. 
create Arduino code transcoding vt100 terminal compatible serial stream to atmega8 tvterm compatible serial stream. Assume serial input on hardware serial port and serial output using software serial. use large output buffer arranged as ring buffer. Use small, 128byte input buffer. Arrange code assuming serial output is much slower than input.
