# Creative_Canvas

I have always liked image manipulation but had never had the chance to really experiment with it. 
I took this assignment as a chance to see what I could create by extracting information from any image. 
After looking at the openFrameworks addons, I decided to use ofxUI, ofxCV, ofxTriangleMesh, and ofxColorQuantizer.

I used ofxUI to give control to the user (me) to be able to manipulate the image.
ofxCV is the one giving me all the main information of the image, mostly the contours. 
ofxColorQuantizer and ofxTriangleMesh are both being used to add the effects to the image.

So Creative Canvas breaks an image into the main color pallet and finds the contours thanks to this colors.
You can switch between contours by clicking ‘c’ to see what it selects.

In the UI, there are 5 main features that can be applied to the image: 
“draw triangle”, “draw mesh”, “draw outline”, “draw particles” and “random”. 
What these features do is decide how you are going to draw over the current contour. 
If you select random, it will randomly select one of this effects.

You can loop as long as you want through the image and very cool effects come out (most of the time).

