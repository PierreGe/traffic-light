//This file was generated from (Academic) UPPAAL 4.1.4 (rev. 5648), July 2014

/*
The bus will never have to wait indefinitely to cross the crossroad
*/
(Bus.Red and busc==true) --> Bus.Green

/*
Not crosswalk light green and another green at the same time
*/
A[] not (((Crosswalk.Green or Crosswalk.Free) and  (Bus.Green or Bus.Free)) or ((Crosswalk.Green or Crosswalk.Free) and TrafficLightFrontBack.Green) or ((Crosswalk.Green or Crosswalk.Free) and TrafficLightLeftRight.Green))

/*
Not bus light green and another green at the same time
*/
A[] not (((Bus.Green or Bus.Free) and  (Crosswalk.Green or Crosswalk.Free)) or ((Bus.Green or Bus.Free) and TrafficLightFrontBack.Green) or ((Bus.Green or Bus.Free) and TrafficLightLeftRight.Green))

/*

*/
A[] Crosswalk.Free imply pulsed==false

/*

*/
A[] Crosswalk.Waiting imply pulsed==false

/*

*/
A[] not deadlock

/*
The pedestrian will never have to wait indefinitely to cross the road
*/
(Crosswalk.Red and pedc==true) --> Crosswalk.Green

/*
if crosswalk and bus are red, one of traffic light are green
*/
(not (Crosswalk.Green or Crosswalk.Free) and not( Bus.Green or Bus.Free)) -->  (TrafficLightLeftRight.Green or TrafficLightFrontBack.Green)

/*
Check if all states verify the follow proprety: If the crosswalk is waiting to be green, a trafficlight cannot be green when the other is in Crosswalk mode and is waiting for the crosswalkLight to be green
*/
A[] not ( (TrafficLightFrontBack.Green and TrafficLightLeftRight.Crosswalk and Crosswalk.Waiting) or (TrafficLightLeftRight.Green and TrafficLightFrontBack.Crosswalk and Crosswalk.Waiting) )
