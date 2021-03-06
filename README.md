#MIDI-Accordion
How can I reuse an old instrument? Right, make it digital.

I had the wish to make digital music and wanted to reawake my small Hohner accordion because I used it seldom. So I started a small project to create a MIDI extension for the keyboard (right hand). After two months of working it's finally sending MIDI notes for each key, can play up to three octaves on one keypress and can interpret chords as commands.

##Technical
The extension is based on an Arduino Mega 2560 and programmed in C++, all the code can be found in the src/ Folder. The USB-port is used to get energy, but the MIDI signals are sent with baudrate 115200 on `Serial` so they can be accessed over USB, too. On the DIN connector (`Serial1` - TX1) are the MIDI signals sent with the baudrate 31250.

If someone has a question about a technical detail, feel free to open an issue or write a comment.

##Chords
<p>Select next program</p>
<img src="img/program_up.jpg" width="200px">
<p>Select previous program</p>
<img src="img/program_down.jpg" width="200px">
<p>Play everything one octave higher</p>
<img src="img/octave_up.jpg" width="200px">
<p>Play everything one octave lower</p>
<img src="img/octave_down.jpg" width="200px">
<p>Reset</p>
<img src="img/reset.jpg" width="200px">

<h2 style="float:left;clear:both;">Images</h2>
<img src="img/accordion_front.jpg" style="width:400px;height:299px;border-radius:10px;margin:5px;clear:both;float:left;">
Image of the presentation at "Testo Schülerwettbewerb (german: student contest)" where
the project got the first price in category "Beginners"
<img src="img/presentation_1.jpg" style="width:400px;height:312px;border-radius:10px;margin:5px;float:left;">
<img src="img/accordion_back.jpg" style="width:400px;height:312px;border-radius:10px;margin:5px;float:left;">
<img src="img/full_top.jpg" style="width:800px;height:592px;border-radius:10px;margin:5px;clear:both;float:left;">
<img src="img/detail_connectors.jpg" style="width:400px;height:296px;border-radius:10px;margin:5px;float:left;">
<img src="img/detail_angle.jpg" style="width:400px;height:296px;border-radius:10px;margin:5px;float:left;clear:both">
<img src="img/detail_switches.jpg" style="width:400px;height:296px;border-radius:10px;margin:5px;float:left;">
<img src="img/side_arduino.jpg" style="width:800px;height:283px;border-radius:10px;margin:5px;clear:both;float:left;">
<img src="img/detail_switches.jpg" style="width:400px;height:296px;border-radius:10px;margin:5px;float:left;clear:both;">
<img src="img/top_arduino.jpg" style="width:400px;height:296px;border-radius:10px;margin:5px;float:left;">
<img src="img/top_detail.jpg" style="width:400px;height:296px;border-radius:10px;margin:5px;float:left;clear:both;">
<img src="img/side_detail.jpg" style="width:400px;height:296px;border-radius:10px;margin:5px;float:left;">
