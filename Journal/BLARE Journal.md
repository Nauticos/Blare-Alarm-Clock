### 30 minutes

I first spent around 30 minutes soldering all the pins onto the ESP32 board and the screen. I had very limited experience soldering, and this was my first time soldering pins onto something, so I was a little scared that I would mess it up, but I watched a few videos beforehand and thankfully I think I did a pretty good job.

Here is the link to the Lapse of me soldering the pins onto the board and screen: https://lapse.hackclub.com/timelapse/SkuZfIy7-1nd

### 30 minutes

On another day, I tried to solder the wires onto the switches. First, I had to strip the ends of the jumper cables so that enough of the copper was out, then I had to wrap it around the switches and carefully solder them together. I found this much harder than soldering pins, since everything kept moving. I managed to do it, but only just about. In the end I soldered 3 switches for my alarm clock.

Here is the link to the Lapse of me soldering the switches: https://lapse.hackclub.com/timelapse/yXSMau4fNFif

### 30 minutes
I spent about 20 minutes connecting everything together. The coloured jumper cables helped me to plan out what cables connect where, but there were a lot of connections, so even with the colour coding it was still quite confusing. After I assembled everything outside of the casing and verified the screen does in fact turn on, I tried to put it in the casing. I spent around 10 minutes on this, before finally giving into the fact that the case I designed was too small because of the large connector pins on the jumper cables. I didn't want to believe it at first, I just hoped that I hadn't tried hard enough, but no matter what I tried, either the buzzer, a switch, or the screen wouldn't fit. This was quite disappointing, but in theory it wouldn't affect the actual function, so I carried on.

### 1h 30m
The next 1h 30m was spent trying to get the screen to display time. At first, it was just a static screen, then it went vertical instead of horizontal, then looked right except that it was offset a lot, then it just went fully black (after about 30 minutes of panicking and tried to find the problem, I realised one of the pins had disconnected), then I tried to get the text red and the background black which posed its own problems, overall this was quite frustrating but it taught me more about designing firmware. I spent the most time trying to figure out what was wrong with my code, but also going and researching how I could fix the problem I had. Once I was happy with how it looked, I moved onto the next stage, checking if the alarm itself actually worked.

### 1 hour
I spent around an hour getting the alarm to work the way I intended. At first, I checked if I could set the alarm time using the switches (I couldn't), so I had to do a few quick fixes to get the switches to actually work. Then, the alarm just didn't work at all, but after quite a bit of debugging I managed to get the screen to display "WAKE UP!!!", but the buzzer was still not going off. I did a bit of research online to try figure out where I went wrong, and managed to get the buzzer to work. I then managed to test the alarm: I set the time to a minute later, waited a minute, and the alarm went off, and was turned off when I pressed the Big Red Button on the top of it, just like I intended.

This was a fun experience, and I managed to get my alarm clock to work as intended (although some of the components were leaking out of the back since they wouldn't fit in the case). I recorded a video of it working and sent it to BLARE. This was a fun experience!

![Picture](</Images/"Blare Clock Picture".png>)