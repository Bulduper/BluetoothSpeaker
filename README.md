# Bluetooth Speaker

![Photo of final result - v1.0](/Photos/BTSpeakerLights_on.gif?raw=true "Bluetooth Speaker")

### *Disclaimer*

This is not a regular github repository. You may consider it as a blog entry. Here, in this *readme*, I share my experience with designing a 3d printed bluetooth speaker. The code I uploaded runs on Atmega328P on the main board of the speaker.


## The goal
I wanted to have a wireless, convenient bluetooth speaker. So simple! So many people use them. I could have just bought one for like 100$, enjoy a decent sound quality and no maintenance issues.
Boooooring...

>I am curious how difficult it is to design and make a consumer electronics product like those we can buy on the market. *~ my thought, 2018*

The point was to try buliding a complete product with no expensive nor advanced technology. My workshop had to be enough!

Having a 3D printer and some inner motivation I challenged myself to create a bluetooth speaker which would meet the following criteria:
- 3D printed, as much as possible
- plays music!
- looks good (a more sophisticated design than speakers in a breakfast box)
- no wires sticking out the case, no duct tape, **no quick fix**
- can be used as comfortably as a commercial one


The general idea was to make a speaker like e.g. JBL Flip in terms of basic principals. And those were:
- two front speaker drivers
- two side passive speakers
- airtight
- wireless - bluetooth connection and no power cord
- convinient charging
- battery level indicator
- roughly the size of a 0.5l water bottle

## First try
My first design was successfull in some way.
![Photo of speaker v1.0](/Photos/speaker_v1.jpg?raw=true  "Version 1.0")
It worked, played music and basically met almost all the requirements to some extend. And to some extend only...
- it was not airtignt enough, a significant amount of air was leaking
- very inconvenient to assemble
- not really good looking
- it did not have visible quick fixes, but inside THERE WAS JUST A MESS

All the mentioned drawbacks were too much and so the build was unreliable. 

## Problems around batteries and charging
The battery of choice happens to be a Lithium Ion 18650 cell.
It's rated for 3.7V (max 4.2V) and has a good energy density. Also it is cheap and popular. Lithium Ion cells are kinda fussy, though. To keep them in good condition they need:
- constant current / constant voltage charging
- ballanced voltage between cells (if more than one)
- some protection against dangerously low voltage

I only needed to provide 12 volts to the amplifier and there came the first problem. I could either connect 3 18650 cells in series or boost the voltage with a boost converter. The latter solution turned out to be ineffective, as I didn't have (and couldn't find) a step up converter that would provide enough power for such a voltage difference. At higher volume there were power cutoffs.
The first idea however ruined my ideal, effortless charging concept. 

Ideally I wanted the speaker to be charged with a standard 5V wall charger. This is easily obtainable with a single lithium cell (opitonally multiple, in parallel). Thus, keeping the series cell connection, I had to abandon this way of charging.

My first attempt was to charge each cell individually with a TP4056 charging module for each one. In order for it to be possible (while keeping the cells in series) I had to power each module individually with a galvanically isolated wall charger, again for each one. I ended up having a custom charger box supplied with mains voltage which included 3 wall chargers. Very inelegant solution.

Finally I changed the concept and hooked up a 3S Battery Management System board. It's job is to ballance cell's voltage at the end of charging and stop the current flow in case of short circuit or over-discharge. To provide a recommended charging program I bought a 12,6V LiIon wall CC/CV charger.

## LEDs are cool
Well, at some point I decided it would be cool to have RGB LEDs dancing to the music. The lights had to be visible, so I printed the case with a transparent filament. I made some tests on audio processing by a microcontroller unit so that LEDs could react to the music beat. It tured out working quite well so I implemented the feature to the circuit and 3d design.

## DIY PCB
I didn't really like my first electronics arrangement. I just soldered all components on a ~~good~~ old breadboard. Is was very messy, little wires everywhere, so much to go wrong.

My plan was to try making my first own printed circuit borard. Not only am I talking about the design, but also about the manufacturing. I used a chemical transfer technique to place the tracks and pads on a copper covered board. I used a solution of sodium persulfate for etching. Then I drilled the through-holes with a dremel on vertical press. Soldering was just a pleasure.

So, the final circuit can be found [here for the main board](https://oshwlab.com/dawid199912/bluetoothspeaker_v2_custompcb) and [here for the top board](https://oshwlab.com/dawid199912/bluetoothspeaker_v2_toppanelpcb)

## Components

I bought most of the parts on Aliexpress. I had absolutely no confidence they would work together and produce a good sound quality.

1. 10W 4 Ohm 1.5inch speaker drivers - [Aliexpress link](https://www.aliexpress.com/item/32816810056.html?spm=a2g0s.9042311.0.0.27424c4d4MThSE)
2. Passive speakers - [Aliexpress link](https://www.aliexpress.com/item/32828412002.html?spm=a2g0s.9042311.0.0.27424c4d4MThSE)
3. Audio amplifier module based on 2x15W TPA3110
4. KRC-86b bluetooth audio module
5. ATMEGA 328P microcontroller

## Final(?) result
Now, considering all the difficulties I mentioned here and those I didn't mention, I must say I'm happy with the final result.

[LINK TO THE DEMO VIDEO](https://youtu.be/Gikhf-romf0)


There are some compromises against my initial plan I accepted such as:
- still not completely airtight
- use of hotglue to seal the connectors
- sound quality is just ok
- there is some resonance at high tones, mainly caused by loose top cover and wires inside the case
- audible noise when no music played 

## Summary
This project has let me explore so many branches of engineering - starting from 3d modeling, through batteries and charging management, ending up with pcb design and manufacturing.
I spent quite a lot of time solving minor problems with the 3d model and tweaking the code and values for LEDs. I got to the point where I still see so much room for improvement, but I'm happy with the result already. I will potentially keep on developing this project when I notice any interest from the community.

I hope I won't get kicked from github for this blog-ish entry ;)
