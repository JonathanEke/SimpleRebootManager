# SimpleRebootManager

This C++ program is designed to create a straightforward system tray menu that enables you to quickly reboot your computer into a different Windows installation without having to navigate through the installation selection screen or go through multiple reboots.

I created this program to save time and frustration since I found myself frequently rebooting my computer and having to wait for the installation selection menu to appear, followed by another reboot to get to the desired installation.

<img src="https://i.imgur.com/3WHP9C1.png" width="70%" height="70%">
Note: the current install you are running will have a tick next to it

<h4>Known issues</h4>
Requires windows langauge to be set to English, this is due to bcdedit outputting in the language your windows is set to.<br>
Might fix for other languages in the future.

<h4>Recommended install</h4>
I recomend you set the process to run on login with task scheduler<br>
so its always active in your system tray for when you want to switch installs.<br>
Quick guide on how todo that:
<ul>
<li>Run Task Scheduler</li>
<li>Click Create Basic Task (under Actions)</li>
<li>Name your task and press next</li>
<li>Select "When i log on" and press next</li>
<li>Select "Start a program" and press next</li>
<li>Select the exe file and press next</li>
<li>Tick "Open the properties dialog for this task when i click finish" and press finish</li>
<li>Tick Run with highest privileges (this is a must, otherwise the program wont run as admin and will fail to launch) and press ok</li>
</ul>
Do this for all your windows installs.
